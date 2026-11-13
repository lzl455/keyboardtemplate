/*******************************************************************************
* Copyright (c) 2013, SHENZHEN OM Co.,LTD.
* All rights reserved.
*
* 文件名称: main.h
*
* 功能说明:
* 修改说明:
*******************************************************************************/
#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdint.h>
#include <stdbool.h>
#include "board_config.h"

//=============================================================================;
// 配置引用区                                                                  ;
//=============================================================================;

//----------------------------------------------------------------------------;
// 通用类型定义
//
#define P00     (8*0+0)         /* PWM / WHB */
#define P01     (8*0+1)         /* PWM / WHA */
#define P02     (8*0+2)         /* D- */
#define P03     (8*0+3)         /* D+ */
#define P04     (8*0+4)         /* UART TX */
#define P05     (8*0+5)        
#define P06     (8*0+6)         /* UART RX */
#define P07     (8*0+7)

#define P10     (8*1+0) 
#define P11     (8*1+1)         /* UART RX */
#define P12     (8*1+2)         /* UART TX */
#define P13     (8*1+3)
#define P14     (8*1+4)         /* FLASH */
#define P15     (8*1+5)         /* FLASH */
#define P16     (8*1+6)         /* FLASH */
#define P17     (8*1+7)         /* FLASH */

#define P20     (8*2+0)
#define P21     (8*2+1)         
#define P22     (8*2+2)         /* TCK */
#define P23     (8*2+3) 
#define P24     (8*2+4)
#define P25     (8*2+5)         /* TMS */
#define P26     (8*2+6)
#define P27     (8*2+7)

#define P30     (8*3+0)
#define P31     (8*3+1)
#define P32     (8*3+2)
#define P33     (8*3+3)
#define P34     (8*3+4)
#define P35     (8*3+5)        
#define P36     (8*3+6)
#define P37     (8*3+7)         /* BOOT */

//-----------------------------------------------------------------------------;
//配置常量定义区
//
#define RC_CLK        3200UL
//#define RC_CLK      2000000UL
#if(2000000UL == RTC)
#define mRTC_US(x)              (x*2L)   //
#else
#define mRTC_US(x)              ((x*4L) / 125L) //
#endif

#define APP_ENABLE    1                         // 0：disable；1：enable

#define RF_RATE       RF_RATE_2MHz
//{
    #define RF_RATE_1MHz      (1<<6)
    #define RF_RATE_2MHz      (0<<6)
//}
#define RF_PROTOCOL   RF_PROTOCOL_OLD
//{
    #define RF_PROTOCOL_OLD   (0<<5)
    #define RF_PROTOCOL_NEW   (1<<5)
//}
#define RF_CONN             RF_CONN_EN
//{
    #define RF_CONN_EN      1
    #define RF_CONN_DIS     0
//}

#define iSCRATCH_BOOT_POWER 0x550A5500
#define iAPP_INTO_BOOT      0x55AA55AA

//----------------------------------------------------------------------------;
// 通用类型定义
//
//----------------------------------------------
// 矩阵IO
//
// 行线、列线、扩展列、指示灯的引脚都在 config/matrix_cfg.h 里配置（由矩阵表配置器
// tools/om62xx-keymap 生成），这里只是按配置展开成读写宏，换脚不用改这里。
//
#include "matrix_cfg.h"

#define MATRIX_RAW              ((uint8_t)MATRIX_ROW_GATHER(~(OM_GPIO0->DATA)))   // 读行线，按下为 1

#define mDELAY_R                delayus(40)
#define mDELAY_EX               delayus(40)
//----------------------------------------------
// 扩展列：由扩展芯片 PA5 控制，每翻转一次 KEY_EX 换到下一列（顺序见 matrix_cfg.h）
//
#if (MATRIX_EX_NUM > 0)
#define KEY_EX_LOW              {OM_GPIO0->DATAOUTCLR = (1u<<MATRIX_EX_PIN);}
#define KEY_EX_HIGH             {OM_GPIO0->DATAOUTSET = (1u<<MATRIX_EX_PIN);}
#define KEY_EX_TOGGLE           {OM_GPIO0->DATAOUT   ^= (1u<<MATRIX_EX_PIN);}
#else
#define KEY_EX_LOW              {}
#define KEY_EX_HIGH             {}
#define KEY_EX_TOGGLE           {}
#endif
//----------------------------------------------
// 直连列线
//
#define MATRX_ALL_LOW           {OM_GPIO0->DATAOUTCLR = MATRIX_DIRECT_MASK; OM_GPIO0->OUTENSET = MATRIX_DIRECT_MASK;}
#define MATRX_ALL_IN            {OM_GPIO0->OUTENCLR = MATRIX_DIRECT_MASK;}
#define MATRX_ALL_OUT           {OM_GPIO0->OUTENSET = MATRIX_DIRECT_MASK;}
//
//----------------------------------------------
// LED
//
#define ARGB_OUT_PIN            BOARD_ARGB_DATA_PIN     // 灯的引脚在 board_config.h 里改
#define ARGB_OFF_PIN            BOARD_LIGHT_PWR_PIN

// 指示灯：引脚和点亮电平在 matrix_cfg.h 里配置；LOW = 点亮，HIGH = 熄灭（沿用原来的名字）
#define mLED_VDD_DIS            MATRIX_LED_VDD_DIS
#define mLED_VDD_EN             MATRIX_LED_VDD_EN
#define mLED_NUM_LOW            MATRIX_LED_NUM_ON
#define mLED_NUM_HIGH           MATRIX_LED_NUM_OFF
#define mLED_CAPS_LOW           MATRIX_LED_CAPS_ON
#define mLED_CAPS_HIGH          MATRIX_LED_CAPS_OFF
#define mLED_LOW_LOW            MATRIX_LED_LOW_ON
#define mLED_LOW_HIGH           MATRIX_LED_LOW_OFF
//
//----------------------------------------------
// 电源其他
//
#define CHR_OK_PIN              (OM_GPIO0->DATA & (1<<(3*8+4)))
#define USB_5V_PIN              (OM_GPIO0->DATA & (1<<(3*8+6)))
#define LVD_PIN_MASK             (1<<(3*8+3))

#define MODE_PIN                (3*8+5)
#define MODE_DATA               (OM_GPIO0->DATA & (1<<MODE_PIN))

#define mMODE_PULL_UP            OM_PMU->PAD_PU_CTRL_1 |= (2<<((MODE_PIN-16)*2));\
                                 OM_PMU->PAD_PD_CTRL &= ~(1<<(MODE_PIN))
#define mMODE_PULL_DOWN          OM_PMU->PAD_PD_CTRL |= (1<<(MODE_PIN));\
                                 OM_PMU->PAD_PU_CTRL_1 &= ~(3<<((MODE_PIN-16)*2))

#define USB_DN_PIN              (0*8+2)
#define USB_DP_PIN              (0*8+3)
//
//----------------------------------------------
// 调试IO
//
#define TIMER_PIN               (2*8+5)

#define CE_PIN                  (2*8+5)
#define IRQ_PIN                 (2*8+5)

#define UART_PIN                (2*8+2)
//
//----------------------------------------------------------------------------;

//----------------------------------------------------------------------------;
// 项目ID定义区 项目立项的时候分配的项目ID
//
#define iPROJECT_ID_H           0x073
#define iPROJECT_ID_L           0x0006

#define iPROJECT_ID_0           (iPROJECT_ID_L&0xFF)
#define iPROJECT_ID_1           ((iPROJECT_ID_L>>8)&0xFF)
#define iPROJECT_ID_2           (iPROJECT_ID_H&0xFF)
#define iPROJECT_ID_3           ((iPROJECT_ID_H>>8)&0xFF)

//----------------------------------------------------------------------------;
// 程序版本定义区 程序版本升级编号
//
#define iVERSION_ID_L           (0x80|RF_RATE|RF_PROTOCOL|0x01)
#define iVERSION_ID_H           iPROJECT_ID_0

#define iUSB_ID_H               '0',0,'1',0
#define iAPP_ID_H               0x12
//
//-----------------------------------------------------------------------------

#define gpio_toggle(pin)        OM_GPIO0->DATAOUT ^= (1u << pin)
#define mMS_TO_TICK(ms)         ((ms)<<5)
#define mSEN_SCAN_US(us)        ((us/ 250) - 1)

//-----------------------------------------------------------------------------;
// 唤醒配置
//
#define WAKEUP_BTN_MASK        0x01
#define WAKEUP_SENSOR_MASK     0x02
#define WAKEUP_WHEEL_MASK      0x04
#define WAKEUP_ALL_ENABLE      0x07
#define WAKEUP_ALL_DISABLE     0x00
#define WAKEUP_SLEEP_LEVEL0    (WAKEUP_ALL_DISABLE)
#define WAKEUP_SLEEP_LEVEL1    (WAKEUP_ALL_ENABLE)
#define WAKEUP_SLEEP_LEVEL2    (WAKEUP_BTN_MASK)

/**
 * @brief  Wireless Mouse RF Modes.
 */
typedef enum {
    iDEVICE_TYPE_BLE  = 1U,   /*<<< EX2_BLE mode */
    iDEVICE_TYPE_BT0  = 0U,   /*<<< EX2_BLE mode */
    iDEVICE_TYPE_BT1  = 1U,   /*<<< EX2_BLE mode */
    iDEVICE_TYPE_BT2  = 2U,   /*<<< EX2_BLE mode */
    iDEVICE_TYPE_24G  = 3U,   /*<<< OnMicro 2.4G proprietary protocol */
    iDEVICE_TYPE_USB  = 4U,
    iDEVICE_TYPE_ERR  = 5U,
} device_type_t;

typedef enum {
    iRF_DATE_RATE_2M  = 0U,   /*<<< EX2_BLE mode */
    iRF_DATE_RATE_2M_SRRC  = 1U,   /*<<< EX2_BLE mode */
    iRF_DATE_RATE_1M  = 2U,   /*<<< EX2_BLE mode */
} rf_rate_t;


//-----------------------------------------------------------------------------;
// 外部引用寄存器
//
extern rf_rate_t                brf_rate;
extern bool                     bmode_swtich_valid;

extern volatile uint8_t         ble_24g_ctrl;
//{
    #define bom6239_sync_ok (1<<3)              // 连接标志 0:没有连接;1：连接上
    #define bom6239_sync_ing (1<<2)             // 同步状态 0:没有回连；1：同步中
    #define bom6239_conn_ok (1<<1)              // 对码状态
    #define bom6239_conning (1<<0)              // 对码状态
//}

extern volatile unsigned char   sys_ctrl;
//{
    #define bmcu_reset      (1<<3)
    #define bscan_flag      (1<<2)
//}
extern uint8_t                  sys_type;
//{
    #define iSYS_IOS            3
    #define iSYS_AND            2
    #define iSYS_MAC            1
    #define iSYS_WIN            0
//}
extern device_type_t            device_type;
extern device_type_t            bt_index;

extern volatile uint32_t        sys_tick_count;
extern uint32_t                 rtc_init_value;
extern uint32_t                 rtc_cur_value;
extern uint32_t                 rtc_temp_value;

extern uint8_t                  wheel_data;
extern uint8_t                  m_tx_count;
extern uint8_t                  key_tx_count;
extern uint8_t                  key_ex_tx_count;
extern uint8_t                  mmk_tx_count;
extern uint8_t                  def_tx_count;

//----------------------------------------------
// 鼠标键盘数据寄存器
//
extern uint8_t                  m_data_flag;
//{
    #define bhid_data_send      (1<<4)
    #define bstd_ex_data_send   (1<<3)
    #define bmmk_data_send      (1<<2)
    #define bstd_kb_data_send   (1<<1)
    #define bmouse_data_send    (1<<0)
//}
extern uint8_t                  hint_led_status;
//{
    #define bled_hint_mode      (1<<7)
    #define bled_low            (1<<4)

    #define bled_24g            (1<<2)
    #define bled_bt1            (1<<1)
    #define bled_bt0            (1<<0)
//}

extern uint8_t                  kb_led_status;
//{
    #define bscroll_lock        (1<<2)
    #define bcaps_lock          (1<<1)
    #define bnum_lock           (1<<0)
//}

extern uint8_t                  mouse_data_key;
extern uint16_t                 mouse_x;
extern uint16_t                 mouse_y;
extern uint8_t                  mouse_data_z;
// 键盘寄存器
extern unsigned char            std_key[];
#define std_kb_data             std_key           // 按键数据
//{
    #define std_kb_data_modifier std_key[0]
    #define std_kb_data_key1    std_key[1]
    #define std_kb_data_key2    std_key[2]
    #define std_kb_data_key3    std_key[3]
    #define std_kb_data_key4    std_key[4]
    #define std_kb_data_key5    std_key[5]
    #define std_kb_data_key6    std_key[6]
//}
// 多媒体寄存器
extern uint8_t                  mmk_data_l;
extern uint8_t                  mmk_data_h;
extern uint8_t                  kb_bd_addr[];

extern uint32_t const           ble_sleep_1_time_tbl[];
extern uint32_t const           ble_sleep_2_time_tbl[];

//----------------------------------------------------------------------------;
// 通用类型定义
//
extern void mouse_periph_wakeup_enable(uint8_t mask);


#endif  /* __MOUSE_H */
