/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: systime.c
*Version: 0.1.0
*Date: 2016-11-22
*Author: WANGJIANHUI
*Description: 本系统将R8025T和系统的RTC作为一个整体时钟,R8025T维护系统
*             的年月日星期时分秒,系统的RTC维护毫秒。
*******************************************************************/
#include "include.h"
#include "systime.h"


/************************************************************!
*Function: GPSTime2R8025TRTCTime
*Description: 将GPS时间更新到R8025T时间及RTC的CNT。
*Input: 1.data: 数组的首地址
*       1.lens: 数组的长度
*Output: null
*Return: null
*************************************************************/
void GPSTime2R8025TRTCTime(uint8_t *data,uint8_t lens)
{
	UTCTIME UTC;
	R8025TTIME R8025T;

	Set_Rtc_Cnt(0);

    memcpy(UTC.DATA,data,6);

    UTC=UTCTime2LocalTime(&UTC);

    for(uint8_t i=0;i<6;i++)
    	R8025T.DATA[i]=Hex2Bcd(UTC.DATA[i]);

    Set_R8025t_RtcTime(R8025T.DATA,6);
}
/************************************************************!
*Function: P101Time2R8025TRTCTime
*Description: 将101时间更新到R8025T时间及RTC的CNT。
*Input: 1.data: 数组的首地址
*       1.lens: 数组的长度
*Output: null
*Return: null
*************************************************************/
void P101Time2R8025TRTCTime(uint8_t *data,uint8_t lens)
{
	P101TIME P101T;
	R8025TTIME R8025T;

	memcpy(P101T.DATA,data,7);

	Set_Rtc_Cnt((P101T.MS%1000)*32768/1000);

	R8025T.YEAR=Hex2Bcd(P101T.YEAR);
	R8025T.MONTH=Hex2Bcd(P101T.MONTH);
	R8025T.DAY=Hex2Bcd(P101T.DAY.DAY);
    R8025T.HOUR=Hex2Bcd(P101T.HOUR);
    R8025T.MINUTE=Hex2Bcd(P101T.MINUTE);
    R8025T.SECOND=Hex2Bcd(P101T.MS/1000);

    R8025T.SECOND=Hex2Bcd(P101T.MS/1000);

    Set_R8025t_RtcTime(R8025T.DATA,6);
}
/************************************************************!
*Function: R8025TRTCTime2UnixTime
*Description: 将8025T的时间更新到UNIX时间。
*Input: null
*Output: null
*Return: UNIX时间
*************************************************************/
uint32_t R8025TRTCTime2UnixTime(void)
{
	R8025TTIME R8025T;

	Get_R8025t_RtcTime(R8025T.DATA,6);

	return LocalTime2UnixTime(R8025T.DATA,6);
}
/************************************************************!
*Function: UnixTime2P101Time
*Description: 将UNIX时间戳转化为101规约时间戳格式。
*Input: null
*Output: null
*Return: null
*************************************************************/
bool UnixTime2P101Time(UNIXTIME *UT,uint8_t *data,uint8_t lens)
{
	P101TIME PT;
	UTCTIME UTC;

	if(lens<7) //防止数组越界
		return false;

	UnixTime2LocalTime(UT->UNIX,UTC.DATA,6);

	PT.MS=((uint16_t)UTC.SECOND)*1000+(UT->CNT*1000/32768);
	PT.MINUTE=UTC.MINUTE;
	PT.HOUR=UTC.HOUR;
	PT.DAY.DAY=UTC.DAY;
	PT.DAY.WEEK=YMD2WeekDay(UTC.DATA,6);
	PT.MONTH=UTC.MONTH;
	PT.YEAR=UTC.YEAR;

	memcpy(data,PT.DATA,7);

	return true;
}
/************************************************************!
*Function: NowTime2P101Time
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void NowTime2P101Time(uint8_t *data,uint8_t lens)
{
	UNIXTIME UT;

	Get_Sys_Time(&UT); //提取当前系统时间

	UnixTime2P101Time(&UT,data,lens);
}
/************************************************************!
*Function: PrintOut_NowTime
*Description: 将UTC时间加上8个小时变成东八区时间。
*Input: null
*Output: null
*Return: 东八区时间
*************************************************************/
void PrintOut_NowTime(void)
{
	uint8_t data[8]; //年月日时分秒毫秒
	DOUBLEBYTE DB;

	NowTime2P101Time(data+1,7);

	memcpy(DB.U8,data+1,2);

	data[5]&=0x1F;           //屏蔽掉星期
	data[2]=DB.U16/1000;    //秒
	DB.U16%=1000;
	data[1]=DB.U16/100;     //毫秒高位
	data[0]=DB.U16%100;     //毫秒低位

	Uart1_TxU8(0xAA); //标识

	for(uint8_t i=0;i<8;i++)
		Uart1_TxU8(Byte2Bcd(data[7-i]));

	Uart1_TxU8(0xAA);
}
/************************************************************!
*Function: UTCTime2LocalTime
*Description: 将UTC时间加上8个小时变成东八区时间。
*Input: null
*Output: null
*Return: 东八区时间
*************************************************************/
UTCTIME UTCTime2LocalTime(UTCTIME *UTC)
{
	uint8_t isleapyear;
	uint8_t DAY[2][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31},
			            {0,31,29,31,30,31,30,31,31,30,31,30,31}};

	UTC->HOUR+=8; //将UTC时间加上8个小时变成本地时间

	if(UTC->HOUR>=24)
	{
		UTC->HOUR-=24;
		UTC->DAY++;
	}

	isleapyear=((UTC->YEAR%4)==0?1:0);

	if(UTC->DAY>DAY[isleapyear][UTC->MONTH])
	{
		UTC->DAY=1;
		UTC->MONTH++;

		if(UTC->MONTH>12)
		{
			UTC->MONTH=1;
			UTC->YEAR++;
		}
	}

	return *UTC;
}
/************************************************************!
*Function: Sys_Tick
*Description: RTC秒中断维护UNIX时间戳.每隔4095(FFF)秒使用8025T
*             更新一次UNIX时间戳和RTC的CNT计数值.
*Input: null
*Output: null
*Return: null
*************************************************************/
void Sys_Tick(void)
{
	gUNIXTIME.UNIX++;

	LETIMER0->CMD|=0x04; //清除LETIMER的CNT的累计误差

	gSYSFLAG.CWS.RADIOTICKS=0;

	Run_Led_Toggle();

//	if((gUNIXTIME.UNIX%gSYSPARA.SYSTIME_UPDATE)==0)  //600秒校时一次
//		return;
//
//	gUNIXTIME.UNIX=R8025TRTCTime2UnixTime();
}
/************************************************************!
*Function: Get_Sys_Time
*Description: 当还有小于8/32768=0.365ms产生1S中断的时候，等产
*             生完中断再拷贝时标，防止已经拷贝了CNT，还没有拷贝
*             UNIX时间戳产生1S中断，导致产生时标误差(会产生1S的
*             误差)。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Get_Sys_Time(UNIXTIME *UT)
{
	while(RTC->CNT>32760);

	UT->CNT=RTC->CNT;
	UT->UNIX=gUNIXTIME.UNIX;
}
/************************************************************!
*Function: Get_YYMMDD
*Description: 将101时间格式转化为32位值
*Input: null
*Output: null
*Return: null
*************************************************************/
uint32_t Get_YYMMDD(uint8_t *data,uint8_t lens)
{
	uint32_t unxittime;

	unxittime=((uint32_t)(data[6]<<16))+((uint32_t)(data[5]<<8))+(data[2]&0x1F);

	return unxittime;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Is_Valid_101Clock(uint8_t *data,uint8_t lens)
{
	P101TIME TIME;
	uint8_t  isleapyear;
	uint8_t DAY[2][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31},
			            {0,31,29,31,30,31,30,31,31,30,31,30,31}};

	if(lens<7) //防止数组越界
		return false;

	memcpy(TIME.DATA,data,7);

	if(TIME.YEAR>99)
		return false;

	if(TIME.MONTH==0||TIME.MONTH>12)
		return false;

	isleapyear=((TIME.YEAR%4)==0?1:0);

	if(TIME.DAY.DAY==0||TIME.DAY.DAY>DAY[isleapyear][TIME.MONTH])  //日
		return false;

	if(TIME.DAY.WEEK==0) //星期,后期更改为判断年月日是否和星期对应
		return false;

	if(TIME.HOUR>23)  //时
		return false;

	if(TIME.MINUTE>59)  //分
		return false;

	if(TIME.MS>59999)  //MS不能大于59999
		return false;

	return true;
}

