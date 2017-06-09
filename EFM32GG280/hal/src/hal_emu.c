/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_emu.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_emu.h"


/************************************************************!
*Function: Emu_Init
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Emu_Init(void)
{
	EMU_EM23Init_TypeDef Em23_Init=EMU_EM23INIT_DEFAULT;

	EMU_EM23Init(&Em23_Init);
}

