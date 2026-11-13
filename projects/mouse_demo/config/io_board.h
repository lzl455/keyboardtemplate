/**
 * @file    io_board.h
 * @brief   板级开关，由 OM62xx 引脚配置器生成
 * @note    工程：SE73K 键盘
 *          放在 projects/mouse_demo/config/，board_config.h 会包含它，这里的值优先于那里的默认值。
 *          Keil 的 C/C++ Define 里写的同名宏优先级更高。只放宏定义，改了请回配置器重新生成。
 */
#ifndef __IO_BOARD_H
#define __IO_BOARD_H

/* ---- 灯光（modules/light/），ARGB 和 PWM 二选一 ------------------------- */
#ifndef BOARD_LIGHT
#define BOARD_LIGHT              LIGHT_ARGB   /* ARGB 灯珠（LEDC） */
#endif
#ifndef BOARD_ARGB_DATA_PIN
#define BOARD_ARGB_DATA_PIN      1   /* P01 */
#endif
#ifndef BOARD_ARGB_LEDC_SEL
#define BOARD_ARGB_LEDC_SEL      PMU_LEDC_SEL_IO_01
#endif
#ifndef BOARD_LIGHT_PWR_PIN
#define BOARD_LIGHT_PWR_PIN      0   /* P00 灯电源开关 */
#endif
#ifndef BOARD_LIGHT_PWR_ON
#define BOARD_LIGHT_PWR_ON       1   /* 打开灯电源时输出的电平 */
#endif

/* ---- 可选功能（关掉的不编进固件；电量检测是必需功能，没有开关） -------- */
#ifndef BOARD_HAS_USB
#define BOARD_HAS_USB            1   /* USB 有线模式 */
#endif
#ifndef BOARD_HAS_FCC_TEST
#define BOARD_HAS_FCC_TEST       1   /* FCC / SRRC 认证测试 */
#endif
#ifndef BOARD_LIGHT_KEYS
#define BOARD_LIGHT_KEYS         1   /* 灯光 Fn 键 */
#endif

/* ---- 旋转编码器（modules/knob/） -------------------------------------- */
/* 配置器里没有启用编码器 */
#ifndef BOARD_HAS_KNOB
#define BOARD_HAS_KNOB           0
#endif

#endif /* __IO_BOARD_H */
