/*************************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: unixtime.h
*Version: 0.1.0
*Date: 2016-11-05
*Author: WANGJIANHUI
*Description:
*************************************************************************/
#ifndef UNIXTIME_H_
#define UNIXTIME_H_


#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


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
}UTCTIME;


uint32_t UTCTime2UnixTime(uint8_t *data,uint8_t lens);
void UnixTime2LocalTime(uint32_t unixtime,uint8_t *data,uint8_t lens);
uint32_t LocalTime2UnixTime(uint8_t *data,uint8_t lens);
uint8_t YMD2WeekDay(uint8_t *data,uint8_t lens);
uint8_t Is_Valid_Clock(uint8_t *data,uint8_t lens);
uint8_t Is_Valid_Bcd(uint8_t bcd);
uint8_t Hex2Bcd(uint8_t hex);
uint8_t Bcd2Hex(uint8_t bcd);


#endif

