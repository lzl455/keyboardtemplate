/**
 * @file    light_pwm.c
 * @brief   灯光输出后端：PWM 驱动普通 LED（BOARD_LIGHT == LIGHT_PWM）
 *
 * 灯效算法照常输出 BOARD_LED_NUM 颗灯的颜色，这里把它折算成 PWM 占空比：
 *   PWM_LIGHT_MONO  单色背光：每颗灯取 R/G/B 里最亮的一路，再对所有灯取平均
 *   PWM_LIGHT_RGB   一颗 RGB 灯：R、G、B 分别对所有灯取平均
 * 所以流水类灯效在 PWM 灯上表现为整体亮度变化，呼吸、常亮、颜色切换都能直接用。
 */
#include "om_driver.h"
#include "main.h"
#include "led.h"
#include "light_port.h"

#if (BOARD_LIGHT == LIGHT_PWM)

#define PWM_PERIOD          255u

#if (BOARD_PWM_LIGHT == PWM_LIGHT_RGB)
#define PWM_CH_NUM          3
static const uint8_t pwm_ch[PWM_CH_NUM]    = {BOARD_PWM_CH_R, BOARD_PWM_CH_G, BOARD_PWM_CH_B};
static const uint8_t pwm_color[PWM_CH_NUM] = {iR_INDEX, iG_INDEX, iB_INDEX};
#elif (BOARD_PWM_LIGHT == PWM_LIGHT_MONO)
#define PWM_CH_NUM          1
static const uint8_t pwm_ch[PWM_CH_NUM]    = {BOARD_PWM_CH_MONO};
#else
#error "BOARD_PWM_LIGHT must be PWM_LIGHT_MONO or PWM_LIGHT_RGB"
#endif

#define PWM_CH_MASK(n)      (1u << pwm_ch[n])

static pwm_channel_t light_pwm_mask(void)
{
    uint32_t mask = 0;
    for (uint8_t n = 0; n < PWM_CH_NUM; n++)
        mask |= PWM_CH_MASK(n);
    return (pwm_channel_t)mask;
}

void light_port_init(void)
{
    const pwm_config_t config = {
        .period    = PWM_PERIOD,
        .pre_scale = BOARD_PWM_PRESCALE,
    };
    const pwm_channel_config_t channel = {
        .output_en      = 1,
        .channel        = light_pwm_mask(),
        .initial_level  = 0,
        .active_level   = BOARD_PWM_ACTIVE_HIGH ? 1 : 0,
        .inactive_width = PWM_PERIOD,           /* 先全灭，第一次 flush 再给亮度 */
    };

    light_port_power(true);
    drv_pwm_init(OM_PWM, &config);
    drv_pwm_channel_config(OM_PWM, &channel);
    drv_pwm_start(OM_PWM, light_pwm_mask());
    bargb_valid = true;
}

void light_port_off(void)
{
    bargb_valid = false;

    drv_pwm_stop(OM_PWM, light_pwm_mask());     /* 停止后引脚保持非点亮电平 */
    drv_pwm_control(OM_PWM, PWM_CONTROL_CLK_DISABLE, NULL);
    light_port_power(false);
}

__RAM_CODE void light_port_flush(void)
{
    static uint8_t frame[iLED_MAX * 3];
    uint32_t sum;

    if (false == bargb_valid)
        return;

    light_port_scale(frame);

#if (BOARD_PWM_LIGHT == PWM_LIGHT_RGB)
    for (uint8_t n = 0; n < PWM_CH_NUM; n++) {
        sum = 0;
        for (uint8_t led = 0; led < iLED_MAX; led++)
            sum += frame[led * 3 + pwm_color[n]];
        OM_PWM->PWMDC[pwm_ch[n]] = PWM_PERIOD - (sum / iLED_MAX);
    }
#else
    sum = 0;
    for (uint8_t led = 0; led < iLED_MAX; led++) {
        uint8_t v = frame[led * 3];
        if (frame[led * 3 + 1] > v) v = frame[led * 3 + 1];
        if (frame[led * 3 + 2] > v) v = frame[led * 3 + 2];
        sum += v;
    }
    OM_PWM->PWMDC[pwm_ch[0]] = PWM_PERIOD - (sum / iLED_MAX);
#endif
}

#endif /* (BOARD_LIGHT == LIGHT_PWM) */
