/******************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_mx25.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_mx25.h"
#include "hal_usart.h"
#include "hal_wdog.h"
#include "include.h"


/************************************************************!
*Function: Mx25_Flash_Init
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Flash_Init(void)
{
	uint8_t id[3];

	GPIO_PinOutClear(FLASH_RST_PORT,FLASH_RST_PIN);
	for(uint32_t i=0;i<20000;i++);

	GPIO_PinOutSet(FLASH_RST_PORT, FLASH_RST_PIN);
	for(uint32_t i=0;i<20000;i++);

	GPIO_PinOutSet(FLASH_RST_PORT,FLASH_RST_PIN);   //拉高WP

	Mx25_Open_Flash();
	MX25_READWRITE_BYTE(0x66);
	MX25_READWRITE_BYTE(0x99);
	Mx25_Close_Flash();

	Mx25_Wait_Idle();

	Mx25_Write_Status(0);

	Mx25_Wait_Idle();

	Mx25_Open_Flash();
	MX25_READWRITE_BYTE(0xB7);	 //进入四字节地址模式,注意MX25型号,大于16M需要进入四字节模式
	Mx25_Close_Flash();

	Mx25_Read_Id(id);
}
/************************************************************!
*Function: Mx25_Read_U8
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t Mx25_Read_U8(uint32_t addr)
{
	uint8_t data,data1[3];

	Mx25_Wait_Idle();

	for(uint8_t i=0;i<3;i++)
	{
		Mx25_Open_Flash();

	    MX25_READWRITE_BYTE(0x13);
	    MX25_READWRITE_BYTE(addr>>24);
	    MX25_READWRITE_BYTE(addr>>16);
	    MX25_READWRITE_BYTE(addr>>8);
	    MX25_READWRITE_BYTE(addr);

	    data1[i]=MX25_READWRITE_BYTE(0);

		Mx25_Close_Flash();
	}

	data=Mx25_Check_Data(data1[0],data1[1],data1[2]);

	Mx25_Wait_Idle();

    return data;
}
/************************************************************!
*Function: Mx25_Read_U8_Array
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Mx25_Read_U8_Array(uint32_t addr,uint8_t *data,uint32_t lens,bool check)
{
	uint8_t checkdata;

	Mx25_Wait_Idle();

	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0x13);
	MX25_READWRITE_BYTE(addr>>24);
	MX25_READWRITE_BYTE(addr>>16);
	MX25_READWRITE_BYTE(addr>>8);
	MX25_READWRITE_BYTE(addr);

	for(uint32_t i=0;i<lens;i++)
		data[i]=MX25_READWRITE_BYTE(0);

	Mx25_Close_Flash();

	Mx25_Wait_Idle();

	if(check==true)
	{
		for(uint32_t i=0;i<lens;i++)
		{
			checkdata=Mx25_Read_U8(addr+i);

			if(checkdata!=data[i])
				return false;
		}
	}

	return true;
}
/************************************************************!
*Function: Mx25_Write_U8
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Mx25_Write_U8(uint32_t addr,uint8_t data)
{
	Mx25_Wait_Idle();

	Mx25_Enable_Write();
	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0x12);
	MX25_READWRITE_BYTE(addr>>24);
	MX25_READWRITE_BYTE(addr>>16);
	MX25_READWRITE_BYTE(addr>>8);
	MX25_READWRITE_BYTE(addr);

	MX25_READWRITE_BYTE(data);

	Mx25_Close_Flash();
	Mx25_Disable_Write();

	Mx25_Wait_Idle();

	if(data!=Mx25_Read_U8(addr))
		return false;

	return true;
}
/************************************************************!
*Function: Mx25_Write_U8_Array
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Mx25_Write_U8_Array(uint32_t addr,uint8_t *data,uint32_t lens,bool check)
{
	uint8_t checkdata;
	uint32_t tolens=0;
	uint32_t relens=0;

	while(lens>0)
	{
		relens=0x100-((uint8_t)(addr&0x000000FF)); //页编程,不能跨页写数据

	    if(relens>lens)
		    relens=lens;

	    lens-=relens;

	    Mx25_Wait_Idle();

	    Mx25_Enable_Write();

	    Mx25_Open_Flash();

	    MX25_READWRITE_BYTE(0x12);
	    MX25_READWRITE_BYTE(addr>>24);
	    MX25_READWRITE_BYTE(addr>>16);
	    MX25_READWRITE_BYTE(addr>>8);
	    MX25_READWRITE_BYTE(addr);

	    for(uint32_t i=0;i<relens;i++)
	    	MX25_READWRITE_BYTE(data[tolens+i]);

	    Mx25_Close_Flash();

	    Mx25_Disable_Write();

	    Mx25_Wait_Idle();

	    addr+=relens;
	    tolens+=relens;
	}

    if(check==true)
    {
    	for (uint32_t i=0;i<lens;i++)
    	{
    		checkdata=Mx25_Read_U8(addr+i);

    		if(checkdata!=data[i])
    			return false;
    	}
    }

  	return true;
}
/************************************************************!
*Function: Mx25_Erase_Chip
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Erase_Chip(void)
{
	Mx25_Wait_Idle();

	Mx25_Enable_Write();
	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0x60);

	Mx25_Close_Flash();
	Mx25_Disable_Write();

	Mx25_Wait_Idle();
}
/************************************************************!
*Function: Mx25_Erase_Sector
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void  Mx25_Erase_Sector(uint32_t addr)
{
	Wdog_Feed();

	addr=addr<<12;

	Mx25_Wait_Idle();

	Mx25_Enable_Write();
	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0x21);
	MX25_READWRITE_BYTE(addr>>24);
	MX25_READWRITE_BYTE(addr>>16);
	MX25_READWRITE_BYTE(addr>>8);
	MX25_READWRITE_BYTE(addr);

	Mx25_Close_Flash();
	Mx25_Disable_Write();

	Mx25_Wait_Idle();
}
/************************************************************!
*Function: Mx25_Erase_32Block
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Erase_32Block(uint32_t addr)
{
	addr=addr<<15;

	Mx25_Wait_Idle();

	Mx25_Enable_Write();
	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0x5C);
	MX25_READWRITE_BYTE(addr>>24);
	MX25_READWRITE_BYTE(addr>>16);
	MX25_READWRITE_BYTE(addr>>8);
	MX25_READWRITE_BYTE(addr);

	Mx25_Close_Flash();
	Mx25_Disable_Write();

	Mx25_Wait_Idle();
}
/************************************************************!
*Function: Mx25_Erase_64Block
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Erase_64Block(uint32_t addr)
{
	addr=addr<<16;

	Mx25_Wait_Idle();

	Mx25_Enable_Write();
	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0xDC);
	MX25_READWRITE_BYTE(addr>>24);
	MX25_READWRITE_BYTE(addr>>16);
	MX25_READWRITE_BYTE(addr>>8);
	MX25_READWRITE_BYTE(addr);

	Mx25_Close_Flash();
	Mx25_Disable_Write();

	Mx25_Wait_Idle();
}
/************************************************************!
*Function: Mx25_Read_Status
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t Mx25_Read_Status(void)
{
	uint8_t status;

	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0x05);

   	status=MX25_READWRITE_BYTE(0);

   	Mx25_Close_Flash();

    return status;
}
/************************************************************!
*Function: Mx25_Write_Status
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Write_Status(uint8_t data)
{
	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0x01);
	MX25_READWRITE_BYTE(data);

	Mx25_Close_Flash();
}
/************************************************************!
*Function: Mx25_Read_Id
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Read_Id(uint8_t *data)
{
	Mx25_Wait_Idle();

	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0x9F);

	for (uint8_t i=0;i<3;i++)
		data[i]=MX25_READWRITE_BYTE(0);

	Mx25_Close_Flash();

	Mx25_Wait_Idle();
}
/************************************************************!
*Function: Mx25_Enable_Write
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Enable_Write(void)
{
	uint8_t status;

	MXFLASH_WP_OUTPUT;
	MXFLASH_WP_HIGH;

	Mx25_Open_Flash();
	MX25_READWRITE_BYTE(0x06);
	Mx25_Close_Flash();

	status=Mx25_Read_Status();

	if((status&0x3c)||((status&0x80)==0))
	{
		if(status&0x3c)
		{
			Mx25_Open_Flash();
			MX25_READWRITE_BYTE(0x01);
			MX25_READWRITE_BYTE(0x00);
			Mx25_Close_Flash();

			Mx25_Wait_Idle();

			Mx25_Open_Flash();
			MX25_READWRITE_BYTE(0x06);
			Mx25_Close_Flash();

			Mx25_Wait_Idle();
		}

		Mx25_Open_Flash();
		MX25_READWRITE_BYTE(0x01);
		MX25_READWRITE_BYTE(0x80);
		Mx25_Close_Flash();
	}

	Mx25_Wait_Idle();
}
/************************************************************!
*Function: Mx25_Disable_Write
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Disable_Write(void)
{
	//MXFLASH_WP_DIR= PIN_INPUT;

	Mx25_Open_Flash();

	MX25_READWRITE_BYTE(0x04);

	Mx25_Close_Flash();
}
/************************************************************!
*Function: Mx25_Wait_Idle
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Wait_Idle(void)
{
	uint8_t status=0x01;

	while(status&0x01) //0x01:WIP: Write operation
    {
		status=Mx25_Read_Status();
    };
}
/************************************************************!
*Function: MX25_READWRITE_BYTE
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t MX25_READWRITE_BYTE(uint8_t data)
{
	uint8_t result;

	result=Usart1_ReadWrite(data);

	return result;
}
/************************************************************!
*Function: Mx25_Open_Flash
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Open_Flash(void)
{
	GPIO_PinModeSet(FLASH_SCK_PORT,FLASH_SCK_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(FLASH_CS_PORT,FLASH_CS_PIN,gpioModePushPull,0);
	GPIO_PinModeSet(FLASH_SI_PORT,FLASH_SI_PIN,gpioModePushPull,1);
	GPIO_PinModeSet(FLASH_SO_PORT,FLASH_SO_PIN,gpioModeInput,0);

	GPIO_PinOutClear(FLASH_SCK_PORT,FLASH_SCK_PIN);
	GPIO_PinOutSet(FLASH_CS_PORT,FLASH_CS_PIN);
	GPIO_PinOutClear(FLASH_CS_PORT,FLASH_CS_PIN);
}
/************************************************************!
*Function: Mx25_Close_Flash
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Close_Flash(void)
{
	GPIO_PinOutClear(FLASH_CS_PORT,FLASH_CS_PIN);
	GPIO_PinOutSet(FLASH_CS_PORT,FLASH_CS_PIN);
	GPIO_PinOutSet(FLASH_SCK_PORT,FLASH_SCK_PIN);
	GPIO_PinModeSet(FLASH_SCK_PORT,FLASH_SCK_PIN,gpioModeInput,0);
	GPIO_PinModeSet(FLASH_SO_PORT,FLASH_SO_PIN,gpioModePushPull,0);
	MXFLASH_WP_INPUT;
}
/************************************************************!
*Function: Mx25_Check_Data
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t Mx25_Check_Data(uint8_t data1,uint8_t data2,uint8_t data3)
{
	uint8_t a,b,c,o;

    a=data1;
    b=data2;
    c=data3;
    b^=a;
    c&=b;
    b=(uint8_t)(~b);
    a=(uint8_t)(a&b);
    o=(uint8_t)(a|c);

    return o;
}
/************************************************************!
*Function: Mx25_Check_Array
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Check_Array(uint8_t *data1,uint8_t *data2,uint8_t *data3,uint32_t lens)
{
	uint8_t a,b,c;

	for(uint32_t i=0;i<lens;i++)
	{
		a=data1[i];
		b=data2[i];
		c=data3[i];
		b^=a;
		c&=b;
		b=(uint8_t)(~b);
		a=(uint8_t)(a&b);

		data1[i]=(uint8_t)(a|c);
	}
}

