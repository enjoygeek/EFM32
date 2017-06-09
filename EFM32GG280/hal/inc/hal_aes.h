/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_aes.h
*Version: 0.1.0
*Date: 2017-03-02
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_AES_H_
#define HAL_AES_H_


#include <stdbool.h>
#include "em_aes.h"


void AES128_CBC_Encryption(uint8_t *in,uint8_t *out,uint32_t lens);
void AES128_CBC_Decryption(uint8_t *in,uint8_t *out,uint32_t lens);
void AES_CBC_XOR(uint8_t *in,uint8_t *key);


#endif

