/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: ecp_process.h
*Version: 0.1.0
*Date: 2016-10-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef ECP_PROCESS_H_
#define ECP_PROCESS_H_


#include <stdint.h>
#include <stdbool.h>


bool Ecp_Process(ECPFRAME *EF);
void App_Send_Ack(ECPFRAME *EF);
void App_Send_NoAck(ECPFRAME *EF);
void App_Send_Linkstate(ECPFRAME *EF);
void App_Reset_Link(ECPFRAME *EF);
void App_Call_Master_Linkstate(ECPFRAME *EF);
void App_Reset_Master_Link(ECPFRAME *EF);
void App_Init_End(ECPFRAME *EF);
bool Check_Ecp_Rtu(ECPFRAME *EF);
bool Is_Sys_Full(void);
uint8_t Get_Empty_ResFrame(ECPFRAME *EF,uint8_t priority);
uint8_t Req_Empty_ResFrame(ECPFRAME *EF,uint8_t priority);
void Fill_Ecp_Frame(ECPFRAME *EF,uint8_t frametype,uint8_t priority,uint8_t sn,uint8_t lens);
void Fill_Ecp_DataCache(ECPFRAME *EF,uint8_t priority,uint8_t sn);
void Ecp_Fcb_Flip(ECPFRAME *EF,uint8_t priority,uint8_t sn);
void Add_P101SoeQueue(SOEQUEUE *SQ,P101SOE *PS);
uint8_t Get_P101SoeQueue(SOEQUEUE *SQ);
bool App_Send_P101SoeQueue(ECPFRAME *EF,SOEQUEUE *SQ);
void Add_P101FaultQueue(FAULTQUEUE *FQ,P101FAULT *PS);
uint8_t Get_P101FaultQueue(FAULTQUEUE *FQ);
bool App_Send_P101FaultQueue(ECPFRAME *EF,FAULTQUEUE *FQ);
void PRINT_ECP_RECEIVE_FRAME(ECPFRAME *EF);
void PRINT_ECP_SEND_FRAME(ECPFRAME *EF,uint8_t priority,uint8_t sn);


#endif

