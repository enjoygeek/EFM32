/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_msc.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_MSC_H_
#define HAL_MSC_H_


#include <stdint.h>
#include <stdbool.h>
#include "em_msc.h"


void Msc_Init(void);
MSC_RAMFUNC_DECLARATOR uint32_t Msc_ReadWord(uint32_t addr);
MSC_RAMFUNC_DECLARATOR MSC_Status_TypeDef Msc_WriteWord(uint32_t addr,uint32_t data);
MSC_RAMFUNC_DECLARATOR MSC_Status_TypeDef Msc_ErasePage(uint32_t addr);


#endif

