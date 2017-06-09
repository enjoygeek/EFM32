/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: icp_parse.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef ICP_PARSE_H_
#define ICP_PARSE_H_


#include <stdint.h>
#include <stdbool.h>


bool Icp_Parse(DATACACHE *DC,ICPFRAME *IF);
uint8_t Check_Icp_Cs(uint8_t *data,uint32_t lens);
void PRINT_ICP_RECEIVE_FRAME_DEBUG(ICPFRAME *IF);


#endif

