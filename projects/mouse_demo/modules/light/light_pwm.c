/**
 * @file    light_pwm.c
 * @brief   灯光输出后端：PWM 驱动普通 LED（BOARD_LIGHT == LIGHT_PWM）
 *
 * 灯效由 light_pwm_fx.c 单独计算（不用 ARGB 那套），每一路的点亮占空比在 light_pwm_duty[] 里：
 *   PWM_LIGHT_MONO  单色背光，用 [0]
 *   PWM_LIGHT_RGB   一颗 RGB 灯，[0] R、[1] G、[2] B
 */
#include <stddef.h>                     /* NULL */
#include "om_driver.h"
#include "main.h"
#include "light_fx.h"

#if (BOARD_LIGHT == LIGHT_PWM)

#define PWM_PERIOD          255u

/* 通道表放 RAM：flush 可能在 2.4G 的 RTC 中断里调用 */

#if (BOARD_PWM_LIGHT == PWM_LIGHT_RGB)
#define PWM_CH_NUM          3
static uint8_t pwm_ch[PWM_CH_NUM]    = {BOARD_PWM_CH_R, BOARD_PWM_CH_G, BOARD_PWM_CH_B};
#elif (BOARD_PWM_LIGHT == PWM_LIGHT_MONO)
#define PWM_CH_NUM          1
static uint8_t pwm_ch[PWM_CH_NUM]    = {BOARD_PWM_CH_MONO};
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
    if (false == bargb_valid)
        return;

    for (uint8_t n = 0; n < PWM_CH_NUM; n++)
        OM_PWM->PWMDC[pwm_ch[n]] = PWM_PERIOD - light_pwm_duty[n];   /* 寄存器写的是不点亮的宽度 */
}

#endif /* (BOARD_LIGHT == LIGHT_PWM) */
