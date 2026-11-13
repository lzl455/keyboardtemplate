/**
 * @file    light_port.h
 * @brief   灯光输出层
 *
 * 灯效算法（logo_mode.c）只负责把每颗灯的颜色写进 pwm_rgb_tbl[BOARD_LED_NUM][3]，
 * 这一层负责把它送到实际的灯上。按 board_config.h 的 BOARD_LIGHT 编进其中一个后端：
 *   LIGHT_ARGB  light_argb.c   LEDC 外设驱动 WS2812 类灯珠
 *   LIGHT_PWM   light_pwm.c    PWM 驱动单色背光（1 路）或一颗 RGB 灯（3 路）
 *   LIGHT_NONE  light_port.c   空实现
 * 三个后端接口相同，模式主循环和休眠代码不用关心用的是哪一种。
 */
#ifndef __LIGHT_PORT_H
#define __LIGHT_PORT_H

#include <stdint.h>
#include <stdbool.h>
#include "board_config.h"

/* 灯光输出已打开（ARGB、PWM 通用）。打开期间 CPU 不进 STOP1（见 mouse_hw.c 的 pm_checker_callback）。
 * 沿用原来的变量名，因为工程链接的预编译 .o 可能引用它，不要改名 */
extern bool bargb_valid;

void light_port_init(void);     /* 打开灯：上电、初始化外设，置 bargb_valid */
void light_port_off(void);      /* 关灯：停外设、关时钟、断电，清 bargb_valid */
void light_port_flush(void);    /* 把当前一帧输出到灯上，按灯效节拍调用 */

/* 后端共用 */
void light_port_scale(uint8_t *out);    /* 按亮度档位把 pwm_rgb_tbl 换算成 0-255，out 长度 BOARD_LED_NUM*3 */
void light_port_power(bool on);         /* 控制灯电源开关脚 BOARD_LIGHT_PWR_PIN */

#endif /* __LIGHT_PORT_H */
