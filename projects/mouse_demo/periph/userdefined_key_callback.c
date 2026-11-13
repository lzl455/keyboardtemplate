/*******************************************************************************
* Copyright (c) 2013, SHENZHEN SEMITEK Co.,LTD.
* All rights reserved.
*
* 文件名称: UserDefined_Key.c
*
* 功能说明:
* 修改说明:
*******************************************************************************/
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "om_driver.h"

#include "main.h"
#include "main_24g.h"

#include "hid_process.h"
#include "userdefined_key.h"
#include "key_code_define.h"
#include "flash_process.h"

#include "matrix_key.h"

//-----------------------------------------------------------------------------;
//常量定义区
//

//----------------------------------------------------------------------------
//变量定义区
//
uint32_t                        press_bt0_time_count;
uint32_t                        press_bt1_time_count;
uint32_t                        press_bt2_time_count;
uint32_t                        press_24g_time_count;
uint32_t                        default_time_count;

//-----------------------------------------------------------------------------
//表格定义区
//

//=============================================================================;
// 代码区                                                                      ;
//=============================================================================;
extern bool flash_read(unsigned int addr, unsigned int length);
extern void laod_key_cfg(void);

/******************************************************************************
* 函数名称: fn_decode_callback()
* 功能描述:
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
__RAM_CODE void fn_decode_callback(unsigned char *key_index)
{

}

/******************************************************************************
* 函数名称: std_decode_callback()
* 功能描述:
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
__RAM_CODE void std_decode_callback(unsigned char *key_index)
{
    if(key_ctrl&bdir_wasd_status)
    {
        if(iKEY_INDEX_UP == *key_index)
            *key_index = iKEY_INDEX_W;
        else if(iKEY_INDEX_DOWN == *key_index)
            *key_index = iKEY_INDEX_S;
        else if(iKEY_INDEX_LEFT == *key_index)
            *key_index = iKEY_INDEX_A;
        else if(iKEY_INDEX_RIGHT == *key_index)
            *key_index = iKEY_INDEX_D;
        else if(iKEY_INDEX_W == *key_index)
            *key_index = iKEY_INDEX_UP;
        else if(iKEY_INDEX_S == *key_index)
            *key_index = iKEY_INDEX_DOWN;
        else if(iKEY_INDEX_A == *key_index)
            *key_index = iKEY_INDEX_LEFT;
        else if(iKEY_INDEX_D == *key_index)
            *key_index = iKEY_INDEX_RIGHT;
    }
}

/******************************************************************************
* 函数名称: key_decode_pre_callback()
* 功能描述: 标准按键解码前回调
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
__RAM_CODE void key_decode_pre_callback(void)
{
    if((key_ctrl&bwin_mac_sataus) && (key_ctrl&bwin_lock_status))
    {
        modifier_key_tmp&= 0x77;
        if(iSTD_APP == decode_data_l)
            key_ctrl &= ~bkey_press;
    }
}

/******************************************************************************
* 函数名称: key_decode_cmd_0x00()
* 功能描述:
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
void __attribute__((noinline)) key_decode_cmd_0x00(void)
{

}

/******************************************************************************
* 函数名称: key_decode_cmd_0x10()
* 功能描述:
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
void __attribute__((noinline)) key_decode_cmd_0x10(void)
{
#if (BOARD_LIGHT_KEYS)                  /* 灯光 Fn 键；关掉时函数保留（预编译的 key_decode 会调用），但为空 */
    if(iDEVICE_TYPE_USB != device_type)
    {
        if(ble_24g_ctrl & bom6239_conning)
            return;
    }

    switch(decode_data_l&0x0F)
    {
        case iLED_BRIGHT_SUB&0x0F:
            special_key |= bkey_light_dn;
            if((0==(special_key_bak&bkey_light_dn_bak)) && (true==bled_bright_en))
            {
                special_key_bak |= bkey_light_dn_bak;
                if(led_light_lev > iLED_LEV_MIN)
                {
                    led_light_lev--;
                    hid_ctrl_reg |= bdevice_status_change;
                    key_ctrl |= bfn_save_cfg;
                }
            }
            break;
        case iLED_BRIGHT_ADD&0x0F:
            special_key |= bkey_light_up;
            if((0==(special_key_bak&bkey_light_up_bak)) && (true==bled_bright_en))
            {
                special_key_bak |= bkey_light_up_bak;
                if(led_light_lev < iLED_LEV_MAX)
                {
                    led_light_lev++;
                    hid_ctrl_reg |= bdevice_status_change;
                    key_ctrl |= bfn_save_cfg;
                }
                else
                {
                    led_light_lev = iLED_LEV_MAX;
                }
            }
            break;

        case iLED_MODE_CYC_ADD&0x0F:
            special_key |= bkey_mode_cyc;
            if((0==(special_key_bak&bkey_mode_cyc_bak)) && (true==bled_bright_en))
            {
                special_key_bak |= bkey_mode_cyc_bak;
                led_cur_mode++;
                if(iMODE_LED_MAX<led_cur_mode)
                    led_cur_mode = iMODE_LED_MIN;

                hid_ctrl_reg |= bdevice_status_change;
                key_ctrl |= bfn_save_status;
            }
            break;
        case iLED_SPEED_SUB&0x0F:
            special_key |= bkey_speed_dn;
            if((0==(special_key_bak&bkey_speed_dn_bak)) && (true==bled_bright_en))
            {
                special_key_bak |= bkey_speed_dn_bak;
                if(iSPEED_LEV_MAX > led_speed_lev)
                {
                    led_speed_lev++;
                    hid_ctrl_reg |= bdevice_status_change;
                    key_ctrl |= bfn_save_cfg;
                }
            }
            break;
        case iLED_SPEED_ADD&0x0F:
            special_key |= bkey_speed_up;
            if((0==(special_key_bak&bkey_speed_up_bak)) && (true==bled_bright_en))
            {
                special_key_bak |= bkey_speed_up_bak;
                if(led_speed_lev > iSPEED_LEV_MIN)
                {
                    led_speed_lev--;
                    hid_ctrl_reg |= bdevice_status_change;
                    key_ctrl |= bfn_save_cfg;
                }
            }
            break;

        case iLED_OFF&0x0F:
            special_key |= bled_off;
            if(0==(special_key_bak&bled_off_back))
            {
                special_key_bak |= bled_off_back;
                bled_bright_en = !bled_bright_en;
                key_ctrl |= bfn_save_status;
            }
            break;
        case iLED_RGB_SWTICH&0x0F:
            special_key |= bkey_rgb_cyc;
            if((0==(special_key_bak&bkey_rgb_cyc_bak)) && (true==bled_bright_en))
            {
                special_key_bak |= bkey_rgb_cyc_bak;

                led_rgb_index++;
                led_rgb_index &= 0x07;
                key_ctrl |= bfn_save_cfg;
            }
            break;
    }
#endif
}

/******************************************************************************
* 函数名称: key_decode_cmd_0x20()
* 功能描述:
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
void __attribute__((noinline)) key_decode_cmd_0x20(void)
{
#if (BOARD_LIGHT_KEYS)                  /* 灯光 Fn 键；关掉时函数保留（预编译的 key_decode 会调用），但为空 */
    switch(decode_data_l&0x0F)
    {
        case iLED_MODE_0&0x0F:
            if(iMODE_LED_LG_L_R != led_cur_mode)
            {
                led_cur_mode = iMODE_LED_LG_L_R;
                hid_ctrl_reg |= bdevice_status_change;
                key_ctrl |= bfn_save_status;
            }

            break;
        case iLED_MODE_1&0x0F:
            if(iMODE_LED_BREATH != led_cur_mode)
            {
                led_cur_mode = iMODE_LED_BREATH;
                hid_ctrl_reg |= bdevice_status_change;
                key_ctrl |= bfn_save_status;
            }

            break;
        case iLED_MODE_2&0x0F:
            if(iMODE_LED_QCNH != led_cur_mode)
            {
                led_cur_mode = iMODE_LED_QCNH;
                hid_ctrl_reg |= bdevice_status_change;
                key_ctrl |= bfn_save_status;
            }

            break;
        case iLED_MODE_3&0x0F:
            if(iMODE_LED_NHJB != led_cur_mode)
            {
                led_cur_mode = iMODE_LED_NHJB;
                hid_ctrl_reg |= bdevice_status_change;
                key_ctrl |= bfn_save_status;
            }

            break;
        case iLED_MODE_4&0x0F:
            special_key |= bled_mode_on;
            if(0==(special_key_bak&bled_mode_on_bak))
            {
                special_key_bak |= bled_mode_on_bak;
                if(iMODE_LED_ON != led_cur_mode)
                {
                    led_cur_mode = iMODE_LED_ON;
                    hid_ctrl_reg |= bdevice_status_change;
                    key_ctrl |= bfn_save_status;
                }
                else
                {
                    led_rgb_index++;
                    led_rgb_index &= 0x07;
                    key_ctrl |= bfn_save_cfg;
                }
            }

            break;
    }
#endif
}

/******************************************************************************
* 函数名称: key_decode_cmd_0x30()
* 功能描述:
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
void __attribute__((noinline)) key_decode_cmd_0x30(void)
{
    uint32_t conn_rtc_value = drv_rtc_read(OM_RTC);
    switch(decode_data_l&0x0F)
    {
        case iMODE_KEY_FN&0x0F:
            special_key |= bfn_press;

            break;
        case iMODE_KEY_CONN&0x0F:
            device_key |= bkey_bt1_press;
            if(0==(device_key_bak&bkey_bt1_press_bak))
            {
                device_key_bak |= bkey_bt1_press_bak;
                press_bt1_time_count = conn_rtc_value;
            }
            else
            {
                if(mMS_TO_TICK(1000) < (conn_rtc_value-press_bt1_time_count))
                {
                    if(iDEVICE_TYPE_24G == device_type)
                    {
                        if(iMCU_MODE_NO_DONGLE == work_mode)
                        {
                            if(iRF_MODE_CONNECT != rf_mode_type)
                            {
                                mMCU_MODE_NORMAL;
                                rf_mode_convert(iRF_MODE_CONNECT);
                            }
                        }
                    }
                    else if(iDEVICE_TYPE_BT2 >= device_type)
                    {
                        if(0 == (ble_24g_ctrl&bom6239_conning))
                        {
                            OM_PMU->SW_RSVD1 |= RSVD1_RF_CONN_EN;
                            NVIC_SystemReset();
                        }
                    }
                }
            }
            break;
        case iMODE_KEY_BT0&0x0F:
            if(iDEVICE_TYPE_BT0 != device_type)
                break;

            device_key |= bkey_bt0_press;
            if(0==(device_key_bak&bkey_bt0_press_bak))
            {
                device_key_bak |= bkey_bt0_press_bak;
                press_bt0_time_count = conn_rtc_value;
            }
            else
            {
                if(mMS_TO_TICK(1000) < (conn_rtc_value-press_bt0_time_count))
                {
                    if(0 == (ble_24g_ctrl&bom6239_conning))
                    {
                        OM_PMU->SW_RSVD1 |= RSVD1_RF_CONN_EN;
                        NVIC_SystemReset();
                    }
                }
            }

            break;
        case iMODE_KEY_BT1&0x0F:
            if(iDEVICE_TYPE_BT1 != device_type)
                break;

            device_key |= bkey_bt1_press;
            if(0==(device_key_bak&bkey_bt1_press_bak))
            {
                device_key_bak |= bkey_bt1_press_bak;
                press_bt1_time_count = conn_rtc_value;
            }
            else
            {
                if(mMS_TO_TICK(3000) < (conn_rtc_value-press_bt1_time_count))
                {
                    if(0 == (ble_24g_ctrl&bom6239_conning))
                    {
                        OM_PMU->SW_RSVD1 |= RSVD1_RF_CONN_EN;
                        NVIC_SystemReset();
                    }
                }
            }

            break;
        case iMODE_KEY_BT2&0x0F:
            if(iDEVICE_TYPE_BT2 != device_type)
                break;

            device_key |= bkey_bt2_press;
            if(0==(device_key_bak&bkey_bt2_press_bak))
            {
                device_key_bak |= bkey_bt2_press_bak;
                press_bt2_time_count = conn_rtc_value;
            }
            else
            {
                if(mMS_TO_TICK(3000) < (conn_rtc_value-press_bt2_time_count))
                {
                    if(0 == (ble_24g_ctrl&bom6239_conning))
                    {
                        OM_PMU->SW_RSVD1 |= RSVD1_RF_CONN_EN;
                        NVIC_SystemReset();
                    }
                }
            }

            break;
        case iMODE_KEY_24G&0x0F:
            if(iDEVICE_TYPE_24G != device_type)
                break;

            device_key |= bkey_24g_press;
            if(0==(device_key_bak&bkey_24g_press_bak))
            {
                device_key_bak |= bkey_24g_press_bak;
                press_24g_time_count = conn_rtc_value;
            }
            else
            {
                if(mMS_TO_TICK(3000) < (conn_rtc_value-press_24g_time_count))
                {
                    if(iMCU_MODE_NO_DONGLE == work_mode)
                    {
                        if(iRF_MODE_CONNECT != rf_mode_type)
                        {
                            mMCU_MODE_NORMAL;
                            rf_mode_convert(iRF_MODE_CONNECT);
                            connect_time_count = conn_rtc_value;
                        }
                    }
                }
            }

            break;
        case iKEY_DIR_WASD&0x0F:
            special_key |= bkey_dir_wasd;
            if(0 == (special_key_bak&bkey_dir_wasd_bak))
            {
                special_key_bak |= bkey_dir_wasd_bak;
                key_ctrl ^= bdir_wasd_status;
                key_ctrl |= bfn_save_status;
            }

            break;
        case iKEY_WIN&0x0F:
            if(0==(key_ctrl&bwin_mac_sataus))
            {
                key_ctrl |= bwin_mac_sataus;;
                key_ctrl |= bfn_save_status;
                laod_key_cfg();
            }
            break;
        case iKEY_MAC&0x0F:
            if(0!=(key_ctrl&bwin_mac_sataus))
            {
                key_ctrl &= ~bwin_mac_sataus;;
                key_ctrl |= bfn_save_status;
                laod_key_cfg();
            }

            break;
        case iKEY_WIN_MAC&0x0F:
            special_key |= bkey_win_mac;
            if(0 == (special_key_bak&bkey_win_mac_bak))
            {
                special_key_bak |= bkey_win_mac_bak;
                key_ctrl ^= bwin_mac_sataus;
                key_ctrl |= bfn_save_status;
                laod_key_cfg();
            }

            break;
        case iKEY_WIN_LOCK&0x0F:
            special_key |= bwin_lock;
            if(0!=(key_ctrl&bwin_mac_sataus))
            {
                if(0==(special_key_bak&bwin_lock_bak))
                {
                    special_key_bak |= bwin_lock_bak;
                    key_ctrl ^= bwin_lock_status;
                    //key_ctrl |= bfn_save_status;  // WIN LOCK掉电不保存
                }
            }
            break;
        case iKEY_DIR_LOCK&0x0F:

            break;
        case iKEY_LOCK&0x0F:
            special_key |= bkey_lock;
            if(0==(special_key_bak&bkey_lock_bak))
            {
                special_key_bak |= bkey_lock_bak;

                key_ctrl ^= bkey_lock_status;
            }

            break;
    }
}

/******************************************************************************
* 函数名称: key_decode_cmd_0x40()
* 功能描述:
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
void __attribute__((noinline)) key_decode_cmd_0x40(void)
{
    uint32_t conn_rtc_value = drv_rtc_read(OM_RTC);
    switch(decode_data_l&0x0F)
    {
        case iKEY_DEFAULT&0x0F:
            special_key |= bdelault_press;
            if(0==(special_key_bak&bdelault_press_bak))
            {
                special_key_bak |= bdelault_press_bak;
                default_time_count = conn_rtc_value;

            }
            else
            {
                if(mMS_TO_TICK(2000) < (conn_rtc_value-default_time_count))
                {
                    OM_RCC->USB_CTRL = 0;           // disconnect
                    periph_blank_all();

                    flash_erase(iKEY_MODE0_BANK);
                    flash_erase(iKEY_MODE1_BANK);
                    flash_erase(iKEY_MODE2_BANK);
                    flash_erase(iKEY_MODE3_BANK);

                    flash_erase(iCFG_BANK);
                    flash_erase(iFLASH_STATUS_BANK);
                    flash_erase(iFLASH_CFG_DPI_BANK);

                    flash_erase(iFLASH_BLE_ID_BANK);
                    flash_erase(iFLASH_BLE_ID_BANK_BAK);

                    for(uint8_t i=0; i<3; i++)
                    {
                        drv_wdt_keep_alive(OM_WDT);
                        hint_led_status = (bled_hint_mode|bled_24g|bled_bt1|bled_bt0);
                        led_status_hint();
                        delayus(250000);
                        hint_led_status = bled_hint_mode;
                        led_status_hint();
                        delayus(250000);
                    }

                    device_type = iDEVICE_TYPE_BT0;
                    memset(flash_fifo, 0xFF, 64);
                    flash_fifo[0x00+0] = iBLE_ID_TPYE;
                    flash_fifo[0x00+1] = (ble_id[0]+1);
                    flash_fifo[0x00+2] = (ble_id[1]+1);
                    flash_fifo[0x00+3] = (ble_id[2]+1);
                    flash_write_ble_addr(0x1D0);
                    delayus(2000);

                    NVIC_SystemReset();
                }
            }
            break;
        case iKEY_ALT_TAB&0x0F:
            if(define_key_flag & bkey_decode_end)
            {
                key_ctrl_ex |= bcom_ctrl_status;
                return;
            }
            else
            {
                key_ctrl_ex &= ~bcom_ctrl_long;
                key_ctrl_ex |= (bcom_ctrl_en|bcom_ctrl_status);
                ctrl_key_value = 0x04;
                std_key_value = iSTD_TAB;
                com_ctrl_index = 0;
            }
            define_key_ctrl |= bdefine_key_send;

            break;
        case iKEY_CMD_TAB&0x0F:
            if(define_key_flag & bkey_decode_end)
            {
                key_ctrl_ex |= bcom_ctrl_status;
                return;
            }
            else
            {
                key_ctrl_ex &= ~bcom_ctrl_long;
                key_ctrl_ex |= (bcom_ctrl_en|bcom_ctrl_status);
                ctrl_key_value = 0x08;
                std_key_value = iSTD_TAB;
                com_ctrl_index = 0;
            }
            define_key_ctrl |= bdefine_key_send;
            break;

        case iKEY_CTRL_SPACE&0x0F:
            if(define_key_flag & bkey_decode_end)
            {
                key_ctrl_ex |= bcom_ctrl_status;
                return;
            }
            else
            {
                key_ctrl_ex &= ~bcom_ctrl_long;
                key_ctrl_ex |= (bcom_ctrl_en|bcom_ctrl_status);
                ctrl_key_value = 0x01;
                std_key_value = iSTD_SPACE;
                com_ctrl_index = 0;
            }
            define_key_ctrl |= bdefine_key_send;

            break;
        case iKEY_CMD_SPACE&0x0F:
            if(define_key_flag & bkey_decode_end)
            {
                return;
            }
            else
            {
                key_ctrl_ex |= (bcom_ctrl_en|bcom_ctrl_long);
                ctrl_key_value = 0x08;
                std_key_value = iSTD_SPACE;
                com_ctrl_index = 0;
            }
            define_key_ctrl |= bdefine_key_send;

            break;
        case iKEY_DESK_MAC&0x0F:
            if(define_key_flag & bkey_decode_end)
            {
                return;
            }
            else
            {
                if(0==(key_ctrl_ex&bdesk_mac))
                {
                    key_ctrl_ex |= bdesk_mac;
                    desk_mac_index = 0;
                }
            }
            define_key_ctrl |= bdefine_key_send;

            break;
    }
}
