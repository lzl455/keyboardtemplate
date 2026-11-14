/*******************************************************************************
* Copyright (c) 2013, SHENZHEN Co.,LTD.
* All rights reserved.
*
* 文件名称: led.h
*
* 功能说明:
* 修改说明:
*******************************************************************************/
#ifndef _LED_H_
#define _LED_H_

#include "board_config.h"
#include "periph.h"           // 可选外设的统一入口

//=============================================================================;
// 配置引用区                                                                  ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// 配置区
//
#define iLED_SNAKE              (8)
#define iLED_DXTW               16
#define iLED_TXTW               11

#define iLED_CFG_POWER_ON       0
#define iLED_CFG_SENSOR_ERR     1
#define iLED_CFG_CONNECT_ING    2
#define iLED_CFG_CONNECT_OK     3
#define iLED_CFG_DPI_CHANGE     4
#define iLED_CFG_REPROT_CHANGE  5
#define iLED_CFG_CHAR_ING       6
#define iLED_CFG_CHAR_OK        7
#define iLED_CFG_OFF            8
#define iLED_CFG_IDLE           9

#define iPWM_R                  0x01
#define iPWM_G                  0x02
#define iPWM_B                  0x04
#define iPWM_RGB                0x07
#define iPWM_W                  0x0F

#define iPWM_MAX                64
#define iPWM_SNAKE_MAX          64
#define iRGB_QICAI              7

#define iR_INDEX                0x01
#define iG_INDEX                0x00
#define iB_INDEX                0x02

//-----------------------------------------------------------------------------;
// 配置区
//
#define iLED_NUM_MAX         	(8)
#define iLED_MAX         	    (BOARD_LED_NUM)   // 灯珠数量在 board_config.h 里改
#define iLED_HALF        	    (4)

enum led_level
{
    iLED_LEV_MIN=0,// 最暗
    iLED_LEV_0 = 0,
    iLED_LEV_1 = 1,
    iLED_LEV_2 = 2,
    iLED_LEV_MAX = 2,	// 最亮
};

enum speed_level
{
    iSPEED_LEV_MIN = 0,
    iSPEED_LEV_1 = 0,
    iSPEED_LEV_2 = 1,
    iSPEED_LEV_3 = 2,
    iSPEED_LEV_MAX = 2,
};

typedef enum {
    iMODE_LED_MIN       =0,

    iMODE_LED_OFF       =0,
    iMODE_LED_ON        =1,                     // 常亮
    iMODE_LED_BREATH    =2,                     // 呼吸
    iMODE_LED_SNAKE     =3,                     // 
    iMODE_LED_NHJB      =4,                     // 霓虹渐变
    iMODE_LED_LG_L_R    =5,                     // 流光
    iMODE_LED_STAR      =6,                     // 繁星点点
    iMODE_LED_SBZL      =7,                     // 随波逐流
    iMODE_LED_BS        =8,                     // 爆闪
    iMODE_LED_QCNH      =9,                     // 七彩霓虹
    iMODE_LED_LHTW      =10,                    // 来回拖尾
    iMODE_LED_DXTW      =11,                    // 单向拖尾
    iMODE_LED_SXTW      =12,                    // 双向拖尾
    iMODE_LED_TXTW      =13,                    // 对向拖尾
    iMODE_LED_FXTW      =14,                    // 反向拖尾
    iMODE_LED_JG        =15,                    // 激光

    iMODE_LED_MAX       =14,
    iMODE_LED_INVALID   =15,
    iLOGO_INVALID   = 0x0F,
} logo_mode_t;

//=============================================================================;
// 外部定义内部引用区                                                           ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// 外部寄存器引用
//

//-----------------------------------------------------------------------------;
// 外部函数引用
//

//=============================================================================;
// 内部定义外部引用区                                                          ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// 寄存器引用
//
extern bool                     blogo_status;
extern bool                     bled_bright_en;


extern uint8_t                  led_cur_mode;
extern uint8_t                  led_mode_bak;




extern uint8_t                  led_cur_mode;
extern enum speed_level         led_speed_lev;
extern unsigned char            led_rgb_index;
extern enum led_level           led_light_lev;


extern uint8_t                  hint_time_count;

//-----------------------------------------------------------------------------;
// 灯效编号（led_cur_mode）
//   ARGB 炫彩灯：上面的 iMODE_LED_*（灯效在 modules/light/light_modes.c）
//   PWM 灯：下面的 PWM_FX_*，单色背光和 RGB 灯各有一套（灯效在 modules/light/light_pwm_fx.c）
// 按键（userdefined_key_callback.c）、Flash 默认值和 USB 配置协议都用 LIGHT_MODE_* / LIGHT_SHORTCUT_*，
// 不直接写某一套的编号。
//
#if (BOARD_LIGHT == LIGHT_PWM)
enum pwm_fx_mode
{
    PWM_FX_ON = 0,                                  // 常亮
    PWM_FX_BREATH,                                  // 呼吸
#if (BOARD_PWM_LIGHT == PWM_LIGHT_RGB)
    PWM_FX_SPECTRUM,                                // 色彩渐变（只有 RGB 灯）
#endif
    PWM_FX_BLINK,                                   // 闪烁
    PWM_FX_HEART,                                   // 心跳
    PWM_FX_REACTIVE,                                // 按键触发：按键时亮，松开后慢慢熄灭
    PWM_FX_NUM
};
#define LIGHT_MODE_MIN          0
#define LIGHT_MODE_MAX          (PWM_FX_NUM - 1)    // 「灯效切换」键在 MIN..MAX 之间循环
// 快捷键尽量和 ARGB 同名：灯效 1 = 呼吸、灯效 4 = 常亮；PWM 灯的灯效 2 = 心跳、3 = 按键触发。
// 矩阵表配置器按灯光类型显示这些名字，改这里要同步改它的 LIGHT_FX 表
#if (BOARD_PWM_LIGHT == PWM_LIGHT_RGB)
#define LIGHT_MODE_DEFAULT      PWM_FX_SPECTRUM     // 恢复出厂后的灯效
#define LIGHT_SHORTCUT_0        PWM_FX_SPECTRUM     // Fn 快捷键 灯效 0..4
#define LIGHT_SHORTCUT_1        PWM_FX_BREATH
#define LIGHT_SHORTCUT_2        PWM_FX_HEART
#define LIGHT_SHORTCUT_3        PWM_FX_REACTIVE
#else
#define LIGHT_MODE_DEFAULT      PWM_FX_ON
#define LIGHT_SHORTCUT_0        PWM_FX_BLINK        // 单色没有颜色渐变，这个位置放闪烁
#define LIGHT_SHORTCUT_1        PWM_FX_BREATH
#define LIGHT_SHORTCUT_2        PWM_FX_HEART
#define LIGHT_SHORTCUT_3        PWM_FX_REACTIVE
#endif
#define LIGHT_SHORTCUT_4        PWM_FX_ON           // 灯效 4 = 常亮，已经是常亮时再按换颜色（和 ARGB 一样）
extern void light_pwm_fx_frame(void);
extern uint8_t                  light_pwm_duty[3];
#else
#define LIGHT_MODE_MIN          iMODE_LED_MIN
#define LIGHT_MODE_MAX          iMODE_LED_MAX
#define LIGHT_MODE_DEFAULT      iMODE_LED_SBZL
#define LIGHT_SHORTCUT_0        iMODE_LED_LG_L_R    // 流光
#define LIGHT_SHORTCUT_1        iMODE_LED_BREATH    // 呼吸
#define LIGHT_SHORTCUT_2        iMODE_LED_QCNH      // 七彩霓虹
#define LIGHT_SHORTCUT_3        iMODE_LED_NHJB      // 霓虹渐变
#define LIGHT_SHORTCUT_4        iMODE_LED_ON        // 常亮，再按换颜色
#endif






//-----------------------------------------------------------------------------;
// 函数引用
//
extern void led_status_hint(void);

extern void device_led_on(void);
extern void device_led_off(void);

extern void device_led_process(void);



//
//-----------------------------------------------------------------------------;
// 界面刷新（保存设置 + 可选外设刷新一帧），见 led.c
extern void ui_frame_process(void);

#endif