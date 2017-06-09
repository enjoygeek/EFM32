/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: datacache.h
*Version: 0.2.0
*Date: 2016-11-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef DATACACHE_H_
#define DATACACHE_H_


#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/************************************************************!
*Function: 数据缓存结构体
*Description:
*************************************************************/
typedef struct
{
	         uint8_t   *CACHE;  //数据指针
    volatile uint32_t  READ;    //读指针
    volatile uint32_t  WRITE;   //写指针
    volatile uint32_t  LENGTH;  //缓存长度
}DATACACHE;


bool CreateDataCache(DATACACHE *DC,uint32_t length);
void ReadDataCache(DATACACHE *DC,uint8_t *data,uint32_t lens);
void VirtualReadDataCache(DATACACHE *DC,uint32_t location,uint8_t *data,uint32_t lens);
bool WriteDataCache(DATACACHE *DC,uint8_t *data,uint32_t lens);
uint32_t DataCacheLength(DATACACHE *DC);
void ClearDataCache(DATACACHE *DC);
void FreeDataCache(DATACACHE *DC);


#endif

