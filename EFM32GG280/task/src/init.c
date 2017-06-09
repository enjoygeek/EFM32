/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: init.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"

/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void BSP_Init(void)
{
	CHIP_Init();
	Cmu_Init();
	Emu_Init();
	Msc_Init();
	Rtc_Init();
	I2c_Init();
	Dma_Init();
	Gpio_Init();
	Wdog_Init();
	Adc0_Init();
	Radio_Init();
	Timer0_Init();
	Timer2_Init();
	Usart0_Init();
	Usart1_Init();
	Rx8025t_Init();
	Letimer0_Init();
	Mx25_Flash_Init();
	Uart0_Init(115200);
	Uart1_Init(115200);
	Leuart1_Init(9600);
	MX25_CHIP_ALL_ERASE();
	Radio_Set_Preamble(90);

	#ifdef PRINT_SYS_INIT_DEBUG
	DEBUG_PRINT("KKNSKK","<-------------------- 汇集单元  BSP 模块初始化完成! -------------------->");
	#endif
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void APP_Init(void)
{
	SysPara_Init();        //系统信息初始化
	EcpPara_Init();
	SysFlag_Init();
	DataFile_Init();       //文件头结构体初始化
	DataCache_Init();      //环形队列初始化
	FlashPara_Init();      //FLASH参数区指针初始化
	Print_Sys_Para();
	PointTable_Init();
	Mcu_Reset_Record();    //记录MCU重启异常状态


	Rtc_Int_CallBack(Sys_Tick);
	Uart0_Rx_CallBack(Gprs_Queue_Rx);           //GPRS接收注册函数
	Uart1_Rx_CallBack(WeiHu_Queue_Rx);          //GPRS接收注册函数
	Leuart1_Rx_CallBack(Gps_Queue_Rx);          //GPS接收缓存注册
	Uart0_Tx_CallBack(Gprs_ContinueTx);
	Uart1_Tx_CallBack(WeiHu_ContinueTx);
    Gpio_Int_CallBack(Radio_Int_Process);       //射频中断注册函数
    Letimer0_Int_CallBack(Radio_Time_Slice);
    Radio_StartRx(gECPPARA.RADIO_CHANNEL,0x40);

    Wdog_Enable();

	#ifdef PRINT_SYS_INIT_DEBUG //初始化信息
    DEBUG_PRINT("KNSK","<-------------------- 汇集单元 APP 模块初始化完成! -------------------->");
    DEBUG_PRINT("KNSKKK","<-------------------- 汇集单元 *** 程序 *** 初始化完成! -------------------->");
	#endif
}
/************************************************************!
*Function: DataFile_Init
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DataFile_Init(void)
{
	YaoCeFile_Init(&gYCFILE);
}
/************************************************************!
*Function: Mcu_Reset_Record
*Description: MCU复位事件记录
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mcu_Reset_Record(void)
{
	RMU_RESETCAUSE RRC;

	RRC.DATA=RMU_ResetCauseGet(); //获得系统的复位源
	RMU_ResetCauseClear();

	if(RRC.POR==0x01) //上电复位
		Log_Data_Record(0x01,"S","汇集单元上电复位！");

	if(RRC.ETR==0x01) //外狗复位
		Log_Data_Record(0x05,"S","汇集单元外部看门狗复位！");

	if(RRC.SRR==0x01) //程序更新复位
		Log_Data_Record(0x0A,"S","汇集单元程序升级完成！");

	if(RRC.WDR==0x01) //看门狗复位
	{
		gSYSFLAG.CWS.SYS_STATUS=1;
		Log_Data_Record(0x05,"S","汇集单元内部看门狗复位！");

		#ifdef PRINT_SYS_INIT_DEBUG
		for(uint32_t i=0;i<10;i++)
			DEBUG_PRINT("KKNSKK","<----- 汇集单元程序复位,请检查程序代码... ----->");
		Run_Led_Set();
		Status_Led1_Set();
		Status_Led2_Set();
		//while(1);
		#endif
	}
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Sys_Status_Report(void)
{
	if(gSYSFLAG.CWS.SYS_STATUS==1) //系统装置异常
	{
		gSYSFLAG.CWS.SYS_STATUS=0;
		Soe_Data_Record(&gECPINFO.YX.STATUS,1,true,0);
	}
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool SysFlag_PrintOut(void)
{
	if(((gUNIXTIME.UNIX%5)!=0))
		return false;

	if(gUNIXTIME.UNIX==gSYSFLAG.STM.SYSFLAG_PRINT_TIME)
		return false;

	gSYSFLAG.STM.SYSFLAG_PRINT_TIME=gUNIXTIME.UNIX;

	vTaskSuspendAll();

	for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)
	{
		if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0)
			continue;

		if(gSYSFLAG.AWS.ASF[line].WORKMODE==0)
			DEBUG_PRINT_NISR("KSDSK","<------------------- ",line," 线采集单元组的工作模式: 全功能模式");

		if(gSYSFLAG.AWS.ASF[line].WORKMODE==1)
			DEBUG_PRINT_NISR("KSDSK","<------------------- ",line," 线采集单元组的工作模式: 退化模式");
	}

	DEBUG_PRINT_NISR("KS","<------------------- 当前链路连接状态: ");
	if(gECPFRAMELIST.LINKSTATE==0)
		DEBUG_PRINT_NISR("SK","链路未连接");
	if(gECPFRAMELIST.LINKSTATE!=0)
		DEBUG_PRINT_NISR("SK","链路已连接");

//	DEBUG_PRINT_NISR("KS","<------------------- 当前GPRS端口状态: ");
//	if(gSYSFLAG.CIS.GPRS==TX)
//		DEBUG_PRINT_NISR("SK","正在发送数据");
//	if(gSYSFLAG.CIS.GPRS==TXC)
//		DEBUG_PRINT_NISR("SK","数据已发送完成");
//
//	DEBUG_PRINT_NISR("KS","<------------------- 当前射频端口状态: ");
//	if(gSYSFLAG.CIS.RADIO==RX)
//		DEBUG_PRINT_NISR("SK","接收状态");
//	if(gSYSFLAG.CIS.RADIO==TX)
//		DEBUG_PRINT_NISR("SK","发送状态");

//	DEBUG_PRINT_NISR("KSDK","<------------------- 当前ICP任务状态 : ",gSYSFLAG.CWS.ICPSTATUS);
//
//	DEBUG_PRINT_NISR("KSDK","<------------------- 当前ECP任务状态 : ",gSYSFLAG.CWS.ECPSTATUS);
//
//	DEBUG_PRINT_NISR("KSDK","<------------------- 当前ICP线程状态 : ",gSYSFLAG.CWS.ICPRUNCOUNT);
//
	DEBUG_PRINT_NISR("SDK","<------------------- 当前ECP线程状态 : ",gSYSFLAG.CWS.ECPRUNCOUNT);
//
//	DEBUG_PRINT_NISR("KSDK","<------------------- 当前射频接收中断: ",gSYSFLAG.CWS.RADIORECEIVECNT);
//
//	DEBUG_PRINT_NISR("KSDK","<------------------- 当前射频缓存长度: ",DataCacheLength(&RADIO_RXDC));
//
	DEBUG_PRINT_NISR("SDK","<------------------- 当前规约缓存长度: ",DataCacheLength(&GPRS_RXDC));
//
//	DEBUG_PRINT_NISR("KSDK","<------------------- 射频数据帧状态  : ",RADIOFRAME.ISLOCK);
//
	DEBUG_PRINT_NISR("SDK","<------------------- 规约数据帧状态  : ",gECPFRAMELIST.REQ.ISLOCK);

	DEBUG_PRINT_NISR("KK");

	xTaskResumeAll();

	return true;
}
/************************************************************!
*Function: Gps_Timing_Process
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Gps_Timing_Process(void)
{
	if(gECPPARA.GPS_TIMING_MODE==0)
		return false;

	if(((gUNIXTIME.UNIX%gECPPARA.GPS_TIMING_CYCLE)!=0))
		return false;

	if(gUNIXTIME.UNIX==gSYSFLAG.STM.GPS_TIMING_TIME)
		return false;

	gSYSFLAG.STM.GPS_TIMING_TIME=gUNIXTIME.UNIX;

	Gps_Power_On();

	return true;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Run_Led_Toggle(void)
{
	gSYSFLAG.CWS.RUN_LED_STATUS^=1;

	if(gSYSFLAG.CWS.RUN_LED_STATUS==0)
		Timer0_Stop();

	if(gSYSFLAG.CWS.RUN_LED_STATUS==1)
		Timer0_Start();
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Run_Led_Set(void)
{
	Timer0_Start();
}
/************************************************************!
*Function: MX25_CHIP_ALL_ERASE
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void MX25_CHIP_ALL_ERASE(void)
{
    #ifdef PRINT_MX25_ERASE_DEBUG
	if(Mx25_Gpio_Pin()==true)
	{
		DEBUG_PRINT_NISR("KKSKK","<----- 正在擦除MX25存储!大约需要38秒,请稍后... ----->");
		Mx25_Erase_Chip();
		DEBUG_PRINT_NISR("KKSKK","<----- MX25存储擦除完毕!正在初始化系统参数,大约需要47秒,请稍后... ----->");
	}
	#endif
}
/************************************************************!
*Function: Default_Handler
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Default_Handler(void)
{
	//__asm volatile ("MRS %0, msp\n" : "=r" (ExceptionAddr) );
	//ExceptionAddr =  *((uint32_t*) ExceptionAddr+7);//记录异常地址
}

