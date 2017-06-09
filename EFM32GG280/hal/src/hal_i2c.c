/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_i2c.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_i2c.h"


/************************************************************!
*Function: I2c_Init
*Description: IIC³õÊ¼»¯º¯Êý
*Input: null
*Output: null
*Return: null
*************************************************************/
void I2c_Init(void)
{
	CMU_ClockEnable(cmuClock_I2C0,true);

	I2C_Init_TypeDef i2cinit=I2C_INIT_DEFAULT;

	I2C_Init(I2C0,&i2cinit);

	I2C0->ROUTE=I2C_ROUTE_SDAPEN|I2C_ROUTE_SCLPEN|I2C_ROUTE_LOCATION_LOC0;

	GPIO_PinModeSet(gpioPortA,1,gpioModeWiredAndPullUpFilter,1);
	GPIO_PinModeSet(gpioPortA,0,gpioModeWiredAndPullUpFilter,1);
}

