/**
 * @file    io_config.c
 * @brief   OM6252 QFN48 引脚初始化，由 OM62xx 引脚配置器生成
 * @note    工程：SE73K 键盘
 */
#include "io_config.h"

void io_config_init(void)
{
    OM_RCC->CLKGATE |= RCC_DEV_CLK_GATE_GPIO_CLK_GATE_MASK;

    /* 上拉，每脚 2 位：00 无，01 30k，10 150k（按手册 V0.1） */
    OM_PMU->PAD_PU_CTRL    = 0x00AAAA00u;   /* 150k：P04 P05 P06 P07 P10 P11 P12 P13 */
    OM_PMU->PAD_PU_CTRL_1  = 0x0A0AAA25u;   /* 30k：P20 P21；150k：P22 P24 P25 P26 P27 P30 P31 P34 P35 */
    /* 下拉，每脚 1 位 */
    OM_PMU->PAD_PD_CTRL    = 0x4000000Cu;   /* P02 P03 P36 */
    /* 数字输入缓冲，接模拟电压的脚要关 */
    OM_PMU->PAD_BUF_EN     = 0xF7FF5FFFu;   /* 关闭：P15 P17 P33 */
    /* 驱动能力，每脚 2 位：0 8mA，1 24mA，2 32mA，3 48mA */
    OM_PMU->PAD_DRV_CTRL   = 0x555555A5u;   /* 32mA：P02 P03；其余 24mA */
    OM_PMU->PAD_DRV_CTRL_1 = 0x55555555u;   /* 全部 24mA */

    /* 复用选择 */
    /* ARGB 数据 P01：LEDC 选脚在开灯时由 light_argb.c 按 BOARD_ARGB_LEDC_SEL 设置（io_board.h），这里不设 */
    /* 电压检测：POFCON 的 LVD 引脚选 POF_LVD_PIN_P33（见 lvd.c） */
    OM_RCC->CLKGATE &= ~RCC_DEV_CLK_GATE_SWD_CLK_GATE_MASK;   /* 关闭 SWD，让出 P22/P25 */

    /* GPIO：先写电平再开输出，避免上电毛刺 */
    OM_GPIO0->DATAOUT  = 0xFFF7FFFEu;   /* 低电平：P00 P23 */
    OM_GPIO0->OUTENSET = 0x04080003u;   /* 输出：P00 P01 P23 P32 */
    OM_GPIO0->OUTENCLR = 0xFBF7FFFCu;   /* 其余为输入 */
}

void io_config_wakeup(bool enable)
{
    OM_CRITICAL_BEGIN();
    if (enable) {
        /* 唤醒，每脚 2 位：高位使能，低位 1 为下降沿 */
        OM_PMU->PAD_WK_CTRL   = 0x00FFFF00u;   /* 下降沿：P04 P05 P06 P07 P10 P11 P12 P13 */
        OM_PMU->PAD_WK_CTRL_1 = 0x00000000u;   /* 无 */
        if (IO_WAKEUP_FALL_MASK) drv_gpio_set_trig(OM_GPIO0, IO_WAKEUP_FALL_MASK, GPIO_TRIG_FALLING_EDGE);
        if (IO_WAKEUP_RISE_MASK) drv_gpio_set_trig(OM_GPIO0, IO_WAKEUP_RISE_MASK, GPIO_TRIG_RISING_EDGE);
        drv_pmu_clear_wakeup();
        NVIC_ClearPendingIRQ(WAKEUP_GPIO0_IRQn);
        NVIC_SetPriority(WAKEUP_GPIO0_IRQn, RTE_GPIO0_IRQ_PRIORITY);
        NVIC_EnableIRQ(WAKEUP_GPIO0_IRQn);
    } else {
        OM_PMU->PAD_WK_CTRL   = 0u;
        OM_PMU->PAD_WK_CTRL_1 = 0u;
        drv_gpio_set_trig(OM_GPIO0, IO_WAKEUP_FALL_MASK | IO_WAKEUP_RISE_MASK, GPIO_TRIG_NONE);
        NVIC_DisableIRQ(WAKEUP_GPIO0_IRQn);
        drv_pmu_clear_wakeup();
    }
    OM_CRITICAL_END();
}
