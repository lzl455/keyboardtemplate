/**
 * @file    periph.c
 * @brief   可选外设模块表，用法见 periph.h
 */
#include <stddef.h>
#include "om_driver.h"
#include "periph.h"
#include "knob.h"

/* 编进固件的外设。新增外设就在这里加一行，用 board_config.h 的开关包住 */
static const periph_ops_t * const periph_table[] = {
#if (BOARD_HAS_KNOB)
    &knob_periph,
#endif
    NULL                                /* 结束标记 */
};

void periph_init_all(void)
{
    for (const periph_ops_t * const *p = periph_table; *p != NULL; p++) {
        if ((*p)->init != NULL)
            (*p)->init();
    }
}

__RAM_CODE void periph_task_all(void)
{
    for (const periph_ops_t * const *p = periph_table; *p != NULL; p++) {
        if ((*p)->task != NULL)
            (*p)->task();
    }
}

void periph_sleep_all(uint8_t wakeup_mask)
{
    for (const periph_ops_t * const *p = periph_table; *p != NULL; p++) {
        if ((*p)->sleep != NULL)
            (*p)->sleep(wakeup_mask);
    }
}
