/**
 * @file    periph.h
 * @brief   可选外设模块框架
 *
 * USB / 2.4G / 蓝牙三种模式都在同样的时机调用下面三个入口：
 *   periph_init_all()        进入某个模式时（main.c 主循环里）
 *   periph_task_all()        每次扫键后、生成按键报文前（matrix_key.c 的 matrixkey_scan）
 *   periph_sleep_all(mask)   休眠级别变化时（main.c 的 mouse_periph_wakeup_enable）
 * 外设模块只要实现自己的 init / task / sleep，就能在所有模式和休眠流程里工作，
 * 不用再去改各模式的主循环和休眠代码。
 *
 * 新增一个外设：
 *   1. 新建 periph/xxx.c，定义 const periph_ops_t xxx_periph = { ... }，用不到的函数填 NULL。
 *      可以照着 knob.c（旋转编码器）写。
 *   2. board_config.h 里加开关 BOARD_HAS_XXX 和引脚参数。
 *   3. periph.c 的 periph_table 里加一行，用 #if (BOARD_HAS_XXX) 包住。
 *   4. Keil 工程的 periph 组里加上 xxx.c。
 * 引脚先用 tools/om6239-io-config 配置器检查有没有冲突。
 */
#ifndef __PERIPH_H
#define __PERIPH_H

#include <stdint.h>
#include "board_config.h"

typedef struct {
    const char *name;

    /* 进入 USB / 2.4G / 蓝牙模式时调用一次 */
    void (*init)(void);

    /* 每次扫键后调用，在按键解码之后、比较并生成报文之前。
     * 想发按键可以写 multi_data_tmp_l/h（多媒体键）或 tmp_std_key[]（普通键）。 */
    void (*task)(void);

    /* 休眠级别变化时调用，wakeup_mask 和 mouse_periph_wakeup_enable() 的参数相同：
     *   WAKEUP_SLEEP_LEVEL0  正常工作
     *   WAKEUP_SLEEP_LEVEL1  浅睡（STOP1），带 WAKEUP_WHEEL_MASK 等位表示允许对应外设唤醒
     *   WAKEUP_SLEEP_LEVEL2  深睡前，只剩按键能唤醒，外设应关掉省电 */
    void (*sleep)(uint8_t wakeup_mask);
} periph_ops_t;

void periph_init_all(void);
void periph_task_all(void);
void periph_sleep_all(uint8_t wakeup_mask);

#endif /* __PERIPH_H */
