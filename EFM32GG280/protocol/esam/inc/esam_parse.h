/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: esam_parse.h
*Version: 0.1.0
*Date: 2017-05-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef ESAM_PARSE_H_
#define ESAM_PARSE_H_


#include <stdint.h>
#include <stdbool.h>
#include "include.h"


bool Esam_Parse(DATACACHE *DC,ESAMFRAME *EF);
void Ecp101_Parse(ESAMFRAME *EF);
uint8_t Check_Esam_Cs(uint8_t *data,uint32_t lens);



#endif
