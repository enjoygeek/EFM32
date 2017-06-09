/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_gpio.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_gps.h"
#include "hal_math.h"


/************************************************************!
*Function: Gps_Abstime_Process
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Gps_Abstime_Process(uint8_t data)
{
	static uint8_t crc=0;
	static uint8_t index=0;
	static uint8_t state=0;
	static uint8_t GPS[GPS_BUFF_LEN];

	switch(state)
	{
		case 0:
			if(data=='$')
			{
				state=1;
				index=0;

				return false;
			}
			break;

		case 1:              //判断Talker ID"GPRMC"
			GPS[index++]=data;

			if(index<5)
				return false;

			if(memcmp(GPS,"GPRMC",5)!=0)
			{
				state=0;
				index=0;
			}
			else
				state++;
			break;

		case 2: //收取帧内容
			if(data!='*')
			{
				GPS[index++]=data;

				if(index>=68)
				{
					state=0;
					index=0;
				}
			}
			else
				state++;
			break;

		case 3: //收取校验的第一个字符
            crc=Get_Hex(data)<<4;
            state++;
			break;

		case 4:
			crc+=Get_Hex(data);

			if(crc==Check_Gps_Crc(GPS,index))
			{
				#ifdef PRINT_GPS_TIMMING_INFO_DEBUG
				DEBUG_PRINT_U8_ARRAY(GPS,index);
				#endif

				index=0;
				state=0;

				if(Get_Gps_Abstime(GPS,GPS_BUFF_LEN)==true)
					return true;
			}

			index=0;
			state=0;

			break;

		 default:
			 state=0;
			 index=0;
			 break;
	}

	return false;
}
/************************************************************!
*Function: Get_Gps_Abstime
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
bool Get_Gps_Abstime(uint8_t *data,uint8_t lens)
{
	UTCTIME UTC;
	uint8_t index;

	if(data[17]!='A') //判断UTC是否合法，不合法返回
		return false;

	index=Get_Specified_Char_Index(data,lens,0x2C,1);
	UTC.HOUR=Char2Uint8_t(data+index+1);
	UTC.MINUTE=Char2Uint8_t(data+index+3);
	UTC.SECOND=Char2Uint8_t(data+index+5);

	index=Get_Specified_Char_Index(data,lens,0x2C,9);
	UTC.DAY=Char2Uint8_t(data+index+1);
	UTC.MONTH=Char2Uint8_t(data+index+3);
	UTC.YEAR=Char2Uint8_t(data+index+5);

	GPSTime2R8025TRTCTime(UTC.DATA,6);

	gUNIXTIME.UNIX=UTCTime2UnixTime(UTC.DATA,6); //UTC时间转换

	return true;
}
/************************************************************!
*Function: Get_Specified_Char_Index
*Description: 在数组中查找第loction个ch字符所在的数组索引。
*Input: 1.data：数组的首地址
*       2.lens：数组长度
*       3.ch: 需要查找的字符
*       4.loction: 需要查找的是第几个该字符
*Output: null
*Return: 所在的数组索引
*************************************************************/
uint8_t Get_Specified_Char_Index(uint8_t *data,uint8_t lens,uint8_t ch,uint8_t loction)
{
	uint8_t number=0;

	for(uint8_t i=0;i<lens;i++)
	{
		if(data[i]==ch)
			number++;

		if(number==loction)
			return i;
	}

	return 0xFF;
}
/************************************************************!
*Function: Check_Gps_Crc
*Description: 检验数组元素的异或值
*Input: 1.data：数组的首地址
*       2.lens：数组长度
*Output: null
*Return: 异或值
*************************************************************/
uint8_t Check_Gps_Crc(uint8_t *data,uint8_t lens)
{
	uint8_t crc=data[0];

    for(uint8_t i=1;i<lens;i++)
    	crc^=data[i];

    return crc;
}

