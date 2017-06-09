/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: icp_process.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef ICP_PROCESS_H_
#define ICP_PROCESS_H_


#include <stdint.h>
#include <stdbool.h>


bool Icp_Process(ICPFRAME *IF);
void Icp_Prmsec_Process(ICPFRAME *IF);
void Icp_Prmpri_Process(ICPFRAME *IF);
void Icp_Fccon_Process(ICPFRAME *IF);
void Icp_Fcnocon_Process(ICPFRAME *IF);
void Icp_Fcresdata_Process(ICPFRAME *IF);
void Icp_Fcnodata_Process(ICPFRAME *IF);
void Icp_Fcresetuse_Process(ICPFRAME *IF);
void Icp_Fcsenddata_Process(ICPFRAME *IF);
void Icp_Fcrequestd_Process(ICPFRAME *IF);
void Fill_Icp_Frame(uint8_t channel,uint8_t *addr,uint8_t ctrl,uint8_t type,uint8_t num,uint8_t seq,uint8_t *data,uint8_t lens);
uint8_t Get_Empty_IcpFrame(void);
bool Is_Valid_Icp_Address(ICPFRAME *IF);


#endif

