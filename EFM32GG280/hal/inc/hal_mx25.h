/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_mx25.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef _HAL_MX25_H_
#define _HAL_MX25_H_


#include "em_gpio.h"



#define MXFLASH_WP_INPUT  GPIO_PinModeSet(FLASH_WP_PORT,FLASH_WP_PIN,gpioModeInput,0)
#define MXFLASH_WP_OUTPUT GPIO_PinModeSet(FLASH_WP_PORT,FLASH_WP_PIN,gpioModePushPull,0)
#define MXFLASH_WP_HIGH   GPIO_PinOutSet(FLASH_WP_PORT,FLASH_WP_PIN)
#define MXFLASH_WP_LOW    GPIO_PinOutClear(FLASH_WP_PORT,FLASH_WP_PIN)



void Mx25_Flash_Init(void);
uint8_t Mx25_Read_U8(uint32_t addr);
bool Mx25_Read_U8_Array(uint32_t addr,uint8_t *data,uint32_t lens,bool check);
bool Mx25_Write_U8(uint32_t addr,uint8_t data);
bool Mx25_Write_U8_Array(uint32_t addr,uint8_t *data,uint32_t lens,bool check);
void Mx25_Erase_Chip(void);
void Mx25_Erase_Sector(uint32_t addr);
void Mx25_Erase_32Block(uint32_t addr);
void Mx25_Erase_64Block(uint32_t addr);
uint8_t Mx25_Read_Status(void);
void Mx25_Write_Status(uint8_t data);
void Mx25_Read_Id(uint8_t *data);
void Mx25_Enable_Write(void);
void Mx25_Disable_Write(void);
void Mx25_Wait_Idle(void);
uint8_t MX25_READWRITE_BYTE(uint8_t data);
void Mx25_Open_Flash(void);
void Mx25_Close_Flash(void);
uint8_t Mx25_Check_Data(uint8_t data1,uint8_t data2,uint8_t data3);
void Mx25_Check_Array(uint8_t *data1,uint8_t *data2,uint8_t *data3,uint32_t lens);


#endif

