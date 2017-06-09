/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_wdog.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_wdog.h"

/************************************************************!
*Function: Wdog_Init
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Wdog_Init(void)
{
	CMU_OscillatorEnable(cmuOsc_ULFRCO,true,true);

	WDOG_Init_TypeDef wdoginit=WDOG_INIT_DEFAULT;

	wdoginit.enable=false;
	wdoginit.debugRun=false;
	wdoginit.clkSel=wdogClkSelULFRCO; //1K频率
	wdoginit.perSel=wdogPeriod_4k; //4K时钟周期,大约4s
	wdoginit.swoscBlock=0;

	WDOG_Init(&wdoginit);
}
/************************************************************!
*Function: Wdog_Feed
*Description: WDOG喂狗函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Wdog_Feed(void)
{
	WDOG_Feed();
}
/************************************************************!
*Function: Wdog_Enable
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Wdog_Enable(void)
{
	WDOG->CTRL|=0x00000001;

	Wdog_Feed();
}
/************************************************************!
*Function: Wdog_Disable
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Wdog_Disable(void)
{
	WDOG->CTRL&=0xFFFFFFFE;
}

