/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_gpio.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "em_cmu.h"
#include "hal_gpio.h"


/**********GPIO中断回调注册变量**********/
GPIO_INT_CB GpioIntCb=NULL;
/************************************************************!
*Function: Gpio_Init
*Description: GPIO初始化函数,包括LED和GPS开关函数的初始化
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gpio_Init(void)
{
	GPIO_PinModeSet(STATUS_LED1_PORT,STATUS_LED1_PIN,gpioModePushPull,1);               //LED1:PE7
	GPIO_PinOutClear(STATUS_LED1_PORT,STATUS_LED1_PIN);

	GPIO_PinModeSet(STATUS_LED2_PORT,STATUS_LED2_PIN,gpioModePushPull,1);                     //LED2:PA3
	GPIO_PinOutClear(STATUS_LED2_PORT,STATUS_LED2_PIN);

	GPIO_PinModeSet(GPS_POWER_CONTROL_PORT,GPS_POWER_CONTROL_PIN,gpioModePushPull,1); //GPS_CRL
	GPIO_PinOutSet(GPRS_POWER_CONTROL_PORT,GPRS_POWER_CONTROL_PIN);  //拉高GPS_CRL,关闭GPS
	//GPIO_PinOutClear(GPS_POWER_CONTROL_PORT,GPS_POWER_CONTROL_PIN);

	GPIO_PinModeSet(GPRS_POWER_CONTROL_PORT,GPRS_POWER_CONTROL_PIN,gpioModePushPull,1); //打开GPRS电源
	GPIO_PinOutSet(GPRS_POWER_CONTROL_PORT,GPRS_POWER_CONTROL_PIN);
	//GPIO_PinOutClear(GPRS_POWER_CONTROL_PORT,GPRS_POWER_CONTROL_PIN);

	GPIO_PinModeSet(ESAM_CTRL_PORT,ESAM_CTRL_PIN,gpioModePushPull,1);
	GPIO_PinOutClear(ESAM_CTRL_PORT,ESAM_CTRL_PIN);
	//GPIO_PinOutSet(ESAM_CTRL_PORT,ESAM_CTRL_PIN);

	GPIO_PinModeSet(gpioPortC,15,gpioModePushPull,1);
	GPIO_PinOutClear(gpioPortC,15);
}
/************************************************************!
*Function: Status_Led1_Toggle
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Status_Led1_Toggle(void)
{
	GPIO_PinOutToggle(STATUS_LED1_PORT,STATUS_LED1_PIN);
}
/************************************************************!
*Function: Status_Led1_Set
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Status_Led1_Set(void)
{
	GPIO_PinOutSet(STATUS_LED1_PORT,STATUS_LED1_PIN);
}
/************************************************************!
*Function: Status_Led1_Clear
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Status_Led1_Clear(void)
{
	GPIO_PinOutClear(STATUS_LED1_PORT,STATUS_LED1_PIN);
}
/************************************************************!
*Function: Status_Led2_Toggle
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Status_Led2_Toggle(void)
{
	GPIO_PinOutToggle(STATUS_LED2_PORT,STATUS_LED2_PIN);
}
/************************************************************!
*Function: Status_Led2_Set
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Status_Led2_Set(void)
{
	GPIO_PinOutSet(STATUS_LED2_PORT,STATUS_LED2_PIN);
}
/************************************************************!
*Function: Status_Led2_Clear
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Status_Led2_Clear(void)
{
	GPIO_PinOutClear(STATUS_LED2_PORT,STATUS_LED2_PIN);
}
/************************************************************!
*Function: Mx25_Gpio_Pin
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Mx25_Gpio_Pin(void)
{
	GPIO_PinModeSet(MX25_ERASE_PORT,MX25_ERASE_PIN,gpioModeInput,1);

	if(GPIO_PinInGet(MX25_ERASE_PORT,MX25_ERASE_PIN)==false)
		return true;

	return false;
}
/************************************************************!
*Function: Gps_Power_On
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gps_Power_On(void)
{
	GPIO_PinOutClear(GPS_POWER_CONTROL_PORT,GPS_POWER_CONTROL_PIN);
}
/************************************************************!
*Function: Gps_Power_Off
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gps_Power_Off(void)
{
	GPIO_PinOutSet(GPS_POWER_CONTROL_PORT,GPS_POWER_CONTROL_PIN);
}
/************************************************************!
*Function: Gpio_Int_CallBack
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Gpio_Int_CallBack(GPIO_INT_CB func)
{
	GpioIntCb=func;
}
/************************************************************!
*Function: GPIO_ODD_IRQHandler
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void GPIO_ODD_IRQHandler(void)
{
	uint32_t intflags=GPIO_IntGet();

	GPIO_IntClear(intflags);

	if(GpioIntCb!=NULL)
		(*GpioIntCb)();
}

