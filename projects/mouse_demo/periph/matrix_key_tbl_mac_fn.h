/*******************************************************************************
* 文件名称: matrix_key_tbl_mac_fn.h
* 文件描述: Mac 模式 Fn 层（按住 Fn 时）键表，由 tools/om62xx-keymap 矩阵表配置器生成
*
* 只在 matrix_cfg.h 里 MATRIX_MAC_KEYMAP 为 1 时编进固件，Mac 模式下用这张表，不再对调 Alt / Win：
* 修饰键按 Mac 写，iLEFT_GUI = ⌘ Command，iLEFT_ALT = ⌥ Option。
*
* 每个键 2 字节：键码, 类型。顺序 C3..C18 × R0..R7（C18 只有 R0-R3），共 124 个，
* 第 n 个键对应 matrix_status[n/8+3] 的第 n%8 位。物理按键和键表位置的对应关系见 matrix_cfg.h。
* 注意：键表存在 Flash 里，烧录后长按 Fn+Esc 2 秒恢复出厂，新键表才生效。
*******************************************************************************/
#ifndef _MATRIX_KEY_MAC_FN_TBL_
#define _MATRIX_KEY_MAC_FN_TBL_
#include "key_code_define.h"

unsigned char           const   matrixkey_mac_fn_tbl[] =
{
// C3
    iSTD_NULL,          iKEY_TYPE_STD|iKEY_PRESS|iLEFT_SHIFT,   // C3 R0
    iSTD_U,             iKEY_TYPE_STD|iKEY_PRESS,               // C3 R1
    iMMK_PREV_TRACK_L,  iKEY_TYPE_MMK|iMMK_PREV_TRACK_H,        // C3 R2
    iMMK_VOLUMEDOWN_L,  iKEY_TYPE_MMK|iMMK_VOLUMEDOWN_H,        // C3 R3
    iMMK_MUTE_L,        iKEY_TYPE_MMK|iMMK_MUTE_H,              // C3 R4
    iSTD_RETURN,        iKEY_TYPE_STD|iKEY_PRESS,               // C3 R5
    iLED_MODE_0,        iKEY_TYPE_SPECIAL,                      // C3 R6 ← C0 R1
    iSTD_KEYBOARD_LANG1, iKEY_TYPE_STD|iKEY_PRESS,               // C3 R7 ← C0 R4
// C4
    iSTD_NULL,          iKEY_TYPE_STD|iKEY_PRESS|iLEFT_CTRL,    // C4 R0
    iSTD_K,             iKEY_TYPE_STD|iKEY_PRESS,               // C4 R1
    iSTD_P,             iKEY_TYPE_STD|iKEY_PRESS,               // C4 R2
    iMODE_KEY_BT1,      iKEY_TYPE_SPECIAL,                      // C4 R3
    iMODE_KEY_BT0,      iKEY_TYPE_SPECIAL,                      // C4 R4
    iLED_MODE_CYC_ADD,  iKEY_TYPE_SPECIAL,                      // C4 R5
    iSTD_ESCAPE,        iKEY_TYPE_STD|iKEY_PRESS,               // C4 R6
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C4 R7
// C5
    iSTD_SPACE,         iKEY_TYPE_STD|iKEY_PRESS,               // C5 R0
    iSTD_I,             iKEY_TYPE_STD|iKEY_PRESS,               // C5 R1
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C5 R2
    iKEY_WIN_LOCK,      iKEY_TYPE_SPECIAL,                      // C5 R3
    iSTD_C,             iKEY_TYPE_STD|iKEY_PRESS,               // C5 R4 ← C1 R0
    iLED_MODE_2,        iKEY_TYPE_SPECIAL,                      // C5 R5 ← C1 R1
    iSTD_KEYBOARD_LANG2, iKEY_TYPE_STD|iKEY_PRESS,               // C5 R6 ← C1 R6
    iSTD_Z,             iKEY_TYPE_STD|iKEY_PRESS,               // C5 R7 ← C0 R0
// C6
    iKEY_WIN,           iKEY_TYPE_SPECIAL,                      // C6 R0
    iMMK_MUTE_L,        iKEY_TYPE_MMK|iMMK_MUTE_H,              // C6 R1
    iSTD_O,             iKEY_TYPE_STD|iKEY_PRESS,               // C6 R2
    iMMK_CALCULATOR_L,  iKEY_TYPE_MMK|iMMK_CALCULATOR_H,        // C6 R3
    iSTD_DOWN,          iKEY_TYPE_STD|iKEY_PRESS,               // C6 R4 ← C18 R4
    iSTD_M,             iKEY_TYPE_STD|iKEY_PRESS,               // C6 R5 ← C18 R5
    iSTD_NULL,          iKEY_TYPE_STD|iKEY_PRESS|iLEFT_GUI,     // C6 R6 ← C18 R6
    iLED_OFF,           iKEY_TYPE_SPECIAL,                      // C6 R7
// C7
    iMODE_KEY_CONN,     iKEY_TYPE_SPECIAL,                      // C7 R0
    iSTD_WAVE,          iKEY_TYPE_STD|iKEY_PRESS,               // C7 R1
    iMMK_PLAY_PAUSE_L,  iKEY_TYPE_MMK|iMMK_PLAY_PAUSE_H,        // C7 R2
    iSTD_Y,             iKEY_TYPE_STD|iKEY_PRESS,               // C7 R3
    iSTD_END,           iKEY_TYPE_STD|iKEY_PRESS,               // C7 R4
    iSTD_7,             iKEY_TYPE_STD|iKEY_PRESS,               // C7 R5
    iSTD_SEMICOLON,     iKEY_TYPE_STD|iKEY_PRESS,               // C7 R6
    iKEY_DEFAULT,       iKEY_TYPE_SPECIAL,                      // C7 R7
// C8
    iKEY_MAC,           iKEY_TYPE_SPECIAL,                      // C8 R0
    iSTD_SUBTRACT,      iKEY_TYPE_STD|iKEY_PRESS,               // C8 R1
    iMMK_NEXT_TRACK_L,  iKEY_TYPE_MMK|iMMK_NEXT_TRACK_H,        // C8 R2
    iLED_MODE_3,        iKEY_TYPE_SPECIAL,                      // C8 R3
    iSTD_SQUARE_BRACKET_LEFT, iKEY_TYPE_STD|iKEY_PRESS,               // C8 R4
    iSTD_8,             iKEY_TYPE_STD|iKEY_PRESS,               // C8 R5
    iSTD_INSERT,        iKEY_TYPE_STD|iKEY_PRESS,               // C8 R6
    iSTD_N,             iKEY_TYPE_STD|iKEY_PRESS,               // C8 R7
// C9
    iSTD_D,             iKEY_TYPE_STD|iKEY_PRESS,               // C9 R0
    iSTD_ADD,           iKEY_TYPE_STD|iKEY_PRESS,               // C9 R1
    iMMK_MAIL_L,        iKEY_TYPE_MMK|iMMK_MAIL_H,              // C9 R2
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C9 R3
    iSTD_SQUARE_BRACKET_RIGHT, iKEY_TYPE_STD|iKEY_PRESS,               // C9 R4
    iSTD_9,             iKEY_TYPE_STD|iKEY_PRESS,               // C9 R5
    iSTD_QUOTATION_SIGN, iKEY_TYPE_STD|iKEY_PRESS,               // C9 R6
    iMODE_KEY_FN,       iKEY_TYPE_SPECIAL,                      // C9 R7
// C10
    iKEY_DIR_WASD,      iKEY_TYPE_SPECIAL,                      // C10 R0
    iSTD_BACKSPACE,     iKEY_TYPE_STD|iKEY_PRESS,               // C10 R1
    iKEY_LOCK,          iKEY_TYPE_SPECIAL,                      // C10 R2
    iSTD_SNAPSHOT,      iKEY_TYPE_STD|iKEY_PRESS,               // C10 R3
    iSTD_HOME,          iKEY_TYPE_STD|iKEY_PRESS,               // C10 R4
    iSTD_DELETE,        iKEY_TYPE_STD|iKEY_PRESS,               // C10 R5
    iSTD_H,             iKEY_TYPE_STD|iKEY_PRESS,               // C10 R6
    iSTD_FULLSTOP,      iKEY_TYPE_STD|iKEY_PRESS,               // C10 R7
// C11
    iSTD_E,             iKEY_TYPE_STD|iKEY_PRESS,               // C11 R0
    iSTD_L,             iKEY_TYPE_STD|iKEY_PRESS,               // C11 R1
    iMMK_CALCULATOR_L,  iKEY_TYPE_MMK|iMMK_CALCULATOR_H,        // C11 R2
    iSTD_APP,           iKEY_TYPE_STD|iKEY_PRESS,               // C11 R3
    iSTD_PAUSE,         iKEY_TYPE_STD|iKEY_PRESS,               // C11 R4
    iLED_BRIGHT_ADD,    iKEY_TYPE_SPECIAL,                      // C11 R5
    iSTD_EUROPE1,       iKEY_TYPE_STD|iKEY_PRESS,               // C11 R6
    iSTD_SLASH,         iKEY_TYPE_STD|iKEY_PRESS,               // C11 R7
// C12
    iSTD_Q,             iKEY_TYPE_STD|iKEY_PRESS,               // C12 R0
    iSTD_J,             iKEY_TYPE_STD|iKEY_PRESS,               // C12 R1
    iMMK_WWW_HOME_L,    iKEY_TYPE_MMK|iMMK_WWW_HOME_H,          // C12 R2
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C12 R3
    iSTD_BACKSLASH,     iKEY_TYPE_STD|iKEY_PRESS,               // C12 R4
    iSTD_0,             iKEY_TYPE_STD|iKEY_PRESS,               // C12 R5
    iLED_BRIGHT_SUB,    iKEY_TYPE_SPECIAL,                      // C12 R6
    iSTD_COMMA,         iKEY_TYPE_STD|iKEY_PRESS,               // C12 R7
// C13
    iSTD_B,             iKEY_TYPE_STD|iKEY_PRESS,               // C13 R0
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C13 R1
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C13 R2
    iSTD_NUMLOCK,       iKEY_TYPE_STD|iKEY_PRESS,               // C13 R3
    iSTD_NUMPAD7,       iKEY_TYPE_STD|iKEY_PRESS,               // C13 R4
    iSTD_NUMPAD4,       iKEY_TYPE_STD|iKEY_PRESS,               // C13 R5
    iSTD_NUMPAD1,       iKEY_TYPE_STD|iKEY_PRESS,               // C13 R6
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C13 R7
// C14
    iSTD_V,             iKEY_TYPE_STD|iKEY_PRESS,               // C14 R0
    iLED_MODE_4,        iKEY_TYPE_SPECIAL,                      // C14 R1
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C14 R2
    iSTD_NUMPAD_DIVIDE, iKEY_TYPE_STD|iKEY_PRESS,               // C14 R3
    iSTD_NUMPAD8,       iKEY_TYPE_STD|iKEY_PRESS,               // C14 R4
    iSTD_NUMPAD5,       iKEY_TYPE_STD|iKEY_PRESS,               // C14 R5
    iSTD_NUMPAD2,       iKEY_TYPE_STD|iKEY_PRESS,               // C14 R6
    iSTD_NUMPAD0,       iKEY_TYPE_STD|iKEY_PRESS,               // C14 R7
// C15
    iSTD_R,             iKEY_TYPE_STD|iKEY_PRESS,               // C15 R0
    iSTD_6,             iKEY_TYPE_STD|iKEY_PRESS,               // C15 R1
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C15 R2
    iSTD_NUMPAD_MULTIPLY, iKEY_TYPE_STD|iKEY_PRESS,               // C15 R3
    iSTD_NUMPAD9,       iKEY_TYPE_STD|iKEY_PRESS,               // C15 R4
    iSTD_NUMPAD6,       iKEY_TYPE_STD|iKEY_PRESS,               // C15 R5
    iSTD_NUMPAD3,       iKEY_TYPE_STD|iKEY_PRESS,               // C15 R6
    iSTD_NUMPAD_FULLSTOP, iKEY_TYPE_STD|iKEY_PRESS,               // C15 R7
// C16
    iSTD_T,             iKEY_TYPE_STD|iKEY_PRESS,               // C16 R0
    iMMK_VOLUMEUP_L,    iKEY_TYPE_MMK|iMMK_VOLUMEUP_H,          // C16 R1
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C16 R2
    iSTD_NUMPAD_SUBTRACT, iKEY_TYPE_STD|iKEY_PRESS,               // C16 R3
    iSTD_NUMPAD_ADD,    iKEY_TYPE_STD|iKEY_PRESS,               // C16 R4
    iSTD_NULL,          iKEY_TYPE_INVALID,                      // C16 R5
    iSTD_NUMPAD_ENTER,  iKEY_TYPE_STD|iKEY_PRESS,               // C16 R6
    iLED_SPEED_ADD,     iKEY_TYPE_SPECIAL,                      // C16 R7 ← C16 R7、C18 R7
// C17
    iSTD_X,             iKEY_TYPE_STD|iKEY_PRESS,               // C17 R0
    iLED_MODE_1,        iKEY_TYPE_SPECIAL,                      // C17 R1
    iSTD_F,             iKEY_TYPE_STD|iKEY_PRESS,               // C17 R2 ← C2 R0
    iMMK_MEDIA_SELECT_L, iKEY_TYPE_MMK|iMMK_MEDIA_SELECT_H,      // C17 R3 ← C2 R1
    iMMK_STOP_L,        iKEY_TYPE_MMK|iMMK_STOP_H,              // C17 R4 ← C2 R2
    iSTD_NULL,          iKEY_TYPE_STD|iKEY_PRESS|iRIGHT_SHIFT,  // C17 R5
    iMMK_MUTE_L,        iKEY_TYPE_MMK|iMMK_MUTE_H,              // C17 R6  没有物理按键
    iSTD_CAPITAL,       iKEY_TYPE_STD|iKEY_PRESS,               // C17 R7 ← C19 R0
// C18
    iSTD_APP,           iKEY_TYPE_STD|iKEY_PRESS,               // C18 R0
    iSTD_G,             iKEY_TYPE_STD|iKEY_PRESS,               // C18 R1
    iSTD_UP,            iKEY_TYPE_STD|iKEY_PRESS,               // C18 R2
    iLED_SPEED_SUB,     iKEY_TYPE_SPECIAL,                      // C18 R3
};

#endif
