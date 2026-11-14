/**
 * @file    matrix_cfg.h
 * @brief   键盘矩阵硬件、物理按键到键表的对应关系、按位置读取的特殊键
 *
 * 由 tools/om62xx-keymap 矩阵表配置器生成，不要手改：改了用配置器重新生成。
 * 本文件只放宏定义，periph/matrix_key.c 按这里的配置扫描：
 *   1. 直连列：依次把 MATRIX_DIRECT_PINS 里的脚拉低，读一次行线，存进 matrix_raw[]
 *   2. 扩展列：每翻转一次 KEY_EX（MATRIX_EX_PIN）换到下一列，共读 MATRIX_EX_NUM 次，接着存
 *   3. 用 MATRIX_SLOT_MAP 把每个物理位置换成键表位置，写进 matrix_status[3..18] 给键表解码
 * 键表位置 s 就是键表的第 s 项，对应 C(s/8+3) R(s%8)。
 */
#ifndef __MATRIX_CFG_H
#define __MATRIX_CFG_H

/* ---- 行线（按下读到低电平）---- */
/* R0 = P04 */
/* R1 = P05 */
/* R2 = P06 */
/* R3 = P07 */
/* R4 = P10 */
/* R5 = P11 */
/* R6 = P12 */
/* R7 = P13 */
#define MATRIX_ROW_MASK         0x00000FF0u
/* 从 ~GPIO0->DATA 取出各行，第 n 位 = 第 n 行 */
#define MATRIX_ROW_GATHER(d)    ((((d) >> 4) & 0xFFu))

/* ---- 直连列线（扫描时拉低）---- */
#define MATRIX_DIRECT_NUM       7
#define MATRIX_DIRECT_PINS      {21, 22, 23, 24, 25, 20, 18}   /* C1 P25, C2 P26, C3 P27, C4 P30, C5 P31, C18 P24, C19 P22 */
#define MATRIX_DIRECT_MASK      0x03F40000u

/* ---- 扩展列（由扩展芯片 PA5 控制，翻转 KEY_EX 换到下一列）---- */
#define MATRIX_EX_NUM           13
#define MATRIX_EX_PIN           26       /* P32 */
/* 翻转顺序：PA7=C8 PA6=C10 PA4=C6 PA3=C7 PA2=C14 PB0=C13 PB1=C12 PB2=C11 PB3=C15 PB4=C9 PB5=C0 PA0=C16 PA1=C17 */
#define MATRIX_RAW_NUM          20       /* 直连列 + 扩展列 */

/* ---- 物理位置 -> 键表位置 ----
 * 下标 = 扫描顺序 i * 8 + 行号，值 = 键表位置，0xFF = 这个位置没有按键 */
#define MATRIX_SLOT_NONE        0xFF
#define MATRIX_SLOT_MAP         { \
    0x14,0x15,0xFF,0xFF,0xFF,0xFF,0x16,0xFF, /* 0 C1  P25  R0→C5R4 R1→C5R5 R6→C5R6 */ \
    0x72,0x73,0x74,0xFF,0xFF,0xFF,0xFF,0xFF, /* 1 C2  P26  R0→C17R2 R1→C17R3 R2→C17R4 */ \
    0x00,0x01,0x02,0x03,0x04,0x05,0xFF,0xFF, /* 2 C3  P27 */ \
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F, /* 3 C4  P30 */ \
    0x10,0x11,0x12,0x13,0xFF,0xFF,0xFF,0xFF, /* 4 C5  P31 */ \
    0x78,0x79,0x7A,0x7B,0x1C,0x1D,0x1E,0x6F, /* 5 C18 P24  R4→C6R4 R5→C6R5 R6→C6R6 R7→C16R7 */ \
    0x77,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 6 C19 P22  R0→C17R7 */ \
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F, /* 7 C8  PA7 */ \
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F, /* 8 C10 PA6 */ \
    0x18,0x19,0x1A,0x1B,0xFF,0xFF,0xFF,0x1F, /* 9 C6  PA4 */ \
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27, /* 10 C7  PA3 */ \
    0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F, /* 11 C14 PA2 */ \
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57, /* 12 C13 PB0 */ \
    0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F, /* 13 C12 PB1 */ \
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47, /* 14 C11 PB2 */ \
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67, /* 15 C15 PB3 */ \
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37, /* 16 C9  PB4 */ \
    0x17,0x06,0xFF,0xFF,0x07,0xFF,0xFF,0xFF, /* 17 C0  PB5  R0→C5R7 R1→C3R6 R4→C3R7 */ \
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F, /* 18 C16 PA0 */ \
    0x70,0x71,0xFF,0xFF,0xFF,0x75,0xFF,0xFF  /* 19 C17 PA1 */ \
}

/* ---- 按位置读取的特殊键（键表位置）----
 * 没有指定的设成 MATRIX_POS_NONE：对应 matrix_status[19]，永远不会按下 */
#define MATRIX_SLOT(c, r)       (((c) - 3) * 8 + (r))
#define MATRIX_POS_NONE         128
#define KEYPOS_FCC_FN           MATRIX_SLOT(9, 7)     /* FCC 测试：进入测试：Fn，物理 C9 R7，现在是 Fn */
#define KEYPOS_FCC_SWITCH       MATRIX_SLOT(6, 4)     /* FCC 测试：切换频点（下）· 也用于进入测试，物理 C18 R4，现在是 ↓ */
#define KEYPOS_FCC_BT0          MATRIX_SLOT(4, 4)     /* FCC 测试：进入测试：蓝牙模式下要按的键，物理 C4 R4，现在是 蓝牙 1 */
#define KEYPOS_FCC_24G          MATRIX_SLOT(7, 0)     /* FCC 测试：进入测试：2.4G 模式下要按的键（没有 2.4G 键时用配对键），物理 C7 R0，现在是 Tab（Fn 层：配对） */
#define KEYPOS_FCC_TX           MATRIX_SLOT(16, 7)    /* FCC 测试：切换定频收发（右），物理 C16 R7、C18 R7，现在是 →（Fn 层：灯速度+） */
#define KEYPOS_FCC_FREQ         MATRIX_SLOT(18, 3)    /* FCC 测试：进入跳频（左），物理 C18 R3，现在是 ←（Fn 层：灯速度-） */
#define KEYPOS_FCC_WAVE         MATRIX_SLOT(18, 2)    /* FCC 测试：切换空载波（上），物理 C18 R2，现在是 ↑ */
#define KEYPOS_WASD_W           MATRIX_SLOT(10, 0)    /* WASD 互换：W，物理 C10 R0，现在是 W（Fn 层：WASD 互换） */
#define KEYPOS_WASD_A           MATRIX_SLOT(6, 0)     /* WASD 互换：A，物理 C6 R0，现在是 A（Fn 层：Win 模式） */
#define KEYPOS_WASD_S           MATRIX_SLOT(8, 0)     /* WASD 互换：S，物理 C8 R0，现在是 S（Fn 层：Mac 模式） */
#define KEYPOS_WASD_D           MATRIX_SLOT(9, 0)     /* WASD 互换：D，物理 C9 R0，现在是 D */
#define KEYPOS_WASD_UP          MATRIX_SLOT(18, 2)    /* WASD 互换：↑，物理 C18 R2，现在是 ↑ */
#define KEYPOS_WASD_LEFT        MATRIX_SLOT(18, 3)    /* WASD 互换：←，物理 C18 R3，现在是 ←（Fn 层：灯速度-） */
#define KEYPOS_WASD_DOWN        MATRIX_SLOT(6, 4)     /* WASD 互换：↓，物理 C18 R4，现在是 ↓ */
#define KEYPOS_WASD_RIGHT       MATRIX_SLOT(16, 7)    /* WASD 互换：→，物理 C16 R7、C18 R7，现在是 →（Fn 层：灯速度+） */

/* ---- 休眠 ----
 * 休眠时直连列全部拉低（MATRX_ALL_LOW），要关掉这些脚的上拉，不然每根列线都经上拉漏电。
 * 下面是 PAD_PU_CTRL / PAD_PU_CTRL_1 里直连列对应的位（每脚 2 位），main.c 和 drv_pmu.c 用
 * 「io_config.h 的上电值去掉这些位」算出休眠时的上拉，换了列线不用改代码。 */
#define MATRIX_DIRECT_PU_MASK   0x00000000u   /* P00-P17 */
#define MATRIX_DIRECT_PU_MASK_1 0x000FFF30u   /* P20-P37 */
/* 深度休眠时输出低 / 高电平的脚：直连列、扩展列控制脚、LED 供电脚（关掉时的电平） */
#define MATRIX_SLEEP_LOW_MASK   0x07FC0000u   /* P22 P23 P24 P25 P26 P27 P30 P31 P32 */
#define MATRIX_SLEEP_HIGH_MASK  0x00000000u   /* 无 */

/* ---- 指示灯 ----
 * LED_x_ON / OFF：点亮 / 熄灭；熄灭时放开引脚（和列线复用时扫描要用）
 * 和列线复用的灯靠 LED 供电脚开关：扫描前关掉供电，扫完再点灯 */
#define MATRIX_LED_VDD_PIN          19      /* P23 */
#define MATRIX_LED_VDD_ON           1       /* 供电脚输出这个电平时灯有电 */
#define MATRIX_LED_VDD_EN           {OM_GPIO0->DATAOUTSET = (1u<<19); OM_GPIO0->OUTENSET = (1u<<19);}   /* P23 */
#define MATRIX_LED_VDD_DIS          {OM_GPIO0->DATAOUTCLR = (1u<<19); OM_GPIO0->OUTENSET = (1u<<19);}
#define MATRIX_LED_NUM_ON           {OM_GPIO0->DATAOUTCLR = (1u<<20); OM_GPIO0->OUTENSET = (1u<<20);}   /* P24，和列 C18 复用 */
#define MATRIX_LED_NUM_OFF          {OM_GPIO0->DATAOUTSET = (1u<<20); OM_GPIO0->OUTENCLR = (1u<<20);}
#define MATRIX_LED_NUM_PIN          20
#define MATRIX_LED_NUM_LEVEL        0       /* 点亮电平 */
#define MATRIX_LED_NUM_DEDICATED    0       /* 1 = 独立引脚（不和列线复用） */
#define MATRIX_LED_CAPS_ON          {OM_GPIO0->DATAOUTCLR = (1u<<21); OM_GPIO0->OUTENSET = (1u<<21);}   /* P25，和列 C1 复用 */
#define MATRIX_LED_CAPS_OFF         {OM_GPIO0->DATAOUTSET = (1u<<21); OM_GPIO0->OUTENCLR = (1u<<21);}
#define MATRIX_LED_CAPS_PIN         21
#define MATRIX_LED_CAPS_LEVEL       0       /* 点亮电平 */
#define MATRIX_LED_CAPS_DEDICATED   0       /* 1 = 独立引脚（不和列线复用） */
#define MATRIX_LED_LOW_ON           {OM_GPIO0->DATAOUTCLR = (1u<<22); OM_GPIO0->OUTENSET = (1u<<22);}   /* P26，和列 C2 复用 */
#define MATRIX_LED_LOW_OFF          {OM_GPIO0->DATAOUTSET = (1u<<22); OM_GPIO0->OUTENCLR = (1u<<22);}
#define MATRIX_LED_LOW_PIN          22
#define MATRIX_LED_LOW_LEVEL        0       /* 点亮电平 */
#define MATRIX_LED_LOW_DEDICATED    0       /* 1 = 独立引脚（不和列线复用） */

/* ---- Mac 独立键表 ----
 * 0：Win / Mac 共用 matrix_key_tbl_win*.h，Mac 模式下 key_process() 把 Alt 和 Win 对调
 * 1：Mac 模式用 matrix_key_tbl_mac*.h（存在 Flash 第 8、12 页），不再对调；切换 Win / Mac 时换表 */
#define MATRIX_MAC_KEYMAP       0

#endif /* __MATRIX_CFG_H */
