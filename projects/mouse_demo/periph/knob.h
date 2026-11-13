/**
 * @file    knob.h
 * @brief   旋转编码器（音量旋钮 / 滚轮），board_config.h 里 BOARD_HAS_KNOB 打开后生效
 */
#ifndef __KNOB_H
#define __KNOB_H

#include <stdint.h>
#include "periph.h"

extern const periph_ops_t knob_periph;

/* ENCODER_IRQHandler（drv_isr.c）每收到一次计数调用，step 为带符号的格数 */
void knob_isr(int8_t step);

#endif /* __KNOB_H */
