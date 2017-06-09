/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_math.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "hal_math.h"


/************************************************************!
*Function: Bcd2Byte
*Description: BCD码转化为字节
*Input: 1.bcd：BCD码
*Output: null
*Return: 字节数据
*************************************************************/
uint8_t Bcd2Byte(uint8_t bcd)
{
	uint8_t data;

	data=(uint8_t)((bcd/16)*10+(bcd%16));

	return data;
}
/************************************************************!
*Function: Byte2Bcd
*Description: 字节转化为BCD码
*Input: 1.byte：字节数据
*Output: null
*Return: BCD码
*************************************************************/
uint8_t Byte2Bcd(uint8_t byte)
{
    uint8_t data;

    data=(uint8_t)(((byte/10)<<4)+(byte%10));

    return data;
}
/************************************************************!
*Function: Is_Valid_BCD
*Description: 判断数组是否是合法的BCD码
*Input: 1.data：BCD码数组首地址
*       2.lens：数组长度
*Output: null
*Return: BCD码是否合法
*************************************************************/
bool Is_Valid_BCD(uint8_t *data,uint8_t lens)
{
   for(uint8_t i=0;i<lens;i++)
   {
      if((data[i]&0x0F)>0x09)
    	  return false;

      if((data[i]&0xF0)>0x90)
    	  return false;
   }

   return true;
}
/************************************************************!
*Function: Char2Uint8_t
*Description: 将两个字符转化为一个Uint8_t,只适用于将10进制的时间格式
*             转化为Uint8_t。
*Input: data：需要转换成Uint8_t的字符的首地址
*Output: null
*Return: uint8_t
*************************************************************/
uint8_t Char2Uint8_t(uint8_t *data)
{
	uint8_t result;

	result=Get_Hex(data[0])*10+Get_Hex(data[1]);

	return result;
}
/************************************************************!
*Function: Get_Hex
*Description: 得到字符的HEX值
*Input: data：需要转换成HEX值的字符
*Output: null
*Return: HEX值
*************************************************************/
uint8_t Get_Hex(uint8_t data)
{
	if(data>='0'&&data<='9')
		data=data-'0';

	if(data>='A'&&data<='F')
		data=data-'0'-7; //此处为啥-7，请看ASCII码表,谢谢

	return data;
}
/************************************************************!
*Function: Get_Hex
*Description: 得到字符的HEX值
*Input: data：需要转换成HEX值的字符
*Output: null
*Return: HEX值
*************************************************************/
uint16_t uint8u2uint16u(uint8_t *data)
{
	return (((uint16_t)data[1])<<8)+data[0];
}

