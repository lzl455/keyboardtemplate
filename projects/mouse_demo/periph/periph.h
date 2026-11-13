/**
 * @file    periph.h
 * @brief   可选外设模块框架
 *
 * 可选外设（灯光、旋转编码器……）都放在 projects/mouse_demo/modules/<名字>/，
 * 每个模块只对外提供一个 periph_ops_t。主流程（USB / 2.4G / 蓝牙三种模式、休眠、
 * 低电关机）只调用下面这几个统一入口，不直接调用任何模块里的函数：
 *
 *   入口                    谁来调用                                  时机
 *   periph_init_all()       main.c 主循环                             进入某个模式时
 *   periph_scan_all()       matrix_key.c matrixkey_scan()             每次扫键后、生成报文前
 *   periph_frame_all()      led.c ui_frame_process()                  每帧（约 7~16ms）
 *   periph_flush_all()      各模式主循环 / 2.4G 的 RTC 中断             每帧把输出送到硬件
 *   periph_blank_all()      同步中、休眠前、低电关机、恢复出厂           关掉显示类输出
 *   periph_sleep_all(mask)  main.c mouse_periph_wakeup_enable()       休眠级别变化时
 *   periph_busy_any()       mouse_hw.c pm_checker_callback()          判断能不能进 STOP1
 *
 * 新增一个外设：
 *   1. 建目录 modules/xxx/，写 xxx.c，定义 const periph_ops_t xxx_periph，用不到的入口填 NULL。
 *      文件内容用 #if (BOARD_HAS_XXX) 包住，关掉时编译为空。可以照着 modules/knob/knob.c 写。
 *   2. board_config.h 里加开关 BOARD_HAS_XXX（默认 0）和引脚参数。
 *   3. periph.c 的外设表里加一行，同样用 #if (BOARD_HAS_XXX) 包住。
 *   4. Keil 工程里加上 modules/xxx/ 的文件和头文件路径。
 * 不需要某个外设时，把开关设成 0；也可以把它的文件从 Keil 工程里移除，主流程不受影响。
 * 引脚先用 tools/om6239-io-config 配置器检查有没有冲突。
 */
#ifndef __PERIPH_H
#define __PERIPH_H

#include <stdint.h>
#include <stdbool.h>
#include "board_config.h"

typedef struct {
    const char *name;

    /* 进入 USB / 2.4G / 蓝牙模式时调用一次 */
    void (*init)(void);

    /* 每次扫键后调用，在按键解码之后、比较并生成报文之前。
     * 想发按键可以写 multi_data_tmp_l/h（多媒体键）或 tmp_std_key[]（普通键）。 */
    void (*scan)(void);

    /* 每帧调用一次（约 7~16ms），用来跑灯效之类的动画 */
    void (*frame)(void);

    /* 把这一帧送到硬件。2.4G 模式会在 RTC 中断里调用，实现函数必须加 __RAM_CODE */
    void (*flush)(void);

    /* 关掉显示类输出：同步中、进休眠前、低电关机、恢复出厂设置时调用 */
    void (*blank)(void);

    /* 休眠级别变化时调用，wakeup_mask 和 mouse_periph_wakeup_enable() 的参数相同：
     *   WAKEUP_SLEEP_LEVEL0  正常工作
     *   WAKEUP_SLEEP_LEVEL1  浅睡（STOP1），带 WAKEUP_WHEEL_MASK 等位表示允许对应外设唤醒
     *   WAKEUP_SLEEP_LEVEL2  深睡前，只剩按键能唤醒，外设应关掉省电 */
    void (*sleep)(uint8_t wakeup_mask);

    /* 返回 true 表示外设正在工作、需要时钟，蓝牙模式下 CPU 不进 STOP1 */
    bool (*busy)(void);
} periph_ops_t;

void periph_init_all(void);
void periph_scan_all(void);
void periph_frame_all(void);
void periph_flush_all(void);
void periph_blank_all(void);
void periph_sleep_all(uint8_t wakeup_mask);
bool periph_busy_any(void);

#endif /* __PERIPH_H */
