/*************************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: rf_spi.h
*Version: 0.1.0
*Date: 2016-08-26
*Author: WANGJIANHUI
*Description:
*************************************************************************/
#ifndef RF_SPI_H_
#define RF_SPI_H_


#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"


#define SPI_MOSI_PORT		 gpioPortB
#define SPI_MOSI_PIN         3
#define SPI_MISO_PORT        gpioPortB
#define SPI_MISO_PIN         4
#define SPI_SCLK_PORT        gpioPortB
#define SPI_SCLK_PIN         5
#define SPI_CS_PORT          gpioPortB
#define SPI_CS_PIN           6

#define RF_NIRQ_PORT         gpioPortC
#define RF_NIRQ_PIN          1
#define RF_SDN_PORT          gpioPortB
#define RF_SDN_PIN           1

#define RF_GPIO0_PORT        gpioPortC
#define RF_GPIO0_PIN         4
#define RF_GPIO1_PORT        gpioPortC
#define RF_GPIO1_PIN         3
#define RF_GPIO2_PORT        gpioPortB
#define RF_GPIO2_PIN         2
#define RF_GPIO3_PORT        gpioPortC
#define RF_GPIO3_PIN         7


typedef enum
{
  eSpi_Nsel_RF,
} eSpi_Nsel;


void Spi_Init(void);
uint8_t Spi_ReadWrite(uint8_t data);
void Spi_WriteDataBurst(uint8_t cnt, uint8_t* data);
void Spi_ReadDataBurst(uint8_t cnt, uint8_t* data);
void Spi_ClearNsel(eSpi_Nsel qiSelect);
void Spi_SetNsel(eSpi_Nsel qiSelect);


#endif

