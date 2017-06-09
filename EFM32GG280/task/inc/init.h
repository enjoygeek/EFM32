/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: init.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef INIT_H_
#define INIT_H_


void BSP_Init(void);
void APP_Init(void);
void DataFile_Init(void);
void Mcu_Reset_Record(void);
void Sys_Status_Report(void);
bool SysFlag_PrintOut(void);
bool Gps_Timing_Process(void);
void Run_Led_Toggle(void);
void Run_Led_Set(void);
void MX25_CHIP_ALL_ERASE(void);
void Default_Handler(void);


#endif

