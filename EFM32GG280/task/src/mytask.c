/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: mytask.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "mytask.h"
#include <stdlib.h>
#include <time.h>


/************************************************************!
*Function: Test_Task
*Description: 测试任务,仅测试使用
*Input: null
*Output: null
*Return: null
*************************************************************/
void Test_Task(void *pvParameters)
{
	#ifdef TASKSTACK_HIGHWATERMARK
	UBaseType_t uxHighWaterMark;
	uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
	#endif

	while(1)
	{
		Wdog_Feed();

		//Gprs_StartTx();
		Gprs_Queue_Tx();
		WeiHu_StartTx();

		SysFlag_PrintOut();
		Au_Timing_Process();
		Gps_Timing_Process();
		Au_Offline_Process();
		//Au_WorkMode_Process();

		#ifdef TASKSTACK_HIGHWATERMARK
		uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
		if(uxHighWaterMark<=100)
			DEBUG_PRINT_NISR("NSDS","当前 Test_Task 任务剩余堆栈空间为: ",uxHighWaterMark*4," 字节!\r\n");
		#endif
	}
}
/************************************************************!
*Function: Power_Monitor
*Description: 电源检测任务,任务的主要功能是定时测量三个电源的电压,
*             若蓄电池电压小于阈值,则开启PWM充电,否则关闭充电。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Power_Monitor(void *pvParameters)
{
	uint8_t erratashift;
	uint32_t samples[3];

	#ifdef TASKSTACK_HIGHWATERMARK
	UBaseType_t uxHighWaterMark;
	uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
	#endif

	erratashift=Adc0_Errata(); //读取ADC的校正数据

	while (1)
	{
		Wdog_Feed();

		Timer2_Stop(); //测量电压前先关闭PWM，防止测量的电压数据不准确

		vTaskDelay(pdMS_TO_TICKS(1000)); //关闭PWM后延时100MS，待电压稳定

		DMA_ActivateBasic(0,true,false,samples,(void *)((uint32_t)&(ADC0->SCANDATA)),3-1);

		ADC_Start(ADC0,adcStartScan);

		while(ADC0->STATUS&ADC_STATUS_SCANACT); //等待SCAN完毕

		if(erratashift) //ADC采样值是否需要校正
		{
			for(uint8_t i=0;i<3;i++)
				samples[i]<<=erratashift;
		}

		gSYSFLAG.CBV.SUN=samples[0]*((3.31/4096)*6*1000);
		gSYSFLAG.CBV.XDC=samples[1]*((3.31/4096)*6*1000);
		gSYSFLAG.CBV.CAP=samples[2]*((3.31/4096)*6*1000);

		Yc_Sudden_Report(&gECPINFO.YC.SUN_VOL,gSYSFLAG.CBV.SUN,gECPPARA.SUN_VOL_REPORT_THRESHOLD);
		Yc_Sudden_Report(&gECPINFO.YC.BAT_VOL,gSYSFLAG.CBV.XDC,gECPPARA.BATT_VOL_REPORT_THRESHOLD);

		if(gECPINFO.YC.BAT_VOL.MSG.U16<=gECPPARA.BATT_VOL_THRESHOLD) //蓄电池电压低于阈值
		{
			if(gECPINFO.YX.LOW_POWER.MSG.U16==0) //之前未报警则报警
				Soe_Data_Record(&gECPINFO.YX.LOW_POWER,1,true,0);
		}

		if(gECPINFO.YC.BAT_VOL.MSG.U16>gECPPARA.BATT_VOL_THRESHOLD) //蓄电池电压高于阈值
		{
			if(gECPINFO.YX.LOW_POWER.MSG.U16==1) //之前已报警则复归
				Soe_Data_Record(&gECPINFO.YX.LOW_POWER,0,true,0);
		}

		if(gECPINFO.YC.BAT_VOL.MSG.U16>gECPINFO.YC.SUN_VOL.MSG.U16)
		{
			if(gECPINFO.YX.POWER_SWITCH.MSG.U16==0)
			{
				Soe_Data_Record(&gECPINFO.YX.POWER_SWITCH,1,true,0);
				Log_Data_Record(0x01,"S","汇集单元主电已由太阳能电池切换为蓄电池!");
			}
		}

		if(gECPINFO.YC.BAT_VOL.MSG.U16<gECPINFO.YC.SUN_VOL.MSG.U16)
		{
			if(gECPINFO.YX.POWER_SWITCH.MSG.U16==1)
			{
				Soe_Data_Record(&gECPINFO.YX.POWER_SWITCH,0,true,0);
				Log_Data_Record(0x01,"S","汇集单元主电已由蓄电池切换为太阳能电池!");
			}
		}

		if((gSYSFLAG.CBV.XDC<12)&&(gSYSFLAG.CBV.SUN>=17))
			Timer2_Start();
		else
			Timer2_Stop();

		#ifdef PRINT_SYS_VOL_DEBUG //维护端口打印系统电压
		DEBUG_PRINT_NISR("KNSDSDSDKK","<----- 超级电容电压: ",(uint32_t)(gSYSFLAG.CBV.CAP)," 蓄电池电压: ",(uint32_t)(gSYSFLAG.CBV.XDC)," 太阳能电压: ",(uint32_t)(gSYSFLAG.CBV.SUN));
		#endif

		vTaskDelay(pdMS_TO_TICKS(100000)); //5分钟检测一次电压

		#ifdef TASKSTACK_HIGHWATERMARK
		uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
		if(uxHighWaterMark<=100)
			DEBUG_PRINT_NISR("NSDS","当前 Power_Monitor 任务剩余堆栈空间为: ",uxHighWaterMark*4," 字节!\r\n");
		#endif
	}
}
/************************************************************!
*Function: Ecp_Protocol_Comm
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Icp_Protocol_Comm(void *pvParameters)
{
	#ifdef TASKSTACK_HIGHWATERMARK
	UBaseType_t uxHighWaterMark;
	uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
	#endif

	while (1)
	{
		Wdog_Feed();

		gSYSFLAG.CWS.ICPSTATUS++;

		Icp_Process(&RADIOFRAME);
		Icp_Process(&WEIHUFRAME);

		Icp_Parse(&RADIO_RXDC,&RADIOFRAME);     //采集单元和维护射频通信处理
		Icp_Parse(&WEIHU_RXDC,&WEIHUFRAME);     //维护232端口通信处理

		#ifdef TASKSTACK_HIGHWATERMARK
		uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
		if(uxHighWaterMark<=100)
			DEBUG_PRINT_NISR("NSDS","当前 Protocol_Comm 任务剩余堆栈空间为: ",uxHighWaterMark*4," 字节!\r\n");
		#endif
	}
}

/************************************************************!
*Function: Ecp_Protocol_Comm
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Ecp_Protocol_Comm(void *pvParameters)
{
    #ifdef TASKSTACK_HIGHWATERMARK
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
	#endif

	while (1)
	{
		Wdog_Feed();

		//gSYSFLAG.CWS.ECPSTATUS++;

		Ecp_Process(&gECPFRAMELIST);

		Ecp_Parse(&GPRS_RXDC,&gECPFRAMELIST);   //GPRS端口101规约处理函数

		#ifdef TASKSTACK_HIGHWATERMARK
		uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
		if(uxHighWaterMark<=100)
			DEBUG_PRINT_NISR("NSDS","当前 Protocol_Comm 任务剩余堆栈空间为: ",uxHighWaterMark*4," 字节!\r\n");
		#endif
	}
}
/************************************************************!
*Function: Call_Wave
*Description: 召唤故障波形任务。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Call_Wave(void *pvParameters)
{
	#ifdef TASKSTACK_HIGHWATERMARK
	UBaseType_t uxHighWaterMark;
	uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
	#endif

	while(1)
	{
		vTaskSuspend(NULL); //挂起本任务,等待波形遥信帧解挂本任务

		vTaskDelay(pdMS_TO_TICKS(2000));

		Cu_Call_Wave(); //召唤波形

		#ifdef TASKSTACK_HIGHWATERMARK
		uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
		if(uxHighWaterMark<=100)
			DEBUG_PRINT_NISR("NSDS","当前 Call_Wave 任务剩余堆栈空间为: ",uxHighWaterMark*4," 字节!\r\n");
		#endif
	}
}
/************************************************************!
*Function: Data_Report
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Data_Report(void *pvParameters)
{
	#ifdef TASKSTACK_HIGHWATERMARK
	UBaseType_t uxHighWaterMark;
	uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
	#endif

	while(1)
	{
		Wdog_Feed();

		Yc_Data_Record();

		Sys_Status_Report();

		ShortCircuit_Fault_Process();

		Ecp_Yc_Timing_Report(&gECPFRAMELIST);

		App_Send_P101SoeQueue(&gECPFRAMELIST,&gSOEQUEUE);     //SOE数据上报
		//App_Send_P101FaultQueue(&gECPFRAMELIST,&gFAULTQUEUE); //故障数据上报

		#ifdef TASKSTACK_HIGHWATERMARK
		uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
		if(uxHighWaterMark<=100)
			DEBUG_PRINT_NISR("NSDS","当前 Data_Report 任务剩余堆栈空间为: ",uxHighWaterMark*4," 字节!\r\n");
		#endif
	}
}
/************************************************************!
*Function: Data_Send_Reply
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Data_Send_Reply(void *pvParameters)
{
	#ifdef TASKSTACK_HIGHWATERMARK
	UBaseType_t uxHighWaterMark;
	uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
	#endif

	while(1)
	{
		Wdog_Feed();

		App_Send_Reply(&gECPFRAMELIST);

		#ifdef TASKSTACK_HIGHWATERMARK
		uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
		if(uxHighWaterMark<=100)
			DEBUG_PRINT_NISR("NSDS","当前 Data_Send_Reply 任务剩余堆栈空间为: ",uxHighWaterMark*4," 字节!\r\n");
		#endif
	}
}
/************************************************************!
*Function: Ecp_File_Report
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Ecp_File_Report(void *pvParameters)
{
	#ifdef TASKSTACK_HIGHWATERMARK
	UBaseType_t uxHighWaterMark;
	uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
	#endif

	while(1)
	{
		Wdog_Feed();

		Ecp_File_Send(&gECPFRAMELIST);

		#ifdef TASKSTACK_HIGHWATERMARK
		uxHighWaterMark=uxTaskGetStackHighWaterMark(NULL);
		if(uxHighWaterMark<=100)
			DEBUG_PRINT_NISR("NSDS","当前 Ecp_File_Report 任务剩余堆栈空间为: ",uxHighWaterMark*4," 字节!\r\n");
		#endif
	}
}
/************************************************************!
*Function: vApplicationIdleHook
*Description: 任务钩子函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void vApplicationIdleHook(void *pvParameters)
{
	//EMU_EnterEM2(false);
}
/************************************************************!
*Function: Statistics_Au_Ticks
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void vApplicationStackOverflowHook(TaskHandle_t xTask,signed char *pcTaskName)
{
	DEBUG_PRINT_NISR("NSS","<------任务 ",pcTaskName," 堆栈溢出!!!!------>\r\n");

	vTaskDelay(pdMS_TO_TICKS(500));
}
/************************************************************!
*Function: Statistics_Au_Ticks
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Power_Management(void)
{
//	if((gUNIXTIME.UNIX%600)!=0)
//		return false;
//
//
//	if(gSYSFLAG.CWS.CHARGE_STATUS==false)
//		Timer2_Stop(); //测量电压前先关闭PWM，防止测量的电压数据不准确
//
//	vTaskDelay(pdMS_TO_TICKS(1000)); //关闭PWM后延时100MS，待电压稳定
//
//	DMA_ActivateBasic(0,true,false,samples,(void *)((uint32_t)&(ADC0->SCANDATA)),3-1);
//
//	ADC_Start(ADC0,adcStartScan);
//
//	while(ADC0->STATUS&ADC_STATUS_SCANACT); //等待SCAN完毕
//
////		if(erratashift) //ADC采样值是否需要校正
////		{
////			for(uint8_t i=0;i<3;i++)
////				samples[i]<<=erratashift;
////		}
//
//	gSYSFLAG.CBV.SUN=samples[0]*((3.31/4096)*6*1000);
//	gSYSFLAG.CBV.XDC=samples[1]*((3.31/4096)*6*1000);
//	gSYSFLAG.CBV.CAP=samples[2]*((3.31/4096)*6*1000);
//
//	Yc_Sudden_Deport(&gECPINFO.YC.SUN_VOL,gSYSFLAG.CBV.SUN,gECPPARA.SUN_VOL_REPORT_THRESHOLD);
//	Yc_Sudden_Deport(&gECPINFO.YC.BAT_VOL,gSYSFLAG.CBV.XDC,gECPPARA.BATT_VOL_REPORT_THRESHOLD);
//
//	if(gECPINFO.YC.BAT_VOL.MSG.U16<=gECPPARA.BATT_VOL_THRESHOLD) //蓄电池电压低于阈值
//	{
//		if(gECPINFO.YX.LOW_POWER.MSG.U16==0) //之前未报警则报警
//			Soe_Data_Record(&gECPINFO.YX.LOW_POWER,1,true,0);
//	}
//
//	if(gECPINFO.YC.BAT_VOL.MSG.U16>gECPPARA.BATT_VOL_THRESHOLD) //蓄电池电压高于阈值
//	{
//		if(gECPINFO.YX.LOW_POWER.MSG.U16==1) //之前已报警则复归
//			Soe_Data_Record(&gECPINFO.YX.LOW_POWER,0,true,0);
//	}
//
//	if(gECPINFO.YC.BAT_VOL.MSG.U16>gECPINFO.YC.SUN_VOL.MSG.U16)
//	{
//		if(gECPINFO.YX.POWER_SWITCH.MSG.U16==0)
//			Soe_Data_Record(&gECPINFO.YX.POWER_SWITCH,1,true,0);
//
//		Log_Data_Record(0x01,"S","汇集单元主电已由太阳能电池切换为蓄电池!");
//	}
//
//	if(gECPINFO.YC.BAT_VOL.MSG.U16<gECPINFO.YC.SUN_VOL.MSG.U16)
//	{
//		if(gECPINFO.YX.POWER_SWITCH.MSG.U16==1)
//			Soe_Data_Record(&gECPINFO.YX.POWER_SWITCH,0,true,0);
//
//		Log_Data_Record(0x01,"S","汇集单元主电已由蓄电池切换为太阳能电池!");
//	}
//
//	if((gSYSFLAG.CBV.XDC<12)&&(gSYSFLAG.CBV.SUN>=17))
//		Timer2_Start();
//	else
//		Timer2_Stop();
//
//	#ifdef PRINT_SYS_VOL_DEBUG //维护端口打印系统电压
//	DEBUG_PRINT_NISR("KNSDSDSDKK","<----- 超级电容电压: ",(uint32_t)(gSYSFLAG.CBV.CAP)," 蓄电池电压: ",(uint32_t)(gSYSFLAG.CBV.XDC)," 太阳能电压: ",(uint32_t)(gSYSFLAG.CBV.SUN));
//	#endif
//
//	vTaskDelay(pdMS_TO_TICKS(10000)); //5分钟检测一次电压

	return true;
}
/************************************************************!
*Function: Statistics_Au_Ticks
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Erase_Test(uint32_t startaddr,uint32_t sectornum)
{
	uint8_t write[200],read[200];

	for(uint32_t i=0;i<200;i++)
		write[i]=i;

	Status_Led2_Clear();

	DEBUG_PRINT_NISR("KKNSDSDSDSKK","当前正在擦除FLASH,范围为第 ",startaddr/MX25_SECTOR_LEN," 节到第 ",(startaddr/MX25_SECTOR_LEN)+sectornum-1," 节,共 ",sectornum," 节FLASH...");

	for(uint32_t i=0;i<sectornum;i++)
	{
		Wdog_Feed();

		Mx25_Erase_Sector((startaddr/MX25_SECTOR_LEN)+i);
	}

	DEBUG_PRINT_NISR("KKNSKK","当前擦除FLASH完成!正在读取空FLASH测试中...");

	for(uint32_t i=0;i<((sectornum*MX25_SECTOR_LEN)/200);i++)
	{
		Wdog_Feed();

		Mx25_Read_U8_Array(startaddr+i*200,read,200,false);

		for(uint32_t j=0;j<200;j++)
		{
			if(read[j]!=0xFF)
			{
				DEBUG_PRINT_NISR("KKNSDSKK","当前第 ",(startaddr+i*200)/MX25_SECTOR_LEN, " 节FLASH不为0xFF!");
				break;
			}
		}
	}

	DEBUG_PRINT_NISR("KKNSKK","空FLASH读取测试完成,正在进行写FLASH操作...");

	for(uint32_t i=0;i<((sectornum*MX25_SECTOR_LEN)/200);i++)
	{
		Wdog_Feed();

		Mx25_Write_U8_Array(startaddr+i*200,write,200,false);
	}

	DEBUG_PRINT_NISR("KKNSKK","写FLASH操作完成,正在进行FLASH写操作验证...");

	for(uint32_t i=0;i<((sectornum*MX25_SECTOR_LEN)/200);i++)
	{
		Wdog_Feed();

		Mx25_Read_U8_Array(startaddr+i*200,read,200,false);

		if(memcmp(write,read,200)!=0)
			DEBUG_PRINT_NISR("KKNSDSKK","当前 第 ",(startaddr+i*200)/MX25_SECTOR_LEN, " 节FLASH写操作失败!");
	}

	DEBUG_PRINT_NISR("KKNSKK","FLASH测试完成!正在擦除FLASH");

	for(uint32_t i=0;i<sectornum;i++)
	{
		Wdog_Feed();

		Mx25_Erase_Sector((startaddr/MX25_SECTOR_LEN)+i);
	}

	DEBUG_PRINT_NISR("KKNSKK","FLASH擦除完成!!!!!");

	Status_Led2_Set();
}

