/*******************************************************************************
* Copyright (c) 2013, SHENZHEN Co.,LTD.
* All rights reserved.
*
* 文件名称: led.c
*
* 功能说明:
* 修改说明:
*******************************************************************************/
#include <string.h>
#include "om_driver.h"

#include "main.h"
#include "lvd.h"
#include "led.h"

#include "flash.h"
#include "flash_process.h"
#include "usb_cfg_info.h"
#include "matrix_key.h"
#include "periph.h"

//-----------------------------------------------------------------------------;
// 常量定义区
//


//-----------------------------------------------------------------------------;
// 变量定义区
//
// 指示灯状态
bool                            bdevice_led_status;
uint8_t                         device_hint_count;
uint32_t                        led_delay_count;
uint8_t                         hint_time_count;

// 灯效设置：按键、Flash、USB 配置工具都会读写，灯光模块按它们显示，所以放在核心
bool                            bled_bright_en;
uint8_t                         device_conn_index=0;
uint8_t                         led_mode_bak;
uint8_t                         led_cur_mode;
enum speed_level                led_speed_lev;
unsigned char                   led_rgb_index;
enum led_level                  led_light_lev;

extern bool                     b24g_nromal;

//=============================================================================;
// 代码区                                                                       ;
//=============================================================================;

/******************************************************************************
* 函数名称: led_status_hint
* 功能描述:
* 输入参数:
* 输出参数:
* 说    明:
******************************************************************************/
void led_status_hint(void)
{
    uint8_t i=0;

    mLED_VDD_DIS;
	if(hint_led_status&bled_hint_mode)
    {
        if(hint_led_status&bled_bt0)
        {
            i = 1;
            mLED_NUM_LOW;
        }
        else
        {
            mLED_NUM_HIGH;
        }

        if(hint_led_status&bled_bt1)
        {
            i = 1;
            mLED_CAPS_LOW;
        }
        else
        {
            mLED_CAPS_HIGH;
        }

        if(hint_led_status&bled_24g)
        {
            i = 1;
            mLED_LOW_LOW;
        }
        else
        {
            mLED_LOW_HIGH;
        }
    }
    else
    {
        if(kb_led_status&bnum_lock)
        {
            i = 1;
            mLED_NUM_LOW;
        }
        else
        {
            mLED_NUM_HIGH;
        }

        if(kb_led_status&bcaps_lock)
        {
            i = 1;
            mLED_CAPS_LOW;
        }
        else
        {
            mLED_CAPS_HIGH;
        }

        if(hint_led_status&bled_low)
        {
            i = 1;
            mLED_LOW_LOW;
        }
        else
        {
            mLED_LOW_HIGH;
        }

        if(0==i)
            return;
    }
    mLED_VDD_EN;
}

/*******************************************************************************
* 函数名称: device_led_on()
* 功能描述:
* 输入参数： 无
* 输出参数： 无
* 说    明:
*******************************************************************************/
void device_led_on(void)
{
    bdevice_led_status = true;

    hint_led_status &= 0xF8;
    if(iDEVICE_TYPE_BT0 == device_type)
        hint_led_status |= (bled_bt0|bled_hint_mode);
    else if(iDEVICE_TYPE_BT1 == device_type)
        hint_led_status |= (bled_bt1|bled_hint_mode);
    else if(iDEVICE_TYPE_24G == device_type)
        hint_led_status |= (bled_24g|bled_hint_mode);
}

/*******************************************************************************
* 函数名称: device_led_off()
* 功能描述:
* 输入参数： 无
* 输出参数： 无
* 说    明:
*******************************************************************************/
void device_led_off(void)
{
    bdevice_led_status = false;

    hint_led_status &= 0xF8;
}

extern void key_into_fcc_test(void);

/******************************************************************************
* 函数名称: device_led_process
* 功能描述:
* 输入参数:
* 输出参数:
* 说    明:
******************************************************************************/
void device_led_process(void)
{
//-----------------------------------------------------------------------------
// 模式提示
//
    if(0 != hint_time_count)
    {
        hint_led_status |= bled_hint_mode;
        if(0 != (hint_time_count&0x08))
        {
            bdevice_led_status = true;
            hint_led_status |= (bled_hint_mode|bled_24g|bled_bt1|bled_bt0);
        }
        else
        {
            device_led_off();
        }

        hint_time_count--;
        if(2>hint_time_count)
        {
            hint_time_count = 0;
            hint_led_status = 0x00;
        }

        return;
    }

    rtc_cur_value = drv_rtc_read(OM_RTC);
    if(iDEVICE_TYPE_USB != device_type)
    {
        if(ble_24g_ctrl & bom6239_conning)
        {
            hint_led_status |= bled_hint_mode;
            if(0 == device_conn_index)
            {
                device_conn_index++;
                led_delay_count = rtc_cur_value;
            }
            else if(1 == device_conn_index)
            {
                if(ble_24g_ctrl&bom6239_conn_ok)
                {
                    device_conn_index++;
                    led_delay_count = rtc_cur_value;
                    device_hint_count = 0;
                }

                key_into_fcc_test();
                if(false == bdevice_led_status)
                {
                    device_led_off();
                    if(mMS_TO_TICK(250) < (rtc_cur_value - led_delay_count))
                    {
                        bdevice_led_status = true;
                        led_delay_count = rtc_cur_value;
                    }
                }
                else
                {
                    device_led_on();
                    if(mMS_TO_TICK(250) < (rtc_cur_value - led_delay_count))
                    {
                        bdevice_led_status = false;
                        led_delay_count = rtc_cur_value;
                    }
                }
            }
            else if(2 == device_conn_index)
            {
                if(false == bdevice_led_status)
                {
                    device_led_off();
                    if(mMS_TO_TICK(125) < (rtc_cur_value - led_delay_count))
                    {
                        bdevice_led_status = true;
                        led_delay_count = rtc_cur_value;

                        device_hint_count++;
                        if(3<device_hint_count)
                        {
                            hint_led_status &= ~bled_hint_mode;
                            device_conn_index = 0;
                            ble_24g_ctrl &= ~bom6239_conning;
                        }
                    }
                }
                else
                {
                    device_led_on();
                    if(mMS_TO_TICK(125) < (rtc_cur_value - led_delay_count))
                    {
                        bdevice_led_status = false;
                        led_delay_count = rtc_cur_value;
                    }
                }
            }
            return;
        }
        else if(ble_24g_ctrl&bom6239_sync_ing)
        {
            if(true == b24g_nromal)
                return;

            hint_led_status |= bled_hint_mode;
            if(0 == device_conn_index)
            {
                device_conn_index++;
                led_delay_count = rtc_cur_value;
            }
            else if(1 == device_conn_index)
            {
                if(ble_24g_ctrl&bom6239_sync_ok)
                {
                    device_conn_index++;
                    led_delay_count = rtc_cur_value;
                    device_hint_count = 0;
                }

                if(false == bdevice_led_status)
                {
                    device_led_off();
                    if(mMS_TO_TICK(1000) < (rtc_cur_value - led_delay_count))
                    {
                        bdevice_led_status = true;
                        led_delay_count = rtc_cur_value;
                    }
                }
                else
                {
                    device_led_on();
                    if(mMS_TO_TICK(1000) < (rtc_cur_value - led_delay_count))
                    {
                        bdevice_led_status = false;
                        led_delay_count = rtc_cur_value;
                    }
                }
            }
            else if(2 == device_conn_index)
            {
                if(false == bdevice_led_status)
                {
                    device_led_off();
                    if(mMS_TO_TICK(125) < (rtc_cur_value - led_delay_count))
                    {
                        bdevice_led_status = true;
                        led_delay_count = rtc_cur_value;

                        device_hint_count++;
                        if(3<device_hint_count)
                        {
                            device_conn_index = 0;
                            ble_24g_ctrl &= ~bom6239_sync_ing;
                        }
                    }
                }
                else
                {
                    device_led_on();
                    if(mMS_TO_TICK(125) < (rtc_cur_value - led_delay_count))
                    {
                        bdevice_led_status = false;
                        led_delay_count = rtc_cur_value;
                    }
                }
            }
            return;
        }
        hint_led_status &= ~bled_hint_mode;
        device_conn_index = 0;
    }
    else
    {
        hint_led_status &= ~bled_hint_mode;
        if(0 != (usb_ctrl_flag&busb_suspend_resume))
            return;
    }
}

/******************************************************************************
* 函数名称: ui_frame_process
* 功能描述: 界面刷新，各模式主循环约每 7~16ms 调用一次
* 说    明: 先保存按键 / 配置工具改过的设置，再让可选外设（灯光等）刷新一帧。
*           保存设置原来放在 logo_process() 开头，挪到这里后，去掉灯光模块也不影响保存。
******************************************************************************/
void ui_frame_process(void)
{
    flash_save_pending();
    periph_frame_all();
}
