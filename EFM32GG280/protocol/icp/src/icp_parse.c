/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: icp_parse.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"


/************************************************************!
*Function: Icp_Parse
*Description:
*Input:
*Output: null
*Return: 函数运行状态
*************************************************************/
//bool Icp_Parse(DATACACHE *DC,ICPFRAME *IF)
//{
//	if(IF->ISLOCK==true) //解析完一帧,并且此帧还未被处理,则不会继续下一帧解析
//		return false;
//
//	while(DataCacheLength(DC)>0)
//	{
//		if(IF->PARSE==false) //还未开始解析数据帧
//		{
//			if(DataCacheLength(DC)<ICP_FRAMEHEAD) //等待队列接收完数据帧头
//				return false;
//
//			ReadDataCache(DC,IF->FRAME,1); //读取队列的第一个字节
//
//			if(IF->STARTBYTE1!=ICP_STARTBYTE)
//				continue;
//
//			VirtualReadDataCache(DC,DC->READ,IF->FRAME+1,ICP_FRAMEHEAD-1);//虚拟读取数据帧头
//
//			if(!((IF->STARTBYTE1==IF->STARTBYTE2)&&(IF->LEN1==IF->LEN2)))
//			{
//				DEBUG_PRINT_NISR("KKNSKK","<----- 当前数据帧帧头错误!");
//				continue;
//			}
//
//			if(!((IF->LEN1>=ICP_MINPDULEN)&&(IF->LEN1<=ICP_MAXPDULEN))) //判断长度域是否正确
//				continue;
//
//			IF->PARSE=true; //找到数据帧头,开始解析数据
//		}
//
//		if(DataCacheLength(DC)<(IF->LEN1+ICP_FRAMETAIL)) //等待整帧数据接收完
//			return false;
//
//		IF->PARSE=false;
//
//		VirtualReadDataCache(DC,DC->READ+ICP_FRAMEHEAD-1,IF->FRAME+ICP_FRAMEHEAD,IF->LEN1); //读取长度域
//		VirtualReadDataCache(DC,DC->READ+ICP_FRAMEHEAD-1+IF->LEN1,IF->FRAME+ICP_FRAMEHEAD+ICP_MAXPDULEN,ICP_FRAMETAIL); //读取校验码和结束符
//
//		#ifdef PRINT_ICP_FRAME_PARSE_DEBUG
//		//PRINT_ICP_RECEIVE_FRAME_DEBUG(IF);
//		#endif
//
//		if(IF->ENDBYTE!=ICP_ENDBYTE)
//		{
//			#ifdef PRINT_ICP_FRAME_PARSE_DEBUG
//			DEBUG_PRINT_NISR("KKNSDKK","<----- 当前数据帧帧结束字符错误!错误的结束符  ",IF->ENDBYTE);
//			#endif
//
//			continue;
//		}
//
//		if(IF->CS!=Check_Icp_Cs(IF->FRAME+ICP_FRAMEHEAD,IF->LEN1))
//		{
//			#ifdef PRINT_ICP_FRAME_PARSE_DEBUG
//			DEBUG_PRINT_NISR("KKNSDKK","<----- 当前数据帧帧CS错误!错误的CS ",IF->CS);
//			#endif
//
//			continue;
//		}
//
//		ReadDataCache(DC,IF->FRAME+1,ICP_FRAMEHEAD-1+IF->LEN1); //读取长度域
//		ReadDataCache(DC,IF->FRAME+ICP_FRAMEHEAD+ICP_MAXPDULEN,ICP_FRAMETAIL); //读取校验码和结束符
//
//		IF->ISLOCK=true; //数据帧解析完毕,将此帧上锁
//
//		return true;
//	}
//
//	return false;
//}
bool Icp_Parse(DATACACHE *DC,ICPFRAME *IF)
{
	if(IF->ISLOCK==true) //解析完一帧,并且此帧还未被处理,则不会继续下一帧解析
		return false;

	while(DataCacheLength(DC)>0)
	{
		if(IF->PARSE==false) //还未开始解析数据帧
		{
			if(DataCacheLength(DC)<ICP_FRAMEHEAD) //等待队列接收完数据帧头
				return false;

			ReadDataCache(DC,IF->FRAME,1); //读取队列的第一个字节

			if(IF->STARTBYTE1!=ICP_STARTBYTE)
				continue;

			VirtualReadDataCache(DC,DC->READ,IF->FRAME+1,ICP_FRAMEHEAD-1);//虚拟读取数据帧头

			if(!((IF->STARTBYTE1==IF->STARTBYTE2)&&(IF->LEN1==IF->LEN2)))
				continue;

			if(!((IF->LEN1>=ICP_MINPDULEN)&&(IF->LEN1<=ICP_MAXPDULEN))) //判断长度域是否正确
				continue;

			IF->PARSE=true; //找到数据帧头,开始解析数据
			ReadDataCache(DC,IF->FRAME+1,ICP_FRAMEHEAD-1); //读取数据帧头
		}

		if(DataCacheLength(DC)<(IF->LEN1+ICP_FRAMETAIL)) //等待整帧数据接收完
			return false;

		IF->PARSE=false;

		ReadDataCache(DC,IF->FRAME+ICP_FRAMEHEAD,IF->LEN1); //读取长度域
		ReadDataCache(DC,IF->FRAME+ICP_FRAMEHEAD+ICP_MAXPDULEN,ICP_FRAMETAIL); //读取校验码和结束符

		if(IF->ENDBYTE!=ICP_ENDBYTE)
			continue;

		if(IF->CS!=Check_Icp_Cs(IF->FRAME+ICP_FRAMEHEAD,IF->LEN1))
			continue;

		IF->ISLOCK=true; //数据帧解析完毕,将此帧上锁

		return true;
	}

	return false;
}
/************************************************************!
*Function: Check_Icp_Cs
*Description:
*Output: null
*Return:
*************************************************************/
uint8_t Check_Icp_Cs(uint8_t *data,uint32_t lens)
{
	uint8_t cs=0;

	for(uint32_t i=0;i<lens;i++)
		cs+=data[i];

	return cs;
}
/************************************************************!
*Function:
*Description:
*Output: null
*Return:
*************************************************************/
void PRINT_ICP_RECEIVE_FRAME_DEBUG(ICPFRAME *IF)
{
	DEBUG_PRINT_NISR("KKNSK","<----- 已接收ICP数据帧内容:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(IF->FRAME,ICP_FRAMEHEAD+IF->LEN1);
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(IF->FRAME+ICP_FRAMEHEAD+ICP_MAXPDULEN,ICP_FRAMETAIL);

	DEBUG_PRINT_NISR("KK");
}


