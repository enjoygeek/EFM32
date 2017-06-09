/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_aes.c
*Version: 0.1.0
*Date: 2017-03-02
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_aes.h"


/************************************************************!
*Function: AES128_CBC_Encryption
*Description: AES 128 CBC 加密函数
*Input:  in: 待加密数组首地址
*       out: 已加密数组首地址
*Output: null
*Return: null
*************************************************************/
void AES128_CBC_Encryption(uint8_t *in,uint8_t *out,uint32_t lens)
{
	uint8_t A_Key[16] ={ 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89 ,
		                 0x9A, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xF0, 0x00 };

	uint8_t B_Key[16] ={ 0x00, 0xF0, 0xEF, 0xDE, 0xCD, 0xBC, 0xAB, 0x9A ,
		                 0x89, 0x78, 0x67, 0x56, 0x45, 0x34, 0x23, 0x12 };

	//AES_CBC_XOR(in,A_Key);

	AES_CBC128(out,in,lens,A_Key,B_Key,true);
}
/************************************************************!
*Function: AES128_CBC_Decryption
*Description: AES 128 CBC 解密函数
*Input:  in: 待解密数组首地址
*       out: 已解密数组首地址
*Output: null
*Return: null
*************************************************************/
void AES128_CBC_Decryption(uint8_t *in,uint8_t *out,uint32_t lens)
{
	uint8_t A_Key[16] ={ 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89 ,
		                 0x9A, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xF0, 0x00 };

	uint8_t B_Key[16] ={ 0x00, 0xF0, 0xEF, 0xDE, 0xCD, 0xBC, 0xAB, 0x9A ,
		                 0x89, 0x78, 0x67, 0x56, 0x45, 0x34, 0x23, 0x12 };

    AES_DecryptKey128(A_Key,A_Key);

    AES_CBC128(out,in,lens,A_Key,B_Key,false);

   // AES_CBC_XOR(out,B_Key);
}
/************************************************************!
*Function: AES_CBC_XOR
*Description: AES CBC数组异或输出
*Input: null
*Output: null
*Return: null
*************************************************************/
void AES_CBC_XOR(uint8_t *in,uint8_t *key)
{
    for(uint8_t i=0;i<16;i++)
        in[i]=in[i]^key[i];
}

