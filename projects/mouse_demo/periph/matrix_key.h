/*******************************************************************************
* Copyright (c) 2013, SHENZHEN SEMITEK Co.,LTD.
* All rights reserved.
*
* 文件名称: matrix_key.h
*
* 功能说明:
* 修改说明:
*******************************************************************************/
#ifndef MATRIX_KEY_H_
#define MATRIX_KEY_H_

#include "matrix_cfg.h"

//=============================================================================;
// 配置引用区                                                                  ;
//=============================================================================;

//-----------------------------------------------------------------------------;
//配置常量定义区
//
#define iMATRIX_COL_NUM         20
#define iMATRIX_ROW_NUM         8
#define iMATRIX_NUM_MAX         iMATRIX_COL_NUM*iMATRIX_ROW_NUM

#define iKEY_DELAY_COUNT        2

//-----------------------------------------------------------------------------;
// 固件按位置读取的键：位置在 config/matrix_cfg.h 里（KEYPOS_xxx，键表位置），
// 用矩阵表配置器的「特殊键位置」页指定，这里换算成原来的写法。
//   KEYPOS_COL(s) / KEYPOS_BIT(s)：matrix_status[] 的下标和位，给 rf_fcc.c 用
//   KEYPOS_INDEX(s)              ：键表里的字节偏移，给 std_decode_callback() 用
// 没指定的位置是 MATRIX_POS_NONE，对应 matrix_status[19] 第 0 位，永远读不到按下。
//
#define KEYPOS_COL(s)           (((s)>>3) + 3)
#define KEYPOS_BIT(s)           (1u<<((s)&0x07))
#define KEYPOS_INDEX(s)         (((s) >= MATRIX_POS_NONE) ? 0xFF : (s)*2)   // 0xFF 不会和任何键匹配

#define iKEY_FN_C               KEYPOS_COL(KEYPOS_FCC_FN)
#define iKEY_FN_R               KEYPOS_BIT(KEYPOS_FCC_FN)

#define iKEY_BT0_C              KEYPOS_COL(KEYPOS_FCC_BT0)
#define iKEY_BT0_R              KEYPOS_BIT(KEYPOS_FCC_BT0)

#define iKEY_24G_C              KEYPOS_COL(KEYPOS_FCC_24G)
#define iKEY_24G_R              KEYPOS_BIT(KEYPOS_FCC_24G)

#define iKEY_INDEX_W            KEYPOS_INDEX(KEYPOS_WASD_W)
#define iKEY_INDEX_A            KEYPOS_INDEX(KEYPOS_WASD_A)
#define iKEY_INDEX_S            KEYPOS_INDEX(KEYPOS_WASD_S)
#define iKEY_INDEX_D            KEYPOS_INDEX(KEYPOS_WASD_D)

#define iKEY_INDEX_UP           KEYPOS_INDEX(KEYPOS_WASD_UP)
#define iKEY_INDEX_LEFT         KEYPOS_INDEX(KEYPOS_WASD_LEFT)
#define iKEY_INDEX_DOWN         KEYPOS_INDEX(KEYPOS_WASD_DOWN)
#define iKEY_INDEX_RIGHT        KEYPOS_INDEX(KEYPOS_WASD_RIGHT)

//-----------------------------------------------------------------------------;
// FCC 测试用的键（rf_fcc.c）
//
#define iKEY_FCC_TX_C           KEYPOS_COL(KEYPOS_FCC_TX)       // 右 切换定频收发
#define iKEY_FCC_TX_R           KEYPOS_BIT(KEYPOS_FCC_TX)

#define iKEY_FCC_FREQ_C         KEYPOS_COL(KEYPOS_FCC_FREQ)     // 左 进入跳频
#define iKEY_FCC_FREQ_R         KEYPOS_BIT(KEYPOS_FCC_FREQ)

#define iKEY_FCC_WAVE_C         KEYPOS_COL(KEYPOS_FCC_WAVE)     // 上 切换空载波
#define iKEY_FCC_WAVE_R         KEYPOS_BIT(KEYPOS_FCC_WAVE)

#define iKEY_SWTICH_FREQ_C      KEYPOS_COL(KEYPOS_FCC_SWITCH)   // 下 切换频点
#define iKEY_SWTICH_FREQ_R      KEYPOS_BIT(KEYPOS_FCC_SWITCH)

//-----------------------------------------------------------------------------;
//配置区
//
#define iMODE_CHECK_POWERUP         0
#define iMODE_CHECK_NO_DELAY        1
#define iMODE_CHECK_DELAY           2
//
//------------------------------------------------------------------------------

//=============================================================================;
// 外部定义内部引用区                                                          ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// 外部寄存器引用
//
extern unsigned char const flash_key_win_tbl[];

//-----------------------------------------------------------------------------;
// 外部函数引用
//
extern uint8_t const key_delay_tbl[4];

//=============================================================================;
// 内部定义外部部引用区                                                        ;
//=============================================================================;
extern uint16_t                 key_ctrl;
//{
    #define bkey_decode_valid   (1<<15)
    #define bdir_wasd_status    (1<<14) // 位置不能更改 ???
    #define bkey_press          (1<<13)
    #define bkey_lock_status    (1<<11) // 位置不能更改 ???
    #define bwin_lock_status    (1<<10) // 位置不能更改 ???
    #define bdir_type_key       (1<<9)
    #define bkey_press_status   (1<<8)

    #define bconn_press_status  (1<<7)
    #define bwin_mac_sataus     (1<<6)  // 位置不能更改 ???
    #define bfn_key_status      (1<<4)

    #define bfn_save_cfg        (1<<3)  // 保存配置：亮度、速度、颜色
    #define bfn_save_status     (1<<2)  // 保存状态：系统模式、WIN LOCK、系统、电量、报告率、LED模式、LED开关
    //  ??? 注意：由于之前数据结构已经确认,保存配置和保存状态一定要注意区分
//}
extern uint16_t                 key_ctrl_ex;
//{
    #define bgost_key           (1<<13)
    #define brgb_off_bak        ukey_ctrl_ex.BIT.b9
    #define brgb_off            ukey_ctrl_ex.BIT.b8

    #define bkey_all_realse     (1<<7)
    #define bdesk_mac           (1<<4)

    #define bcom_ctrl_long      (1<<2)
    #define bcom_ctrl_status    (1<<1)
    #define bcom_ctrl_en        (1<<0)
//}

extern uint8_t                  key_temp_flag;
//{
    #define bkey_temp0          (1<<0)
//}

extern uint16_t                 special_key;
//{
    #define bkey_led_dir        (1<<15)
    #define bled_off            (1<<14)
    #define bwin_lock           (1<<13)
    #define bkey_rgb_cyc        (1<<12)
    #define bkey_speed_up       (1<<11)
    #define bkey_speed_dn       (1<<10)
    #define bkey_light_up       (1<<9) 
    #define bkey_light_dn       (1<<8) 
                                       
    #define bkey_mode_cyc       (1<<7) 
    #define bkey_win_mac        (1<<6) 
    #define bkey_dir_wasd       (1<<5) 
    #define bfn_press           (1<<4)
    #define bdelault_press      (1<<3) 

    #define bkey_lock           (1<<1)
    #define bled_mode_on        (1<<0)
//}

extern uint16_t                 special_key_bak;
//{
    #define bkey_led_dir_bak    (1<<15)
    #define bled_off_back       (1<<14)
    #define bwin_lock_bak       (1<<13)
    #define bkey_rgb_cyc_bak    (1<<12)
    #define bkey_speed_up_bak   (1<<11)
    #define bkey_speed_dn_bak   (1<<10)
    #define bkey_light_up_bak   (1<<9)
    #define bkey_light_dn_bak   (1<<8)

    #define bkey_mode_cyc_bak   (1<<7)
    #define bkey_win_mac_bak    (1<<6)
    #define bkey_dir_wasd_bak   (1<<5)
    #define bfn_press_bak       (1<<4)
    #define bdelault_press_bak  (1<<3) 

    #define bkey_lock_bak       (1<<1)
    #define bled_mode_on_bak    (1<<0)
//}
extern uint8_t                  device_key;
//
    #define bkey_usb_press      (1<<4)
    #define bkey_24g_press      (1<<3)
    #define bkey_bt2_press      (1<<2)
    #define bkey_bt1_press      (1<<1)
    #define bkey_bt0_press      (1<<0)
//}
extern uint8_t                  device_key_bak;
//{	
    #define bkey_usb_press_bak  (1<<4)
    #define bkey_24g_press_bak  (1<<3)
    #define bkey_bt2_press_bak  (1<<2)
    #define bkey_bt1_press_bak  (1<<1)
    #define bkey_bt0_press_bak  (1<<0)
//}
extern unsigned char            decode_data_l_n;
extern unsigned char            decode_data_h_n;

extern unsigned char            matrix_status[];
extern unsigned char            define_key_fifo[];
extern unsigned char            key_fn_status[];
//{
    #define iNULL_MATRIX        0
    #define iNORMAL_MATRIX      1
    #define iFN_MATRIX          2
//}

extern unsigned char            gKeyScan_bak[];
extern unsigned char            device_type_bak;
extern unsigned char            decode_index;

extern unsigned char            multi_data_tmp_l;
extern unsigned char            multi_data_tmp_h;
extern unsigned char            tmp_std_key[22];

#define key_data_tmp            tmp_std_key
#define modifier_key_tmp        tmp_std_key[0]
#define key_data_tmp_1          tmp_std_key[1]
#define key_data_tmp_2          tmp_std_key[2]
#define key_data_tmp_3          tmp_std_key[3]
#define key_data_tmp_4          tmp_std_key[4]
#define key_data_tmp_5          tmp_std_key[5]
#define key_data_tmp_6          tmp_std_key[6]

extern unsigned char            ctrl_key_value;
extern unsigned char            std_key_value;
extern unsigned char            com_ctrl_index;
extern uint8_t                  desk_mac_index;

extern unsigned char    const   matrixkey_win_tbl[];
extern unsigned char    const   matrixkey_win_fn_tbl[];

//-----------------------------------------------------------------------------;
// 内部函数引用
//
extern void matrixkey_scan(void);
extern void scan_mode_swtich(uint8_t type);

extern void mode_swtich_init(void);
//
//-----------------------------------------------------------------------------;
#endif
