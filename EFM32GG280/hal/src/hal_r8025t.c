/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_r8025t.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "hal_math.h"
#include "hal_r8025t.h"


/************************************************************!
*Function: Rx8025t_Init
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Rx8025t_Init(void)
{
	uint8_t data[16];

	data[0]=0x00;
	data[1]=0x00;
	data[2]=0x40;

	Rx8025t_WriteByte(0x0D,data,3); //此处强制将8025T的秒脉冲关掉,降功耗

	if(Check_R8025t_Data(data,16)==false)
	{
		R8025t_Reset();

		return false;
	}

	Check_R8025t_Data(data,16);

	if((data[0x0E]&0x02)||(Is_Valid_R8025t_Clock(data,7)==false)) //VLF置位或者时钟格式不对
	{
		R8025t_Reset();

		return false;
	}

	return true;
}
/************************************************************!
*Function: R8025t_Reset
*Description: R8025T时钟复位
*Input: null
*Output: null
*Return: null
*************************************************************/
void R8025t_Reset(void)
{
	uint8_t time[7]={0x00,0x33,0x16,0x00,0x15,0x02,0x17}; //BCD码SB!
	Rx8025t_WriteByte(0,time,7);

	time[0]=0x00;
	time[1]=0x00;
	time[2]=0x40;

	Rx8025t_WriteByte(0x0D,time,3);
}
/************************************************************!
*Function: Get_R8025t_RtcTime
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Get_R8025t_RtcTime(uint8_t *data,uint8_t lens)
{
	uint8_t time[16];

	if(lens<6)
		return false;

	Rx8025t_ReadByte(0,time,16);

	if((time[0x0E]&0x02)||(Is_Valid_R8025t_Clock(time,7)==false))
	{
		UnixTime2LocalTime(gUNIXTIME.UNIX,time,6);

		Set_R8025t_RtcTime(time,6);

		return false;
	}

	data[0]=Bcd2Byte(time[0]); //HEX格式
	data[1]=Bcd2Byte(time[1]);
	data[2]=Bcd2Byte(time[2]);
	data[3]=Bcd2Byte(time[4]);
	data[4]=Bcd2Byte(time[5]);
	data[5]=Bcd2Byte(time[6]);

//	data[0]=time[0]; //BCD格式
//	data[1]=time[1];
//	data[2]=time[2];
//	data[3]=time[4];
//	data[4]=time[5];
//	data[5]=time[6];

	return true;
}
/************************************************************!
*Function: Set_R8025t_RtcTime
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Set_R8025t_RtcTime(uint8_t *data,uint8_t lens)
{
	uint8_t time[7];

	if(lens<6)
		return false;

	time[0]=Byte2Bcd(data[0]);
	time[1]=Byte2Bcd(data[1]);
	time[2]=Byte2Bcd(data[2]);
	time[3]=1;
	time[4]=Byte2Bcd(data[3]);
	time[5]=Byte2Bcd(data[4]);
	time[6]=Byte2Bcd(data[5]);

	Rx8025t_WriteByte(0,time,7);

	time[0]=0x00;
	time[1]=0x00;
	time[2]=0x60;

	Rx8025t_WriteByte(0x0D,time,3);

	return false;
}
/************************************************************!
*Function: Check_R8025t_Data
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Check_R8025t_Data(uint8_t *data,uint8_t lens)
{
	uint8_t cnt=15;
	uint8_t buff[32];

	while(cnt--)
	{
		Rx8025t_ReadByte(0,buff,32);

		if(memcmp(buff,buff+16,16)!=0) //重复读取数据
			continue;

		if(Is_Valid_R8025t_Clock(buff,7)==false)
			continue;

		memcpy(data,buff,16);

		return true;
	}

	return false;
}
/************************************************************!
*Function: Is_Valid_R8025t_Clock
*Description: 先判断时钟格式是否是合法的BCD码,然后判断时钟格式是否
*             超出计量范围。
*Input: null
*Output: null
*Return: 时钟格式是否合法
*************************************************************/
bool Is_Valid_R8025t_Clock(uint8_t *data,uint8_t lens)
{
	uint8_t isleapyear;
	R8025TTIME RTCTIME;
	R8025T_SINGLEBYTE RSB;

	uint8_t DAY[2][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31},
			            {0,31,29,31,30,31,30,31,31,30,31,30,31}};

	if(lens<7) //防止数组越界
		return false;

	for(uint8_t i=0;i<7;i++) //BCD格式判断
	{
		if(i==3) //跳过星期判断
			continue;

		RSB.DATA=data[i];

		if(!(RSB.U03<=9&&RSB.U47<=9))
			return false;
	}

	RTCTIME.DATA[0]=Bcd2Byte(data[0]); //将BCD时间转化为HEX时间格式
	RTCTIME.DATA[1]=Bcd2Byte(data[1]);
	RTCTIME.DATA[2]=Bcd2Byte(data[2]);
	RTCTIME.DATA[3]=Bcd2Byte(data[4]);
	RTCTIME.DATA[4]=Bcd2Byte(data[5]);
	RTCTIME.DATA[5]=Bcd2Byte(data[6]);

	if(RTCTIME.YEAR>99) //年判断
		return false;

	if(RTCTIME.MONTH==0||RTCTIME.MONTH>12) //月判断
		return false;

	isleapyear=((RTCTIME.YEAR%4)==0?1:0); //闰年判断

    if(RTCTIME.DAY>DAY[isleapyear][RTCTIME.MONTH]) //日判断
    	return false;

    //if(Rx8025t_week2data(RTCTIME.WEEK)==0) //星期判断
    	//return false;

	if(RTCTIME.HOUR>23) //小时判断
		return false;

	if(RTCTIME.MINUTE>59) //分钟判断
		return false;

	if(RTCTIME.SECOND>59) //秒判断
		return false;

	return true;
}
/************************************************************!
*Function: Rx8025t_week2data
*Description: 将8025t的星期转化为16进制数,16进制星期的范围为1-7.
*Input: week: 8025t的星期
*Output: null
*Return: 16进制的星期,错误返回0
*************************************************************/
uint8_t Rx8025t_week2data(uint8_t week)
{
	if(week==1) //星期日
		return 7;

	if(week==2) //星期一
		return 1;

	if(week==4) //星期二
		return 2;

	if(week==8) //星期三
		return 3;

	if(week==16) //星期四
		return 4;

	if(week==32) //星期五
		return 5;

	if(week==64) //星期六
		return 6;

	return 0;
}
/************************************************************!
*Function: Rx8025t_week2data
*Description: 将范围为1-7的16进制星期转化为8025t的星期.
*Input: week: 16进制星期
*Output: null
*Return: 8025t的星期,错误返回0
*************************************************************/
uint8_t Rx8025t_data2week(uint8_t data)
{
	if(data==7) //星期日
		return 1;

	if(data==1) //星期一
		return 2;

	if(data==2) //星期二
		return 4;

	if(data==3) //星期三
		return 8;

	if(data==4) //星期四
		return 16;

	if(data==5) //星期五
		return 32;

	if(data==6) //星期六
		return 64;

	return 0;
}
/************************************************************!
*Function: Rx8025t_ReadByte
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t Rx8025t_ReadByte(uint8_t addr,uint8_t *data,uint8_t lens)
{
	uint8_t result;

	I2C_TransferSeq_TypeDef i2ctransfer;

	i2ctransfer.addr       =RX8025T_SLAVE_ADDR;
	i2ctransfer.flags      =I2C_FLAG_WRITE_READ;
	i2ctransfer.buf[0].data=&addr;
	i2ctransfer.buf[0].len =1;
	i2ctransfer.buf[1].data=data;
	i2ctransfer.buf[1].len =lens;

	I2C_TransferInit(I2C0,&i2ctransfer);

	while(I2C_Transfer(I2C0)==i2cTransferInProgress){;}

	result=(sizeof(data)/sizeof(uint8_t));  //?????????????????报错

	return result;
}
/************************************************************!
*Function: Rx8025t_WriteByte
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Rx8025t_WriteByte(uint8_t addr,uint8_t *data,uint8_t lens)
{
	I2C_TransferSeq_TypeDef i2ctransfer;

	i2ctransfer.addr       =RX8025T_SLAVE_ADDR;
	i2ctransfer.flags      =I2C_FLAG_WRITE_WRITE;
	i2ctransfer.buf[0].data=&addr;
	i2ctransfer.buf[0].len =1;
	i2ctransfer.buf[1].data=data;
	i2ctransfer.buf[1].len =lens;

	I2C_TransferInit(I2C0,&i2ctransfer);

	while(I2C_Transfer(I2C0)==i2cTransferInProgress){;}
}

