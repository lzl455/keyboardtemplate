/**
 * @file    knob.c
 * @brief   旋转编码器模块，也是新增外设的参考样板
 *
 * 编码器外设在中断里报告转了几格（knob_isr 累计），每拍扫键时（knob_task）换成动作：
 *   KNOB_ACTION_VOLUME  每格发一次音量加 / 减（按下、松开各一帧），走多媒体键通路，
 *                       USB、2.4G、蓝牙三种模式都直接支持
 *   KNOB_ACTION_WHEEL   转动量写进鼠标滚轮，走鼠标报文通路
 * 引脚、动作、方向、去抖在引脚配置器里设置（生成 config/io_board.h），也可以直接改 board_config.h。
 * 编码器脚需要上拉，同样在配置器里设置。
 */
#include "om_driver.h"
#include "main.h"
#include "matrix_key.h"
#include "key_code_define.h"
#include "knob.h"

#if (BOARD_HAS_KNOB)

#define KNOB_PENDING_MAX    16          /* 最多积压的格数，转得太快时多出的部分丢掉 */

static volatile int8_t      knob_delta;     /* 中断里累计、task 里取走 */
static int8_t               knob_pending;   /* 还没发出去的格数 */
static bool                 knob_pressing;  /* 音量键已按下，等待被接收 */
static uint8_t              knob_key_l;
static uint8_t              knob_key_h;
static bool                 knob_ready;

static int8_t clamp8(int16_t v, int8_t lim)
{
    if (v > lim)  return lim;
    if (v < -lim) return (int8_t)-lim;
    return (int8_t)v;
}

void knob_isr(int8_t step)
{
#if (BOARD_KNOB_INVERT)
    step = (int8_t)-step;
#endif
    knob_delta = clamp8((int16_t)knob_delta + step, 64);
}

static void knob_hw_run(bool wakeup)
{
    uint32_t en = OM_ENCODER->EN;

    en &= ~(ENCODER_EN_PINMUX_SEL_MASK | ENCODER_EN_WAKEUP_EN_MASK);
    en |= ((uint32_t)BOARD_KNOB_PINS << ENCODER_EN_PINMUX_SEL_POS)
        | ENCODER_EN_INTERRUPT_EN_MASK | ENCODER_EN_ENCODER_EN_MASK;
    if (wakeup)
        en |= ENCODER_EN_WAKEUP_EN_MASK;
    OM_ENCODER->EN = en;
}

static void knob_init(void)
{
    drv_encoder_init();                 /* 时钟、中断 */
    OM_ENCODER->SAM = BOARD_KNOB_DEBOUNCE;  /* 去抖时间，board_config.h / io_board.h 设置 */
    knob_hw_run(false);                 /* 按 board_config.h 选脚并开始计数 */

    knob_delta    = 0;
    knob_pending  = 0;
    knob_pressing = false;
    knob_ready    = true;
}

__RAM_CODE static void knob_task(void)
{
    int8_t d;

    if (!knob_ready)
        return;

    OM_CRITICAL_BEGIN();
    d = knob_delta;
    knob_delta = 0;
    OM_CRITICAL_END();

#if (BOARD_KNOB_ACTION == KNOB_ACTION_WHEEL)
    if (d != 0) {
        mouse_data_z += (uint8_t)d;
        m_tx_count = 1;
        m_data_flag |= bmouse_data_send;
    }
#else
    knob_pending = clamp8((int16_t)knob_pending + d, KNOB_PENDING_MAX);

    if (knob_pressing) {
        if (mmk_data_l == knob_key_l) {
            knob_pressing = false;      /* 按下已被接收，这一拍不再注入，key_process 会发出松开 */
            return;
        }
    } else {
        /* 等上一个多媒体键完全松开、用户也没按别的多媒体键时，才发下一格 */
        if (knob_pending == 0 || mmk_data_l != 0 || mmk_data_h != 0)
            return;
        if (knob_pending > 0) {
            knob_key_l = iMMK_VOLUMEUP_L;
            knob_key_h = iMMK_VOLUMEUP_H;
            knob_pending--;
        } else {
            knob_key_l = iMMK_VOLUMEDOWN_L;
            knob_key_h = iMMK_VOLUMEDOWN_H;
            knob_pending++;
        }
        knob_pressing = true;
    }

    if (multi_data_tmp_l == 0 && multi_data_tmp_h == 0) {
        multi_data_tmp_l = knob_key_l;
        multi_data_tmp_h = knob_key_h;
    }
#endif
}

static void knob_sleep(uint8_t wakeup_mask)
{
    if (!knob_ready)
        return;

    if (wakeup_mask == WAKEUP_SLEEP_LEVEL0) {
        knob_hw_run(false);             /* 正常工作 */
    } else if (wakeup_mask & WAKEUP_WHEEL_MASK) {
        knob_hw_run(true);              /* 浅睡：继续计数，转动可以唤醒 */
    } else {
        /* 深睡前：停掉省电。深睡醒来会复位，重新 init */
        OM_ENCODER->EN &= ~(ENCODER_EN_ENCODER_EN_MASK | ENCODER_EN_INTERRUPT_EN_MASK | ENCODER_EN_WAKEUP_EN_MASK);
    }
}

const periph_ops_t knob_periph = {
    .name  = "knob",
    .init  = knob_init,
    .task  = knob_task,
    .sleep = knob_sleep,
};

#endif /* (BOARD_HAS_KNOB) */
