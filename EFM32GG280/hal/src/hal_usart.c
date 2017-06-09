/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_usart.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_usart.h"
#include "include.h"


/**********LEUART0 RX中断回调注册变量**********/
LEUART1_RX_CB Leuart1RxCb=NULL;
/**********LEUART0 TX中断回调注册变量**********/
LEUART1_TX_CB Leuart1TxCb=NULL;
/**********UART0 RX中断回调注册变量**********/
UART0_RX_CB Uart0RxCb=NULL;
/**********UART0 TX中断回调注册变量**********/
UART0_TX_CB Uart0TxCb=NULL;
/**********UART1 RX中断回调注册变量**********/
UART1_RX_CB Uart1RxCb=NULL;
/**********UART0 TX中断回调注册变量**********/
UART1_TX_CB Uart1TxCb=NULL;

/************************************************************!
*Function: usart0init
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Usart0_Init(void)
{
	CMU_ClockEnable(cmuClock_USART0,true);

	USART_InitSync_TypeDef usart0init=USART_INITSYNC_DEFAULT;

	usart0init.clockMode=usartClockMode3;
	usart0init.baudrate=5000000;
	usart0init.msbf=1;

	USART_InitSync(USART0,&usart0init);

	GPIO_PinModeSet(ESAM_MOSI_PORT,ESAM_MOSI_PIN,gpioModePushPull,0);
	GPIO_PinModeSet(ESAM_MISO_PORT,ESAM_MISO_PIN,gpioModeInputPull,0);
	GPIO_PinModeSet(ESAM_SCK_PORT,ESAM_SCK_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(ESAM_CS_PORT,ESAM_CS_PIN,gpioModePushPull,1);  //手册低电平

	USART0->ROUTE=USART_ROUTE_CLKPEN|USART_ROUTE_RXPEN|USART_ROUTE_TXPEN|USART_ROUTE_LOCATION_LOC0;
}
/************************************************************!
*Function: Usart0_ReadWrite
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t Usart0_ReadWrite(uint8_t data)
{
	USART0->TXDATA=data;

	while(!(USART0->STATUS&USART_STATUS_TXC));

	return (uint8_t)(USART0->RXDATA);
}
/************************************************************!
*Function: Usart1_Init
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Usart1_Init(void)
{
	CMU_ClockEnable(cmuClock_USART1,true);

	USART_InitSync_TypeDef usart1init=USART_INITSYNC_DEFAULT;

	usart1init.baudrate=1000000;
	usart1init.msbf=1;

	USART_InitSync(USART1,&usart1init);

	GPIO_PinModeSet(FLASH_SI_PORT,FLASH_SI_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(FLASH_SO_PORT,FLASH_SO_PIN,gpioModeInputPull,0);
	GPIO_PinModeSet(FLASH_SCK_PORT,FLASH_SCK_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(FLASH_CS_PORT,FLASH_CS_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(FLASH_RST_PORT,FLASH_RST_PIN,gpioModePushPull,0);

	USART1->ROUTE =USART_ROUTE_CLKPEN|USART_ROUTE_RXPEN|USART_ROUTE_TXPEN|USART_ROUTE_LOCATION_LOC1;
}
/************************************************************!
*Function: Usart1_ReadWrite
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t Usart1_ReadWrite(uint8_t data)
{
	USART1->TXDATA=data;
	while(!(USART1->STATUS&USART_STATUS_TXC));
	return (uint8_t)(USART1->RXDATA);
}
/************************************************************!
*Function: Uart0_Init
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart0_Init(uint32_t baudrate)
{
	CMU_ClockEnable(cmuClock_UART0,true);

	USART_InitAsync_TypeDef uart0init=USART_INITASYNC_DEFAULT;

	uart0init.baudrate=baudrate;
	//uart0init.parity=usartEvenParity; //101规约有一位偶校验！！！！

	USART_InitAsync(UART0,&uart0init);

	GPIO_PinModeSet(TX_GPRS_PORT,TX_GPRS_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(RX_GPRS_PORT,RX_GPRS_PIN,gpioModeInputPull,1);

	UART0->ROUTE|=UART_ROUTE_TXPEN|UART_ROUTE_RXPEN|UART_ROUTE_LOCATION_LOC0;
	UART0->IEN=UART_IEN_RXDATAV;//打开发送和接受中断
	//UART0->IEN=UART_IEN_TXC|UART_IEN_RXDATAV;//打开发送和接受中断

	//USART_IntEnable(UART0,UART_IF_TXC);
	NVIC_ClearPendingIRQ(UART0_RX_IRQn);
	NVIC_EnableIRQ(UART0_RX_IRQn);
//	NVIC_ClearPendingIRQ(UART0_TX_IRQn);
//	NVIC_EnableIRQ(UART0_TX_IRQn);

	//NVIC_SetPriority(UART1_RX_IRQn,9);
}
/************************************************************!
*Function: Uart0_TxU8
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart0_TxU8(uint8_t data)
{
	USART_Tx(UART0,data);
}
/************************************************************!
*Function: Uart0_TxU32
*Description: USART0发送字函数
*Input: data：需要发送的字数据
*Output: null
*Return: null
*************************************************************/
void Uart0_TxU32(uint32_t data)
{
	uint8_t temp[4];

	memcpy(temp,&data,sizeof(uint32_t));

    for(uint8_t i=0;i<4;i++)
    	USART_Tx(UART0,temp[i]);
}
/************************************************************!
*Function: Uart1_Init
*Description:
*Input: baudrate：波特率
*Output: null
*Return: null
*************************************************************/
void Uart1_Init(uint32_t baudrate)
{
	CMU_ClockEnable(cmuClock_UART1,true);

	USART_InitAsync_TypeDef uart1init=USART_INITASYNC_DEFAULT;

	uart1init.baudrate=baudrate;

	USART_InitAsync(UART1,&uart1init);

	GPIO_PinModeSet(TX_WEIHU_PORT,TX_WEIHU_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(RX_WEIHU_PORT,RX_WEIHU_PIN,gpioModeInputPull,1);

	UART1->ROUTE|=UART_ROUTE_TXPEN|UART_ROUTE_RXPEN|UART_ROUTE_LOCATION_LOC0;
	UART1->IEN=UART_IEN_TXC|UART_IEN_RXDATAV;//打开发送和接受中断

	USART_IntEnable(UART1,UART_IF_TXC);
	NVIC_ClearPendingIRQ(UART1_RX_IRQn);
	NVIC_EnableIRQ(UART1_RX_IRQn);
	NVIC_ClearPendingIRQ(UART1_TX_IRQn);
	NVIC_EnableIRQ(UART1_TX_IRQn);

	//NVIC_SetPriority(UART1_RX_IRQn,9);
}
/************************************************************!
*Function: Uart1_TxU8
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart1_TxU8(uint8_t data)
{
	USART_Tx(UART1,data);
}
/************************************************************!
*Function: Uart1_TxU32
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Uart1_TxU32(uint32_t data)
{
	uint8_t temp[4];

	memcpy(temp,&data,sizeof(uint32_t));

    for(uint8_t i=0;i<4;i++)
    	USART_Tx(UART1,temp[i]);
}
/************************************************************!
*Function: Leuart1_Init
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Leuart1_Init(uint32_t baudrate)
{
	CMU_ClockSelectSet(cmuClock_LFB,cmuSelect_LFXO); //LFB采用外部低频晶振
	CMU_ClockEnable(cmuClock_LEUART1,true);

	LEUART_Init_TypeDef leuart1init=LEUART_INIT_DEFAULT;

	leuart1init.baudrate=baudrate;

	LEUART_Init(LEUART1,&leuart1init);

	GPIO_PinModeSet(TX_GPS_PORT,TX_GPS_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(RX_GPS_PORT,RX_GPS_PIN,gpioModeInputPull,1);

	LEUART1->ROUTE=LEUART_ROUTE_TXPEN|LEUART_ROUTE_RXPEN|LEUART_ROUTE_LOCATION_LOC0;

	LEUART_IntClear(LEUART1,LEUART_IF_RXDATAV);
	LEUART_IntEnable(LEUART1,LEUART_IEN_RXDATAV);

	NVIC_ClearPendingIRQ(LEUART1_IRQn);
	NVIC_EnableIRQ(LEUART1_IRQn);
}
/************************************************************!
*Function: Leuart0_TxU8
*Description: LEUART0发送字节函数
*Input: data：发送的字节
*Output: null
*Return: null
*************************************************************/
void Leuart1_TxU8(uint8_t data)
{
	LEUART_Tx(LEUART1,data);
}
/************************************************************!
*Function: Leuart0_TxU32
*Description: LEUART0发送字节函数
*Input: data：需要发送的字数据
*Output: null
*Return: null
*************************************************************/
void Leuart1_TxU32(uint32_t data)
{
	uint8_t temp[4];

	memcpy(temp,&data,sizeof(uint32_t));

    for(uint8_t i=0;i<4;i++)
    	LEUART_Tx(LEUART1,temp[i]);
}
/************************************************************!
*Function: Uart0_Rx_CallBack
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart0_Rx_CallBack(UART0_RX_CB func)
{
	Uart0RxCb=func;
}
/************************************************************!
*Function: Uart0_Tx_CallBack
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart0_Tx_CallBack(UART0_TX_CB func)
{
	Uart0TxCb=func;
}
/************************************************************!
*Function: Uart1_Rx_CallBack
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart1_Rx_CallBack(UART1_RX_CB func)
{
	Uart1RxCb=func;
}
/************************************************************!
*Function: Uart1_Tx_CallBack
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Uart1_Tx_CallBack(UART1_TX_CB func)
{
	Uart1TxCb=func;
}
/************************************************************!
*Function: Leuart1_Rx_CallBack
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Leuart1_Rx_CallBack(LEUART1_RX_CB func)
{
	Leuart1RxCb=func;
}
/************************************************************!
*Function: Leuart1_Tx_CallBack
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Leuart1_Tx_CallBack(LEUART1_TX_CB func)
{
	Leuart1TxCb=func;
}
/************************************************************!
*Function: UART0_RX_IRQHandler
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void UART0_RX_IRQHandler(void)
{
	if(Uart0RxCb!=NULL)
		(*Uart0RxCb)();
}
/************************************************************!
*Function: UART0_TX_IRQHandler
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void UART0_TX_IRQHandler(void)
{
	UART0->IFC=UART_IFC_TXC;//清发送完成中断标志位

	if(Uart0TxCb!=NULL)
		(*Uart0TxCb)();
}
/************************************************************!
*Function: UART1_RX_IRQHandler
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void UART1_RX_IRQHandler(void)
{
	if(Uart1RxCb!=NULL)
		(*Uart1RxCb)();
}
/************************************************************!
*Function: UART1_TX_IRQHandler
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void UART1_TX_IRQHandler(void)
{
	UART1->IFC=UART_IFC_TXC;//清发送完成中断标志位

	if(Uart1TxCb!=NULL)
		(*Uart1TxCb)();
}
/************************************************************!
*Function: LEUART1_IRQHandler
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void LEUART1_IRQHandler(void)
{
	uint8_t data;

	if(LEUART1->IF&LEUART_IF_RXDATAV)//接收数据中断
	{
		LEUART1->IFC=LEUART_IFC_RXOF;

		 data=LEUART_Rx(LEUART1);

		if(Leuart1RxCb!=NULL)
			(*Leuart1RxCb)(data);
	}

	if(LEUART1->IF&LEUART_IF_TXC)//发送完成中断
	{
		LEUART1->IFC=LEUART_IFC_TXC;

		if(Leuart1TxCb!=NULL)
			(*Leuart1TxCb)();
	}
}

