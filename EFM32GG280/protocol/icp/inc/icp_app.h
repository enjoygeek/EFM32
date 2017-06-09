/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: icp_app.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef ICP_APP_H_
#define ICP_APP_H_


#include <stdint.h>
#include <stdbool.h>


void Icp_PduType_Process(ICPFRAME *IF);
void App_Auticks_Process(ICPFRAME *IF);
void App_Timestamp_Process(ICPFRAME *IF);
bool App_Yc_Process(ICPFRAME *IF);
bool App_Soe_Yx_Process(ICPFRAME *IF);
bool App_Wave_Process(ICPFRAME *IF);
bool App_Para_Process(ICPFRAME *IF);
bool App_Program_Process(ICPFRAME *IF);
bool App_Faultwrg_Process(ICPFRAME *IF);
void App_Led_Process(ICPFRAME *IF);
void App_WakeUp_Process(ICPFRAME *IF);
void App_Reset_Process(ICPFRAME *IF);
bool App_WorkMode_Process(ICPFRAME *IF);
bool Modify_Au_WorkMode(uint8_t line,uint8_t phase,uint8_t workmode);
bool Au_Online_Process(ICPFRAME *IF);
void Au_Offline_Process(void);
bool Au_WorkMode_Process(void);
bool Au_Timing_Process(void);
bool Cu_Call_Wave(void);
bool ShortCircuit_Fault_Process(void);
bool Trigger_Start_Call_Wave(uint8_t line,uint8_t *wavetime);
void Au_Timing_Frame(void);
void Au_WakeUp_Frame(uint8_t line);
uint16_t Calc_NVA_F16_Value(uint16_t data,uint32_t max,uint16_t scale);
void Correc_The_Phase(void);
void Wave_Cfg_File_Init(void);
uint8_t UnixTime2StringTime(uint8_t *datetime,uint8_t *data);
void DEBUG_ERASE_MX25(ICPFRAME *IF);
void DEBUG_READ_LOG(void);
void DEBUG_ECP_READ_LOG(void);
bool DEBUG_PRINT_LOG(uint32_t read);
void DEBUG_READ_SOE(void);
void DEBUG_ECP_READ_SOE(void);
bool DEBUG_PRINT_SOE(uint32_t read);
void DEBUG_READ_YC(void);
void DEBUG_ECP_READ_YC(uint16_t fileid);
void DEBUG_PRINT_YC_DATA(ICPFRAME *IF);
bool DEBUG_PRINT_YC_FILE_SECTION(uint32_t read,uint32_t sn);
bool DEBUG_PRINT_YC_FILE(uint32_t read);
void DEBUG_READ_FAULT(void);
void DEBUG_ECP_READ_FAULT(void);
bool DEBUG_PRINT_FAULT(uint32_t read);
void DEBUG_READ_YK(void);
void DEBUG_ECP_READ_YK(void);
bool DEBUG_PRINT_YK(uint32_t read);
void DEBUG_READ_WAVE(void);
bool DEBUG_ECP_READ_WAVE(uint16_t fileid);
bool DEBUG_PRINT_WAVE(uint32_t read);
void DEBUG_READ_NOWTIME(void);
void DEBUG_ERASE_PARABLOCK(void);
bool DEBUG_ECP_START_WAVE(ICPFRAME *IF);


#endif

