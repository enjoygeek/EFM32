/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: ecp_parse.h
*Version: 0.1.0
*Date: 2016-10-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef ECP_PARSE_H_
#define ECP_PARSE_H_


#include <stdint.h>


bool Ecp_Parse(DATACACHE *DC,ECPFRAME *EF);
uint8_t Check_Ecp_Cs(uint8_t *data,uint32_t lens);


#endif

