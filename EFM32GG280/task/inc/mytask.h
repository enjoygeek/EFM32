/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: mytask.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef MYTASK_H_
#define MYTASK_H_


void Test_Task(void *pvParameters);
void Power_Monitor(void *pvParameters);
void Icp_Protocol_Comm(void *pvParameters);
void Ecp_Protocol_Comm(void *pvParameters);
void Call_Wave(void *pvParameters);
void Data_Report(void *pvParameters);
void Data_Send_Reply(void *pvParameters);
void Ecp_File_Report(void *pvParameters);
void vApplicationIdleHook(void *pvParameters);
void vApplicationStackOverflowHook(TaskHandle_t xTask,signed char *pcTaskName);
bool Power_Management(void);
void Mx25_Erase_Test(uint32_t startaddr,uint32_t sectornum);


#endif

