/**
 * @file    board_config.h
 * @brief   板级配置：可选外设的开关和参数都集中在这里
 *
 * 换灯、加外设时只改这个文件，不用去改 USB / 2.4G / 蓝牙的主循环。
 * 本文件只能放宏定义（RTE_om6239.h 会包含它），唯一例外是同样只有宏定义的 io_board.h。
 * 带 #ifndef 的几个开关也可以在 Keil 的 C/C++ Define 里覆盖，方便一套代码出多个型号，
 * 例如 BOARD_LIGHT=LIGHT_PWM,BOARD_HAS_KNOB=1。
 *
 * 新增一个外设的步骤见 periph/periph.h。
 */
#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

/* 引脚配置器（tools/om6239-io-config）生成的板级开关。
 * 它先于下面的默认值生效：优先级 Keil Define > io_board.h > 本文件默认值。
 * 灯光（ARGB / PWM 二选一）和编码器都可以在配置器里选，生成到 io_board.h。 */
#include "io_board.h"

/* =============================================================================
 * 灯光
 * ========================================================================== */
#define LIGHT_NONE              0       /* 没有灯 */
#define LIGHT_ARGB              1       /* WS2812 类灯珠，LEDC 外设输出 */
#define LIGHT_PWM               2       /* PWM 驱动的普通 LED */

#ifndef BOARD_LIGHT
#define BOARD_LIGHT             LIGHT_ARGB
#endif
#if (BOARD_LIGHT != LIGHT_NONE) && (BOARD_LIGHT != LIGHT_ARGB) && (BOARD_LIGHT != LIGHT_PWM)
#error "BOARD_LIGHT 只能是 LIGHT_NONE / LIGHT_ARGB / LIGHT_PWM（ARGB 和 PWM 二选一）"
#endif

/* 灯效缓冲里的灯珠数量（灯效算法按这个数计算） */
#ifndef BOARD_LED_NUM
#define BOARD_LED_NUM           8
#endif

/* 灯电源开关脚（低功耗时关掉灯的供电）。没有就设为 -1 */
#ifndef BOARD_LIGHT_PWR_PIN
#define BOARD_LIGHT_PWR_PIN     0       /* P00 */
#endif
#ifndef BOARD_LIGHT_PWR_ON
#define BOARD_LIGHT_PWR_ON      1       /* 打开时输出的电平 */
#endif

/* ---- LIGHT_ARGB 参数 ---- */
#ifndef BOARD_ARGB_DATA_PIN
#define BOARD_ARGB_DATA_PIN     1       /* P01 */
#endif
/* LEDC 输出脚选择，只能是 P01 / P05 / P21 / P33：
 * PMU_LEDC_SEL_IO_01 / PMU_LEDC_SEL_IO_05 / PMU_LEDC_SEL_IO_17 / PMU_LEDC_SEL_IO_27 */
#ifndef BOARD_ARGB_LEDC_SEL
#define BOARD_ARGB_LEDC_SEL     PMU_LEDC_SEL_IO_01
#endif

/* ---- LIGHT_PWM 参数 ---- */
#define PWM_LIGHT_MONO          1       /* 单色背光，1 路 PWM */
#define PWM_LIGHT_RGB           3       /* 一颗 RGB 灯，3 路 PWM */

#ifndef BOARD_PWM_LIGHT
#define BOARD_PWM_LIGHT         PWM_LIGHT_MONO
#endif

/* PWM 通道号 0-7，按 OM6252 QFN48 封装图：
 *   P00 = PWM0   P01 = PWM1   P04 = PWM2   P05 = PWM3   P26 = PWM6   P32 = PWM7
 *   P06、P22 图上只标了 PWM，是 PWM4 / PWM5 中的一个，用之前先上板确认
 * 下面的默认值只是示例，按实际接灯的脚填写，引脚冲突用配置器检查 */
#ifndef BOARD_PWM_CH_MONO
#define BOARD_PWM_CH_MONO       0       /* 例：单色背光接 P00 */
#endif
#ifndef BOARD_PWM_CH_R
#define BOARD_PWM_CH_R          0       /* 例：RGB 灯 R 接 P00 */
#endif
#ifndef BOARD_PWM_CH_G
#define BOARD_PWM_CH_G          1       /*      G 接 P01 */
#endif
#ifndef BOARD_PWM_CH_B
#define BOARD_PWM_CH_B          6       /*      B 接 P26 */
#endif

#ifndef BOARD_PWM_ACTIVE_HIGH
#define BOARD_PWM_ACTIVE_HIGH   1       /* 1：高电平点亮；0：低电平点亮 */
#endif
/* PWM 分频，周期固定为 255 级。48MHz / 8 / 256 约 23kHz，蓝牙模式 12MHz 时约 6kHz */
#ifndef BOARD_PWM_PRESCALE
#define BOARD_PWM_PRESCALE      PWM_PRE_SCALE_8
#endif

/* =============================================================================
 * 旋转编码器（音量旋钮 / 滚轮）
 * ========================================================================== */
#ifndef BOARD_HAS_KNOB
#define BOARD_HAS_KNOB          0
#endif

/* 编码器脚只能四选一：
 * ENCODER_PINMUX_SEL_IO_00_01  A=P01 B=P00
 * ENCODER_PINMUX_SEL_IO_05_06  A=P06 B=P05
 * ENCODER_PINMUX_SEL_IO_20_21  A=P21 B=P20   ← 本工程目前唯一空闲的一组
 * ENCODER_PINMUX_SEL_IO_24_25  A=P25 B=P24 */
#ifndef BOARD_KNOB_PINS
#define BOARD_KNOB_PINS         ENCODER_PINMUX_SEL_IO_20_21
#endif

#define KNOB_ACTION_VOLUME      0       /* 旋转 = 音量加减（多媒体键） */
#define KNOB_ACTION_WHEEL       1       /* 旋转 = 鼠标滚轮 */
#ifndef BOARD_KNOB_ACTION
#define BOARD_KNOB_ACTION       KNOB_ACTION_VOLUME
#endif
#ifndef BOARD_KNOB_INVERT
#define BOARD_KNOB_INVERT       0       /* 方向反了就改成 1 */
#endif

/* 去抖时间：ENCODER_DEBOUNCE_TIME_0 ~ ENCODER_DEBOUNCE_TIME_1MS，共 16 档（drv_encoder.h） */
#ifndef BOARD_KNOB_DEBOUNCE
#define BOARD_KNOB_DEBOUNCE     ENCODER_DEBOUNCE_TIME_250US
#endif

/* =============================================================================
 * 由上面的开关推出驱动开关，RTE_om6239.h 使用，不要手改
 * ========================================================================== */
#define BOARD_RTE_LEDC          (BOARD_LIGHT == LIGHT_ARGB)
#define BOARD_RTE_PWM           (BOARD_LIGHT == LIGHT_PWM)
#define BOARD_RTE_ENCODER       (BOARD_HAS_KNOB)

#endif /* __BOARD_CONFIG_H */
