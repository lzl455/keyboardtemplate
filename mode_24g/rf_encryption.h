/**
 * @file rf_encryption.h
 * @author
 * @brief
 * @version 0.2
 * @date 2026-03-06
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _RF_ENCRYPTION_H_
#define _RF_ENCRYPTION_H_

#include "DEFINES.H"


/* ============================================================
 *  Configuration
 * ============================================================ */
#define RF_ENCRYPT_ENABLE    1

#if RF_ENCRYPT_ENABLE

#define CFG_FOR_DONGLE  0
#define CFG_FOR_TX      !CFG_FOR_DONGLE

#if CFG_FOR_DONGLE
#define rf_decrypt      cipher_process
#endif 

#define PACKET_TYPE_FOR_3MODE_1KHZ_MOUSE        0xA0
#define PACKET_TYPE_FOR_3MODE_500HZ_MOUSE       0xA1
#define PAKCET_TYPE_FOR_NORMAL_MOUSE            0xA2
#define PACKET_TYPE_FOR_NORMAL_KEYBOARD         0xA3 
#define PACKET_TYPE_FOR_OTHERS                  0xA4 
#define PACKET_TYPE_FOR_KB_BITMAP               0xA5

#define ENCRYPT_PACKET_TYPE_IDX                 (17-1)

#define ENCRYPT_TOTAL_LENGTH                    19


/* ============================================================
 *  Public API
 * ============================================================ */
void rf_encrypt(unsigned char *dat);
void rf_decrypt(unsigned char *dat);
void cipher_process(unsigned char *dat);
void rf_encrypt_test(void);

#endif /* RF_ENCRYPT_ENABLE */

#endif  /* RF_ENCRYPT_ENABLE */
