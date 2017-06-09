/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_adc.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_adc.h"


/************************************************************!
*Function: Adc0_Init
*Description: ADC0 SCAN初始化函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Adc0_Init(void)
{
	CMU_ClockEnable(cmuClock_ADC0,true);

	ADC_Init_TypeDef adc0init=ADC_INIT_DEFAULT;
	ADC_InitScan_TypeDef adc0scaninit=ADC_INITSCAN_DEFAULT;

	adc0init.ovsRateSel=adcOvsRateSel2; //关闭过采样
	adc0init.timebase=ADC_TimebaseCalc(0);
	adc0init.prescale=ADC_PrescaleCalc(7000000,0);

	adc0scaninit.reference=adcRefVDD; //3.31V
	adc0scaninit.resolution=adcRes12Bit; //12位分辨率
	adc0scaninit.input=ADC_SCANCTRL_INPUTMASK_CH4| //轮询通道CH4\CH5\CH6(PD4\PD5\PD6)
	                   ADC_SCANCTRL_INPUTMASK_CH5|
	                   ADC_SCANCTRL_INPUTMASK_CH6;

	ADC_Init(ADC0,&adc0init);

	ADC_InitScan(ADC0,&adc0scaninit);
}
/************************************************************!
*Function: Adc0_Errata
*Description: ADC0勘误函数
*Input: null
*Output: null
*Return: true：需要校正
*        false：不需要校正
*************************************************************/
bool Adc0_Errata(void)
{
	SYSTEM_ChipRevision_TypeDef ChipRev;

	SYSTEM_ChipRevisionGet(&ChipRev);

	if((ChipRev.major==1)&&(ChipRev.minor==1))
		return true;
	else
		return false;
}

