/**
 * @file    light_fx.h
 * @brief   灯光模块内部共享的声明（灯效引擎状态、灯效花样函数、灯效表）
 *          只给 modules/light/ 里的文件用，核心代码不要包含它。
 */
#ifndef __LIGHT_FX_H
#define __LIGHT_FX_H

#include "led.h"            /* 灯效设置、灯数量、亮度档位等定义 */
#include "light_port.h"     /* 输出后端 */

extern bool                     bback_led_status;
extern uint8_t                  gtemp_i;
extern uint8_t                  gtemp_j;
extern uint8_t                  gtemp_x;
extern uint8_t                  led_hint_mode;
extern uint8_t                  logo_hint_time_count;
extern uint8_t                  logo_hint_on_time;
extern uint8_t                  logo_hint_off_time;
extern uint8_t                  rgb_enable;
extern uint16_t                 led_ctrl_reg;
extern uint16_t                 led_status_reg;
extern uint8_t                  led_use_flag;
extern uint8_t                  led_pre_mode;
extern unsigned char            pwm_value;
extern uint16_t                 lfsr_value;
extern uint8_t                  start_dec_count;
extern uint8_t                  snake_led_index;
extern uint8_t                  pwm_time_count;
extern uint16_t                 delay_time_count;
extern uint8_t                  rgb_index;
extern uint8_t                  led_process_index;
extern uint8_t                  pwm_rgb_tbl[iLED_MAX][3];
extern uint8_t                  pwm_duty_array_0[];
extern uint8_t                  pwm_duty_array_1[];
extern uint8_t                  pwm_duty_array_2[];
extern uint8_t                  pwm_duty_array_3[];
extern unsigned char    const   rgb_en_tbl[];
extern unsigned char    const   led_liuguang_l_r_tbl[];
extern unsigned char    const   led_liuguang_rgb_tbl[];
extern unsigned char    const   sbzl_r_tbl[];
extern unsigned char    const   sbzl_g_tbl[];
extern unsigned char    const   sbzl_b_tbl[];
extern unsigned char    const   sbzl_ctrl_tbl[];
extern void logo_off(void);
extern void logo_process(void);
extern void clr_rgb(void);
extern void rgb_pwm0_w(void);
extern void rgb_w_max(void);
extern void led_mode_on(void);
extern void led_mode_breath(void);
extern void led_mode_snake(void);
extern void led_mode_nhjb(void);
extern void led_mode_liuguang(void);
extern void led_mode_star(void);
extern void led_mode_sbzl(void);
extern void led_mode_baoshan(void);
extern void led_mode_qcnh(void);
extern void led_mode_lhtw(void);
extern void led_mode_dxtw(void);
extern void led_mode_sxtw(void);
extern void led_mode_txtw(void);

#endif /* __LIGHT_FX_H */
