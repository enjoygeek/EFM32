/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_r8025t.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_R8025T_H_
#define HAL_R8025T_H_


#include <string.h>
#include <stdbool.h>
#include "em_i2c.h"


#define  RX8025T_SLAVE_ADDR  0x64
/************************************************************!
*Function: R8025T_SINGLEBYTE
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
}R8025T_SINGLEBYTE;
/************************************************************!
*Function: R8025TTIME
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[6];
	struct
	{
		uint8_t SECOND;
		uint8_t MINUTE;
		uint8_t HOUR;
		uint8_t DAY;
		uint8_t MONTH;
		uint8_t YEAR;
	};
}R8025TTIME;


bool Rx8025t_Init(void);
void R8025t_Reset(void);
bool Get_R8025t_RtcTime(uint8_t *data,uint8_t lens);
bool Set_R8025t_RtcTime(uint8_t *data,uint8_t lens);
bool Check_R8025t_Data(uint8_t *data,uint8_t lens);
bool Is_Valid_R8025t_Clock(uint8_t *data,uint8_t lens);
uint8_t Rx8025t_week2data(uint8_t week);
uint8_t Rx8025t_data2week(uint8_t data);
uint8_t Rx8025t_ReadByte(uint8_t addr,uint8_t *data,uint8_t lens);
void Rx8025t_WriteByte(uint8_t addr,uint8_t *data,uint8_t lens);


#endif
