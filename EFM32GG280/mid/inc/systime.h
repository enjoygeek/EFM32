/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: systime.h
*Version: 0.1.0
*Date: 2016-11-22
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef SYSTIME_H_
#define SYSTIME_H_


void GPSTime2R8025TRTCTime(uint8_t *data,uint8_t lens);
void P101Time2R8025TRTCTime(uint8_t *data,uint8_t lens);
uint32_t R8025TRTCTime2UnixTime(void);
UTCTIME UTCTime2LocalTime(UTCTIME *UTC);
bool UnixTime2P101Time(UNIXTIME *UT,uint8_t *data,uint8_t lens);
void NowTime2P101Time(uint8_t *data,uint8_t lens);
void PrintOut_NowTime(void);
void Sys_Tick(void);
void Get_Sys_Time(UNIXTIME *UT);
void SysTimer_10ms(TimerHandle_t xTimer);
uint32_t Get_YYMMDD(uint8_t *data,uint8_t lens);
bool Is_Valid_101Clock(uint8_t *data,uint8_t lens);


#endif

