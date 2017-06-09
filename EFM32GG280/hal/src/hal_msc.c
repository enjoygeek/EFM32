/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_msc.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_msc.h"


/************************************************************!
*Function: Msc_Init
*Description: MSC初始化函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void Msc_Init(void)
{
	MSC_Init();
}
/************************************************************!
*Function: Msc_ReadWord
*Description: MSC读字函数,在RAM运行,地址必须为4的倍数,非任意地址读
*Input: null
*Output: null
*Return: null
*************************************************************/
MSC_RAMFUNC_DEFINITION_BEGIN
uint32_t Msc_ReadWord(uint32_t addr)
{
	uint32_t data;

	data = *((uint32_t *)addr);

	return data;
}
MSC_RAMFUNC_DEFINITION_END
/************************************************************!
*Function: Msc_WriteWord
*Description: MSC写字函数,在RAM运行,地址必须为4的倍数,非任意地址写
*Input: null
*Output: null
*Return: null
*************************************************************/
MSC_RAMFUNC_DEFINITION_BEGIN
MSC_Status_TypeDef Msc_WriteWord(uint32_t addr,uint32_t data)
{
	MSC_WriteWordFast((uint32_t *)addr,&data,4);

	return mscReturnOk;
}
MSC_RAMFUNC_DEFINITION_END
/************************************************************!
*Function: Msc_ErasePage
*Description: MSC擦除块函数
*Input: null
*Output: null
*Return: null
*************************************************************/
MSC_RAMFUNC_DEFINITION_BEGIN
MSC_Status_TypeDef Msc_ErasePage(uint32_t addr)
{
	uint32_t and=0xFFFFFFFF;

	for(uint32_t i=addr;i<(addr+4096);i++)
		and&=*((uint32_t *)i);

	if(and!=0xFFFFFFFF)
		MSC_ErasePage((uint32_t *)addr);

	return mscReturnOk;
}
MSC_RAMFUNC_DEFINITION_END

