/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_rtc.h
*Version: 0.1.0
*Date: 2016-11-22
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_RTC_H_
#define HAL_RTC_H_


#include <stddef.h>
#include "em_cmu.h"
#include "em_rtc.h"


/**********函数指针回调声明**********/
typedef void(*RTC_INT_CB)(void);


void Rtc_Init(void);
uint16_t Get_Rtc_Cnt(void);
void Set_Rtc_Cnt(uint16_t cnt);
void Rtc_Int_CallBack(RTC_INT_CB func);
void RTC_IRQHandler(void);


#endif

