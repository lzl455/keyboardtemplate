/**
 * @file    io_board.h
 * @brief   板级开关，由 OM62xx 引脚配置器生成
 * @note    工程：SE73K 键盘
 *          放在 projects/mouse_demo/config/，board_config.h 会包含它，这里的值优先于那里的默认值。
 *          Keil 的 C/C++ Define 里写的同名宏优先级更高。只放宏定义，改了请回配置器重新生成。
 */
#ifndef __IO_BOARD_H
#define __IO_BOARD_H

/* ---- 旋转编码器（periph/knob.c） ------------------------------------- */
/* 配置器里没有启用编码器 */
#ifndef BOARD_HAS_KNOB
#define BOARD_HAS_KNOB           0
#endif

#endif /* __IO_BOARD_H */
