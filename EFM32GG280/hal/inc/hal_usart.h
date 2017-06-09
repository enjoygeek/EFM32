/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_usart.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_USART_H_
#define HAL_USART_H_


#include <stddef.h>
#include <string.h>
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_usart.h"
#include "em_leuart.h"


/**********函数指针回调声明**********/
typedef void(*LEUART1_RX_CB)(uint8_t data);
/**********函数指针回调声明**********/
typedef void(*LEUART1_TX_CB)(void);
/**********函数指针回调声明**********/
typedef void(*UART0_RX_CB)(void);
/**********函数指针回调声明**********/
typedef void(*UART0_TX_CB)(void);
/**********函数指针回调声明**********/
typedef void(*UART1_RX_CB)(void);
/**********函数指针回调声明**********/
typedef void(*UART1_TX_CB)(void);


/**********ESAM端口定义**********/
#define ESAM_MOSI_PORT      gpioPortE
#define ESAM_MOSI_PIN       10
#define ESAM_MISO_PORT      gpioPortE
#define ESAM_MISO_PIN       11
#define ESAM_SCK_PORT       gpioPortE
#define ESAM_SCK_PIN        12
#define ESAM_CS_PORT        gpioPortE
#define ESAM_CS_PIN         13

/**********GPRS端口定义**********/
#define TX_WEIHU_PORT         gpioPortC
#define TX_WEIHU_PIN          12
#define RX_WEIHU_PORT         gpioPortC
#define RX_WEIHU_PIN          13

/**********GPRS端口定义**********/
#define TX_GPRS_PORT         gpioPortF
#define TX_GPRS_PIN          6
#define RX_GPRS_PORT         gpioPortF
#define RX_GPRS_PIN          7

/**********GPS端口定义**********/
#define TX_GPS_PORT          gpioPortC
#define TX_GPS_PIN           6
#define RX_GPS_PORT          gpioPortC
#define RX_GPS_PIN           7

/**********MX25端口定义**********/
#define FLASH_SI_PORT      gpioPortD
#define FLASH_SI_PIN       0
#define FLASH_SO_PORT      gpioPortD
#define FLASH_SO_PIN       1
#define FLASH_SCK_PORT     gpioPortD
#define FLASH_SCK_PIN      2
#define FLASH_CS_PORT      gpioPortD
#define FLASH_CS_PIN       3
#define FLASH_WP_PORT      gpioPortA
#define FLASH_WP_PIN       9
#define FLASH_RST_PORT     gpioPortA
#define FLASH_RST_PIN      11


#define ENABLE_ESAM_SPI_CS   GPIO_PinOutClear(ESAM_CS_PORT,ESAM_CS_PIN)
#define DISABLE_ESAM_SPI_CS  GPIO_PinOutSet(ESAM_CS_PORT,ESAM_CS_PIN)


void Usart0_Init(void);
uint8_t Usart0_ReadWrite(uint8_t data);
void Usart1_Init(void);
uint8_t Usart1_ReadWrite(uint8_t data);
void Uart0_Init(uint32_t baudrate);
void Uart0_TxU8(uint8_t data);
void Uart0_TxU32(uint32_t data);
void Uart1_Init(uint32_t baudrate);
void Uart1_TxU8(uint8_t data);
void Uart1_TxU32(uint32_t data);
void Leuart1_Init(uint32_t baudrate);
void Leuart1_TxU8(uint8_t data);
void Leuart1_TxU32(uint32_t data);
void Uart0_Rx_CallBack(UART0_RX_CB func);
void Uart0_Tx_CallBack(UART0_TX_CB func);
void Uart1_Rx_CallBack(UART1_RX_CB func);
void Uart1_Tx_CallBack(UART1_TX_CB func);
void Leuart1_Rx_CallBack(LEUART1_RX_CB func);
void Leuart1_Tx_CallBack(LEUART1_TX_CB func);
void UART0_RX_IRQHandler(void);
void UART0_TX_IRQHandler(void);
void UART1_RX_IRQHandler(void);
void UART1_TX_IRQHandler(void);
void LEUART1_IRQHandler(void);


#endif

