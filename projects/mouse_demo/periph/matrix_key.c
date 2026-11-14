/*******************************************************************************
* Copyright (c) 2012, SHENZHEN Co.,LTD.
* All rights reserved.
*
* 文件名称: matrix_key.c
* 文件描述: 矩阵处理程序
*
*******************************************************************************/
#include <string.h>
#include "om_driver.h"
#include "main.h"
#include "mouse.h"
#include "main_24g.h"

#include "matrix_key.h"
#include "hid_process.h"
#include "usb_cfg_info.h"
#include "flash_process.h"
#include "key_code_define.h"
#include "periph.h"
#include "io_config.h"

//----------------------------------------------------------------------------;
// 矩阵表配置器（config/matrix_cfg.h）和引脚配置器（periph/io_config.h）各管一半：
// 前者管扫描哪些脚、键在哪，后者管这些脚的上拉、唤醒、方向。两边对不上时编译报错，
// 按提示在对应的配置器里改好、重新生成即可。
//
#if (MATRIX_ROW_MASK != IO_KEY_ROW_MASK)
#error "矩阵行线不一致：matrix_cfg.h 的 MATRIX_ROW_MASK 和 io_config.h 的 IO_KEY_ROW_MASK 不同，两个配置器里的行线要设成同样的脚，再重新生成"
#endif
#if ((MATRIX_ROW_MASK & IO_PULLUP_MASK) != MATRIX_ROW_MASK)
#error "矩阵行线没有全部开上拉：在引脚配置器里给行线设上拉，重新生成 io_config.h"
#endif
#if ((MATRIX_ROW_MASK & IO_WAKEUP_FALL_MASK) != MATRIX_ROW_MASK)
#error "矩阵行线没有全部设成下降沿唤醒，休眠后按键唤不醒：在引脚配置器里设好，重新生成 io_config.h"
#endif
#if (MATRIX_DIRECT_MASK != IO_KEY_COL_MASK)
#error "矩阵直连列不一致：matrix_cfg.h 的 MATRIX_DIRECT_MASK 和 io_config.h 的 IO_KEY_COL_MASK 不同，两个配置器里的列线要设成同样的脚，再重新生成"
#endif
#if ((MATRIX_DIRECT_MASK & IO_PULLUP_MASK) != MATRIX_DIRECT_MASK)
#error "矩阵直连列没有全部开上拉（扫完放开后靠上拉回到高电平）：在引脚配置器里给列线设上拉，重新生成 io_config.h"
#endif
#if (MATRIX_EX_NUM > 0)
#if (MATRIX_EX_PIN < 0)
#error "用了扩展列但没有扩展列控制脚（KEY_EX）：在矩阵表配置器的「矩阵硬件」页设好，重新生成 matrix_cfg.h"
#elif ((IO_OUTPUT_MASK & (1u << MATRIX_EX_PIN)) == 0)
#error "扩展列控制脚（KEY_EX）在引脚配置器里不是输出：设成 GPIO 输出，重新生成 io_config.h"
#elif !defined(IO_KEY_EX_PIN) || (IO_KEY_EX_PIN != MATRIX_EX_PIN)
#error "扩展列控制脚（KEY_EX）两边不是同一个脚：引脚配置器里要把 MATRIX_EX_PIN 这个脚设成 GPIO 输出、标签 KEY_EX，重新生成 io_config.h"
#endif
#endif
#if (MATRIX_LED_VDD_PIN >= 0)
#if ((IO_OUTPUT_MASK & (1u << MATRIX_LED_VDD_PIN)) == 0)
#error "LED 供电脚在引脚配置器里不是输出：设成 GPIO 输出，重新生成 io_config.h"
#elif !defined(IO_LED_VDD_PIN) || (IO_LED_VDD_PIN != MATRIX_LED_VDD_PIN)
#error "LED 供电脚两边不是同一个脚：引脚配置器里要把 MATRIX_LED_VDD_PIN 这个脚设成 GPIO 输出、标签 LED_VDD，重新生成 io_config.h"
#elif (((IO_DATAOUT_VAL >> MATRIX_LED_VDD_PIN) & 1u) == MATRIX_LED_VDD_ON)
#error "LED 供电脚上电时是开的，开机时指示灯会亮一下：在引脚配置器里把它的初始电平设成关电的电平，重新生成 io_config.h"
#endif
#endif
// 独立接线（不和列线复用）的指示灯：引脚配置器里要设成 GPIO 输出、标签 LED_NUM / LED_CAPS / LED_LOW，初始电平是灭灯
#if (MATRIX_LED_NUM_DEDICATED)
#if !defined(IO_LED_NUM_PIN) || (IO_LED_NUM_PIN != MATRIX_LED_NUM_PIN) || ((IO_OUTPUT_MASK & (1u << MATRIX_LED_NUM_PIN)) == 0)
#error "Num Lock 灯接在独立引脚上：引脚配置器里要把 MATRIX_LED_NUM_PIN 这个脚设成 GPIO 输出、标签 LED_NUM，重新生成 io_config.h"
#elif (((IO_DATAOUT_VAL >> MATRIX_LED_NUM_PIN) & 1u) == MATRIX_LED_NUM_LEVEL)
#error "Num Lock 灯上电时是亮的：在引脚配置器里把初始电平设成灭灯的电平，重新生成 io_config.h"
#endif
#endif
#if (MATRIX_LED_CAPS_DEDICATED)
#if !defined(IO_LED_CAPS_PIN) || (IO_LED_CAPS_PIN != MATRIX_LED_CAPS_PIN) || ((IO_OUTPUT_MASK & (1u << MATRIX_LED_CAPS_PIN)) == 0)
#error "Caps Lock 灯接在独立引脚上：引脚配置器里要把 MATRIX_LED_CAPS_PIN 这个脚设成 GPIO 输出、标签 LED_CAPS，重新生成 io_config.h"
#elif (((IO_DATAOUT_VAL >> MATRIX_LED_CAPS_PIN) & 1u) == MATRIX_LED_CAPS_LEVEL)
#error "Caps Lock 灯上电时是亮的：在引脚配置器里把初始电平设成灭灯的电平，重新生成 io_config.h"
#endif
#endif
#if (MATRIX_LED_LOW_DEDICATED)
#if !defined(IO_LED_LOW_PIN) || (IO_LED_LOW_PIN != MATRIX_LED_LOW_PIN) || ((IO_OUTPUT_MASK & (1u << MATRIX_LED_LOW_PIN)) == 0)
#error "低电灯接在独立引脚上：引脚配置器里要把 MATRIX_LED_LOW_PIN 这个脚设成 GPIO 输出、标签 LED_LOW，重新生成 io_config.h"
#elif (((IO_DATAOUT_VAL >> MATRIX_LED_LOW_PIN) & 1u) == MATRIX_LED_LOW_LEVEL)
#error "低电灯上电时是亮的：在引脚配置器里把初始电平设成灭灯的电平，重新生成 io_config.h"
#endif
#endif

//----------------------------------------------------------------------------;
//常量定义
//
extern void key_decode(void);

//----------------------------------------------------------------------------;
//变量定义
//
uint16_t                        key_ctrl;
uint16_t                        key_ctrl_ex;

uint8_t                         key_temp_flag;
uint8_t                         key_temp_flag_1;
uint8_t                         ukey_temp_delay;
uint8_t                         device_key;
uint8_t                         device_key_bak;

//unsigned char                   std_key_count;  // 按下按键的个数
uint8_t                         matrix_status[iMATRIX_COL_NUM];
uint8_t                         key_fn_status[128];

uint8_t                         gKeyScan_bak[iMATRIX_COL_NUM];
uint8_t                         device_type_bak;

uint8_t                         decode_index;
uint8_t                         col_index;

uint8_t                         ctrl_key_value;
uint8_t                         std_key_value;
uint8_t                         com_ctrl_index;

uint8_t                         desk_mac_index;

//******************************************************************************
// 查表获取 数值中1的个数
unsigned char const get_1_count[256] =
{
    0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

uint8_t const key_delay_tbl[4] = {
    0x20,
    0x40,
    0x80,
    0xFE,
};

uint8_t const desk_mac[] = {
//----------------------------------------------0x110
    (iLEFT_GUI&0x0F),           iSTD_NULL,  iSTD_NULL,  iSTD_NULL, 
    (iLEFT_GUI|iLEFT_ALT)&0x0F, iSTD_NULL,  iSTD_NULL,  iSTD_NULL, 
    (iLEFT_GUI|iLEFT_ALT)&0x0F, iSTD_H,     iSTD_NULL,  iSTD_NULL, 
    (iLEFT_GUI|iLEFT_ALT)&0x0F, iSTD_H,     iSTD_M,     iSTD_NULL, 
    (iLEFT_GUI|iLEFT_ALT)&0x0F, iSTD_H,     iSTD_NULL,  iSTD_NULL, 
    (iLEFT_GUI|iLEFT_ALT)&0x0F, iSTD_NULL,  iSTD_NULL,  iSTD_NULL, 
    0x00,                       iSTD_NULL,  iSTD_NULL,  iSTD_NULL, 
};

/******************************************************************************
* 函数名称: void key_process(void)
* 功能描述:
* 输入参数:
* 输出参数:
* 说    明:
******************************************************************************/
void __attribute__((noinline)) key_process(void)
{
    uint8_t i;

    if(0 != (key_ctrl&bkey_lock_status))
    {
        multi_data_tmp_l = 0;
        multi_data_tmp_l = 0;
        memset(key_data_tmp, 0x00, 22);
        key_ctrl_ex &= ~bcom_ctrl_en;
    }

#if (!MATRIX_MAC_KEYMAP)                        // Win / Mac 共用键表时，Mac 模式下把 Alt 和 Win 对调；有 Mac 独立键表就不对调
    if(0==(key_ctrl&bwin_mac_sataus))
    {                                           // mac系统，按键交换
        uint8_t temp_bit = modifier_key_tmp;
        modifier_key_tmp &= 0x33;
        if(temp_bit&(1<<7))
            modifier_key_tmp |= 0x40;
        if(temp_bit&(1<<6))
            modifier_key_tmp |= 0x80;
        if(temp_bit&(1<<2))
            modifier_key_tmp |= 0x08;
        if(temp_bit&(1<<3))
            modifier_key_tmp |= 0x04;
    }
#endif
    if(key_ctrl_ex&bcom_ctrl_en)
	{
	    if(10 > com_ctrl_index)
		{
		    tmp_std_key[0] |= ctrl_key_value;
		}
		else if(20 > com_ctrl_index)
		{
		    tmp_std_key[0] |= ctrl_key_value;
            tmp_std_key[1] = std_key_value;
		}
		else if(250 > com_ctrl_index)
		{
		    tmp_std_key[0] |= ctrl_key_value;
            if(key_ctrl_ex&bcom_ctrl_long)
                tmp_std_key[1] = std_key_value;
            else
                tmp_std_key[1] = 0;
		}
        else if(250 <= com_ctrl_index)
		{
		    tmp_std_key[0] |= ctrl_key_value;
            tmp_std_key[1] = 0;
		    if(key_ctrl_ex&bcom_ctrl_status)
				com_ctrl_index = 0;
			else
		        key_ctrl_ex &= ~bcom_ctrl_en;
		}

		com_ctrl_index++;
        if(iDEVICE_TYPE_BT2 >= device_type)
            com_ctrl_index++;
	}

    if(0 != (key_ctrl_ex&bdesk_mac))
    {
        if(0==(desk_mac_index&0x03))
        {
            i = desk_mac_index;

            tmp_std_key[0] = desk_mac[i++];
            tmp_std_key[1] = desk_mac[i++];
            tmp_std_key[2] = desk_mac[i++];
        }

        desk_mac_index++;
        if(sizeof(desk_mac) <= desk_mac_index)
            key_ctrl_ex &= ~bdesk_mac;
    }

    for(i=0; i<6; i++)
    {
        if(key_data_tmp[i] != std_key[i])
        {
            memcpy(std_key, tmp_std_key, 6);

            key_tx_count = 8;
            m_data_flag |= bstd_kb_data_send;
            return;
        }
    }

    for(i=7; i<22; i++)
    {
        if(tmp_std_key[i] != std_key[i])
        {
            memcpy(std_key+7, tmp_std_key+7, 15);
            key_ex_tx_count = 8;
            m_data_flag |= bstd_ex_data_send;
            return;
        }
    }

    if(multi_data_tmp_l != mmk_data_l)
    {
        if(0==(m_data_flag & bmmk_data_send))
        {
            mmk_data_l = multi_data_tmp_l;
            mmk_data_h = multi_data_tmp_h;
            m_data_flag |= bmmk_data_send;
            mmk_tx_count = 8;
        }
    }
}

/******************************************************************************
* 函数名称: mode_swtich_init
* 功能描述:
* 输入参数:
* 输出参数:
* 说    明:
******************************************************************************/
void mode_swtich_init(void)
{
	memset(std_key, 0x00, 22);

    mmk_data_l = 0;
    mmk_data_h = 0;

    m_data_flag |= (bmouse_data_send|bstd_kb_data_send|bmmk_data_send);
}

/******************************************************************************
* 函数名称: matrixkey_value
* 功能描述: 矩阵按键扫描
* 输入参数:
* 输出参数:
* 说    明: 为提高时间效率,赋值等操作不采用循环模式
******************************************************************************/
__RAM_CODE void matrixkey_value(void)
{
    unsigned char i, x;

    special_key = 0;

    modifier_key_tmp = 0;
    multi_data_tmp_l = 0;
    multi_data_tmp_h = 0;

    key_ctrl &= ~bkey_press_status;

    decode_index = 0;
    for(col_index=3; col_index<19; col_index++)
    {
        key_temp_flag_1 = (gKeyScan_bak[col_index] ^ matrix_status[col_index]);
        gKeyScan_bak[col_index] = matrix_status[col_index];

        key_temp_flag = gKeyScan_bak[col_index];
// 0
        if(0!=key_temp_flag)
            key_ctrl |= bkey_press_status;

        x=0x01;
        for(i=0; i<iMATRIX_ROW_NUM; i++)
        {
            if(0 == (define_key_flag_n&bkey_decode))
            {
                if(key_temp_flag_1&x)
                {                               // 按键有变化
                    goto define_key_process;
                }
                else
                {
			        if(decode_index == com_key_index)
				        com_key_index = 0xFF;
                }
            }
            else
            {
                if(key_temp_flag_1&x)
                {                               // 有变化
define_key_process:
                    if(key_temp_flag&x)
                    {
                        define_key_flag_n = (bdefine_key_press|bkey_decode);
                        key_ctrl |= bkey_press_status;

                        if(define_key_ctrl & bcom_key_break)
                        {                       // 按键可打断
                            define_key_ctrl &= ~bcom_key_break;
                            define_key_status |= bkey_status_change;
                        }
                    }
                    else
                    {
                        define_key_flag_n &= ~bdefine_key_press;
                    }
                }

                key_decode();
                if(key_ctrl&bkey_decode_valid)
                {
                    if((define_key_flag&bkey_decode_end)
			    	    && (0==(define_key_flag&bdefine_key_press)))
                    {
                        define_key_flag = 0;
                    }
                    define_key_flag_n = define_key_flag;
                    if(define_key_flag&bdefine_key_com)
                    {
                        decode_data_l_n = decode_data_l;
                        decode_data_h_n = decode_data_h;
                    }
                }
                if(0==(key_temp_flag&x))
			        key_fn_status[decode_index] = iNULL_MATRIX;
            }
            
            x<<=1;
            decode_index++;
        }
    }

    if(bkey_break_or_realse)
    {
        bkey_break_or_realse = 0;

	    memset(key_data_tmp, 0x00, 22);
    }
	else
	{
	    if((0==(key_ctrl&bkey_press_status)) && (0 == (define_key_ctrl&bcom_decode_ing)))
		{
            multi_data_tmp_l = 0;
            multi_data_tmp_l = 0;
	        memset(key_data_tmp, 0x00, 22);
		}
	}

// 按键去抖处理_END
    device_key_bak &= device_key;
    special_key_bak &= special_key;

    if(special_key&bfn_press)
    {
        key_ctrl |= bfn_key_status;
    }
    else
    {
        if(key_ctrl&bfn_key_status)
        {
            key_ctrl &= ~bfn_key_status;
            if(key_ctrl&bfn_save_status)
            {
                key_ctrl &= ~bfn_save_status;
                bsave_status = true;
            }

            if(key_ctrl&bfn_save_cfg)
            {
                key_ctrl &= ~bfn_save_cfg;
                bsave_cfg_dpi = true;
            }
        }
    }
}

//----------------------------------------------------------------------------;
// 矩阵配置（config/matrix_cfg.h，由矩阵表配置器生成）
//
static const uint8_t matrix_direct_pin[] = MATRIX_DIRECT_PINS;
static const uint8_t matrix_slot_map[MATRIX_RAW_NUM*8] = MATRIX_SLOT_MAP;   // 物理位置 -> 键表位置

uint8_t                         matrix_raw[MATRIX_RAW_NUM];                 // 按扫描顺序存的原始行值

#define  DELAY

#if (MATRIX_EX_NUM > 0)
__RAM_CODE void ctrl_54e_inpull(void)
{
    OM_GPIO0->OUTENSET = (1u<<MATRIX_EX_PIN);
    OM_CRITICAL_BEGIN();
    KEY_EX_LOW;
    delayus(1);
    KEY_EX_HIGH;
    OM_CRITICAL_END();
}
#endif

/******************************************************************************
* 函数名称: scan_key
* 功能描述: 扫一遍矩阵，结果按扫描顺序存进 matrix_raw[]
* 说    明: 先扫直连列（逐个拉低），再扫扩展列（每翻转一次 KEY_EX 换一列）
******************************************************************************/
void scan_key(void)
{
    uint8_t i;
    uint32_t mask;

    mLED_VDD_DIS;                               // 指示灯和列线复用：扫描时关掉供电
    MATRX_ALL_IN;
#if (MATRIX_EX_NUM > 0)
    ctrl_54e_inpull();
#endif
    delayus(20);

    for(i=0; i<MATRIX_DIRECT_NUM; i++)
    {
        mask = (1u<<matrix_direct_pin[i]);
        OM_GPIO0->DATAOUTCLR = mask;            // 拉低这一列
        OM_GPIO0->OUTENSET = mask;
        mDELAY_R;
        matrix_raw[i] = MATRIX_RAW;
        OM_GPIO0->DATAOUTSET = mask;            // 放开
        OM_GPIO0->OUTENCLR = mask;
    }
    mDELAY_R;

#if (MATRIX_EX_NUM > 0)
    KEY_EX_TOGGLE;
    mDELAY_EX;
    for(i=0; i<MATRIX_EX_NUM; i++)
    {
        mDELAY_EX;
        matrix_raw[MATRIX_DIRECT_NUM + i] = MATRIX_RAW;
        KEY_EX_TOGGLE;
    }
    KEY_EX_HIGH;
#endif
}

/******************************************************************************
* 函数名称: matrix_pack
* 功能描述: 按 MATRIX_SLOT_MAP 把 matrix_raw[] 换成键表顺序，写进 matrix_status[3..18]
* 说    明: 原来 matrixkey_scan() 里手写的位搬移（C0/C1/C2/C19/C18 的键挪到 C3/C5/C6/C16/C17
*           空出来的位置）现在由配置器生成的表完成；matrix_status[0..2] 和 [19] 保持为 0，
*           MATRIX_POS_NONE 指向 [19]，所以没配置的特殊键永远读不到按下。
******************************************************************************/
__RAM_CODE static void matrix_pack(void)
{
    uint8_t i, r, v, s;
    const uint8_t *map;

    memset(matrix_status, 0x00, iMATRIX_COL_NUM);
    for(i=0; i<MATRIX_RAW_NUM; i++)
    {
        v = matrix_raw[i];
        map = &matrix_slot_map[i*8];
        for(r=0; 0!=v; r++, v>>=1)
        {
            if(0 == (v&0x01))
                continue;
            s = map[r];
            if(MATRIX_SLOT_NONE != s)
                matrix_status[(s>>3) + 3] |= (uint8_t)(1u<<(s&0x07));
        }
    }
}

/******************************************************************************
* 函数名称: matrixkey_scan
* 功能描述: 矩阵按键扫描
* 输入参数:
* 输出参数:
* 说    明: 为提高时间效率,赋值等操作不采用循环模式
******************************************************************************/
__RAM_CODE void matrixkey_scan(void)
{
    unsigned char i,j,k,m,key_temp[MATRIX_RAW_NUM];

    device_key = 0;
    special_key = 0;
    key_ctrl_ex &= ~(bgost_key|bcom_ctrl_status);

    modifier_key_tmp = 0;
    multi_data_tmp_l = 0;
    multi_data_tmp_h = 0;

    scan_key();
    if(iDEVICE_TYPE_24G == device_type)
    {
        if(iMCU_MODE_NORMAL == work_mode)
            led_status_hint();
    }
    else
    {
        led_status_hint();
    }

    k = 0;
    for(i=0; i<MATRIX_RAW_NUM; i++)
    {
        if(0 != matrix_raw[i])
        {
            key_temp[k] = matrix_raw[i];
            k++;
        }
    }

    if((0!=k) && (0==(key_ctrl&bkey_press_status)))
    {
        key_ctrl |= bkey_press_status;
    }

    for(j=0; j<k; j++)                          // 鬼键检测，用物理位置判断
    {
        i = key_temp[j];

        if(2 <= get_1_count[i])
        {
            key_temp[j] = 0;
            for(m=0; m<k; m++)
            {
                if(0 != (key_temp[m]&i))
                {
                    key_ctrl_ex |= bgost_key;
                    return;
                }
            }
            key_temp[j] = i;
        }
    }

    matrix_pack();

    matrixkey_value();
    periph_scan_all();                  // 可选外设在这里注入按键（如编码器的音量键）
    key_process();
}

extern void                 hi5_usb_init(void);
extern void                 usb_disconnect(void);

static bool b5v_check_in;
static uint32_t usb_io_status, mode_swtich_time;

extern bool busb_valid;

#define iMODE_24G           0
#define iMODE_BLE0          1
#define iMODE_BLE1          2

/******************************************************************************
* 函数名称: scan_mode_swtich()
* 功能描述: 模式切换
* 输入参数:
* 输出参数: 无
* 说    明: 手动模式切换的时候，usb切换到其他状态，采用复位处理，其他模式切换到USB，采用直接切换
******************************************************************************/
void scan_mode_swtich(uint8_t type)
{
    static uint8_t mode_status;
    uint8_t i,j, mode_status_bak;

//    if(bat_status&bcharging)
//    {
//        if(iMODE_CHECK_POWERUP != type)
//        {
//            if(false == b5v_check_in)
//            {
//                b5v_check_in = true;
//                hi5_usb_init();
//                usb_ctrl_flag |= busb_suspend_resume;
//            }

//            if(0 == (usb_ctrl_flag&busb_suspend_resume))
//            {
//                if(iDEVICE_TYPE_USB != device_type)
//                {
//                    busb_valid = true;
//                    bmode_swtich_valid = true;

//                    device_type = iDEVICE_TYPE_USB;
//                    //flash_save_status();
//                }
//                return;
//            }

//            if(true == busb_valid)
//                return;
//        }
//        else
//        {
//            b5v_check_in = true;
//            hi5_usb_init();
//            usb_ctrl_flag |= busb_suspend_resume;
//            return;
//        }
//    }
//    else
//    {
//        if(true == b5v_check_in)
//        {
//            b5v_check_in = false;
//            busb_valid = false;
//            usb_disconnect();
//        }
//    }

    j = 4;
    mMODE_PULL_DOWN;
    delayus(40);
    if(0 == MODE_DATA)
        j--;
    mMODE_PULL_UP;
    delayus(40);
    if(0 != MODE_DATA)
        j++;
    mMODE_PULL_DOWN;

    if(4 > j)
        mode_status_bak = iMODE_BLE0;
    else if(4 < j)
        mode_status_bak = iMODE_24G;
    else
        mode_status_bak = iMODE_BLE1;

    if(mode_status != mode_status_bak)
    {
        mode_status = mode_status_bak;
        if(iMODE_CHECK_POWERUP != type)
            return;
    }

//mode_status = iMODE_USB;
    if(iMODE_BLE0 == mode_status)
    {
        if(iMODE_CHECK_POWERUP != type)
        {
            if(iDEVICE_TYPE_BT0 != device_type)
                NVIC_SystemReset();
        }
        else
        {
            device_type = iDEVICE_TYPE_BT0;
        }
    }
    else if(iMODE_BLE1 == mode_status)
    {
        if(iMODE_CHECK_POWERUP != type)
        {
            if(iDEVICE_TYPE_BT1 != device_type)
                NVIC_SystemReset();
        }
        else
        {
            device_type = iDEVICE_TYPE_BT1;
        }
    }
    else //if(iMODE_24G == mode_status)
    {                                           // 下拉 24g
        if(iMODE_CHECK_POWERUP != type)
        {
            if(iDEVICE_TYPE_24G != device_type)
                NVIC_SystemReset();
        }
        else
        {
            device_type = iDEVICE_TYPE_24G;
        }
    }
}

