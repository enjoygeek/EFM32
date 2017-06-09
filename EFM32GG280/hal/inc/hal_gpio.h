/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_gpio.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_


#include <stddef.h>
#include "em_gpio.h"


/**********函数指针回调声明**********/
typedef void(*GPIO_INT_CB)(void);


#define STATUS_LED1_PORT          gpioPortE
#define STATUS_LED1_PIN           7

#define STATUS_LED2_PORT          gpioPortC
#define STATUS_LED2_PIN           8

#define GPS_POWER_CONTROL_PORT    gpioPortD
#define GPS_POWER_CONTROL_PIN     8

#define GPRS_POWER_CONTROL_PORT   gpioPortC
#define GPRS_POWER_CONTROL_PIN    11

#define ESAM_CTRL_PORT            gpioPortE
#define ESAM_CTRL_PIN             14

#define MX25_ERASE_PORT           gpioPortC
#define MX25_ERASE_PIN            15


void Gpio_Init(void);
void Status_Led1_Toggle(void);
void Status_Led1_Set(void);
void Status_Led1_Clear(void);
void Status_Led2_Toggle(void);
void Status_Led2_Set(void);
void Status_Led2_Clear(void);
bool Mx25_Gpio_Pin(void);
void Gps_Power_On(void);
void Gps_Power_Off(void);
void Gpio_Int_CallBack(GPIO_INT_CB func);
void GPIO_ODD_IRQHandler(void);


#endif

