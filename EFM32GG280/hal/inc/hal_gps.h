/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_gpio.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_GPS_H_
#define HAL_GPS_H_


#include <stdbool.h>
#include <string.h>
#include "sysdef.h"


//设置GPS数据缓冲数组长度
#define GPS_BUFF_LEN 80


bool Gps_Timing(void);
bool Gps_Abstime_Process(uint8_t data);
bool Get_Gps_Abstime(uint8_t *data,uint8_t lens);
uint8_t Get_Specified_Char_Index(uint8_t *data,uint8_t lens,uint8_t ch,uint8_t loction);
uint8_t Check_Gps_Crc(uint8_t *data,uint8_t lens);


#endif

