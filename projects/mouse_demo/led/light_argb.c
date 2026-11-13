/**
 * @file    light_argb.c
 * @brief   灯光输出后端：LEDC 外设驱动 WS2812 类灯珠（BOARD_LIGHT == LIGHT_ARGB）
 *
 * 由原来 led.c 的 ledc_init() / logo_off() 和 drv_ledc.c 的 drv_ledc_write() 搬过来，时序和行为不变。
 */
#include "om_driver.h"
#include "main.h"
#include "led.h"
#include "light_port.h"

#if (BOARD_LIGHT == LIGHT_ARGB)

void light_port_init(void)
{
    ledc_config_t config = {
        .reset_cycles         = 100,
        .long_pulse_cycles    = 28,
        .short_pulse_cycles   = 14,
    };

    OM_GPIO0->DATAOUTCLR = (1u << BOARD_ARGB_DATA_PIN);
    light_port_power(true);

    drv_ledc_init(OM_LEDC, &config);
    drv_pmu_ledc_sel(BOARD_ARGB_LEDC_SEL);
    bargb_valid = true;
}

void light_port_off(void)
{
    bargb_valid = false;

    DRV_RCC_CLOCK_ENABLE(RCC_CLK_LEDC, 0U);
    OM_GPIO0->DATAOUTSET = (1u << BOARD_ARGB_DATA_PIN);
    light_port_power(false);
}

__RAM_CODE void light_port_flush(void)
{
    __ALIGNED(4) static uint8_t frame[iLED_MAX * 3];

    if (false == bargb_valid)
        return;

    OM_CRITICAL_BEGIN();
    light_port_scale(frame);
    OM_LEDC->DMA_CTL = ((uint32_t)frame << LEDC_DMA_CTL_DMA_ADDR_POS) | (LEDC_DMA_CTL_DMA_LEN_MASK & (iLED_MAX * 3));
    OM_LEDC->CR |= LEDC_CR_ENABLE_MASK;
    OM_CRITICAL_END();
}

#endif /* (BOARD_LIGHT == LIGHT_ARGB) */
