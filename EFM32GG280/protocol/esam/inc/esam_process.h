/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: esam_process.h
*Version: 0.1.0
*Date: 2017-05-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef ESAM_PROCESS_H_
#define ESAM_PROCESS_H_


#include <stdint.h>
#include <stdbool.h>
#include "include.h"


bool Esam_Process(ESAMFRAME *EF);
bool Fill_Esam_Frame(ESAMFRAME *EF,uint8_t sn);
uint8_t Get_Empty_EsamFrame(ESAMFRAME *EF);
uint8_t Req_Empty_EsamFrame(ESAMFRAME *EF);





#endif
