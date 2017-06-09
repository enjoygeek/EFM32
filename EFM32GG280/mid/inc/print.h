/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: print.h
*Version: 0.1.0
*Date: 2017-01-03
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef PRINT_H_
#define PRINT_H_


#include <stdarg.h>
#include <stdlib.h>


#pragma pack(1)
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA;
	struct
	{
		uint8_t U03:4;
		uint8_t U47:4;
	};
}UNSIGN_U8_PRINT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	int8_t DATA;
	struct
	{
		uint8_t DAT:7;  //Êý¾ÝÓò
		uint8_t SIGN:1; //·ûºÅÓò
	};
}SIGN_U8_PRINT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint16_t        DATA;
	uint8_t         U8[2];
	UNSIGN_U8_PRINT U4[2];
}UNSIGN_U16_PRINT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	int16_t DATA;
	struct
	{
		uint16_t DAT:15;
		uint8_t  SIGN:1;
	};
}SIGN_U16_PRINT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint32_t          DATA;
	UNSIGN_U16_PRINT  U16[2];
	uint8_t           U8[4];
	UNSIGN_U8_PRINT   U4[4];
}UNSIGN_U32_PRINT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	int32_t DATA;
	struct
	{
		uint32_t DAT:31;
		uint8_t  SIGN:1;
	};
}SIGN_U32_PRINT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA;
	struct
	{
		uint8_t U03:4;
		uint8_t U47:4;
	};
}SINGLEBYTE_PRINT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint16_t         U16;
	uint8_t          U8[2];
	SINGLEBYTE_PRINT U4[2];
}DOUBLEBYTE_PRINT;
#pragma pack()


void DEBUG_PRINT(char *ind, ...);
void DEBUG_PRINT_NISR(char *ind, ...);
void DEBUG_PRINT_TIME(uint8_t *time);
void PRINT_U8(uint8_t data);
void PRINT_S8(int8_t data);
void PRINT_U8CHAR(uint8_t data);
void PRINT_U8DEC(uint8_t data);
void DEBUG_PRINT_U8_ARRAY(uint8_t *data,uint32_t lens);
void DEBUG_PRINT_U8_ARRAY_NISR(uint8_t *data,uint32_t lens);
void DEBUG_PRINT_S8_ARRAY(int8_t *data,uint32_t lens);
void DEBUG_PRINT_S8_ARRAY_NISR(int8_t *data,uint32_t lens);
void DEBUG_PRINT_U8CHAR_ARRAY(uint8_t *data,uint32_t lens);
void DEBUG_PRINT_U8CHAR_ARRAY_NISR(uint8_t *data,uint32_t lens);
void PRINT_U16(uint16_t data);
void PRINT_S16(int16_t data);
void PRINT_U16CHAR(uint16_t data);
void PRINT_U16DEC(uint16_t data);
void DEBUG_PRINT_U16_ARRAY(uint16_t *data,uint32_t lens);
void DEBUG_PRINT_U16_ARRAY_NISR(uint16_t *data,uint32_t lens);
void DEBUG_PRINT_S16_ARRAY(int16_t *data,uint32_t lens);
void DEBUG_PRINT_S16_ARRAY_NISR(int16_t *data,uint32_t lens);
void DEBUG_PRINT_U16CHAR_ARRAY(uint16_t *data,uint32_t lens);
void DEBUG_PRINT_U16CHAR_ARRAY_NISR(uint16_t *data,uint32_t lens);
void PRINT_U32(uint32_t data);
void PRINT_S32(int32_t data);
void PRINT_U32CHAR(uint32_t data);
void PRINT_U32DEC(uint32_t data);
void DEBUG_PRINT_U32_ARRAY(uint32_t *data,uint32_t lens);
void DEBUG_PRINT_U32_ARRAY_NISR(uint32_t *data,uint32_t lens);
void DEBUG_PRINT_S32_ARRAY(int32_t *data,uint32_t lens);
void DEBUG_PRINT_S32_ARRAY_NISR(int32_t *data,uint32_t lens);
void DEBUG_PRINT_U32CHAR_ARRAY(uint32_t *data,uint32_t lens);
void DEBUG_PRINT_U32CHAR_ARRAY_NISR(uint32_t *data,uint32_t lens);


#endif

