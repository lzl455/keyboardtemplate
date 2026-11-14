/**
 * @file    io_config.h
 * @brief   OM6252 QFN48 引脚定义，由 OM62xx 引脚配置器生成
 * @note    工程：SE73K 键盘
 *          引脚序号 = 端口 x 8 + 位号，和 OM_GPIO0 的位一一对应
 *          改引脚请回到配置器里改，再重新生成
 */
#ifndef __IO_CONFIG_H
#define __IO_CONFIG_H

#include <stdbool.h>
#include "om_driver.h"

/* ---- 引脚序号 ---------------------------------------------------------- */
#define IO_ARGB_OFF_PIN          0u      /* P00  第31脚  灯电源开关 */
#define IO_ARGB_OUT_PIN          1u      /* P01  第32脚  ARGB 数据 (LEDC) */
#define IO_USB_DN_PIN            2u      /* P02  第35脚  USB D- */
#define IO_USB_DP_PIN            3u      /* P03  第36脚  USB D+ */
#define IO_KEY_R0_PIN            4u      /* P04  第37脚  矩阵行 */
#define IO_KEY_R1_PIN            5u      /* P05  第38脚  矩阵行 */
#define IO_KEY_R2_PIN            6u      /* P06  第39脚  矩阵行 */
#define IO_KEY_R3_PIN            7u      /* P07  第40脚  矩阵行 */
#define IO_KEY_R4_PIN            8u      /* P10  第41脚  矩阵行 */
#define IO_KEY_R5_PIN            9u      /* P11  第42脚  矩阵行 */
#define IO_KEY_R6_PIN            10u     /* P12  第43脚  矩阵行 */
#define IO_KEY_R7_PIN            11u     /* P13  第44脚  矩阵行 */
#define IO_KEY_C19_PIN           18u     /* P22  第 7脚  矩阵列 */
#define IO_LED_VDD_PIN           19u     /* P23  第 8脚  GPIO 输出 */
#define IO_KEY_C18_PIN           20u     /* P24  第 9脚  矩阵列 */
#define IO_KEY_C1_PIN            21u     /* P25  第10脚  矩阵列 */
#define IO_KEY_C2_PIN            22u     /* P26  第11脚  矩阵列 */
#define IO_KEY_C3_PIN            23u     /* P27  第12脚  矩阵列 */
#define IO_KEY_C4_PIN            24u     /* P30  第13脚  矩阵列 */
#define IO_KEY_C5_PIN            25u     /* P31  第14脚  矩阵列 */
#define IO_KEY_EX_PIN            26u     /* P32  第30脚  GPIO 输出 */
#define IO_BAT_LVD_PIN           27u     /* P33  第45脚  电压检测 (LVD) */
#define IO_CHR_OK_PIN            28u     /* P34  第46脚  GPIO 输入 */
#define IO_MODE_SW_PIN           29u     /* P35  第 3脚  GPIO 输入 */
#define IO_USB_5V_PIN            30u     /* P36  第 4脚  GPIO 输入 */

/* ---- 分组掩码 ---------------------------------------------------------- */
#define IO_OUTPUT_MASK           0x04080003u   /* P00 P01 P23 P32 */
#define IO_WAKEUP_FALL_MASK      0x00000FF0u   /* P04 P05 P06 P07 P10 P11 P12 P13 */
#define IO_WAKEUP_RISE_MASK      0x00000000u   /* 无 */
#define IO_KEY_ROW_MASK          0x00000FF0u   /* P04 P05 P06 P07 P10 P11 P12 P13 */
#define IO_KEY_COL_MASK          0x03F40000u   /* P22 P24 P25 P26 P27 P30 P31 */

/* ---- 上电寄存器值（和 io_config_init() 写的一样）--------------------------
 * 休眠代码在这个基础上改（main.c 的 mouse_periph_wakeup_enable()、drv_pmu.c 的深度休眠），
 * periph/matrix_key.c 用它们在编译时检查矩阵的行列设置。 */
#define IO_PULLUP_MASK           0x33F70FF0u   /* 有上拉：P04 P05 P06 P07 P10 P11 P12 P13 P20 P21 P22 P24 P25 P26 P27 P30 P31 P34 P35 */
#define IO_PAD_PU_CTRL_VAL       0x00AAAA00u
#define IO_PAD_PU_CTRL_1_VAL     0x0A0AAA25u
#define IO_PAD_PD_CTRL_VAL       0x4000000Cu
#define IO_DATAOUT_VAL           0xFFF7FFFEu

/* 读矩阵行：按下为 1，返回值的 bit0 对应 P04 */
#define IO_KEY_ROW_READ()        ((~OM_GPIO0->DATA & IO_KEY_ROW_MASK) >> 4)

/* ---- 读写宏 ------------------------------------------------------------ */
#define IO_ARGB_OFF_HIGH()       (OM_GPIO0->DATAOUTSET = (1u << IO_ARGB_OFF_PIN))
#define IO_ARGB_OFF_LOW()        (OM_GPIO0->DATAOUTCLR = (1u << IO_ARGB_OFF_PIN))
#define IO_ARGB_OFF_TOGGLE()     (OM_GPIO0->DATAOUT ^= (1u << IO_ARGB_OFF_PIN))
#define IO_LED_VDD_HIGH()        (OM_GPIO0->DATAOUTSET = (1u << IO_LED_VDD_PIN))
#define IO_LED_VDD_LOW()         (OM_GPIO0->DATAOUTCLR = (1u << IO_LED_VDD_PIN))
#define IO_LED_VDD_TOGGLE()      (OM_GPIO0->DATAOUT ^= (1u << IO_LED_VDD_PIN))
#define IO_KEY_EX_HIGH()         (OM_GPIO0->DATAOUTSET = (1u << IO_KEY_EX_PIN))
#define IO_KEY_EX_LOW()          (OM_GPIO0->DATAOUTCLR = (1u << IO_KEY_EX_PIN))
#define IO_KEY_EX_TOGGLE()       (OM_GPIO0->DATAOUT ^= (1u << IO_KEY_EX_PIN))
#define IO_CHR_OK_READ()         ((OM_GPIO0->DATA >> IO_CHR_OK_PIN) & 1u)
#define IO_MODE_SW_READ()        ((OM_GPIO0->DATA >> IO_MODE_SW_PIN) & 1u)
#define IO_USB_5V_READ()         ((OM_GPIO0->DATA >> IO_USB_5V_PIN) & 1u)
/* 矩阵列：扫描时拉低，扫完释放为输入（靠上拉回到高电平） */
#define IO_KEY_C19_SCAN()        do { OM_GPIO0->DATAOUTCLR = (1u << IO_KEY_C19_PIN); OM_GPIO0->OUTENSET = (1u << IO_KEY_C19_PIN); } while (0)
#define IO_KEY_C19_RELEASE()     do { OM_GPIO0->DATAOUTSET = (1u << IO_KEY_C19_PIN); OM_GPIO0->OUTENCLR = (1u << IO_KEY_C19_PIN); } while (0)
#define IO_KEY_C18_SCAN()        do { OM_GPIO0->DATAOUTCLR = (1u << IO_KEY_C18_PIN); OM_GPIO0->OUTENSET = (1u << IO_KEY_C18_PIN); } while (0)
#define IO_KEY_C18_RELEASE()     do { OM_GPIO0->DATAOUTSET = (1u << IO_KEY_C18_PIN); OM_GPIO0->OUTENCLR = (1u << IO_KEY_C18_PIN); } while (0)
#define IO_KEY_C1_SCAN()         do { OM_GPIO0->DATAOUTCLR = (1u << IO_KEY_C1_PIN); OM_GPIO0->OUTENSET = (1u << IO_KEY_C1_PIN); } while (0)
#define IO_KEY_C1_RELEASE()      do { OM_GPIO0->DATAOUTSET = (1u << IO_KEY_C1_PIN); OM_GPIO0->OUTENCLR = (1u << IO_KEY_C1_PIN); } while (0)
#define IO_KEY_C2_SCAN()         do { OM_GPIO0->DATAOUTCLR = (1u << IO_KEY_C2_PIN); OM_GPIO0->OUTENSET = (1u << IO_KEY_C2_PIN); } while (0)
#define IO_KEY_C2_RELEASE()      do { OM_GPIO0->DATAOUTSET = (1u << IO_KEY_C2_PIN); OM_GPIO0->OUTENCLR = (1u << IO_KEY_C2_PIN); } while (0)
#define IO_KEY_C3_SCAN()         do { OM_GPIO0->DATAOUTCLR = (1u << IO_KEY_C3_PIN); OM_GPIO0->OUTENSET = (1u << IO_KEY_C3_PIN); } while (0)
#define IO_KEY_C3_RELEASE()      do { OM_GPIO0->DATAOUTSET = (1u << IO_KEY_C3_PIN); OM_GPIO0->OUTENCLR = (1u << IO_KEY_C3_PIN); } while (0)
#define IO_KEY_C4_SCAN()         do { OM_GPIO0->DATAOUTCLR = (1u << IO_KEY_C4_PIN); OM_GPIO0->OUTENSET = (1u << IO_KEY_C4_PIN); } while (0)
#define IO_KEY_C4_RELEASE()      do { OM_GPIO0->DATAOUTSET = (1u << IO_KEY_C4_PIN); OM_GPIO0->OUTENCLR = (1u << IO_KEY_C4_PIN); } while (0)
#define IO_KEY_C5_SCAN()         do { OM_GPIO0->DATAOUTCLR = (1u << IO_KEY_C5_PIN); OM_GPIO0->OUTENSET = (1u << IO_KEY_C5_PIN); } while (0)
#define IO_KEY_C5_RELEASE()      do { OM_GPIO0->DATAOUTSET = (1u << IO_KEY_C5_PIN); OM_GPIO0->OUTENCLR = (1u << IO_KEY_C5_PIN); } while (0)
#define IO_KEY_COL_RELEASE_ALL() (OM_GPIO0->OUTENCLR = IO_KEY_COL_MASK)

void io_config_init(void);
void io_config_wakeup(bool enable);

#endif /* __IO_CONFIG_H */
