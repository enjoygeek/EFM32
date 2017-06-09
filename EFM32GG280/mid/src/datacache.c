/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: datacache.c
*Version: 0.2.0
*Date: 2016-11-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "datacache.h"


/************************************************************!
*Function: CreateDataCache
*Description: 创建数据缓存
*Input: 1.DC: 数据缓存名
*       2.length: 数据缓存长度
*Output: null
*Return: 函数运行状态
*************************************************************/
bool CreateDataCache(DATACACHE *DC,uint32_t length)
{
	DC->READ=0;
	DC->WRITE=0;
	DC->LENGTH=length;

	DC->CACHE=(uint8_t *)malloc(DC->LENGTH*sizeof(uint8_t));

	if(DC->CACHE==NULL) //临时处理,后期移植需要注意
		while(1);

	memset(DC->CACHE,0,DC->LENGTH*sizeof(uint8_t));

	return true;
}
/************************************************************!
*Function: ReadDataCache
*Description: 读取数据缓存数据
*Input: 1.DC: 数据缓存名
*       2.data: 数组首地址
*       3.lens: 数组长度.若读取的数据长度大于数据缓存中的未读
*               取的数据的长度,将未读到的数据赋0.
*Output: null
*Return: 函数运行状态
*************************************************************/
void ReadDataCache(DATACACHE *DC,uint8_t *data,uint32_t lens)
{
	for(uint32_t i=0;i<lens;i++)
	{
		if(((DC->LENGTH+DC->WRITE-DC->READ)%DC->LENGTH)>0)
		{
			data[i]=DC->CACHE[DC->READ++];

			DC->READ%=DC->LENGTH;

			continue;
		}

		data[i]=0;
	}
}
/************************************************************!
*Function: VirtualReadDataCache
*Description: 虚拟读取数据缓存数据(不会改变数据缓存的读指针)
*Input: 1.DC: 数据缓存名
*       2.loc: 要读取的数据的起始位置
*       3.data: 数组首地址
*       4.lens: 数组长度.若读取的数据长度大于数据缓存中
*               的数据长度,将未读到的数据位赋0,
*Output: null
*Return: 函数运行状态
*************************************************************/
void VirtualReadDataCache(DATACACHE *DC,uint32_t loc,uint8_t *data,uint32_t lens)
{
	uint32_t newloc=loc;

	for(uint32_t i=0;i<lens;i++)
	{
		if(((DC->LENGTH+DC->WRITE-newloc)%DC->LENGTH)>0)
		{
			data[i]=DC->CACHE[newloc++];

			newloc%=DC->LENGTH;

			continue;
		}

		data[i]=0;
	}
}
//void VirtualReadDataCache(DATACACHE *DC,uint32_t loc,uint8_t *data,uint32_t lens)
//{
//	uint32_t newloc=loc;
//
//	for(uint32_t i=0;i<lens;i++)
//	{
//		newloc%=DC->LENGTH;
//
//		if(((DC->LENGTH+DC->WRITE-newloc)%DC->LENGTH)>0)
//		{
//			data[i]=DC->CACHE[newloc++];
//
//			continue;
//		}
//
//		data[i]=0;
//	}
//}
/************************************************************!
*Function: WriteDataCache
*Description: 写入数据缓存数据
*Input: 1.DC: 数据缓存名
*       2.data: 数组首地址
*       3.lens: 数组长度
*Output: null
*Return: 函数运行状态
*************************************************************/
bool WriteDataCache(DATACACHE *DC,uint8_t *data,uint32_t lens)
{
	if((DataCacheLength(DC)+lens)>=DC->LENGTH)
		return false;

	for(uint32_t i=0;i<lens;i++)
	{
		DC->CACHE[DC->WRITE++]=data[i];

		DC->WRITE%=DC->LENGTH;
	}

	return true;
}
//bool WriteDataCache(DATACACHE *DC,uint8_t *data,uint32_t lens)
//{
//	for(uint32_t i=0;i<lens;i++)
//	{
//		DC->WRITE=(DC->WRITE+1)%DC->LENGTH;
//
//		DC->CACHE[DC->WRITE]=data[i];
//
//		DC->READ=(DC->WRITE>=DC->READ)?(DC->WRITE+1):DC->READ; //+1:防止DataCacheLength出错
//	}
//
//	return true;
//}
/************************************************************!
*Function: DataCacheLength
*Description: 得到数据缓存中未读取的数据的长度
*Input: DC: 数据缓存名
*Output: null
*Return: 数据缓存中未读取的数据的长度
*************************************************************/
uint32_t DataCacheLength(DATACACHE *DC)
{
	return ((DC->LENGTH+DC->WRITE-DC->READ)%DC->LENGTH);
}
/************************************************************!
*Function: ClearDataCache
*Description: 清空数据缓存
*Input: 1.DC: 数据缓存名
*Output: null
*Return: null
*************************************************************/
void ClearDataCache(DATACACHE *DC)
{
	DC->READ=0;
	DC->WRITE=0;

	memset(DC->CACHE,0,DC->LENGTH*sizeof(uint8_t)); //清空队列
}
/************************************************************!
*Function: FreeDataCache
*Description: 释放数据缓存
*Input: DC: 数据缓存名
*Output: null
*Return: null
*************************************************************/
void FreeDataCache(DATACACHE *DC)
{
	DC->READ=0;
	DC->WRITE=0;
	DC->LENGTH=0;

	if(DC->CACHE!=NULL)
		free(DC->CACHE);
}

