/*******************************************************************************
* Copyright (c) 2013, SHENZHEN HUNTERSUN Co.,LTD.
* All rights reserved.
*
* 文件名称: rf_data_process.c
* 文件描述: rf数据处理
*
* 版    本: V1.0.0
* 作    者:
* 日    期:
* 功能说明:
* 使用说明:
*******************************************************************************/
#include <string.h>
#include "main_24g.h"
#include "usb_cfg_info.h"
#include "hid_process.h"
#include "rf_encryption.h"

//-----------------------------------------------------------------------------;
//常量定义区
//

//----------------------------------------------------------------------------
//变量定义区
//
uint32_t                    device_info_rtc;
uint32_t                    connect_time_count;

//----------------------------------------------------------------------------
//表格定义区
//

//============================================================================;
// 代码区                                                                     ;
//============================================================================;

//------------------------------------------------------------------------------------------------
//
/******************************************************************************
* 函数名称: rf_device_info
* 功能描述:
* 输入参数:
* 输出参数:
* 说    明: 无
******************************************************************************/
void rf_device_info(void)
{
    rf_data_fifo[0] = iRF_K_TYPE_CONN_RSP_1;
    rf_data_fifo[1] = kb_bd_addr[2];
    rf_data_fifo[2] = kb_bd_addr[3];
    rf_data_fifo[3] = kb_bd_addr[4];
    rf_data_fifo[4] = kb_bd_addr[5];
    rf_data_fifo[5] = iVERSION_ID_L;
    rf_data_fifo[6] = iVERSION_ID_H;
    rf_data_fifo[7] = (0&0x1F);
}

/******************************************************************************
* 函数名称: rf_conn_ing_callback()
* 功能描述: rf数据组织
* 输入参数: 无
* 输出参数: true:继续保持程序执行；false不进入配对流程
* 说    明: 无
******************************************************************************/
bool rf_conn_ing_callback(void)
{
    if(0==(ble_24g_ctrl&bom6239_conn_ok))
    {
        if(mMS_TO_TICK(15*1000) > (drv_rtc_read(OM_RTC) - connect_time_count))
            return true;                             // 对码驻留15s

        ble_24g_ctrl &= ~bom6239_conning;
        rf_ctrl_reg |= (brf_sync_time|brf_sync_status);
        rf_mode_convert(iRF_MODE_SYNC);
    }
    else
    {
        rf_ctrl_reg |= (bsearch_dongle|brf_sync_time|brf_sync_status);
        sync_word = iSEARCH_SYNC_WORD;
        rf_mode_convert(iRF_MODE_SYNC);
    }

    return false;
}

/******************************************************************************
* 函数名称: rf_conn_rsp_data()
* 功能描述: rf数据组织
* 输入参数: 无
* 输出参数: true:继续保持程序执行；false不进入配对流程
* 说    明: 无
******************************************************************************/
void rf_conn_rsp_data(uint8_t *rf_fifo, uint8_t *tx_length)
{
    rf_fifo[8] = rf_fifo[3];
    rf_fifo[9] = rf_fifo[4];
    rf_device_info();
    rf_fifo[10] = iPROJECT_ID_2;
    rf_fifo[11] = iPROJECT_ID_3;
    rf_fifo[12] = iPROJECT_ID_0;
    rf_fifo[13] = iPROJECT_ID_1;
    rf_fifo[14] = 0x04;
    rf_fifo[15] = iAPP_ID_H;

    *tx_length = iRF_LENGTH_CONN_RSP_EX;
    return;
}

//------------------------------------------------------------------------------------------------
//
/******************************************************************************
* 函数名称: clr_mouse_data()
* 功能描述:
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
void clr_mouse_data(void)
{
    mouse_x_1 = 0;
    mouse_y_1 = 0;
    mouse_xy_1 = 0;

    mouse_x_2 = 0;
    mouse_y_2 = 0;
    mouse_xy_2 = 0;
}

/******************************************************************************
* 函数名称: rf_sreach_ok_callback()
* 功能描述:
* 输入参数:
* 输出参数: true:搜索成功，转换为正常同步；false:继续搜索
* 说    明:
******************************************************************************/
bool rf_sreach_ok_callback(uint8_t *rf_data_fifo)
{
    return true;
}

void rf_sync_ok_callback(void)
{
    mMCU_MODE_NORMAL;
    clr_mouse_data();

    if(adv_cfg_value&iPOWER_LOW)
        report_24g = iREPORT_125;
    else
        report_24g = ((report_dpi[key_mode]>>4)&0x03);
    m_tx_count = 0;

    if(true == b24g_nromal)
        ble_24g_ctrl &= ~bom6239_sync_ing;

    hid_ctrl_reg |= bdevice_status_change;
}

//------------------------------------------------------------------------------------------------
//
/******************************************************************************
* 函数名称: rf_data_process()
* 功能描述: rf数据组织
* 输入参数: 无
* 输出参数: 无
* 说    明: 无
******************************************************************************/
void rf_hid_rx_callback(void)
{
    out_report_data[1] &= 0x3F;

    hid_ctrl_reg |= bhid_data_process;
    hid_process();
}

extern void hid_get_fw_mode_info(void);
extern uint8_t lvd_value;

/******************************************************************************
* 函数名称: rf_data_process()
* 功能描述: rf数据组织
* 输入参数: 无
* 输出参数: 无
* 说    明: 无
******************************************************************************/
__RAM_CODE void rf_tx_data_process(void)
{
    if(0==(m_data_flag & bhid_data_send))
    {
        if(def_tx_count)
        {
            def_tx_count--;
            hid_ctrl_reg &= ~bdevice_status_change;
            hid_get_fw_mode_info();
            goto sensor_dat_process;
        }
    }

    if(true == bbat_update)
    {
        bbat_update = false;
        requst_cmd = 7;
    }

    if(7 == requst_cmd)
    {
        device_info_rtc = drv_rtc_read(OM_RTC);
        rf_device_info();
        rf_data_fifo[8] = (bat_value&0x7F);
        if(0 != (bat_status&bcharging))
            rf_data_fifo[8] |= (1<<7);
        rf_data_fifo[9] = 0x80;
        rf_data_fifo[9] |= (lvd_value&0x1F);

        rf_data_length = iRF_MK_LENGTH_10B;

        rf_ctrl_reg |= brf_cmd_tx;
        rf_ctrl_reg |= brf_data_tx;
        requst_cmd = 0; 
    }
    else if(mmk_tx_count)
    {
        mmk_tx_count--;
        rf_data_fifo[0] = iRF_K_TYPE_KB;
        rf_data_fifo[1] = (iRF_K_OTHER_MMK|(mmk_data_h&0x1F));
        rf_data_fifo[2] = mmk_data_l;
        rf_data_length = iRF_K_LENGTH_OTHER;
        m_data_flag |= bmmk_data_send;
        rf_ctrl_reg |= brf_data_tx;
        pkt_press_count = 0;
        rtc_init_value = drv_rtc_read(OM_RTC);
    }
    else if(key_tx_count)
    {
        key_tx_count--;

        rf_data_fifo[0] = iRF_K_TYPE_KB;
        memcpy(rf_data_fifo+1, std_key, 7);
        rf_data_length = iRF_K_LENGTH_STD;

        m_data_flag |= bstd_kb_data_send;
        rf_ctrl_reg |= brf_data_tx;
        pkt_press_count = 0;
        rtc_init_value = drv_rtc_read(OM_RTC);
    }
    else if(key_ex_tx_count)
    {
        key_ex_tx_count--;

        rf_data_fifo[0] = iRF_K_TYPE_KB;
        memcpy(rf_data_fifo+1, std_key+7, 15);
        rf_data_length = iRF_K_LENGTH_EX;

        m_data_flag |= bstd_ex_data_send;
        rf_ctrl_reg |= brf_data_tx;
        pkt_press_count = 0;
        rtc_init_value = drv_rtc_read(OM_RTC);
    }
    else if(m_tx_count)
    {
        m_tx_count--;
        mouse_z &= 0x0F;
        mouse_z |= ((mouse_data_z<<4)&0xF0);

        rf_data_fifo[0] = (((0<<4) & 0x70) | 0x80);

        rf_data_fifo[1] = mouse_data_key;
        rf_data_fifo[2] = (mouse_x&0xFF);
        rf_data_fifo[3] = (mouse_y&0xFF);

        rf_data_fifo[4] = key_data_1;
        rf_data_fifo[5] = mouse_x_1;
        rf_data_fifo[6] = mouse_y_1;
        rf_data_fifo[7] = mouse_z;
        rf_data_fifo[8] = (((mouse_x>>8) & 0x0F) | ((mouse_y>>4) & 0xF0));
        rf_data_fifo[9] = mouse_xy_1;

        rf_data_fifo[10] = mouse_x_2;
        rf_data_fifo[11] = mouse_y_2;
        rf_data_fifo[12] = mouse_xy_2;
        rf_data_length = iRF_M_LENGTH_MOUSE;

        m_data_flag |= bmouse_data_send;
        rf_ctrl_reg |= brf_data_tx;

        rtc_init_value = drv_rtc_read(OM_RTC);
    }

    pkt_press_count++;
    if(64 <= pkt_press_count)
    {
        if(mmk_data_l)
        {
            mmk_tx_count = 2;
        }
        else
        {
            for(uint8_t i=0; i<22; i++)
            {
                if(0 != std_key[i])
                {
                    key_tx_count = 2;
                    break;
                }
            }
        }
    }

sensor_dat_process:
    key_data_2 = key_data_1;
    mouse_x_2 = mouse_x_1;
    mouse_y_2 = mouse_y_1;
    mouse_xy_2 = mouse_xy_1;

    key_data_1 = mouse_data_key;
    mouse_x_1 = (mouse_x&0xFF);
    mouse_y_1 = (mouse_y&0xFF);
    mouse_xy_1 = (((mouse_x>>8) & 0x0F) | ((mouse_y>>4) & 0xF0));
    mouse_z>>=4;

    mouse_x = 0;
    mouse_y = 0;
    mouse_data_z = 0;
}

__RAM_CODE void rf_normal_mode_sleep(void)
{
    OM_CRITICAL_BEGIN();
    drv_pmu_set_low_power_mode(PMU_LPM_STOP1);
    OM_CRITICAL_END();
}

/******************************************************************************
* 函数名称:  发射中回调函数()
   
------------------------------------------------------------------------------
|              2ms               |                2ms               |  // 周期
               |  TX0          |                    |  TX1        |    // 发射
|                 iqr          |                       iqr        |    // iqr时间点
               tx0 idle          |                tx1 idle          |  // 无数据发射
------------------------------------------------------------------------------

******************************************************************************/

//----------------------------------------------------------------------------
void rf_tx1_idle_callback(void)
{
    lvd_scan_count++;
    if(4==(lvd_scan_count&0x07))
        device_led_process();                     // 16ms
    if(0==(lvd_scan_count&0x01))
        logo_process();
}

void rf_tx1_irq_callback(void)
{
    lvd_scan_count++;
    if(4==(lvd_scan_count&0x07))
        device_led_process();
    if(0==(lvd_scan_count&0x01))
        logo_process();
}

void rf_rx_begin_callback(void)
{
    led_hint_process();
}

void rf_rsp_ok_callback(uint8_t * rf_fifo)
{
    if(mMS_TO_TICK(250) < (rtc_reload_value-device_info_rtc))
        requst_cmd = 7;

    if(kb_led_status != (rf_fifo[2]&0x07))
    {
        kb_led_status = (rf_fifo[2]&0x07);
    }
}

void rf_no_rx_callback(void)
{

}


/**
 * @brief  
 * @note   
 * @param  lfsr: 
 * @retval None
 */
void rf_encrypt_get_random_val(uint16_t* lfsr)
{
    DRV_RCC_CLOCK_ENABLE(RCC_CLK_RTC, 1);
    if (!(OM_RTC->CR & RTC_CR_TIMER_EN_MASK)) {
        drv_rtc_init(OM_RTC);
    }
    uint32_t rtc_val = drv_rtc_read(OM_RTC);
    *lfsr ^= (uint16_t) (rtc_val>>16) ^ (uint16_t)(rtc_val);
}

/**
 * @brief  
 * @note   
 * @param  p_rf_data_fifo: 
 * @retval None
 */
void rf_encrypt_cb(uint8_t* p_rf_data_fifo,uint8_t rf_data_length_temp)
{
    switch(rf_data_length_temp) {
        case iRF_M_LENGTH_MOUSE:
            p_rf_data_fifo[ENCRYPT_PACKET_TYPE_IDX] = PAKCET_TYPE_FOR_NORMAL_MOUSE;
            break;
        case iRF_K_LENGTH_STD:
            p_rf_data_fifo[ENCRYPT_PACKET_TYPE_IDX] = PACKET_TYPE_FOR_NORMAL_KEYBOARD;
            break;
        case iRF_K_LENGTH_EX:
            p_rf_data_fifo[ENCRYPT_PACKET_TYPE_IDX] = PACKET_TYPE_FOR_KB_BITMAP;
            break;
        case iRF_K_LENGTH_OTHER:
        default:
            p_rf_data_fifo[ENCRYPT_PACKET_TYPE_IDX] = PACKET_TYPE_FOR_OTHERS;
            break;
    }
    rf_encrypt(p_rf_data_fifo);
    drv_rf_write_tx_payload(rf_data_fifo, ENCRYPT_TOTAL_LENGTH);
}
