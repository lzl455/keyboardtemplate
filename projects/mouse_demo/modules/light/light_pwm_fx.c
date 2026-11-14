/**
 * @file    light_pwm_fx.c
 * @brief   PWM 灯（单色背光 / 一颗 RGB 灯）专用的灯效，不和 ARGB 炫彩灯共用灯效
 *
 * ARGB 的灯效（light_modes.c）是按一排灯珠设计的，流光、拖尾这类效果合成到一路 / 三路 PWM 上
 * 只剩常亮或者忽明忽暗，所以 PWM 灯单独做一套整体变化的灯效：
 *
 *   编号（led_cur_mode）  单色背光        RGB 灯
 *   PWM_FX_ON            常亮            常亮（选的颜色；七彩 = 慢慢变色）
 *   PWM_FX_BREATH        呼吸            呼吸（七彩 = 每呼吸一次换一种颜色）
 *   PWM_FX_SPECTRUM      ——              色彩渐变
 *   PWM_FX_BLINK         闪烁            闪烁（七彩 = 每闪一次换颜色）
 *   PWM_FX_HEART         心跳            心跳（七彩 = 每跳一次换颜色）
 *   PWM_FX_REACTIVE      按键触发        按键触发（七彩 = 每按一次换颜色）
 *
 * Fn 快捷键（灯效 0-4）：                 单色背光        RGB 灯
 *   灯效 0                                闪烁            色彩渐变
 *   灯效 1                                呼吸            呼吸
 *   灯效 2                                心跳            心跳
 *   灯效 3                                按键触发        按键触发
 *   灯效 4                                常亮            常亮（再按换颜色）
 * 和 ARGB 同名的放在同一个位置（灯效 1 呼吸、灯效 4 常亮）。对应关系在 led/led.h 的
 * LIGHT_SHORTCUT_*，矩阵表配置器按灯光类型显示这些名字（tools/om62xx-keymap 的 LIGHT_FX 表）。
 * 亮度（led_light_lev 3 档）、速度（led_speed_lev 3 档，0 最快）、颜色（led_rgb_index 0-7）
 * 沿用原来的按键和 Flash 保存。
 *
 * 时间按 RTC 实际经过的时间算，不按帧数，帧间隔不同（蓝牙 / 2.4G / USB）速度也一样。
 * 输出写进 light_pwm_duty[]（点亮占空比 0-255，单色只用 [0]），由 light_pwm.c 的 flush 送到 PWM。
 */
#include "om_driver.h"
#include "main.h"
#include "matrix_key.h"                 /* key_ctrl / bkey_press_status：按键触发用 */
#include "light_fx.h"

#if (BOARD_LIGHT == LIGHT_PWM)

uint8_t                 light_pwm_duty[3];          /* R、G、B（单色只用 [0]），flush 会在中断里读，放 RAM */

/* 各灯效一个周期的时长（毫秒），按速度档：0 最快、1、2 最慢 */
static const uint16_t   fx_period_ms[PWM_FX_NUM][3] = {
    [PWM_FX_ON]       = {6000, 10000, 16000},       /* 七彩常亮时转一圈颜色的时间 */
    [PWM_FX_BREATH]   = {2000,  3500,  5500},
#if (BOARD_PWM_LIGHT == PWM_LIGHT_RGB)
    [PWM_FX_SPECTRUM] = {4000,  8000, 14000},       /* 转一圈颜色 */
#endif
    [PWM_FX_BLINK]    = { 500,  1000,  1600},
    [PWM_FX_HEART]    = { 800,  1100,  1500},
    [PWM_FX_REACTIVE] = { 300,   700,  1500},       /* 松开按键后熄灭用的时间 */
};

/* 亮度档位对应的最大亮度（gamma 之前），换算后约 12% / 50% / 100% */
static const uint8_t    fx_bright[3] = {90, 180, 255};

#if (BOARD_PWM_LIGHT == PWM_LIGHT_RGB)
/* 颜色：和 led_rgb_index 一致，0 红 1 绿 2 蓝 3 紫 4 黄 5 青 6 白，7 = 七彩（自动换色） */
#define FX_COLOR_RAINBOW    7
static const uint8_t    fx_color[7][3] = {
    {255,   0,   0}, {  0, 255,   0}, {  0,   0, 255}, {255,   0, 255},
    {255, 255,   0}, {  0, 255, 255}, {255, 255, 255},
};
static uint8_t          fx_cycle_color;             /* 七彩时当前用到第几种颜色 */
#endif

static uint8_t          fx_mode = 0xFF;             /* 当前在跑的灯效，和 led_cur_mode 不同时重新开始 */
static uint32_t         fx_last_tick;               /* 上一帧的 RTC 时间 */
static uint32_t         fx_ms;                      /* 当前周期里已经过的毫秒数 */
static uint8_t          fx_react;                   /* 按键触发：当前亮度 */
static bool             fx_key_down;                /* 按键触发：上一帧是否有键按着 */
static uint16_t         fx_tick_frac;               /* RTC 计数换算毫秒剩下的零头 */

/* 亮度曲线：人眼对亮度的感觉接近平方关系，先在「感觉亮度」上算，最后平方换成占空比 */
static uint8_t fx_gamma(uint32_t v)                  /* v: 0-255 */
{
    return (uint8_t)((v * v + 127u) / 255u);
}

/* 三角波 0..255..0，x: 0..len */
static uint32_t fx_tri(uint32_t x, uint32_t len)
{
    uint32_t h = len / 2u;
    if (h == 0u) return 0u;
    return (x < h) ? (x * 255u / h) : ((len - x) * 255u / (len - h));
}

/* 平滑一点：三角波再做一次 smoothstep，呼吸的两头不会显得「顿」 */
static uint32_t fx_smooth(uint32_t v)                /* 0-255 */
{
    return v * v * (765u - 2u * v) / (255u * 255u);  /* 255 * s^2 * (3 - 2s) */
}

#if (BOARD_PWM_LIGHT == PWM_LIGHT_RGB)
/* 色轮：h 0-1535 → 全饱和的 RGB */
static void fx_hue(uint32_t h, uint8_t rgb[3])
{
    uint8_t seg = (uint8_t)(h / 256u), f = (uint8_t)(h % 256u);
    switch (seg) {
        case 0:  rgb[0] = 255;     rgb[1] = f;       rgb[2] = 0;       break;   /* 红 → 黄 */
        case 1:  rgb[0] = 255 - f; rgb[1] = 255;     rgb[2] = 0;       break;   /* 黄 → 绿 */
        case 2:  rgb[0] = 0;       rgb[1] = 255;     rgb[2] = f;       break;   /* 绿 → 青 */
        case 3:  rgb[0] = 0;       rgb[1] = 255 - f; rgb[2] = 255;     break;   /* 青 → 蓝 */
        case 4:  rgb[0] = f;       rgb[1] = 0;       rgb[2] = 255;     break;   /* 蓝 → 紫 */
        default: rgb[0] = 255;     rgb[1] = 0;       rgb[2] = 255 - f; break;   /* 紫 → 红 */
    }
}
#endif

/* 心跳：一个周期里先一下强、再一下弱，然后停一会 */
static uint32_t fx_heart(uint32_t x, uint32_t len)
{
    uint32_t a = len * 14u / 100u, b0 = len * 22u / 100u, b = len * 12u / 100u;
    if (x < a) return fx_tri(x, a);
    if (x >= b0 && x < b0 + b) return fx_tri(x - b0, b) * 160u / 255u;
    return 0u;
}

/******************************************************************************
* 函数名称: light_pwm_fx_frame
* 功能描述: 算一帧 PWM 灯效，结果写进 light_pwm_duty[]
* 说    明: 由 light.c 的 logo_process() 每帧调用（灯开着、不在同步 / 休眠时）
******************************************************************************/
void light_pwm_fx_frame(void)
{
    uint32_t now = drv_rtc_read(OM_RTC), dt, period, level = 0;
    uint8_t  mode = led_cur_mode, rgb[3] = {255, 255, 255};
    bool     wrapped = false, pressed;

    if (mode >= PWM_FX_NUM)
        mode = LIGHT_MODE_DEFAULT;
    if (mode != fx_mode) {                          /* 换了灯效：从头开始 */
        fx_mode = mode;
        fx_ms = 0;
        fx_react = 0;
        fx_last_tick = now;
    }

    /* 经过的时间（RTC 每毫秒 32 个计数）；一次最多算 100ms，休眠回来不会跳一大截 */
    dt = (now - fx_last_tick) + fx_tick_frac;
    fx_last_tick = now;
    if (dt > mMS_TO_TICK(100)) dt = mMS_TO_TICK(100);
    fx_tick_frac = (uint16_t)(dt & 0x1Fu);
    dt >>= 5;

    period = fx_period_ms[mode][led_speed_lev <= 2 ? led_speed_lev : 2];
    fx_ms += dt;
    if (fx_ms >= period) {
        fx_ms %= period;
        wrapped = true;
    }

    switch (mode) {
        case PWM_FX_BREATH:   level = fx_smooth(fx_tri(fx_ms, period)); break;
        case PWM_FX_BLINK:    level = (fx_ms < period / 2u) ? 255u : 0u; break;
        case PWM_FX_HEART:    level = fx_heart(fx_ms, period);         break;
        case PWM_FX_REACTIVE:
            pressed = (0 != (key_ctrl & bkey_press_status));
            wrapped = pressed && !fx_key_down;      /* 只有新按下一个键时才算一轮：七彩时换颜色 */
            fx_key_down = pressed;
            if (pressed) {
                fx_react = 255;
            } else if (fx_react) {                  /* 松开后在一个周期里熄灭 */
                uint32_t dec = dt * 255u / period;
                fx_react = (dec >= fx_react) ? 0 : (uint8_t)(fx_react - dec);
            }
            level = fx_react;
            break;
        default:              level = 255u;                            break;   /* 常亮、色彩渐变 */
    }

    /* 颜色 */
#if (BOARD_PWM_LIGHT == PWM_LIGHT_RGB)
    if (PWM_FX_SPECTRUM == mode || (PWM_FX_ON == mode && FX_COLOR_RAINBOW == (led_rgb_index & 0x07))) {
        fx_hue(fx_ms * 1536u / period, rgb);
    } else if (FX_COLOR_RAINBOW == (led_rgb_index & 0x07)) {
        if (wrapped) fx_cycle_color = (uint8_t)((fx_cycle_color + 1u) % 7u);
        rgb[0] = fx_color[fx_cycle_color][0]; rgb[1] = fx_color[fx_cycle_color][1]; rgb[2] = fx_color[fx_cycle_color][2];
    } else {
        const uint8_t *c = fx_color[led_rgb_index & 0x07];
        rgb[0] = c[0]; rgb[1] = c[1]; rgb[2] = c[2];
    }
#else
    (void)wrapped;                              /* 单色背光不用颜色 */
#endif

    /* 亮度档位 + 亮度曲线 */
    level = level * fx_bright[led_light_lev <= 2 ? led_light_lev : 2] / 255u;
    for (uint8_t n = 0; n < 3; n++)
        light_pwm_duty[n] = fx_gamma(level * rgb[n] / 255u);   /* 单色背光 rgb 是 255，只用 [0] */
}

#endif /* (BOARD_LIGHT == LIGHT_PWM) */
