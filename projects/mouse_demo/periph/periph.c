/**
 * @file    periph.c
 * @brief   可选外设模块表和统一入口，用法见 periph.h
 */
#include <stddef.h>
#include "om_driver.h"
#include "periph.h"

/* ---- 外设表 ------------------------------------------------------------------
 * 新增外设就在这里加一行，用 board_config.h 的开关包住。
 * 模块的实现都在 modules/<名字>/，这里只引用它们导出的 periph_ops_t。 */
#if (BOARD_LIGHT != LIGHT_NONE)
extern const periph_ops_t light_periph;     /* modules/light/：ARGB 或 PWM 灯 */
#endif
#if (BOARD_HAS_KNOB)
extern const periph_ops_t knob_periph;      /* modules/knob/：旋转编码器 */
#endif

static const periph_ops_t * const periph_table[] = {
#if (BOARD_LIGHT != LIGHT_NONE)
    &light_periph,
#endif
#if (BOARD_HAS_KNOB)
    &knob_periph,
#endif
    NULL                                    /* 结束标记 */
};

#define PERIPH_MAX      (sizeof(periph_table) / sizeof(periph_table[0]))

/* flush 可能在 2.4G 的 RTC 中断里调用，函数列表放一份在 RAM 里，中断里不用读 Flash */
static void (*flush_list[PERIPH_MAX])(void);

#define FOR_EACH(p)     for (const periph_ops_t * const *p = periph_table; *p != NULL; p++)

void periph_init_all(void)
{
    uint32_t n = 0;

    FOR_EACH(p) {
        if ((*p)->flush != NULL)
            flush_list[n++] = (*p)->flush;
    }
    flush_list[n] = NULL;

    FOR_EACH(p) {
        if ((*p)->init != NULL)
            (*p)->init();
    }
}

__RAM_CODE void periph_scan_all(void)
{
    FOR_EACH(p) {
        if ((*p)->scan != NULL)
            (*p)->scan();
    }
}

void periph_frame_all(void)
{
    FOR_EACH(p) {
        if ((*p)->frame != NULL)
            (*p)->frame();
    }
}

__RAM_CODE void periph_flush_all(void)
{
    for (uint32_t i = 0; flush_list[i] != NULL; i++)
        flush_list[i]();
}

void periph_blank_all(void)
{
    FOR_EACH(p) {
        if ((*p)->blank != NULL)
            (*p)->blank();
    }
}

void periph_sleep_all(uint8_t wakeup_mask)
{
    FOR_EACH(p) {
        if ((*p)->sleep != NULL)
            (*p)->sleep(wakeup_mask);
    }
}

bool periph_busy_any(void)
{
    FOR_EACH(p) {
        if ((*p)->busy != NULL && (*p)->busy())
            return true;
    }
    return false;
}
