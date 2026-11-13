/**
 * @file    light_port.c
 * @brief   灯光输出层的共用部分：亮度换算、灯电源开关、无灯时的空实现
 */
#include <string.h>
#include "om_driver.h"
#include "main.h"
#include "led.h"
#include "light_port.h"

bool bargb_valid;

/* 亮度档位换算，算法和原来 drv_ledc_write() 里的一致 */
__RAM_CODE void light_port_scale(uint8_t *out)
{
    memcpy(out, pwm_rgb_tbl, iLED_MAX * 3);

    if (iLED_LEV_2 == led_light_lev) {
        for (uint8_t i = 0; i < iLED_MAX * 3; i++) {
            if (iPWM_MAX <= out[i])
                out[i] = 0xFF;
            else
                out[i] <<= 2;
        }
    } else if (iLED_LEV_1 == led_light_lev) {
        for (uint8_t i = 0; i < iLED_MAX * 3; i++)
            out[i] <<= 1;
    } else if (iLED_LEV_0 == led_light_lev) {
        for (uint8_t i = 0; i < iLED_MAX * 3; i++)
            out[i] >>= 1;
    }
}

void light_port_power(bool on)
{
#if (BOARD_LIGHT_PWR_PIN >= 0)
    if (on == (BOARD_LIGHT_PWR_ON != 0))
        OM_GPIO0->DATAOUTSET = (1u << BOARD_LIGHT_PWR_PIN);
    else
        OM_GPIO0->DATAOUTCLR = (1u << BOARD_LIGHT_PWR_PIN);
#else
    (void)on;
#endif
}

/* 兼容旧接口：工程链接的预编译 .o（rf_protocol.o 等）可能还在调用这两个旧名字 */
void ledc_init(void)
{
    light_port_init();
}

__RAM_CODE om_error_t drv_ledc_write(void)
{
    light_port_flush();
    return OM_ERROR_OK;
}

#if (BOARD_LIGHT == LIGHT_NONE)
void light_port_init(void)  { }
void light_port_off(void)   { bargb_valid = false; }
void light_port_flush(void) { }
#endif
