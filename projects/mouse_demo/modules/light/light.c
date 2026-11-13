/**
 * @file    light.c
 * @brief   灯光模块：灯效引擎和模块入口
 *
 * 由 periph/periph.c 的外设表调用，主流程不直接调用这里的函数：
 *   frame  每帧刷新灯效            logo_process()
 *   flush  把一帧送到灯上          light_port_flush()，后端见 light_argb.c / light_pwm.c
 *   blank  关灯（同步中、休眠前）    logo_off()
 *   busy   灯亮着时不进 STOP1
 * 灯效设置（模式、亮度、速度、颜色、开关）由核心的 led.c 保存，这里只读。
 * 不需要灯时把 board_config.h / io_board.h 的 BOARD_LIGHT 设成 LIGHT_NONE，
 * 这个目录的文件可以从 Keil 工程里移除。
 */
#include <string.h>
#include "om_driver.h"
#include "main.h"
#include "lvd.h"
#include "flash.h"
#include "flash_process.h"
#include "usb_cfg_info.h"
#include "matrix_key.h"
#include "periph.h"
#include "light_fx.h"

#if (BOARD_LIGHT != LIGHT_NONE)

//-----------------------------------------------------------------------------;
// 灯效引擎的运行状态（原来在 led/led.c）
//
bool                            bback_led_status;
uint8_t                         gtemp_i;
uint8_t                         gtemp_j;
uint8_t                         gtemp_x;
uint8_t                         led_hint_mode;
uint8_t                         logo_hint_time_count;
uint8_t                         rgb_enable;
uint8_t                         pwm_value;
uint8_t                         pwm_count;
uint8_t                         nhjb_led_index;
uint16_t                        flash_on_time;
uint16_t                        flash_off_time;
uint16_t                        led_ctrl_reg;
uint16_t                        led_status_reg;
uint8_t                         led_use_flag;
uint8_t                         led_pre_mode;
unsigned char                   pwm_value;
uint8_t                         pwm_time_count;
uint16_t                        delay_time_count;
uint8_t                         rgb_index;
uint8_t                         led_process_index;
__ALIGNED(4) uint8_t            pwm_rgb_tbl[iLED_MAX][3];
uint8_t                         pwm_duty_array_0[0x20];
uint8_t                         pwm_duty_array_1[0x20];
uint8_t                         pwm_duty_array_2[0x20];
uint8_t                         pwm_duty_array_3[0x20];

extern bool                     b24g_nromal;

void logo_off(void)
{
    light_port_off();               // 具体怎么关灯由 light_argb.c / light_pwm.c 决定
}

void clr_rgb(void)
{
    memset(pwm_rgb_tbl, 0x00, iLED_MAX*3);
}

void rgb_pwm0_w(void)
{
    gtemp_j &= 0x0F;
    if(rgb_enable&iPWM_R)
         pwm_rgb_tbl[gtemp_j][iR_INDEX] = pwm_duty_array_0[gtemp_x];
     else
         pwm_rgb_tbl[gtemp_j][iR_INDEX] = 0;
    if(rgb_enable&iPWM_G)
         pwm_rgb_tbl[gtemp_j][iG_INDEX] = pwm_duty_array_0[gtemp_x];
     else
         pwm_rgb_tbl[gtemp_j][iG_INDEX] = 0;
    if(rgb_enable&iPWM_B)
         pwm_rgb_tbl[gtemp_j][iB_INDEX] = pwm_duty_array_0[gtemp_x];
     else
         pwm_rgb_tbl[gtemp_j][iB_INDEX] = 0;
}

__RAM_CODE void rgb_w_max(void)
{
    gtemp_j &= 0x0F;
    if(rgb_enable&iPWM_R)
        pwm_rgb_tbl[gtemp_j][iR_INDEX] = iPWM_MAX;
    else
        pwm_rgb_tbl[gtemp_j][iR_INDEX] = 0;
    if(rgb_enable&iPWM_G)
        pwm_rgb_tbl[gtemp_j][iG_INDEX] = iPWM_MAX;
    else
        pwm_rgb_tbl[gtemp_j][iG_INDEX] = 0;
    if(rgb_enable&iPWM_B)
        pwm_rgb_tbl[gtemp_j][iB_INDEX] = iPWM_MAX;
    else
        pwm_rgb_tbl[gtemp_j][iB_INDEX] = 0;
}

/******************************************************************************
* 函数名称: logo_process
* 功能描述: 灯效刷新一帧（模块的 frame 入口）
* 输入参数:
* 输出参数:
* 说    明:
******************************************************************************/
void logo_process(void)
{
    uint8_t i;

    if(iDEVICE_TYPE_24G == device_type)
    {
        if(true==bflash_ctrl)
            return;
    }

    if(iDEVICE_TYPE_USB == device_type)
    {
        if(0 != (usb_ctrl_flag&busb_suspend_resume))
        {
            logo_off();
            return;
        }
    }
    else
    {
        if(ble_24g_ctrl&bom6239_sync_ing)
        {
            logo_off();
            return;
        }
    }

    if((false == bled_bright_en) || (iLOW_POWER>=bat_value))
    {
        clr_rgb();
        logo_off();
        return;
    }

    if(false==bargb_valid)
        light_port_init();          // 具体怎么开灯由 light_argb.c / light_pwm.c 决定

    if(led_pre_mode != led_cur_mode)
    {
        led_pre_mode = led_cur_mode;

        led_process_index = 0;
        rgb_index = 0;

        clr_rgb();
        memset(pwm_duty_array_0, 0x00, 16);
        memset(pwm_duty_array_1, 0x00, 16);
        memset(pwm_duty_array_2, 0x00, 16);
        memset(pwm_duty_array_3, 0x00, 16);

        switch(led_cur_mode)
        {
            case iMODE_LED_OFF:

                break;
            case iMODE_LED_ON:

                break;
            case iMODE_LED_BREATH:
                pwm_time_count = 0;
                delay_time_count = 0xFF;
                rgb_enable = rgb_en_tbl[led_rgb_index&0x0F];

                break;
            case iMODE_LED_SNAKE:
                snake_led_index = 4;
                pwm_duty_array_0[4] = ((iPWM_MAX*4)/4);

                break;
            case iMODE_LED_NHJB:
                delay_time_count = 0;
                pwm_duty_array_0[0] = iPWM_MAX;
                pwm_duty_array_0[1] = 0x00;
                pwm_duty_array_0[2] = 0x00;

                break;
            case iMODE_LED_LG_L_R:
                delay_time_count = 0;
                led_status_reg = 0xCCCC;

                for(i=0; i<iLED_MAX; i++)
                {
                    pwm_duty_array_0[i] = led_liuguang_l_r_tbl[i];
                    pwm_duty_array_1[i] = led_liuguang_rgb_tbl[i];
                }

                break;
            case iMODE_LED_STAR:
                delay_time_count = 0;
                snake_led_index = 0xFF;
                lfsr_value = 0x55AA;
                break;
            case iMODE_LED_SBZL:
                for(i=0; i<iLED_MAX; i++)
                {
                    pwm_duty_array_0[i] = sbzl_r_tbl[iLED_MAX-1-i];
                    pwm_duty_array_1[i] = sbzl_g_tbl[iLED_MAX-1-i];
                    pwm_duty_array_2[i] = sbzl_b_tbl[iLED_MAX-1-i];
                    pwm_duty_array_3[i] = sbzl_ctrl_tbl[iLED_MAX-1-i];
                }
                break;
            case iMODE_LED_BS:
                break;
            case iMODE_LED_QCNH:
                break;
            case iMODE_LED_LHTW:
                break;
            case iMODE_LED_DXTW:
                snake_led_index = 3;
                pwm_duty_array_0[snake_led_index] = iPWM_SNAKE_MAX;
                break;
            case iMODE_LED_SXTW:
                snake_led_index = 3;
                pwm_duty_array_0[snake_led_index] = iPWM_SNAKE_MAX;
                bback_led_status = 0;
                break;
            case iMODE_LED_TXTW:
            case iMODE_LED_FXTW:
                snake_led_index = 3;
                pwm_duty_array_0[snake_led_index] = iPWM_SNAKE_MAX;
                break;
            case iMODE_LED_JG:
                led_use_flag = 0;
                break;
        }
    }
    else
    {
        //bled_hint_ing = 1;
        switch(led_cur_mode)
        {
            case iMODE_LED_OFF:
                break;
            case iMODE_LED_ON:
                led_mode_on();
                break;
            case iMODE_LED_BREATH:
                led_mode_breath();
                break;
            case iMODE_LED_SNAKE:
                led_mode_snake();
                break;
            case iMODE_LED_NHJB:
                led_mode_nhjb();
                break;
            case iMODE_LED_LG_L_R:
                led_mode_liuguang();
                break;
            case iMODE_LED_STAR:
                led_mode_star();
                break;
            case iMODE_LED_SBZL:
                led_mode_sbzl();
                break;
            case iMODE_LED_BS:
                led_mode_baoshan();
                break;
            case iMODE_LED_QCNH:
                led_mode_qcnh();
                break;
            case iMODE_LED_LHTW:
                led_mode_lhtw();
                break;
            case iMODE_LED_DXTW:
                led_mode_dxtw();
                break;
            case iMODE_LED_SXTW:
                led_mode_sxtw();
                break;
            case iMODE_LED_TXTW:
            case iMODE_LED_FXTW:
                led_mode_txtw();
                break;
        }
    }
}

//-----------------------------------------------------------------------------;
// 模块入口
//
static bool light_busy(void)
{
    return bargb_valid;
}

const periph_ops_t light_periph = {
    .name  = "light",
    .frame = logo_process,
    .flush = light_port_flush,
    .blank = logo_off,
    .busy  = light_busy,
};

#endif /* (BOARD_LIGHT != LIGHT_NONE) */
