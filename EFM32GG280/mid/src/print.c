/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: print.c
*Version: 0.1.0
*Date: 2017-01-03
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "print.h"


/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT(char *ind, ...)
{
	char *para;
	va_list vl;
	uint8_t data[11];
	uint32_t u32,lens;


	va_start(vl,ind);

	while(*ind!='\0')
	{
		switch(*(ind++))
		{
			case 'D':      //十进制
				u32=va_arg(vl,uint32_t);
				PRINT_U32DEC(u32);
				break;

			case 'E':      //2017-03-17 10:09:12.123
				u32=va_arg(vl,uint32_t);
				DEBUG_PRINT_TIME((uint8_t *)u32);
				break;

			case 'F':      //有符号的32位数
				u32=va_arg(vl,uint32_t);
				PRINT_S16(u32);
				break;

			case 'K':      //打印CRLF
				PRINT_U16(0x0A0D);
				break;

			case 'H':      //十六进制
				u32=va_arg(vl,uint32_t);
				PRINT_U32CHAR(u32);
				break;

			case 'N':      //当前时间
				NowTime2P101Time(data,7);
				DEBUG_PRINT_TIME(data);
				break;

			case 'S':      //字符串
				para=va_arg(vl,char*);
				while(*para!='\0')
					PRINT_U8(*para++);
				break;

			case 'T':      //17-02-15
				u32=va_arg(vl,uint32_t);
				lens=Get_U32_Time(u32,data,8);
				for(uint8_t i=0;i<lens;i++)
					PRINT_U8(data[i]);
				break;

			default :
				break;
		}
	}

	va_end(vl);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_NISR(char *ind, ...)
{
	char *para;
	va_list vl;
	uint8_t data[11];
	uint32_t u32,lens;

	vTaskSuspendAll();
	Wdog_Feed();

	va_start(vl,ind);

	while(*ind!='\0')
	{
		switch(*(ind++))
		{
			case 'D':      //十进制
				u32=va_arg(vl,uint32_t);
				PRINT_U32DEC(u32);
				break;

			case 'E':      //2017-03-17 10:09:12.123
				u32=va_arg(vl,uint32_t);
				DEBUG_PRINT_TIME((uint8_t *)u32);
				break;

			case 'F':      //有符号的32位数
				u32=va_arg(vl,uint32_t);
				PRINT_S16(u32);
				break;

			case 'K':      //打印CRLF
				PRINT_U16(0x0A0D);
				break;

			case 'H':      //十六进制
				u32=va_arg(vl,uint32_t);
				PRINT_U32CHAR(u32);
				break;

			case 'N':      //当前时间
				NowTime2P101Time(data,7);
				DEBUG_PRINT_TIME(data);
				break;

			case 'S':      //字符串
				para=va_arg(vl,char*);
				while(*para!='\0')
					PRINT_U8(*para++);
				break;

			case 'T':      //17-02-15
				u32=va_arg(vl,uint32_t);
				lens=Get_U32_Time(u32,data,8);
				for(uint8_t i=0;i<lens;i++)
					PRINT_U8(data[i]);
				break;

			default :
				break;
		}
	}

	va_end(vl);

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_PRINT_TIME
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_TIME(uint8_t *time)
{
	DOUBLEBYTE_PRINT DB;
	uint8_t data[8]; //年月日时分秒毫秒

	memcpy(data+1,time,7);
	memcpy(DB.U8,data+1,2*sizeof(uint8_t));

	data[5]&=0x1F;          //屏蔽掉星期
	data[2]=DB.U16/1000;    //秒
	DB.U16%=1000;
	data[1]=DB.U16/100;     //毫秒高位
	data[0]=DB.U16%100;     //毫秒低位

	for(uint8_t i=0;i<8;i++)
		data[i]=Byte2Bcd(data[i]);

	PRINT_U8CHAR(data[7]);
	PRINT_U8(0x2D);
	PRINT_U8CHAR(data[6]);
	PRINT_U8(0x2D);
	PRINT_U8CHAR(data[5]);
	PRINT_U8(0x20);
	PRINT_U8CHAR(data[4]);
	PRINT_U8(0x3A);
	PRINT_U8CHAR(data[3]);
	PRINT_U8(0x3A);
	PRINT_U8CHAR(data[2]);
	PRINT_U8(0x2E);
	PRINT_U8(data[1]+0x30);
	PRINT_U8CHAR(data[0]);
	PRINT_U8(0x20);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_U8(uint8_t data)
{
	USART_Tx(UART1,data);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_S8(int8_t data)
{
	SIGN_U8_PRINT S8;

	S8.DATA=data;

	if(S8.SIGN==1)
	{
		PRINT_U8(0x2D);
		PRINT_U8DEC((~(0x80|S8.DAT))+1);
	}
	else
	{
		PRINT_U8DEC(S8.DAT);
	}
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_U8CHAR(uint8_t data)
{
	uint8_t ch[2];
	SINGLEBYTE_PRINT SB;

	SB.DATA=data;

	if(SB.U47>9)
		ch[0]=SB.U47+0x37;
	else
		ch[0]=SB.U47+0x30;

	if(SB.U03>9)
		ch[1]=SB.U03+0x37;
	else
		ch[1]=SB.U03+0x30;

	for(uint8_t i=0;i<2;i++)
		PRINT_U8(ch[i]);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_U8DEC(uint8_t data)
{
	uint8_t flag=0;
	uint8_t bit[3];

	memset(bit,0,3*sizeof(uint8_t));

	for(uint8_t i=3;i>0;i--)
	{
		bit[i-1]=data%10;
		data/=10;
	}

	for(uint8_t i=0;i<3;i++)
	{
		if(flag==0)
		{
			if(bit[i]==0)
			{
				if(i!=2) //修正16进制为0时打印输出不显示0的BUG
					continue;
			}

			flag=1;
			PRINT_U8(bit[i]+0x30);
		}
		else
			PRINT_U8(bit[i]+0x30);
	}

	PRINT_U8(0x20);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U8_ARRAY(uint8_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
		PRINT_U8(data[i]);

	PRINT_U8(0x0D);
	PRINT_U8(0x0A);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U8_ARRAY_NISR(uint8_t *data,uint32_t lens)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_U8_ARRAY(data,lens);

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_S8_ARRAY(int8_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
		PRINT_S8(data[i]);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_S8_ARRAY_NISR(int8_t *data,uint32_t lens)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_S8_ARRAY(data,lens);

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U8CHAR_ARRAY(uint8_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
	{
		PRINT_U8CHAR(data[i]);
		PRINT_U8(0x20);
	}

	PRINT_U8(0x0D);
	PRINT_U8(0x0A);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U8CHAR_ARRAY_NISR(uint8_t *data,uint32_t lens)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_U8CHAR_ARRAY(data,lens);

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_U16(uint16_t data)
{
	UNSIGN_U16_PRINT U16;

	U16.DATA=data;

	for(uint8_t i=0;i<2;i++)
		PRINT_U8(U16.U8[i]);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_S16(int16_t data)
{
	SIGN_U16_PRINT S16;

	S16.DATA=data;

	if(S16.SIGN==1)
	{
		PRINT_U8(0x2D);
		PRINT_U16DEC((~(0x8000|S16.DAT))+1);
	}
	else
		PRINT_U16DEC(S16.DAT);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_U16CHAR(uint16_t data)
{
	uint8_t ch[2];
	UNSIGN_U16_PRINT U16;

	U16.DATA=data;

	for(uint8_t i=2;i>0;i--)
	{
		if(U16.U4[i-1].U47>9)
			ch[0]=U16.U4[i-1].U47+0x37;
		else
			ch[0]=U16.U4[i-1].U47+0x30;

		if(U16.U4[i-1].U03>9)
			ch[1]=U16.U4[i-1].U03+0x37;
		else
			ch[1]=U16.U4[i-1].U03+0x30;

		for(uint8_t i=0;i<2;i++)
			PRINT_U8(ch[i]);
	}
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_U16DEC(uint16_t data)
{
	uint8_t flag=0;
	uint8_t bit[5];

	memset(bit,0,5*sizeof(uint8_t));

	for(uint8_t i=5;i>0;i--)
	{
		bit[i-1]=data%10;
		data/=10;
	}

	for(uint8_t i=0;i<5;i++)
	{
		if(flag==0)
		{
			if(bit[i]==0)
			{
				if(i!=4) //修正16进制为0时打印输出不显示0的BUG
					continue;
			}

			flag=1;
			PRINT_U8(bit[i]+0x30);
		}
		else
			PRINT_U8(bit[i]+0x30);
	}

	PRINT_U8(0x20);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U16_ARRAY(uint16_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
		PRINT_U16(data[i]);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U16_ARRAY_NISR(uint16_t *data,uint32_t lens)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_U16_ARRAY(data,lens);

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_S16_ARRAY(int16_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
		PRINT_S16(data[i]);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_S16_ARRAY_NISR(int16_t *data,uint32_t lens)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_S16_ARRAY(data,lens);

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U16CHAR_ARRAY(uint16_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
	{
		PRINT_U16CHAR(data[i]);
		PRINT_U8(0x20);
	}

	PRINT_U8(0x0D);
	PRINT_U8(0x0A);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U16CHAR_ARRAY_NISR(uint16_t *data,uint32_t lens)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_U16CHAR_ARRAY(data,lens);

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_U32(uint32_t data)
{
	UNSIGN_U32_PRINT U32;

	U32.DATA=data;

	for(uint8_t i=0;i<4;i++)
		PRINT_U8(U32.U8[i]);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_S32(int32_t data)
{
	SIGN_U32_PRINT S32;

	S32.DATA=data;

	if(S32.SIGN==1)
	{
		PRINT_U8(0x2D);
		PRINT_U32DEC((~(0x80000000|S32.DAT))+1);
	}
	else
		PRINT_U32DEC(S32.DAT);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_U32CHAR(uint32_t data)
{
	UNSIGN_U32_PRINT U32;

	U32.DATA=data;

	for(uint8_t i=4;i>0;i--)
	{
		 if(U32.U4[i-1].U47>9)
			 PRINT_U8(U32.U4[i-1].U47+0x37);
		 else
			 PRINT_U8(U32.U4[i-1].U47+0x30);

		 if(U32.U4[i-1].U03>9)
			 PRINT_U8(U32.U4[i-1].U03+0x37);
		 else
			 PRINT_U8(U32.U4[i-1].U03+0x30);
	}

	PRINT_U8(0x20);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void PRINT_U32DEC(uint32_t data)
{
	uint8_t flag=0;
	uint8_t bit[10];

	memset(bit,0,10*sizeof(uint8_t));

	for(uint8_t i=10;i>0;i--)
	{
		bit[i-1]=data%10;
		data/=10;
	}

	for(uint8_t i=0;i<10;i++)
	{
		if(flag==0)
		{
			if(bit[i]==0)
			{
				if(i!=9) //修正16进制为0时打印输出不显示0的BUG
					continue;
			}

			flag=1;
			PRINT_U8(bit[i]+0x30);
		}
		else
			PRINT_U8(bit[i]+0x30);
	}
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U32_ARRAY(uint32_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
		PRINT_U32(data[i]);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U32_ARRAY_NISR(uint32_t *data,uint32_t lens)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_U32_ARRAY(data,lens);

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_S32_ARRAY(int32_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
		PRINT_S32(data[i]);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_S32_ARRAY_NISR(int32_t *data,uint32_t lens)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_S32_ARRAY(data,lens);

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U32CHAR_ARRAY(uint32_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
		PRINT_U32CHAR(data[i]);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_U32CHAR_ARRAY_NISR(uint32_t *data,uint32_t lens)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_U32CHAR_ARRAY(data,lens);

	Wdog_Feed();
	xTaskResumeAll();
}


