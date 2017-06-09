/*************************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: unixtime.c
*Version: 0.1.0
*Date: 2016-11-05
*Author: WANGJIANHUI
*Description:
*************************************************************************/
#include "include.h"
#include "unixtime.h"


/************************************************************!
*Function: UTCTime2UnixTime
*Description: 将UTC时间格式转化为UNIX时间戳格式。
*Input: 1.data: 数组
*       2.lens: 数组长度
*Output: null
*Return: UNIX时间戳
*************************************************************/
uint32_t UTCTime2UnixTime(uint8_t *data,uint8_t lens)
{
	UTCTIME UTC;
	uint8_t  isleapyear;
	uint32_t day,unixtime;
	uint32_t MONTH[2][13]={{0,0,31,59,90,120,151,181,212,243,273,304,334},
						   {0,0,31,60,91,121,152,182,213,244,274,305,335}};

	if(lens<6)
		return 0;

	memcpy(UTC.DATA,data,6);

	day=(UTC.YEAR/4)*(366+365*3);

	if((UTC.YEAR%4)>=1)
		day+=366+((UTC.YEAR%4)-1)*365;

	isleapyear=((UTC.YEAR%4)==0?1:0); //闰年判断

	day+=MONTH[isleapyear][UTC.MONTH];

	day+=UTC.DAY-1; //日从1号开始,将当前的天减去

	unixtime=day*24*3600+UTC.HOUR*3600+UTC.MINUTE*60+UTC.SECOND;

	unixtime+=23*365*24*3600+7*366*24*3600; //将unixtime的时间基准由2000年1月1日变为1970年1月1日

	return unixtime;
}
/************************************************************!
*Function: LocalTime2UnixTime
*Description: 将东八区时间转化为UNIX时间戳格式
*Input: 1.data: 数组
*       2.lens: 数组长度
*Output: null
*Return: UNIX时间戳
*************************************************************/
uint32_t LocalTime2UnixTime(uint8_t *data,uint8_t lens)
{
	UTCTIME UTC;
	uint8_t  isleapyear;
	uint32_t day,unixtime;
	uint32_t MONTH[2][13]={{0,0,31,59,90,120,151,181,212,243,273,304,334},
						   {0,0,31,60,91,121,152,182,213,244,274,305,335}};

	memcpy(UTC.DATA,data,6);

	day=(UTC.YEAR/4)*(366+365*3);

	if((UTC.YEAR%4)>=1)
		day+=366+((UTC.YEAR%4)-1)*365;

	isleapyear=((UTC.YEAR%4)==0?1:0); //闰年判断

	day+=MONTH[isleapyear][UTC.MONTH];

	day+=UTC.DAY-1; //日从1号开始,将当前的天减去

	unixtime=day*24*3600+UTC.HOUR*3600+UTC.MINUTE*60+UTC.SECOND;

	unixtime+=23*365*24*3600+7*366*24*3600; //将unixtime的时间基准由2000年1月1日变为1970年1月1日

	unixtime-=8*3600; //将东八区的时间戳转为标准时间的时间戳

	return unixtime;
}
/************************************************************!
*Function: UnixTime2LocalTime
*Description: 将UNIX时间戳转化为东八区时间。
*Input: 1.unixtime: UNIX时间戳
*       2.data: 数组
*       2.lens: 数组长度
*Output: null
*Return: null
*************************************************************/
void UnixTime2LocalTime(uint32_t unixtime,uint8_t *data,uint8_t lens)
{
	uint8_t  isleapyear;
	UTCTIME UTC;
	uint32_t day;
	uint32_t LEAPYEAR=366*24*3600; //闰年的秒数
	uint32_t COMMYEAR=365*24*3600; //平年的秒数
	uint32_t SOFY=LEAPYEAR+3*COMMYEAR; //2000年开始每一个4年的秒数(seconds of four year)

	uint16_t month[2][14]={{0,0,31,59,90,120,151,181,212,243,273,304,334,365},
			               {0,0,31,60,91,121,152,182,213,244,274,305,335,366}};

	unixtime+=8*3600; //将标准时间转为东八区时间

	unixtime-=23*365*24*3600+7*366*24*3600; //将UNIX基准从1970年1月1日变为2000年1月1日

	UTC.YEAR=(unixtime/SOFY)*4;

	unixtime-=(UTC.YEAR/4)*SOFY;

	for(uint8_t i=1;i<4;i++)
	{
		if((unixtime>=(LEAPYEAR+(i-1)*COMMYEAR))&&(unixtime<(LEAPYEAR+i*COMMYEAR)))
		{
			UTC.YEAR+=i;

			unixtime-=LEAPYEAR+(i-1)*COMMYEAR;

			break;
		}
	}

	day=unixtime/(24*3600);

	isleapyear=(UTC.YEAR%4)==0?1:0;

	for(uint8_t i=1;i<13;i++)
	{
		if(day>=month[isleapyear][i]&&day<month[isleapyear][i+1])
		{
			UTC.MONTH=i;
			UTC.DAY=day-month[isleapyear][i]+1; //当前天需要加1
			unixtime-=day*24*3600;
			break;
		}
	}

	UTC.HOUR=unixtime/3600;

	UTC.MINUTE=((unixtime%3600)/60);

	UTC.SECOND=unixtime%60;

	memcpy(data,UTC.DATA,6);
}
/************************************************************!
*Function: YMD2WeekDay
*Description: 年月日转化星期函数
*Input: 1.data: 数组首地址
*       2.lens: 数组的长度
*Output: null
*Return: 星期
*************************************************************/
uint8_t YMD2WeekDay(uint8_t *data,uint8_t lens)
{
	UTCTIME UTC;
	uint32_t day;
	uint8_t week,isleapyear;

	uint32_t MONTH[2][13]={{0,0,31,59,90,120,151,181,212,243,273,304,334},
						   {0,0,31,60,91,121,152,182,213,244,274,305,335}};

	memcpy(UTC.DATA,data,6);

	day=(UTC.YEAR/4)*(366+365*3);

	if((UTC.YEAR%4)>=1)
		day+=366+((UTC.YEAR%4)-1)*365;

	isleapyear=(((UTC.YEAR%4)==0)?1:0); //闰年判断

	day+=MONTH[isleapyear][UTC.MONTH];

	day+=UTC.DAY;

	week=(day+5)%7; //2000年1月1号为星期5

	if(week==0) //星期从1-7
		week=7;

	return week;
}
/************************************************************!
*Function: Is_Valid_Clock
*Description: 判断时钟是否合法函数.主要包括格式是否是BCD码,年月日
*             时分秒是否超出计量范围。
*Input: 1.data: 数组首地址
*       2.lens: 数组的长度
*Output: null
*Return: null
*************************************************************/
uint8_t Is_Valid_Clock(uint8_t *data,uint8_t lens)
{
	UTCTIME UTC;
	uint8_t isleapyear;
	uint8_t DAY[2][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31},
			            {0,31,29,31,30,31,30,31,31,30,31,30,31}};

	if(lens<6) //防止数组越界
		return false;

	for(uint8_t i=0;i<lens;i++) //判断每一个字节是否是有效的BCD码
	{
		if(Is_Valid_Bcd(data[i])==false)
			return false;
	}

	memcpy(UTC.DATA,data,6);

	if(UTC.YEAR>99)
		return false;

	if(UTC.MONTH==0||UTC.MONTH>12)
		return false;

	isleapyear=((UTC.YEAR%4)==0?1:0);

	if(UTC.DAY==0||UTC.DAY>DAY[isleapyear][UTC.MONTH])
		return false;

	if(UTC.HOUR>23)
		return false;

	if(UTC.MINUTE>59)
		return false;

	if(UTC.SECOND>59)
		return false;

	return true;
}
/************************************************************!
*Function: Is_Valid_Bcd
*Description: 判断字节是否是合法的BCD码。
*Input: bcd: BCD码
*Output: null
*Return: 是否合法
*************************************************************/
uint8_t Is_Valid_Bcd(uint8_t bcd)
{
	SINGLEBYTE SB;

	SB.DATA=bcd;

	if(SB.U03<=9&&SB.U47<=9)
		return true;

	return false;
}
/************************************************************!
*Function: Hex2Bcd
*Description: 将HEX转化为BCD码
*Input: hex: hex
*Output: null
*Return: BCD码
*************************************************************/
uint8_t Hex2Bcd(uint8_t hex)
{
	uint8_t bcd;

	bcd=(uint8_t)(((hex/16)<<4)+(hex%16));

	return bcd;
}
/************************************************************!
*Function: Bcd2Hex
*Description: 将BCD码转化为HEX
*Input: bcd: BCD码
*Output: null
*Return:HEX
*************************************************************/
uint8_t Bcd2Hex(uint8_t bcd)
{
	uint8_t hex;

	hex=(uint8_t)((((bcd>>4)&0x0F)*10)+(bcd&0x0F));

	return hex;
}

