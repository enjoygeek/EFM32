/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: icp_app.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"


/************************************************************!
*Function: Icp_PduType_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_PduType_Process(ICPFRAME *IF)
{
	switch (IF->TYPE)
	{
		case TIMESTAMP:  //时标
			App_Timestamp_Process(IF);
			break;

		case DBYC:       //遥测
			App_Yc_Process(IF);
			break;

		case DBSOE:      //SOE
			App_Soe_Yx_Process(IF);
			break;

		case DBWAVE:     //录波数据
			App_Wave_Process(IF);
			break;

		case DBPARA:     //参数
			App_Para_Process(IF);
			break;

		case DBPROG:     //程序
			App_Program_Process(IF);
			break;

		case FAULTWRG:   //启动录波
			App_Faultwrg_Process(IF);
			break;

		case LED:        //点灯测试
			App_Led_Process(IF);
			break;

		case WAKEUP:     //维护单元唤醒
			App_WakeUp_Process(IF);
			break;

		case DBYX:       //无时标遥信
			App_Soe_Yx_Process(IF);
			break;

		case RSTPROCESS: //复位进程
			App_Reset_Process(IF);
			break;

		case WORKMODE:   //工作模式
			App_WorkMode_Process(IF);
			break;

		case ERASEFLASH: //擦除FLASH
			DEBUG_ERASE_MX25(IF);
			break;

		case READLOG:    //读取LOG
			DEBUG_READ_LOG();
			break;

		case ECPREADLOG: //模拟读取LOG
		    DEBUG_ECP_READ_LOG();
			break;

		case READSOE:    //读取SOE
			DEBUG_READ_SOE();
			break;

		case ECPREADSOE: //模拟读取SOE
			DEBUG_ECP_READ_SOE();
			break;

		case READYC:     //读取YC
			DEBUG_READ_YC();
			break;

		case ECPREADYC:  //模拟读取YC
			DEBUG_ECP_READ_YC(IF->IFD.DATA[0]);
			break;

		case READFAULT:  //读取FAULT
			DEBUG_READ_FAULT();
			break;

		case ECPREADFAULT://模拟读取FAULT
			DEBUG_ECP_READ_FAULT();
			break;

		case READYK:  //读取YK
			DEBUG_READ_YK();
			break;

		case ECPREADYK://模拟读取YK
			DEBUG_ECP_READ_YK();
			break;

		case READWAVE:  //读取WAVE
			DEBUG_READ_WAVE();
			break;

		case ECPREADWAVE://模拟读取WAVE
			DEBUG_ECP_READ_WAVE(IF->IFD.DATA[0]);
			break;

		case NOWTIME:     //读取TIME
			DEBUG_READ_NOWTIME();
			break;

		case ERASEPARA:     //擦除参数块数据
			DEBUG_ERASE_PARABLOCK();
			break;

		case ECPSTARTWAVE:
			DEBUG_ECP_START_WAVE(IF);
			break;

		default:
			break;
	}

	Au_Online_Process(IF);  //采集单元掉线处理
}
/************************************************************!
*Function: App_Timestamp_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Timestamp_Process(ICPFRAME *IF)
{
	if(IF->PHASE==0)
		gSYSFLAG.AWS.ASF[IF->LINE].AU_TIMING_ACK+=1;
	if(IF->PHASE==1)
		gSYSFLAG.AWS.ASF[IF->LINE].AU_TIMING_ACK+=2;
	if(IF->PHASE==2)
		gSYSFLAG.AWS.ASF[IF->LINE].AU_TIMING_ACK+=4;

	#ifdef PRINT_AU_TIMING_ASYNC_DEBUG
	DEBUG_PRINT_NISR("KNSDSDSK","<----- 收到 ",IF->LINE," 线 ",IF->PHASE," 号采集单元的校时应答帧! ----->");
	if(gSYSFLAG.AWS.ASF[IF->LINE].AU_TIMING_ACK!=0&&gSYSFLAG.AWS.ASF[IF->LINE].AU_TIMING_ACK%7==0)
		DEBUG_PRINT_NISR("KNSDSKK","<----- 当前 ",IF->LINE," 线的时钟已经同步! ----->");
	#endif
}
/************************************************************!
*Function: App_Yc_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool App_Yc_Process(ICPFRAME *IF)
{
	if((IF->LEN1-ICP_MINPDULEN)<9) //防越界,遥测数据一共9个字节
		return false;

	gSYSFLAG.STM.YC_RECEIVE=gUNIXTIME.UNIX; //更新遥测数据接收时间

	if(gYCFILE.TIME.YEAR==0) //判断遥测缓存时间为空
		NowTime2P101Time(gYCFILE.TIME.DATA,7); //拷贝当前时间戳

	Yc_Sudden_Report(&gECPINFO.YC.ECPYC[IF->LINE].CUR[IF->PHASE],IF->IFD.I,gECPPARA.I_REPORT_THRESHOLD);

	Yc_Sudden_Report(&gECPINFO.YC.ECPYC[IF->LINE].VOL[IF->PHASE],IF->IFD.U,gECPPARA.U_REPORT_THRESHOLD);

	Yc_Sudden_Report(&gECPINFO.YC.ECPYC[IF->LINE].TEM[IF->PHASE],IF->IFD.T,gECPPARA.T_REPORT_THRESHOLD);

	Yc_Sudden_Report(&gECPINFO.YC.ECPYC[IF->LINE].AU_BAT_VOL[IF->PHASE],IF->IFD.B,gECPPARA.B_REPORT_THRESHOLD);


	if(gSYSFLAG.AWS.ASF[IF->LINE].AU_WORKMODE[IF->PHASE]!=IF->IFD.WORKMODE)  //工作模式不一致
	{
		Modify_Au_WorkMode(IF->LINE,IF->PHASE,IF->IFD.WORKMODE);
		//gSYSFLAG.AWS.ASF[IF->LINE].AU_WORKMODE[IF->PHASE]=IF->IFD.WORKMODE;  //更新汇集单元中的工作模式

//		if(gSYSFLAG.AWS.ASF[IF->LINE].AU_WORKMODE[IF->PHASE]==0)
//			Log_Data_Record(0x0A,"DSDS",IF->LINE," 线 ",IF->PHASE," 号采集单元已切换为全功能模式!");
//		if(gSYSFLAG.AWS.ASF[IF->LINE].AU_WORKMODE[IF->PHASE]==1)
//			Log_Data_Record(0x0A,"DSDS",IF->LINE," 线 ",IF->PHASE," 号采集单元已切换为退化模式!");
	}

	#ifdef PRINT_AU_YC_DATA_DEBUG
	DEBUG_PRINT_YC_DATA(IF);
	#endif

	#ifdef AU_YC_LOST_STATS_DEBUG //遥测数据掉线统计,仅测试使用
	if(IF->PHASE==0)
		gSYSFLAG.AWS.ASF[IF->LINE].AU_YC_REPORT+=1;
	if(IF->PHASE==1)
		gSYSFLAG.AWS.ASF[IF->LINE].AU_YC_REPORT+=2;
	if(IF->PHASE==2)
		gSYSFLAG.AWS.ASF[IF->LINE].AU_YC_REPORT+=4;
	#endif

	return true;
}
/************************************************************!
*Function: App_Yx_Soe_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool App_Soe_Yx_Process(ICPFRAME *IF)
{
	if((gUNIXTIME.UNIX-gSYSFLAG.STM.STC[IF->LINE].AU_SOE_TIME[IF->PHASE])<5) //屏蔽N秒内的SOE重发数据
		return false;

	gSYSFLAG.STM.STC[IF->LINE].AU_SOE_TIME[IF->PHASE]=gUNIXTIME.UNIX;    //更新SOE接收时间

	if(IF->TYPE==DBSOE) //带时标的遥信
	{
		if((IF->LEN1-ICP_MINPDULEN)<9) //防越界,带时标的遥信数据一共9个字节
			return false;

		UnixTime2P101Time(&IF->IFD.TIME,IF->IFD.DATA,7); //将采集单元时标转化为101时间
	}

	if(IF->TYPE==DBYX) //不带时标的遥信
	{
		if((IF->LEN1-ICP_MINPDULEN)<1) //防越界,不带时标的遥信数据一共1个字节
			return false;

		IF->IFD.SOE.DATA=IF->IFD.YX.DATA;
		NowTime2P101Time(IF->IFD.DATA,7); //记录当前时标
	}

	if(IF->IFD.SOE.SHUNSHI==0x01) //瞬时性短路
		Soe_Data_Record(&gECPINFO.YX.ECPYX[IF->LINE].SHUNSHI_FAULT[IF->PHASE],IF->IFD.SOE.FUGUI,true,IF->IFD.DATA);

	if(IF->IFD.SOE.YONGJIU==0x01) //永久性短路
		Soe_Data_Record(&gECPINFO.YX.ECPYX[IF->LINE].YONGJIU_FAULT[IF->PHASE],IF->IFD.SOE.FUGUI,true,IF->IFD.DATA);

	if(IF->IFD.SOE.IOVERRUN==0x01) //电流超限
		Soe_Data_Record(&gECPINFO.YX.ECPYX[IF->LINE].CUR_OVER[IF->PHASE],IF->IFD.SOE.FUGUI,true,IF->IFD.DATA);

	if(IF->IFD.SOE.LOWPOWER==0x01) //电池欠压
		Soe_Data_Record(&gECPINFO.YX.ECPYX[IF->LINE].AU_LOW_POWER[IF->PHASE],IF->IFD.SOE.FUGUI,true,IF->IFD.DATA);

	Fill_Icp_Frame(IF->CHANNEL,IF->SRCADDR,0x00,0x03,1,1,0,0);
	Fill_Icp_Frame(IF->CHANNEL,IF->SRCADDR,0x00,0x03,1,1,0,0);

	return true;
}
/************************************************************!
*Function: App_Wave_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool App_Wave_Process(ICPFRAME *IF)
{
	WAVE_DAT_FORMAT WDF;

	if(((IF->LEN1-ICP_MINPDULEN)%4)!=0) //此处检查录波数据长度必须要是4的倍数,防止下面数组越界,要求采集单元控制此长度
		return false;

	if(IF->FRAMESEQ!=gFAULTINFO.FRAMESEQ) //发生丢帧,记录日志
		return false;

	if(IF->FRAMESEQ!=gFAULTINFO.FRAMENUM) //非最后一帧,数据长度应为 WAVE_FRAME_LEN,防护
	{
		if((IF->LEN1-ICP_MINPDULEN)!=WAVE_FRAME_LEN)
			return false;
	}

	for(uint8_t write=0;write<((IF->LEN1-ICP_MINPDULEN)/4);write++) //将混合数据分开存储,电流数据在前
	{
		memcpy(gFAULTINFO.WAVE[IF->PHASE].CUR+(IF->FRAMESEQ-1)*(WAVE_FRAME_LEN/4)+write,IF->IFD.DATA+(write*4+0),2*sizeof(uint8_t));
		memcpy(gFAULTINFO.WAVE[IF->PHASE].VOL+(IF->FRAMESEQ-1)*(WAVE_FRAME_LEN/4)+write,IF->IFD.DATA+(write*4+2),2*sizeof(uint8_t));
	}

	xTaskNotifyGive(xCall_Wave); //召唤波形任务通知

	#ifdef PRINT_AU_WAVE_DATA_DEBUG //打印录波数据帧
	DEBUG_PRINT_NISR("KNSDSDSDSDSDSK","<----- 收到来自 ",IF->LINE," 线 ",IF->PHASE," 号采集单元的波形数据,共 ",gFAULTINFO.FRAMENUM," 帧,第 ",gFAULTINFO.FRAMESEQ," 帧,共 ",IF->LEN1-ICP_MINPDULEN," 字节数据!");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(IF->DATA,IF->LEN1-ICP_MINPDULEN);
	DEBUG_PRINT_NISR("KK");
	#endif

	if((IF->FRAMESEQ==IF->FRAMENUM)&&(IF->PHASE==2)) //当ABC的最后一相C也录波完成时
	{
		//Correct_The_Phase(); //ABC三相的相位矫正

		for(uint32_t i=0;i<gFAULTINFO.WAVELEN;i++) //三相电压电流数据合成零相数据
		{
			gFAULTINFO.ZEROI[i]=gFAULTINFO.WAVE[0].CUR[i]+gFAULTINFO.WAVE[1].CUR[i]+gFAULTINFO.WAVE[2].CUR[i];
			gFAULTINFO.ZEROU[i]=gFAULTINFO.WAVE[0].VOL[i]+gFAULTINFO.WAVE[1].VOL[i]+gFAULTINFO.WAVE[2].VOL[i];
		}

		#ifdef PRINT_WAVE_DEBUG //打印电压电流数据
		vTaskSuspendAll();
		for(uint8_t i=0;i<3;i++)
		{
			DEBUG_PRINT_NISR("KKSDSDSKK","<----- 正在打印第 ",gFAULTINFO.LINE, " 线 ",i," 号采集单元的电流数据:");
			DEBUG_PRINT_S16_ARRAY_NISR(gFAULTINFO.WAVE[i].CUR,gFAULTINFO.WAVELEN);
			DEBUG_PRINT_NISR("KKSDSDSKK","<----- 正在打印第 ",gFAULTINFO.LINE, " 线 ",i," 号采集单元的电压数据:");
			DEBUG_PRINT_S16_ARRAY_NISR(gFAULTINFO.WAVE[i].VOL,gFAULTINFO.WAVELEN);
			DEBUG_PRINT_NISR("KK");
		}

		DEBUG_PRINT_NISR("KKSDSKK","<----- 正在打印第 ",gFAULTINFO.LINE, " 线的零序电流数据:");
		DEBUG_PRINT_S16_ARRAY_NISR(gFAULTINFO.ZEROI,gFAULTINFO.WAVELEN);
		DEBUG_PRINT_NISR("KKSDSKK","<----- 正在打印第 ",gFAULTINFO.LINE, " 线的零序电压数据:");
		DEBUG_PRINT_S16_ARRAY_NISR(gFAULTINFO.ZEROU,gFAULTINFO.WAVELEN);
		DEBUG_PRINT_NISR("KK");
		xTaskResumeAll();
		#endif

		if(gFAULTINFO.TYPE==1) //判断是否为接地故障
		{
			//if(接地故障波形判断算法==false)
				//return false;

			Fill_Icp_Frame(RADIO_CHANNEL,gSYSPARA.BROADCAST,0x40,0x08,1,1,&gFAULTINFO.LINE,1); //发送闪灯信号
		}

		Switch_NextWave_Position(); //切换波形存储指针至下一文件

		Wave_Cfg_File_Init(); //波形文件头初始化

		memset(WDF.DATA,0,WAVE_INFO_LEN); //清空

		vTaskSuspendAll();

		for(uint32_t i=0;i<gFAULTINFO.WAVELEN;i++) //波形数据计算及存储
		{
			Wdog_Feed(); //暂时喂狗

			WDF.N=i+1;
			WDF.TIMESTAMP=0;
			WDF.AU=(uint16_t)gFAULTINFO.WAVE[0].VOL[i];
			WDF.BU=(uint16_t)gFAULTINFO.WAVE[1].VOL[i];
			WDF.CU=(uint16_t)gFAULTINFO.WAVE[2].VOL[i];
			WDF.AI=(uint16_t)gFAULTINFO.WAVE[0].CUR[i];
			WDF.BI=(uint16_t)gFAULTINFO.WAVE[1].CUR[i];
			WDF.CI=(uint16_t)gFAULTINFO.WAVE[2].CUR[i];
			WDF.ZU=(uint16_t)gFAULTINFO.ZEROU[i];
			WDF.ZI=(uint16_t)gFAULTINFO.ZEROI[i];
			WDF.YX=0x0000;     //SOE数据未处理

			Wave_Data_Record(WDF.DATA,WAVE_INFO_LEN);	 //波形存储
		}

		xTaskResumeAll();

		memset(gFAULTINFO.DATA,0,FAULT_DATA_LEN); //清空gFAULTINFO

		Soe_Data_Record(&gECPINFO.YX.WAVEFILE,1,true,0);

		#ifdef PRINT_WAVE_FILE_DEBUG //打印当前波形文件
		DEBUG_PRINT_WAVE(WAVE_START_ADDR1+((gSYSFLAG.MFW.WAVE-WAVE_START_ADDR1)/WAVE_FILE_LEN)*WAVE_FILE_LEN);
		#endif
	}

	return true;
}
/************************************************************!
*Function: App_Para_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool App_Para_Process(ICPFRAME *IF)
{
	SYSPARA SP;
	uint8_t priority,sn;

	if(IF->CTRL.PRM==1) //维护软件读取汇集单元参数
	{
		if(IF->CTRL.CC==FCSENDDATA) //修改参数
		{
			if((IF->LEN1-ICP_MINPDULEN)!=SYSPARA_NUM)
				return false;

			Mx25_Erase_Sector(SYSPARA_ADDR/MX25_SECTOR_LEN);

			Mx25_Data_Storage(SYSPARA_ADDR,IF->IFD.DATA,SYSPARA_NUM);

			Fill_Icp_Frame(IF->CHANNEL,IF->SRCADDR,0x00,0x06,1,1,0,0);

			Log_Data_Record(0x0A,"S","维护软件已修改汇集单元的参数信息!");

			vTaskDelay(pdMS_TO_TICKS(2000));

			SCB->AIRCR=0x05FA0004;  //M3内核复位命令
		}

		if(IF->CTRL.CC==FCREQUESTD) //读参数
		{
			Mx25_Read_U8_Array(SYSPARA_ADDR,SP.DATA,SYSPARA_NUM,false);

			if(SP.LINEGROUP.LINE[0].LINENO!=0xFF)  //有MX25参数
				memcpy(IF->IFD.DATA,SP.DATA,SYSPARA_NUM);

			if(SP.LINEGROUP.LINE[0].LINENO==0xFF)  //无MX25参数,保持默认参数
				memcpy(IF->IFD.DATA,gSYSPARA.DATA,SYSPARA_NUM);

			Fill_Icp_Frame(IF->CHANNEL,IF->SRCADDR,0x00,0x06,1,1,IF->IFD.DATA,SYSPARA_NUM);

			Log_Data_Record(0x0A,"S","维护软件已读取汇集单元的参数信息!");

			#ifdef PRINT_READ_SYSPARA_DEBUG
			#endif
		}
	}

	if(IF->CTRL.PRM==0) //主站读取采集单元参数
	{
		memcpy(gECPFRAMELIST.AP.DATA,IF->IFD.DATA,25);

		priority=8;
		sn=Req_Empty_ResFrame(&gECPFRAMELIST,priority); //申请可用的应答帧

		gECPFRAMELIST.RES[priority].ETQ[sn].EF.VF.CTRL.DATA=0xD3;
		gECPFRAMELIST.RES[priority].ETQ[sn].EF.VF.ASDU.TI=102;
		gECPFRAMELIST.RES[priority].ETQ[sn].EF.VF.ASDU.VSQ=1;
		gECPFRAMELIST.RES[priority].ETQ[sn].EF.VF.ASDU.COT.U16=7;
		gECPFRAMELIST.RES[priority].ETQ[sn].EF.VF.ASDU.MSGADDR.U16=gECPFRAMELIST.AP.MSGADDR.U16;

		if(gECPFRAMELIST.AP.MSGADDR.U8[0]<=7) //前7个参数为16位,地址从01-14
		{
			memcpy(gECPFRAMELIST.RES[priority].ETQ[sn].EF.VF.ASDU.MSG,gECPFRAMELIST.AP.DATA+(gECPFRAMELIST.AP.MSGADDR.U8[0]-1)*2,2);
			Fill_Ecp_Frame(&gECPFRAMELIST,VAR_FRAME,priority,sn,2);
		}
		if(gECPFRAMELIST.AP.MSGADDR.U8[0]>7)//后面的参数为8位,从15-
		{
			memcpy(gECPFRAMELIST.RES[priority].ETQ[sn].EF.VF.ASDU.MSG,gECPFRAMELIST.AP.DATA+6+gECPFRAMELIST.AP.MSGADDR.U8[0],1);
			Fill_Ecp_Frame(&gECPFRAMELIST,VAR_FRAME,priority,sn,1);
		}

		Log_Data_Record(0x0A,"S","主站已读取采集单元的参数信息!");
	}

	return true;
}
/************************************************************!
*Function: App_Prog_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool App_Program_Process(ICPFRAME *IF)
{
	DOUBLEBYTE FRAMENUM,FRAMESEQ,TMP;
	static uint32_t framenum,datalens;

	memcpy(FRAMENUM.U8,IF->IFD.DATA+0,2*sizeof(uint8_t)); //拷贝帧总数
    memcpy(FRAMESEQ.U8,IF->IFD.DATA+2,2*sizeof(uint8_t)); //拷贝帧序号

	if(FRAMESEQ.U16==1) //当收到编程数据的第一帧
	{
		datalens=0;
		framenum=FRAMENUM.U16;
		gSYSFLAG.CWS.UPDATING=true;

		for(uint32_t i=0;i<BACKUP_PAGE_NUM;i++) //擦除备份程序区
			Msc_ErasePage(BACKUP_PRM_KEY+i*MSC_PAGE_LEN); //暂时没有考虑关中断
	}

	if(gSYSFLAG.CWS.UPDATING==false) //防护
		return false;

	if(framenum!=FRAMENUM.U16)
	{
		//Log_Data_Record(0x03,"SDSD","更新程序的数据帧数错误!正确帧数为: ",framenum," 错误的帧数为: ",FRAMENUM.U16);

		return false;
	}

	#ifdef PRINT_PROGRAM_DEBUG
	DEBUG_PRINT_NISR("KNSDSDSK","<----- 收到程序更新数据帧,共 ",FRAMENUM.U16," 帧,第 ",FRAMESEQ.U16," 帧:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(IF->IFD.DATA,IF->LEN1-ICP_MINPDULEN-4);
	DEBUG_PRINT_NISR("KK");
	#endif

	if(FRAMESEQ.U16!=FRAMENUM.U16) //非最后一帧数据
	{
		if(Msc_ReadWord(BACKUP_PROGRAM+(FRAMESEQ.U16-1)*BACKUP_PRM_BLOCK)==0xFFFFFFFF) //防重写
		{
			datalens+=IF->LEN1-ICP_MINPDULEN-4;  //计算程序文件长度

			Program_Storage(FRAMESEQ.U16,IF->IFD.DATA+4,IF->LEN1-ICP_MINPDULEN-4); //写入编程程序数据
		}
	}

	if(FRAMESEQ.U16==FRAMENUM.U16) //当收到编程数据的最后一帧
	{
		for(uint32_t i=0;i<(FRAMENUM.U16-1);i++)
		{
			if(Msc_ReadWord(BACKUP_PROGRAM+i*BACKUP_PRM_BLOCK)==0xFFFFFFFF)
			{
				memcpy(TMP.U8,&i,2);

				Fill_Icp_Frame(RADIO_CHANNEL,IF->SRCADDR,0x00,0x06,1,1,TMP.U8,2); //丢失帧发送

				#ifdef PRINT_PROGRAM_DEBUG
				DEBUG_PRINT_NISR("NSDS","<----- 更新程序文件缺少第 ",TMP.U16," 帧!");
				#endif

				return false;
			}
		}

		#ifdef PRINT_PROGRAM_DEBUG
		DEBUG_PRINT_NISR("KNSK","<----- 更新程序无丢失帧,程序数据完整!正在校验程序文件......");
		#endif

		memcpy(TMP.U8,IF->IFD.DATA+4,2); //拷贝上位机的CS

		if(TMP.U16!=Get_Program_Cs(datalens))
		{
			#ifdef PRINT_PROGRAM_DEBUG
			DEBUG_PRINT_NISR("KSKK","<-----更新程序的文件校验码错误!");
			#endif

			return false;
		}

		#ifdef PRINT_PROGRAM_DEBUG
		DEBUG_PRINT_NISR("KNSKK","<----- 程序文件校验和正确,正在重启汇集单元,请稍后......");
		#endif

		gSYSFLAG.CWS.UPDATING=false;

		Msc_WriteWord(BACKUP_PRM_KEY,0x05FA05FB); //写入编程密钥

		Fill_Icp_Frame(RADIO_CHANNEL,IF->SRCADDR,0x00,0x06,1,1,IF->IFD.DATA,4);

		vTaskDelay(pdMS_TO_TICKS(2000));

		SCB->AIRCR=0x05FA0004;  //M3内核复位命令
	}

	Fill_Icp_Frame(RADIO_CHANNEL,IF->SRCADDR,0x00,0x06,1,1,IF->IFD.DATA,4); //发送ACK

	return true;
}
/************************************************************!
*Function: App_Faultwrg_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool App_Faultwrg_Process(ICPFRAME *IF)
{
	if((IF->LEN1-ICP_MINPDULEN)<8) //防越界,启动录波数据一共9个字节
		return false;

	Trigger_Start_Call_Wave(IF->LINE,IF->IFD.DATA);

	return true;
}
/************************************************************!
*Function: App_Led_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Led_Process(ICPFRAME *IF)
{
	Status_Led2_Toggle();
}
/************************************************************!
*Function: App_WakeUp_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_WakeUp_Process(ICPFRAME *IF)
{
	Fill_Icp_Frame(IF->CHANNEL,IF->SRCADDR,0x00,0x0A,1,1,0,0);

	//Log_Data_Record(0x01,"S","汇集单元收到维护软件的ID搜索帧!");
}
/************************************************************!
*Function: App_Rst_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Reset_Process(ICPFRAME *IF)
{
	if(memcmp(gSYSPARA.WEIHU,IF->SRCADDR,6)==0) //收到维护软件的复位命令
	{
		Fill_Icp_Frame(IF->CHANNEL,IF->SRCADDR,0x00,0x0C,1,1,0,0);

		Log_Data_Record(0x01,"S","汇集单元收到维护软件的程序复位命令!");

		vTaskDelay(pdMS_TO_TICKS(1000)); //延时1S

		SCB->AIRCR=0x05FA0004;  //M3内核复位命令
	}

	Log_Data_Record(0x05,"DSDS",IF->LINE," 线 ",IF->PHASE," 号采集单元程序复位!");
}
/************************************************************!
*Function: App_WorkMode_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool App_WorkMode_Process(ICPFRAME *IF)
{
	if((IF->LEN1-ICP_MINPDULEN)<1) //防越界,模式切换数据一共1个字节
		return false;

	if(IF->IFD.DATA[0]==1)  //退化
	{
		gSYSFLAG.AWS.ASF[IF->LINE].WORKMODE=1;  //退化

		#ifdef PRINT_AU_WORKMODE_DEBUG
		DEBUG_PRINT_NISR("KKDSKK",IF->LINE," 线的采集单元已切换为退化模式!");
		#endif

		return true;
	}

	if(IF->PHASE==0)
		gSYSFLAG.AWS.ASF[IF->LINE].AU_WORKMODE_ACK+=1;
	if(IF->PHASE==1)
		gSYSFLAG.AWS.ASF[IF->LINE].AU_WORKMODE_ACK+=2;
	if(IF->PHASE==2)
		gSYSFLAG.AWS.ASF[IF->LINE].AU_WORKMODE_ACK+=4;

	gSYSFLAG.STM.STC[IF->LINE].AU_WORKMODE_MARK[IF->LINE]=gUNIXTIME.UNIX;

	if(IF->IFD.DATA[0]==1)
		Log_Data_Record(0x0A,"DSDS",IF->LINE," 线 ",IF->PHASE," 号采集单元已切换为退化模式!");
	if(IF->IFD.DATA[0]==0)
		Log_Data_Record(0x0A,"DSDS",IF->LINE," 线 ",IF->PHASE," 号采集单元已切换为全功能模式!");

	return true;
}
/************************************************************!
*Function: Modify_Au_WorkMode
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Modify_Au_WorkMode(uint8_t line,uint8_t phase,uint8_t workmode)
{
	if(workmode==0) //切换为全功能模式
	{
		//gSYSFLAG.AWS.ASF[line].WORKMODE_SWITCH=1;
		gSYSFLAG.AWS.ASF[line].AU_WORKMODE[phase]=0;                     //此处的主要目的是修改该线的工作状态,使该线立即校时
		gSYSFLAG.AWS.ASF[line].AU_TIMING_ASYNC=AU_TIMING_ASYNC_BUFFER;   //修改校时异步统计,立即对该线进行唤醒校时同步
	}

	if(workmode==1) //切换为退化模式
	{
		gSYSFLAG.AWS.ASF[line].AU_WORKMODE[phase]=1;
	}

	for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)
	{
		if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0)
			continue;

		if(gSYSFLAG.AWS.ASF[line].WORKMODE==0) //全功能
		{
			for(uint8_t phase=0;phase<3;phase++)
			{
				if(gSYSFLAG.AWS.ASF[line].AU_WORKMODE[phase]==0)
					continue;

				gSYSFLAG.AWS.ASF[line].WORKMODE=1;

				Soe_Data_Record(&gECPINFO.YX.LINE_WORKMODE[line],1,true,0);

				Log_Data_Record(0x0A,"DS",line," 线的采集单元已全部切换为退化模式!");

				return true;
			}
		}

		if(gSYSFLAG.AWS.ASF[line].WORKMODE==1) //退化
		{
			for(uint8_t phase=0;phase<3;phase++)
			{
				if(gSYSFLAG.AWS.ASF[line].AU_WORKMODE[phase]==1)
					return true;
			}

			gSYSFLAG.AWS.ASF[line].WORKMODE=0;

			Soe_Data_Record(&gECPINFO.YX.LINE_WORKMODE[line],0,true,0);

			Log_Data_Record(0x0A,"DS",line," 线的采集单元已全部切换为全功能模式!");
		}
	}

	return true;
}
/************************************************************!
*Function: Au_Online_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Au_Online_Process(ICPFRAME *IF)
{
	if((IF->LINE==0xFF)||(IF->PHASE==0xFF)) //非采集单元组的地址上报的数据
		return false;

	gSYSFLAG.STM.STC[IF->LINE].AU_OFFLINE_MARK[IF->PHASE]=gUNIXTIME.UNIX;

	if(gECPINFO.YX.ECPYX[IF->LINE].COMM_FAULT[IF->PHASE].MSG.U16==0) //之前未产生通信故障
		return false;

	Modify_Au_WorkMode(IF->LINE,IF->PHASE,0);

	Soe_Data_Record(&gECPINFO.YX.ECPYX[IF->LINE].COMM_FAULT[IF->PHASE],0,true,0);

	return true;
}
/************************************************************!
*Function: Au_Offline_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Au_Offline_Process(void)
{
	for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)
	{
		if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0) //无效采集单元组
			continue;

		for(uint8_t phase=0;phase<3;phase++)
		{
			if((gUNIXTIME.UNIX-gSYSFLAG.STM.STC[line].AU_OFFLINE_MARK[phase]<gECPPARA.AU_OFFLINE_TIME)) //超过N秒采集单元不通信,视为通信故障
				continue;

			if(gECPINFO.YX.ECPYX[line].COMM_FAULT[phase].MSG.U16==1) //已产生通信故障
				continue;

			Modify_Au_WorkMode(line,phase,1);

			Soe_Data_Record(&gECPINFO.YX.ECPYX[line].COMM_FAULT[phase],1,true,0);
		}
	}
}
/************************************************************!
*Function: App_WorkMode_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Au_WorkMode_Process(void)
{
//	for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)
//	{
//		if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0)
//			continue;
//
//		if(gSYSFLAG.AWS.ASF[line].WORKMODE==0) //全功能
//		{
//			for(uint8_t phase=0;phase<3;phase++)
//			{
//				if(gSYSFLAG.AWS.ASF[line].AU_WORKMODE[phase]==1)
//				{
//					Modify_Au_WorkMode(line,1);
//
//					Log_Data_Record(0x0A,"DS",line," 线的采集单元已全部切换为退化模式!");
//
//					return true;
//				}
//			}
//		}
//
//		if(gSYSFLAG.AWS.ASF[line].WORKMODE==1) //退化
//		{
//			for(uint8_t phase=0;phase<3;phase++)
//			{
//				if(gSYSFLAG.AWS.ASF[line].AU_WORKMODE[phase]==1)
//					return true;
//			}
//
//			Modify_Au_WorkMode(line,0);
//
//			Log_Data_Record(0x0A,"DS",line," 线的采集单元已全部切换为全功能模式!");
//		}
//	}

	return true;
}
/************************************************************!
*Function:  Au_Timing_Process
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Au_Timing_Process(void)
{
	if((gUNIXTIME.UNIX-3)%5!=0)  //每隔5S处理一次
		return false;

	if(gUNIXTIME.UNIX==gSYSFLAG.STM.AU_TIMING_TIME)  //5S内只进行一次校时
		return false;

	gSYSFLAG.STM.AU_TIMING_TIME=gUNIXTIME.UNIX;  //记录校时时刻

	for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)
	{
		if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0)
			continue;

		if(gSYSFLAG.AWS.ASF[line].AU_TIMING_ACK!=0&&gSYSFLAG.AWS.ASF[line].AU_TIMING_ACK%7==0) //采集单元组校时应答已经同步
		{
			gSYSFLAG.AWS.ASF[line].AU_TIMING_ACK=0;
			gSYSFLAG.AWS.ASF[line].AU_TIMING_ASYNC=0;

			#ifdef STATUS_LED2_AU_TIMING_ASYNC
			Status_Led2_Clear();
			#endif

			continue;
		}

		#ifdef STATUS_LED2_AU_TIMING_ASYNC
		Status_Led2_Set();
		#endif

		#ifdef PRINT_AU_TIMING_ASYNC_DEBUG
		DEBUG_PRINT_NISR("KNSDSDKK","<----- ",line," 线的时钟没有同步,校时帧计数器: ",gSYSFLAG.AWS.ASF[line].AU_TIMING_ACK);
		#endif

		gSYSFLAG.AWS.ASF[line].AU_TIMING_ACK=0;
		gSYSFLAG.AWS.ASF[line].AU_TIMING_ASYNC+=1; //校时异步

		if(gSYSFLAG.AWS.ASF[line].AU_TIMING_ASYNC<=AU_TIMING_ASYNC_BUFFER)   //校时异步缓冲,连续丢失两次校时ACK才需要唤醒该采集单元组
			continue;

		if(gSYSFLAG.AWS.ASF[line].WORKMODE==1) //退化模式
		{
			//if((gSYSFLAG.AWS.ASF[line].AU_TIMING_ASYNC%AU_ASYNC_ENTER_LPOWER_MODE)!=0) //退化模式后,1分钟一次唤醒校时
				continue;
		}

		Au_WakeUp_Frame(line);

		Log_Data_Record(0x03,"DS",line," 线的时钟没有同步,已发送该线的同步帧!");
	}

	return true;
}
/************************************************************!
*Function: App_Call_Wave
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Cu_Call_Wave(void)
{
	#ifdef PRINT_WAVE_DEBUG
	DEBUG_PRINT_NISR("KKNSDSKK","<----- 汇集单元正在召唤 ",gFAULTINFO.LINE," 线的故障波形!");
	#endif

	uint32_t cnt=0;

	gSYSFLAG.CWS.ISCALLWAVE=true;

	DEBUG_PRINT_NISR("KNSKK","<----- 已进入召录波形程序");

	for(uint8_t phase=0;phase<3;phase++) //开始对于发生故障的一线召唤波形,相号轮询
	{
		for(gFAULTINFO.FRAMESEQ=1;gFAULTINFO.FRAMESEQ<=gFAULTINFO.FRAMENUM;gFAULTINFO.FRAMESEQ++)//数据帧帧序轮询
		{
			for(uint8_t retrans=0;retrans<5;retrans++) //每一帧的数据帧的重发次数
			{
				Fill_Icp_Frame(RADIO_CHANNEL,gSYSPARA.LINEGROUP.LINE[gFAULTINFO.LINE].AU[phase].ADDR,0x42,0x04,gFAULTINFO.FRAMENUM,gFAULTINFO.FRAMESEQ,0,0);

				cnt++;

				#ifdef PRINT_AU_WAVE_DATA_DEBUG
				if(retrans>=1)
					DEBUG_PRINT_NISR("KNSDSDSDSDSKK","<----- 正在第 ",retrans+1," 次发送 ",gFAULTINFO.LINE," 线 ",phase," 号采集单元的第 ",gFAULTINFO.FRAMESEQ," 帧召唤波形帧!");
				#endif

				if(ulTaskNotifyTake(pdTRUE,pdMS_TO_TICKS(2500))==pdTRUE) //等待2S超时重发
					break;

				if(retrans!=4) //5次数据召唤都超时，记录召唤失败事件
					continue;

				gSYSFLAG.CWS.ISCALLWAVE=false;

				memset(gFAULTINFO.DATA,0,FAULT_DATA_LEN); //清空gFAULTINFO

				Log_Data_Record(0x03,"DSDSDS",gFAULTINFO.LINE," 线 ",phase," 号采集单元召唤第 ",gFAULTINFO.FRAMESEQ," 帧波形失败!");

				return false;
			}
		}
	}

	gSYSFLAG.CWS.ISCALLWAVE=false;

	Log_Data_Record(12,"DS",gFAULTINFO.LINE," 线的故障波形召唤成功!");

	DEBUG_PRINT_NISR("KNSDSKK","<----- 召唤波形一共发送 ",cnt, " 帧数据帧");

	return true;
}
/************************************************************!
*Function: App_Call_Wave
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool ShortCircuit_Fault_Process(void)
{
	if(gECPINFO.YX.ALL_FAULT.MSG.U16==0)
	{
		for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)
		{
			if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0)
				continue;

			for(uint8_t phase=0;phase<3;phase++)
			{
				if(gECPINFO.YX.ECPYX[line].SHUNSHI_FAULT[phase].MSG.U16==1||gECPINFO.YX.ECPYX[line].YONGJIU_FAULT[phase].MSG.U16==1) //产生新的短路故障
				{
					Soe_Data_Record(&gECPINFO.YX.ALL_FAULT,1,true,0);

					return true;
				}
			}
		}
	}

	if(gECPINFO.YX.ALL_FAULT.MSG.U16==1)
	{
		for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)
		{
			if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0)
				continue;

			for(uint8_t phase=0;phase<3;phase++)
			{
				if(gECPINFO.YX.ECPYX[line].SHUNSHI_FAULT[phase].MSG.U16==1||gECPINFO.YX.ECPYX[line].YONGJIU_FAULT[phase].MSG.U16==1) //产生新的短路故障
					return false;
			}
		}

		Soe_Data_Record(&gECPINFO.YX.ALL_FAULT,0,true,0);

		return true;
	}

	return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Trigger_Start_Call_Wave(uint8_t line,uint8_t *wavetime)
{
	uint8_t waveinfo[9];

	if(gSYSFLAG.AWS.ASF[line].WORKMODE==1)  //退化模式
	{
		#ifdef PRINT_TRIGGER_CALL_WAVE_DEBUG
		DEBUG_PRINT_NISR("KKNSDSKK","<----- ",line,"线已进入退化模式,无法进行启动录波线程!");
		#endif

		return false;
	}

	if(gSYSFLAG.CWS.ISCALLWAVE==true)
	{
		#ifdef PRINT_TRIGGER_CALL_WAVE_DEBUG
		DEBUG_PRINT_NISR("KKNSKK","<----- 启动录波线程已启动,无法再次进入启动录波线程!");
		#endif

		return false;
	}

	gFAULTINFO.LINE=line;

	gSYSFLAG.CWS.ISCALLWAVE=true;   //召唤波形标志

	gFAULTINFO.WAVENUM.DATA=(((gECPPARA.FAULT_WAVE_NUM>>4)&0xF0)+gECPPARA.FAULT_WAVE_NUM)&0xFF;
	gFAULTINFO.WAVELEN=(AU_ADC_SAMPLE*(gFAULTINFO.WAVENUM.U03+gFAULTINFO.WAVENUM.U47));   //得到录波数据的周波数

	if(((gFAULTINFO.WAVELEN*4)%WAVE_FRAME_LEN)==0)
		gFAULTINFO.FRAMENUM=(gFAULTINFO.WAVELEN*4)/WAVE_FRAME_LEN;
	else
		gFAULTINFO.FRAMENUM=((gFAULTINFO.WAVELEN*4)/WAVE_FRAME_LEN)+1;

	memcpy(gFAULTINFO.DATA,wavetime,8); //拷贝时标

	memcpy(waveinfo,wavetime,8);

	waveinfo[8]=gFAULTINFO.WAVENUM.DATA;

	Fill_Icp_Frame(RADIO_CHANNEL,gSYSPARA.BROADCAST,0x40,0x07,1,1,waveinfo,9); //广播发送启动录波帧,未考虑未同步的情况

	Fault_Data_Record(1,wavetime,line);

	vTaskResume(xCall_Wave); //给召唤波形任务通知

	#ifdef PRINT_TRIGGER_CALL_WAVE_DEBUG
	DEBUG_PRINT_NISR("KKNSDSKK","<----- 收到 ",line," 线的启动录波帧,正在启动故障录波线程!");
	#endif

	return true;
}
/************************************************************!
*Function: Au_Timing_Frame
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Au_Timing_Frame(void)
{
	gICPFRAMELIST[0].FRAME[0]=ICP_STARTBYTE;
	gICPFRAMELIST[0].FRAME[1]=24;
	gICPFRAMELIST[0].FRAME[2]=24;
	gICPFRAMELIST[0].FRAME[3]=ICP_STARTBYTE;
	gICPFRAMELIST[0].FRAME[4]=0x41;

	memcpy(gICPFRAMELIST[0].FRAME+5,gSYSPARA.BENJI,6);
	memcpy(gICPFRAMELIST[0].FRAME+11,gSYSPARA.BROADCAST,6);

	gICPFRAMELIST[0].FRAME[17]=0x01;
	gICPFRAMELIST[0].FRAME[18]=1;
	gICPFRAMELIST[0].FRAME[19]=1;

	while(RTC->CNT>32760);
	gUNIXTIME.CNT=RTC->CNT+278;

	memcpy(gICPFRAMELIST[0].FRAME+20,gUNIXTIME.DATA,8);

	gICPFRAMELIST[0].FRAME[28]=Check_Icp_Cs(gICPFRAMELIST[0].FRAME+ICP_FRAMEHEAD,24);
	gICPFRAMELIST[0].FRAME[29]=ICP_ENDBYTE;

	Radio_ShortPkt_Tx(gICPFRAMELIST[0].FRAME,30);

	#ifdef PRINT_AUTIMING_TIMESTAMP_DEBUG //维护端口打印测试时标
	DEBUG_PRINT("KKNSDSDSDSKK","<------ 射频已于第 ",gSYSFLAG.CWS.RADIOTICKS," 时间片发送JIAOSHI时帧,当前RTC计数值: ",RTC->CNT," 时间为: ",(gUNIXTIME.CNT-278)*1000/32768," MS ----->");
	#endif
}

/************************************************************!
*Function:  Au_WakeUp_Frame
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Au_WakeUp_Frame(uint8_t line)
{
	memset(gICPFRAMELIST[1].FRAME,0x55,39);

	gICPFRAMELIST[1].FRAME[39]=0x2D;
	gICPFRAMELIST[1].FRAME[40]=0xD4;

	gICPFRAMELIST[1].FRAME[41]=ICP_STARTBYTE;
	gICPFRAMELIST[1].FRAME[42]=17;
	gICPFRAMELIST[1].FRAME[43]=17;
	gICPFRAMELIST[1].FRAME[44]=ICP_STARTBYTE;
	gICPFRAMELIST[1].FRAME[45]=0x40;

	memcpy(gICPFRAMELIST[1].FRAME+46,gSYSPARA.BENJI,6);
	memcpy(gICPFRAMELIST[1].FRAME+52,gSYSPARA.BROADCAST,6);

	gICPFRAMELIST[1].FRAME[58]=0x0A;
	gICPFRAMELIST[1].FRAME[59]=1;
	gICPFRAMELIST[1].FRAME[60]=1;
	gICPFRAMELIST[1].FRAME[61]=line;
	gICPFRAMELIST[1].FRAME[62]=Check_Icp_Cs(gICPFRAMELIST[1].FRAME+45,17);
	gICPFRAMELIST[1].FRAME[63]=ICP_ENDBYTE;

	vTaskSuspendAll();

	for(uint8_t i=0;i<25;i++)
		Radio_WakeUp_StartTx(gICPFRAMELIST[1].FRAME,64);

	xTaskResumeAll();
}
/************************************************************!
*Function:  Calc_NVA_F16_Value
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint16_t Calc_NVA_F16_Value(uint16_t data,uint32_t max,uint16_t scale)
{
	NVA F16NVA;
	uint8_t SIGN;
	uint32_t U32;

	F16NVA.F16=data;

	SIGN=F16NVA.SIGN;

	U32=(uint16_t)((~(0x8000|F16NVA.DATA))+1);

	F16NVA.F16=(uint16_t)((max*scale*U32)/32768);

	F16NVA.SIGN=SIGN;

	return F16NVA.F16;
}
/************************************************************!
*Function:  Correct_The_Phase
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Correc_The_Phase(void)
{
	uint8_t CORRECT_FLAG=0;
	int16_t MAX_A=0,MAX_B=0,MAX_C=0;
	uint32_t IDX_A,IDX_B,IDX_C,ABS_AB,ABS_AC;

	for(uint32_t i=0;i<82;i++)
	{
		if(gFAULTINFO.WAVE[0].CUR[i]>MAX_A)
		{
			IDX_A=i;
			MAX_A=gFAULTINFO.WAVE[0].CUR[i];
		}

		if(gFAULTINFO.WAVE[1].CUR[i]>MAX_B)
		{
			IDX_B=i;
			MAX_B=gFAULTINFO.WAVE[1].CUR[i];
		}

		if(gFAULTINFO.WAVE[2].CUR[i]>MAX_C)
		{
			IDX_C=i;
			MAX_C=gFAULTINFO.WAVE[2].CUR[i];
		}
	}

	ABS_AB=abs(IDX_A-IDX_B);
	ABS_AC=abs(IDX_A-IDX_C);

	if(ABS_AB<=21)
		CORRECT_FLAG+=1;
	else if(ABS_AB>=48&&ABS_AB<=60)
		CORRECT_FLAG+=1;
	else if(ABS_AB>60)
		CORRECT_FLAG+=1;

	if(ABS_AC<=21)
		CORRECT_FLAG+=2;
	else if(ABS_AC>=48&&ABS_AC<=60)
		CORRECT_FLAG+=2;
	else if(ABS_AC>60)
		CORRECT_FLAG+=2;

	if(CORRECT_FLAG==1) //B相反
	{
		for(uint32_t i=0;i<MAX_WAVE_LEN;i++)
		{
			gFAULTINFO.WAVE[1].CUR[i]=-gFAULTINFO.WAVE[1].CUR[i];
			gFAULTINFO.WAVE[1].VOL[i]=-gFAULTINFO.WAVE[1].VOL[i];
		}
	}
	else if(CORRECT_FLAG==2) //C相反
	{
		for(uint32_t i=0;i<MAX_WAVE_LEN;i++)
		{
			gFAULTINFO.WAVE[2].CUR[i]=-gFAULTINFO.WAVE[2].CUR[i];
			gFAULTINFO.WAVE[2].VOL[i]=-gFAULTINFO.WAVE[2].VOL[i];
		}
	}
	else if(CORRECT_FLAG==3) //A相反
	{
		for(uint32_t i=0;i<MAX_WAVE_LEN;i++)
		{
			gFAULTINFO.WAVE[0].CUR[i]=-gFAULTINFO.WAVE[0].CUR[i];
			gFAULTINFO.WAVE[0].VOL[i]=-gFAULTINFO.WAVE[0].VOL[i];
		}
	}

	#ifdef PRINT_CORRECT_PHASE_DEBUG
	if(CORRECT_FLAG==1)
		DEBUG_PRINT_NISR("KS","<----- B相的电流方向反向,已矫正B相电流方向! ----->");
	if(CORRECT_FLAG==2)
		DEBUG_PRINT_NISR("KS","<----- C相的电流方向反向,已矫正C相电流方向! ----->");
	if(CORRECT_FLAG==3)
		DEBUG_PRINT_NISR("KS","<----- A相的电流方向反向,已矫正A相电流方向! ----->");
	#endif
}
/************************************************************!
*Function: Wave_Cfg_File_Init
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Wave_Cfg_File_Init(void)
{
	typedef union
	{
	    char     C8[150];
		uint8_t  U8[150];
	}UINT8_CHAR;

	UNIXTIME UT;
	DOUBLEBYTE DB;
	UINT8_CHAR UC;
	uint8_t u32lens;
	uint32_t read,lens;

	DB.U16=0;
	read=gSYSFLAG.MFW.WAVE;
	gSYSFLAG.MFW.WAVE+=2;

	memcpy(UT.DATA,gFAULTINFO.DATA,8);
	UnixTime2P101Time(&UT,UC.U8,7);
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,7);
	gSYSFLAG.MFW.WAVE+=7;

	strcpy(UC.C8,
	"FAULT INDICATOR ");

	lens=strlen(UC.C8);
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

	gFAULTINFO.LINE+=0x31;
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,&gFAULTINFO.LINE,1);
	gSYSFLAG.MFW.WAVE+=1;
	DB.U16+=1;

	gFAULTINFO.LINE-=0x31;

strcpy(UC.C8,
" LINE,01,1999\r\n\
11,8A,3D\r\n\
1,UA,A,FI,V,1,0,0,-32767,32767\r\n\
2,UB,B,FI,V,1,0,0,-32767,32767\r\n");

	lens=strlen(UC.C8);
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

strcpy(UC.C8,
"3,UC,C,FI,V,1,0,0,-32767,32767\r\n\
4,UZ,Z,FI,V,1,0,0,-32767,32767\r\n\
5,IA,A,FI,A,1,0,0,-32767,32767\r\n");

	lens=strlen(UC.C8);
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

strcpy(UC.C8,
"6,IB,B,FI,A,1,0,0,-32767,32767\r\n\
7,IC,C,FI,A,1,0,0,-32767,32767\r\n\
8,IZ,Z,FI,A,1,0,0,-32767,32767\r\n");

	lens=strlen(UC.C8);
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

strcpy(UC.C8,
"1,ASOE,0\r\n\
2,BSOE,0\r\n\
3,CSOE,0\r\n\
50\r\n\
1\r\n\
4096,");

	lens=strlen(UC.C8);
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

	u32lens=Get_U32_Dec(gFAULTINFO.WAVELEN,UC.U8,10);

	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8+u32lens,10-u32lens);
	gSYSFLAG.MFW.WAVE+=10-u32lens;
	DB.U16+=10-u32lens;

strcpy(UC.C8,"\r\n");

	lens=strlen(UC.C8);
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

	memcpy(UT.DATA,gFAULTINFO.DATA,8);

	if(UT.CNT>=(20*gFAULTINFO.WAVENUM.U47*32768/1000))
		UT.CNT-=20*gFAULTINFO.WAVENUM.U47*32768/1000;
	else
	{
		UT.UNIX--;
		UT.CNT=32768-(20*gFAULTINFO.WAVENUM.U47*32768/1000)+UT.CNT;
	}

	lens=UnixTime2StringTime(UT.DATA,UC.U8);

	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

	strcpy(UC.C8,"\r\n");

	lens=strlen(UC.C8);
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

	lens=UnixTime2StringTime(gFAULTINFO.DATA,UC.U8);

	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

strcpy(UC.C8,
"\r\nBINARY\r\n\
1\r\n");

	lens=strlen(UC.C8);
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,UC.U8,lens);
	gSYSFLAG.MFW.WAVE+=lens;
	DB.U16+=lens;

	Mx25_Data_Storage(read,DB.U8,2);
}
/************************************************************!
*Function: Wave_Cfg_File_Init
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t UnixTime2StringTime(uint8_t *datetime,uint8_t *data)
{
	UNIXTIME UT;
	P101TIME PT;
	DOUBLEBYTE DB;
	uint8_t localtime[8];

	memcpy(UT.DATA,datetime,8);
	UnixTime2P101Time(&UT,PT.DATA,7);

	memcpy(localtime+1,PT.DATA,7);

	memcpy(DB.U8,localtime+1,2*sizeof(uint8_t));

	localtime[5]&=0x1F;           //屏蔽掉星期
	localtime[2]=DB.U16/1000;    //秒
	DB.U16%=1000;
	localtime[1]=DB.U16/100;     //毫秒高位
	localtime[0]=DB.U16%100;     //毫秒低位

	for(uint8_t i=0;i<8;i++)
		localtime[i]=Byte2Bcd(localtime[i]);

	DB.U4[0].DATA=localtime[5];   //日
	data[0]=DB.U4[0].U47+0x30;
	data[1]=DB.U4[0].U03+0x30;
	data[2]=0x2F;
	DB.U4[0].DATA=localtime[6];  //月
	data[3]=DB.U4[0].U47+0x30;
	data[4]=DB.U4[0].U03+0x30;
	data[5]=0x2F;
	DB.U4[0].DATA=localtime[7];  //年
	data[6]=0x32;
	data[7]=0x30;
	data[8]=DB.U4[0].U47+0x30;
	data[9]=DB.U4[0].U03+0x30;
	data[10]=0x2C;
	DB.U4[0].DATA=localtime[4];  //时
	data[11]=DB.U4[0].U47+0x30;
	data[12]=DB.U4[0].U03+0x30;
	data[13]=0x3A;
	DB.U4[0].DATA=localtime[3];  //分
	data[14]=DB.U4[0].U47+0x30;
	data[15]=DB.U4[0].U03+0x30;
	data[16]=0x3A;
	DB.U4[0].DATA=localtime[2];  //秒
	data[17]=DB.U4[0].U47+0x30;
	data[18]=DB.U4[0].U03+0x30;
	data[19]=0x2E;
	DB.U4[0].DATA=localtime[1];  //毫秒
	data[20]=DB.U4[0].U03+0x30;
	DB.U4[0].DATA=localtime[0];  //毫秒
	data[21]=DB.U4[0].U47+0x30;
	data[22]=DB.U4[0].U03+0x30;
	data[23]=0x30;
	data[24]=0x30;
	data[25]=0x30;

	return 26;
}
/************************************************************!
*Function: DEBUG_ERASE_MX25
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_ERASE_MX25(ICPFRAME *IF)
{
	vTaskSuspendAll();
	Wdog_Disable();

	DEBUG_PRINT_NISR("KNSKK","<----- 开始擦除MX25存储... ----->");

	Mx25_Erase_Chip();
	FlashPara_Init();

	DEBUG_PRINT_NISR("KKNSKK","<----- MX25存储擦除成功!正在重启汇集单元! ----->");

	for(uint32_t i=0;i<20000;i++);

	SCB->AIRCR=0x05FA0004;  //M3内核复位命令

	Wdog_Enable();
	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_READ_LOG
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_READ_LOG(void)
{
	vTaskSuspendAll();

	DEBUG_PRINT_NISR("KKSKK","<--------------- 正在读取历史 LOG 数据 --------------->");

	for(uint32_t i=0;i<(LOG_FLASH_LEN/LOG_INFO_LEN);i++)
	{
		if(DEBUG_PRINT_LOG(LOG_START_ADDR1+i*LOG_INFO_LEN)==false)
			break;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- LOG 块1读取完毕 --------------->");

	for(uint32_t i=0;i<(LOG_FLASH_LEN/LOG_INFO_LEN);i++)
	{
		if(DEBUG_PRINT_LOG(LOG_START_ADDR2+i*LOG_INFO_LEN)==false)
			break;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- LOG 块2读取完毕 --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_ECP_READ_LOG
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void DEBUG_ECP_READ_LOG(void)
{
	uint32_t sn;
	FILEINFO FI;

	vTaskSuspendAll();

	Locate_Log_FileInfo(&FI);

	if(((FI.READ-LOG_START_ADDR1)/LOG_FLASH_LEN)==0)  //块1
		sn=((FI.READ-LOG_START_ADDR1)/LOG_INFO_LEN)+1;
	if(((FI.READ-LOG_START_ADDR1)/LOG_FLASH_LEN)==1)  //块2
		sn=(LOG_FLASH_LEN/LOG_INFO_LEN)+((FI.READ-LOG_START_ADDR1)/LOG_INFO_LEN)+1;

	DEBUG_PRINT_NISR("KKNSKK","<--------------- 模拟读取历史 LOG 文件如下 --------------->");
	DEBUG_PRINT_NISR("KSDSDSESDSKK","<--------------- 当前 LOG数量: ",FI.INFONUM," LOG文件长度: ",FI.LENS," LOG时间: ",FI.TIME," 第一条LOG为MX25中的第: ",sn," 条LOG!");

	for(uint32_t i=0;i<FI.INFONUM;i++)
	{
		if(DEBUG_PRINT_LOG(FI.READ)==false)
		{
			if(((FI.READ-LOG_START_ADDR1)/LOG_FLASH_LEN)==0)  //块1
				sn=((FI.READ-LOG_START_ADDR1)/LOG_INFO_LEN)+1;
			if(((FI.READ-LOG_START_ADDR1)/LOG_FLASH_LEN)==1)  //块2
				sn=(LOG_FLASH_LEN/LOG_INFO_LEN)+((FI.READ-LOG_START_ADDR1)/LOG_INFO_LEN)+1;

			DEBUG_PRINT_NISR("KKSDSK","<--------------- 当前第: ",sn," 条LOG内容为空! --------------->");
		}

		FI.READ+=LOG_INFO_LEN;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- 历史 LOG 文件读取完毕 --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_PRINT_LOG
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool DEBUG_PRINT_LOG(uint32_t read)
{
	uint32_t sn;
	LOGRECORD LOG;

	if(((read-LOG_START_ADDR1)/LOG_FLASH_LEN)==0) //块1
	{
		if((LOG_START_ADDR1+LOG_FLASH_LEN-read)<LOG_INFO_LEN)
			read=LOG_START_ADDR2;
	}

	if(((read-LOG_START_ADDR1)/LOG_FLASH_LEN)==1) //块2
	{
		if((LOG_START_ADDR2+LOG_FLASH_LEN-read)<LOG_INFO_LEN)
			read=LOG_START_ADDR1;
	}

	Mx25_Read_U8_Array(read,LOG.DATA,LOG_INFO_LEN,false);

	if(LOG.LENS==0xFF) //防护
		return false;

	if(((read-LOG_START_ADDR1)/LOG_FLASH_LEN)==0)
		sn=((read-LOG_START_ADDR1)/LOG_INFO_LEN)+1;
	if(((read-LOG_START_ADDR1)/LOG_FLASH_LEN)==1)
		sn=(LOG_FLASH_LEN/LOG_INFO_LEN)+((read-LOG_START_ADDR2)/LOG_INFO_LEN)+1;

	DEBUG_PRINT_NISR("KSDSDSESDS","当前为第 ",sn," 条日志,类型: ",LOG.TYPE," 时间: ",LOG.TIME.DATA," 长度: ",LOG.LENS," 内容: ");
	DEBUG_PRINT_U8_ARRAY_NISR((uint8_t *)LOG.ASCII,LOG.LENS-8); //8:一字节类型,七个字节的时间
	DEBUG_PRINT_NISR("S","\r\n");

	return true;
}
/************************************************************!
*Function: DEBUG_READ_SOE
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_READ_SOE(void)
{
	vTaskSuspendAll();

	DEBUG_PRINT_NISR("KKNSKK","<--------------- 正在读取历史 SOE 数据 --------------->");

	for(uint32_t i=0;i<(SOE_FLASH_LEN/SOE_INFO_LEN);i++)
	{
		if(DEBUG_PRINT_SOE(SOE_START_ADDR1+i*SOE_INFO_LEN)==false)
			break;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- SOE 块1读取完毕 --------------->");

	for(uint32_t i=0;i<(SOE_FLASH_LEN/SOE_INFO_LEN);i++)
	{
		if(DEBUG_PRINT_SOE(SOE_START_ADDR2+i*SOE_INFO_LEN)==false)
			break;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- SOE 块2读取完毕 --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_ECP_READ_SOE
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_ECP_READ_SOE(void)
{
	uint32_t sn;
	FILEINFO FI;

	vTaskSuspendAll();

	Locate_Soe_FileInfo(&FI);

	if(((FI.READ-SOE_START_ADDR1)/SOE_FLASH_LEN)==0) //块1
		sn=((FI.READ-SOE_START_ADDR1)/SOE_INFO_LEN)+1;
	if(((FI.READ-SOE_START_ADDR1)/SOE_FLASH_LEN)==1) //块2
		sn=(SOE_FLASH_LEN/SOE_INFO_LEN)+((FI.READ-SOE_START_ADDR1)/SOE_INFO_LEN)+1;

	DEBUG_PRINT_NISR("KKNSKK","<--------------- 模拟读取历史 SOE 文件如下 --------------->");
	DEBUG_PRINT_NISR("SDSDSESDSKK","<--------------- 当前SOE数量: ",FI.INFONUM," SOE文件长度: ",FI.LENS," SOE时间: ",FI.TIME," 第一条SOE为MX25中的第: ",sn," 条SOE !");

	for(uint32_t i=0;i<FI.INFONUM;i++)
	{
		if(DEBUG_PRINT_SOE(FI.READ)==false)
		{
			if(((FI.READ-SOE_START_ADDR1)/SOE_FLASH_LEN)==0) //块1
				sn=((FI.READ-SOE_START_ADDR1)/SOE_INFO_LEN)+1;
			if(((FI.READ-SOE_START_ADDR1)/SOE_FLASH_LEN)==1) //块2
				sn=(SOE_FLASH_LEN/SOE_INFO_LEN)+((FI.READ-SOE_START_ADDR1)/SOE_INFO_LEN)+1;

			DEBUG_PRINT_NISR("KKSDSKK","<--------------- 当前第: ",sn," 条SOE内容为空! --------------->");
		}

		FI.READ+=SOE_INFO_LEN;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- 历史SOE文件读取完毕 --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_PRINT_SOE
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
bool DEBUG_PRINT_SOE(uint32_t read)
{
	P101SOE PS;
	uint32_t sn;

	vTaskSuspendAll();

	if(((read-SOE_START_ADDR1)/SOE_FLASH_LEN)==0) //块1
	{
		if((SOE_START_ADDR1+SOE_FLASH_LEN-read)<SOE_INFO_LEN)
			read=SOE_START_ADDR2;
	}

	if(((read-SOE_START_ADDR1)/SOE_FLASH_LEN)==1) //块2
	{
		if((SOE_START_ADDR2+SOE_FLASH_LEN-read)<SOE_INFO_LEN)
			read=SOE_START_ADDR1;
	}

	if(((read-SOE_START_ADDR1)/SOE_FLASH_LEN)==0)
		sn=((read-SOE_START_ADDR1)/SOE_INFO_LEN)+1;
	if(((read-SOE_START_ADDR1)/SOE_FLASH_LEN)==1)
		sn=(SOE_FLASH_LEN/SOE_INFO_LEN)+((read-SOE_START_ADDR2)/SOE_INFO_LEN)+1;

	Mx25_Read_U8_Array(read,PS.DATA,SOE_INFO_LEN,false);

	if(PS.TYPE==0xFF) //内容为空
		return false;

	if(PS.MSGADDR.U16==gECPINFO.YX.STATUS.ADDR.U16)
	{
		DEBUG_PRINT_NISR("KNSDSDSH","第 ",sn," 条SOE,MX25指针: ",read," 信息体地址 ",PS.MSGADDR.U16);
		DEBUG_PRINT_NISR("SESDS"," 时间 ",PS.TIME," 类型 ",PS.TYPE," 内容: 装置异常: ");
	}

	if(PS.MSGADDR.U16==gECPINFO.YX.LOW_POWER.ADDR.U16)
	{
		DEBUG_PRINT_NISR("KNSDSDSH","第 ",sn," 条SOE,MX25指针: ",read," 信息体地址 ",PS.MSGADDR.U16);
		DEBUG_PRINT_NISR("SESDS"," 时间 ",PS.TIME," 类型 ",PS.TYPE," 内容: 电池电压低: ");
	}

	if(PS.MSGADDR.U16==gECPINFO.YX.POWER_SWITCH.ADDR.U16)
	{
		DEBUG_PRINT_NISR("KNSDSDSH","第 ",sn," 条SOE,MX25指针: ",read," 信息体地址 ",PS.MSGADDR.U16);
		DEBUG_PRINT_NISR("SESDS"," 时间 ",PS.TIME," 类型 ",PS.TYPE," 内容: 后备电源投入: ");
	}

	if(PS.MSGADDR.U16==gECPINFO.YX.RF_FAULT.ADDR.U16)
	{
		DEBUG_PRINT_NISR("KNSDSDSH","第 ",sn," 条SOE,MX25指针: ",read," 信息体地址 ",PS.MSGADDR.U16);
		DEBUG_PRINT_NISR("SESDS"," 时间 ",PS.TIME," 类型 ",PS.TYPE," 内容: 小无线模块故障: ");
	}

	if(PS.MSGADDR.U16==gECPINFO.YX.GPS_LOCK.ADDR.U16)
	{
		DEBUG_PRINT_NISR("KNSDSDSH","第 ",sn," 条SOE,MX25指针: ",read," 信息体地址 ",PS.MSGADDR.U16);
		DEBUG_PRINT_NISR("SESDS"," 时间 ",PS.TIME," 类型 ",PS.TYPE," 内容: GPS锁定状态: ");
	}

	if(PS.MSGADDR.U16==gECPINFO.YX.WAVEFILE.ADDR.U16)
	{
		DEBUG_PRINT_NISR("KNSDSDSH","第 ",sn," 条SOE,MX25指针: ",read," 信息体地址 ",PS.MSGADDR.U16);
		DEBUG_PRINT_NISR("SESDS"," 时间 ",PS.TIME," 类型 ",PS.TYPE," 内容: 新录波文件生成: ");
	}

	if(PS.MSGADDR.U16==gECPINFO.YX.ALL_FAULT.ADDR.U16)
	{
		DEBUG_PRINT_NISR("KNSDSDSH","第 ",sn," 条SOE,MX25指针: ",read," 信息体地址 ",PS.MSGADDR.U16);
		DEBUG_PRINT_NISR("SESDS"," 时间 ",PS.TIME," 类型 ",PS.TYPE," 内容: 相线故障: ");
	}

	for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)
	{
		if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0) //无效采集单元组
			continue;

		for(uint8_t phase=0;phase<3;phase++)
		{
			if(PS.MSGADDR.U16==gECPINFO.YX.ECPYX[line].CUR_OVER[phase].ADDR.U16)
			{
				DEBUG_PRINT_NISR("KNSDSDSDSDS","第 ",sn," 条SOE,MX25指针: ",read," 来自 ",line," 线 ",phase," 号采集单元,信息体地址 ");
				DEBUG_PRINT_NISR("HSESDS",PS.MSGADDR.U16," 时间 ",PS.TIME," 类型: ",PS.TYPE," 内容: 电流过流: ");
			}

			if(PS.MSGADDR.U16==gECPINFO.YX.ECPYX[line].COMM_FAULT[phase].ADDR.U16)
			{
				DEBUG_PRINT_NISR("KNSDSDSDSDS","第 ",sn," 条SOE,MX25指针: ",read," 来自 ",line," 线 ",phase," 号采集单元,信息体地址 ");
				DEBUG_PRINT_NISR("HSESDS",PS.MSGADDR.U16," 时间 ",PS.TIME," 类型: ",PS.TYPE," 内容: 通信故障: ");
			}

			if(PS.MSGADDR.U16==gECPINFO.YX.ECPYX[line].SHUNSHI_FAULT[phase].ADDR.U16)
			{
				DEBUG_PRINT_NISR("KNSDSDSDSDS","第 ",sn," 条SOE,MX25指针: ",read," 来自 ",line," 线 ",phase," 号采集单元,信息体地址 ");
				DEBUG_PRINT_NISR("HSESDS",PS.MSGADDR.U16," 时间 ",PS.TIME," 类型: ",PS.TYPE," 内容: 瞬时性短路: ");
			}

			if(PS.MSGADDR.U16==gECPINFO.YX.ECPYX[line].YONGJIU_FAULT[phase].ADDR.U16)
			{
				DEBUG_PRINT_NISR("KNSDSDSDSDS","第 ",sn," 条SOE,MX25指针: ",read," 来自 ",line," 线 ",phase," 号采集单元,信息体地址 ");
				DEBUG_PRINT_NISR("HSESDS",PS.MSGADDR.U16," 时间 ",PS.TIME," 类型: ",PS.TYPE," 内容: 永久性性短路: ");
			}

			if(PS.MSGADDR.U16==gECPINFO.YX.ECPYX[line].AU_LOW_POWER[phase].ADDR.U16)
			{
				DEBUG_PRINT_NISR("KNSDSDSDSDS","第 ",sn," 条SOE,MX25指针: ",read," 来自 ",line," 线 ",phase," 号采集单元,信息体地址 ");
				DEBUG_PRINT_NISR("HSESDS",PS.MSGADDR.U16," 时间 ",PS.TIME," 类型: ",PS.TYPE," 内容: 电池欠压: ");
			}

			if(PS.MSGADDR.U16==gECPINFO.YX.ECPYX[line].AU_POWER_SWITCH[phase].ADDR.U16)
			{
				DEBUG_PRINT_NISR("KNSDSDSDSDS","第 ",sn," 条SOE,MX25指针: ",read," 来自 ",line," 线 ",phase," 号采集单元,信息体地址 ");
				DEBUG_PRINT_NISR("HSESDS",PS.MSGADDR.U16," 时间 ",PS.TIME," 类型: ",PS.TYPE," 内容: 备用电池切换: ");
			}

			if(PS.MSGADDR.U16==gECPINFO.YX.ECPYX[line].FUHE_OVER[phase].ADDR.U16)
			{
				DEBUG_PRINT_NISR("KNSDSDSDSDS","第 ",sn," 条SOE,MX25指针: ",read," 来自 ",line," 线 ",phase," 号采集单元,信息体地址 ");
				DEBUG_PRINT_NISR("HSESDS",PS.MSGADDR.U16," 时间 ",PS.TIME," 类型: ",PS.TYPE," 内容: 负荷越限: ");
			}

			if(PS.MSGADDR.U16==gECPINFO.YX.ECPYX[line].ZHONGZAI[phase].ADDR.U16)
			{
				DEBUG_PRINT_NISR("KNSDSDSDSDS","第 ",sn," 条SOE,MX25指针: ",read," 来自 ",line," 线 ",phase," 号采集单元,信息体地址 ");
				DEBUG_PRINT_NISR("HSESDS",PS.MSGADDR.U16," 时间 ",PS.TIME," 类型: ",PS.TYPE," 内容: 重载: ");
			}
		}
	}

	if(PS.SOE==0)
		DEBUG_PRINT_NISR("SKK","复归");
	if(PS.SOE==1)
		DEBUG_PRINT_NISR("SKK","开始");

	xTaskResumeAll();

	return true;
}
/************************************************************!
*Function: DEBUG_READ_YC
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_READ_YC(void)
{
	vTaskSuspendAll();

	DEBUG_PRINT_NISR("KKNSKK","<--------------- 开始分析MX25中的历史遥测文件  --------------->");

	for(uint8_t i=0;i<((2*YC_SECTOR_NUM)/YC_FILE_SECTOR_NUM);i++)
	{
		if(DEBUG_PRINT_YC_FILE(YC_START_ADDR1+i*YC_FILE_LEN)==false)
			DEBUG_PRINT_NISR("KKSDSKK","<--------------- 当前第: ",i+1," 号遥测文件,文件为空! --------------->");
	}

	DEBUG_PRINT_NISR("KKNSKK","<--------------- MX25中的历史遥测文件分析完成  --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_ECP_READ_YC
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_ECP_READ_YC(uint16_t fileid)
{
	uint8_t sn;
	FILEINFO FI;

	FI.ID=fileid;

	vTaskSuspendAll();

	Locate_Yc_FileInfo(&FI);

	sn=((FI.READ-YC_START_ADDR1)/YC_FILE_LEN)+1;

	if(DEBUG_PRINT_YC_FILE(FI.READ)==false)
	{
		DEBUG_PRINT_NISR("KNSDSK","<--------------- 当前第: ",sn," 号遥测文件,文件为空! --------------->");
		return;
	}

	DEBUG_PRINT_NISR("NSDSDSESDSDSDSKK","<----- 模拟ECP读取的遥测文件号为: ",FI.ID," 为MX25中第: ",sn," 号遥测文件,文件时间: ",FI.TIME," 文件节数: ",FI.INFONUM," 文件长度: ",FI.LENS," 文件指针: ",FI.READ," ----->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_PRINT_YC_DATA
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_PRINT_YC_DATA(ICPFRAME *IF)
{
	vTaskSuspendAll();

	DEBUG_PRINT_NISR("KKNSDSDSK","<----- 收到 ",IF->LINE," 线 ",IF->PHASE," 号采集单元的遥测数据:");

	DEBUG_PRINT_NISR("S","<----- 工作模式: ");

	if(IF->IFD.WORKMODE==0)
		DEBUG_PRINT_NISR("SK","全功能模式");
	if(IF->IFD.WORKMODE==1)
		DEBUG_PRINT_NISR("SK","退化模式");

	DEBUG_PRINT_NISR("SFK","<----- 电流数据: ",IF->IFD.I);

	DEBUG_PRINT_NISR("SFK","<----- 电场数据: ",IF->IFD.U);

	DEBUG_PRINT_NISR("SFK","<----- 温度数据: ",IF->IFD.T);

	DEBUG_PRINT_NISR("SFKK","<----- 电池电压: ",IF->IFD.B);



	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_PRINT_YC_FILE_SECTION
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
bool DEBUG_PRINT_YC_FILE_SECTION(uint32_t read,uint32_t sn)
{
	FOURBYTE FB;
	YAOCEFILE YCF;

	vTaskSuspendAll();
	Wdog_Feed();

	for(uint8_t i=0;i<YC_INFO_NUM;i++) //获得有效的定点数据文件头时间
	{
		Mx25_Read_U8_Array(read+i*YC_INFO_LEN+8,FB.U8,4,false); //+4:读取节数据的年月日的时间位置

		if(FB.U32==0xFFFFFFFF||FB.U32!=0x00000000)
			break;
	}

	Mx25_Read_U8_Array(read+(sn-1)*YC_INFO_LEN,YCF.DATA,YC_INFO_LEN,false);

	YCF.OLDTIME=((FB.U32&0xFFFF1F00)>>8);
	DEBUG_PRINT_NISR("NSDSTSDSKK","<----- 当前写到第 ",((read-YC_START_ADDR1)/YC_FILE_LEN)+1," 号遥测文件( ",YCF.OLDTIME," )的第 ",YCF.SN," 号定点!");

	if(YCF.N==0xFF)
	{
		DEBUG_PRINT_NISR("KSDSKK","<---------- 当前节名: ",YCF.SN," 节内容为空! ----------->");

		Wdog_Feed();
		xTaskResumeAll();

		return false;
	}

	DEBUG_PRINT_NISR("KSDSESDSDSK","<---------- 当前节名: ",YCF.SN," 定点时间: ",YCF.TIME.DATA," 定点数量: ",YCF.N," 定点长度 ",YCF.LENS," ----------->");

	YCF.N=YCF.N>=27?27:YCF.N;

	for(uint32_t i=0;i<YCF.N;i+=9)
		for(uint32_t j=0;j<9;j++)
			DEBUG_PRINT_NISR("SHSHK","信息体地址 ",YCF.YC[i+j].ADDR.U16," 信息数据 ",YCF.YC[i+j].DATA);

	DEBUG_PRINT_NISR("S","\r\n\r\n");

	Wdog_Feed();
	xTaskResumeAll();

	return true;
}
/************************************************************!
*Function: DEBUG_PRINT_YC_FILE
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
bool DEBUG_PRINT_YC_FILE(uint32_t read)
{
	FOURBYTE FB;
	uint32_t sn;
	YAOCEFILE YCF;

	vTaskSuspendAll();
	Wdog_Feed();

	for(uint32_t i=0;i<YC_INFO_NUM;i++)
	{
		Mx25_Read_U8_Array(read+i*YC_INFO_LEN,FB.U8,4,false);

		if(FB.U32!=0xFFFFFFFF)
			break;

		if(i==(YC_INFO_NUM-1))
			return false;
	}

	for(uint32_t i=0;i<YC_INFO_NUM;i++) //获得有效的定点数据文件头时间
	{
		Mx25_Read_U8_Array(read+i*YC_INFO_LEN+8,FB.U8,4,false); //+4:读取节数据的年月日的时间位置

		if(FB.U32==0xFFFFFFFF||FB.U32!=0x00000000)
			break;
	}

	YCF.OLDTIME=(FB.U32&0xFFFF1F00)>>8;

	sn=((read-YC_START_ADDR1)/YC_FILE_LEN)+1;

	DEBUG_PRINT_NISR("KKNSDSTSKK","<---------- 当前第 ",sn," 号遥测文件( ",YCF.OLDTIME," )内容如下: ");

	for(uint32_t i=0;i<YC_INFO_NUM;i++)
	{
		Mx25_Read_U8_Array(read+i*YC_INFO_LEN,YCF.DATA,YC_INFO_LEN,false);

		if(YCF.N==0xFF)
		{
			DEBUG_PRINT_NISR("KSDSK","<---------- 当前节名: ",YCF.SN," 节内容为空! ----------->");
			continue;
		}

		DEBUG_PRINT_NISR("KSDSESDSDSK","<---------- 当前节名: ",YCF.SN," 定点时间: ",YCF.TIME.DATA," 定点数量: ",YCF.N," 定点长度 ",YCF.LENS," ----------->");

		YCF.N=YCF.N>=27?27:YCF.N;

		for(uint32_t j=0;j<YCF.N;j+=9)
			for(uint32_t k=0;k<9;k++)
				DEBUG_PRINT_NISR("SHSHK","信息体地址 ",YCF.YC[j+k].ADDR.U16," 信息数据 ",YCF.YC[j+k].DATA);

		DEBUG_PRINT_NISR("SK","\r\n");
	}

	Wdog_Feed();
	xTaskResumeAll();

	return true;
}
/************************************************************!
*Function: DEBUG_READ_FAULT
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_READ_FAULT(void)
{
	vTaskSuspendAll();

	DEBUG_PRINT_NISR("KKNSKK","<--------------- 正在读取历史 FAULT 数据 --------------->");

	for(uint32_t i=0;i<(FAULT_FLASH_LEN/FAULT_INFO_LEN);i++)
	{
		if(DEBUG_PRINT_FAULT(FAULT_START_ADDR1+i*FAULT_INFO_LEN)==false)
			break;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- FAULT 块1读取完毕 --------------->");

	for(uint32_t i=0;i<(FAULT_FLASH_LEN/FAULT_INFO_LEN);i++)
	{
		if(DEBUG_PRINT_FAULT(FAULT_START_ADDR2+i*FAULT_INFO_LEN)==false)
			break;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- FAULT 块2读取完毕 --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_ECP_READ_FAULT
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_ECP_READ_FAULT(void)
{
	uint32_t sn;
	FILEINFO FI;

	vTaskSuspendAll();

	Locate_Fault_FileInfo(&FI);

	if(((FI.READ-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==0) //块1
		sn=((FI.READ-FAULT_START_ADDR1)/FAULT_INFO_LEN)+1;
	if(((FI.READ-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==1) //块2
		sn=(FAULT_FLASH_LEN/FAULT_INFO_LEN)+((FI.READ-FAULT_START_ADDR1)/FAULT_INFO_LEN)+1;

	DEBUG_PRINT_NISR("KKNSKK","<--------------- 模拟读取历史 FAULT 文件如下 --------------->");
	DEBUG_PRINT_NISR("SDSDSESDSKK","<--------------- 当前FAULT数量: ",FI.INFONUM," FAULT文件长度: ",FI.LENS," FAULT时间: ",FI.TIME," 第一条FAULT为MX25中的第: ",sn," 条FAULT !");

	for(uint32_t i=0;i<FI.INFONUM;i++)
	{
		if(DEBUG_PRINT_FAULT(FI.READ)==false)
		{
			if(((FI.READ-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==0) //块1
				sn=((FI.READ-FAULT_START_ADDR1)/FAULT_INFO_LEN)+1;
			if(((FI.READ-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==1) //块2
				sn=(FAULT_FLASH_LEN/FAULT_INFO_LEN)+((FI.READ-FAULT_START_ADDR1)/FAULT_INFO_LEN)+1;

			DEBUG_PRINT_NISR("KKSDSKK","<--------------- 当前第: ",sn," 条FAULT内容为空! --------------->");
		}

		FI.READ+=FAULT_INFO_LEN;
	}

	DEBUG_PRINT_NISR("SKK","<--------------- 历史FAULT文件读取完毕 --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_PRINT_FAULT
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool DEBUG_PRINT_FAULT(uint32_t read)
{
	uint32_t sn;
	P101FAULT PF;

	if(((read-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==0) //块1
	{
		if((FAULT_START_ADDR1+FAULT_FLASH_LEN-read)<FAULT_INFO_LEN)
			read=FAULT_START_ADDR2;
	}

	if(((read-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==1) //块2
	{
		if((FAULT_START_ADDR2+FAULT_FLASH_LEN-read)<FAULT_INFO_LEN)
			read=FAULT_START_ADDR1;
	}

	Mx25_Read_U8_Array(read,PF.DATA,FAULT_INFO_LEN,false);

	if(PF.FAULT==0xFF)
		return false;

	if(((read-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==0)
		sn=((read-FAULT_START_ADDR1)/FAULT_INFO_LEN)+1;
	if(((read-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==1)
		sn=(FAULT_FLASH_LEN/FAULT_INFO_LEN)+((read-FAULT_START_ADDR2)/FAULT_INFO_LEN)+1;

	DEBUG_PRINT_NISR("KNSDSDSESHKK","当前第 ",sn," 条故障信息,MX25指针位置: ",read," 故障时间: ",PF.TIME," 内容: ",PF.FAULT);

	return true;

}
/************************************************************!
*Function: DEBUG_READ_YK
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_READ_YK(void)
{
	vTaskSuspendAll();

	DEBUG_PRINT_NISR("KKNSKK","<--------------- 正在读取历史 YK 数据 --------------->");

	for(uint32_t i=0;i<(YK_FLASH_LEN/YK_INFO_LEN);i++)
	{
		if(DEBUG_PRINT_YK(YK_START_ADDR1+i*YK_INFO_LEN)==false)
			break;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- YK 块1读取完毕 --------------->");

	for(uint32_t i=0;i<(YK_FLASH_LEN/YK_INFO_LEN);i++)
	{
		if(DEBUG_PRINT_YK(YK_START_ADDR2+i*YK_INFO_LEN)==false)
			break;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- YK 块2读取完毕 --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_ECP_READ_YK
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_ECP_READ_YK(void)
{
	uint32_t sn;
	FILEINFO FI;

	vTaskSuspendAll();

	Locate_Yk_FileInfo(&FI);

	if(((FI.READ-YK_START_ADDR1)/YK_FLASH_LEN)==0) //块1
		sn=((FI.READ-YK_START_ADDR1)/YK_INFO_LEN)+1;
	if(((FI.READ-YK_START_ADDR1)/YK_FLASH_LEN)==1) //块2
		sn=(YK_FLASH_LEN/YK_INFO_LEN)+((FI.READ-YK_START_ADDR1)/YK_INFO_LEN)+1;

	DEBUG_PRINT_NISR("KKNSKK","<--------------- 模拟读取历史 YK 文件如下 --------------->");
	DEBUG_PRINT_NISR("SDSDSESDSKK","<--------------- 当前YK数量: ",FI.INFONUM," YK文件长度: ",FI.LENS," YK时间: ",FI.TIME," 第一条YK为MX25中的第: ",sn," 条YK !");

	for(uint32_t i=0;i<FI.INFONUM;i++)
	{
		if(DEBUG_PRINT_YK(FI.READ)==false)
		{
			if(((FI.READ-YK_START_ADDR1)/YK_FLASH_LEN)==0) //块1
				sn=((FI.READ-YK_START_ADDR1)/YK_INFO_LEN)+1;
			if(((FI.READ-YK_START_ADDR1)/YK_FLASH_LEN)==1) //块2
				sn=(YK_FLASH_LEN/YK_INFO_LEN)+((FI.READ-YK_START_ADDR1)/YK_INFO_LEN)+1;

			DEBUG_PRINT_NISR("KKSDSK","<--------------- 当前第: ",sn," 条YK内容为空! --------------->");
		}

		FI.READ+=YK_INFO_LEN;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- 历史 YK 文件读取完毕 --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_ECP_READ_YK
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool DEBUG_PRINT_YK(uint32_t read)
{
	P101YK YK;
	uint32_t sn;

	if(((read-YK_START_ADDR1)/YK_FLASH_LEN)==0) //块1
	{
		if((YK_START_ADDR1+YK_FLASH_LEN-read)<YK_INFO_LEN)
			read=YK_START_ADDR2;
	}

	if(((read-YK_START_ADDR1)/YK_FLASH_LEN)==1) //块2
	{
		if((YK_START_ADDR2+YK_FLASH_LEN-read)<YK_INFO_LEN)
			read=YK_START_ADDR1;
	}

	Mx25_Read_U8_Array(read,YK.DATA,YK_INFO_LEN,false);

	if(YK.TYPE==0xFF)
		return false;

	if(((read-YK_START_ADDR1)/YK_FLASH_LEN)==0)
		sn=((read-YK_START_ADDR1)/YK_INFO_LEN)+1;
	if(((read-YK_START_ADDR1)/YK_FLASH_LEN)==1)
		sn=(YK_FLASH_LEN/YK_INFO_LEN)+((read-YK_START_ADDR2)/YK_INFO_LEN)+1;

	DEBUG_PRINT_NISR("KSDSDSDSHSHSESHKK","当前第 ",sn," 条YK,MX25指针位置: ",read," YK类型: ",YK.TYPE," YK信息体地址: ",YK.MSGADDR.U16," YK传送原因: ",YK.COT.U16," 时间: ",YK.TIME," 内容: ",YK.SCO);

	return true;
}
/************************************************************!
*Function: DEBUG_READ_WAVE
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_READ_WAVE(void)
{
	vTaskSuspendAll();

	DEBUG_PRINT_NISR("KKNSKK","<--------------- 正在读取历史 WAVE 数据 --------------->");

	for(uint32_t i=0;i<((2*WAVE_FLASH_LEN)/WAVE_FILE_LEN);i++)
	{
		DEBUG_PRINT_NISR("KKSDSKK","<--------------- 当前第: ",i+1," 条WAVE ! --------------->");

		if(DEBUG_PRINT_WAVE(WAVE_START_ADDR1+i*WAVE_FILE_LEN)==false)
			break;
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- 历史 WAVE 文件读取完毕 --------------->");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_ECP_READ_WAVE
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool DEBUG_ECP_READ_WAVE(uint16_t fileid)
{
	uint8_t sn;
	FILEINFO FI;

	FI.ID=fileid;

	vTaskSuspendAll();

	Locate_Wave_FileInfo(&FI);

	sn=((FI.READ-WAVE_START_ADDR1)/WAVE_FILE_LEN)+1;

	if(DEBUG_PRINT_WAVE(FI.READ)==false)
	{
		DEBUG_PRINT_NISR("KNSDSK","<--------------- 当前第: ",sn," 号 WAVE 文件,文件为空! --------------->");
		return false;
	}

	DEBUG_PRINT_NISR("KNSDSDSESDSDSDSK","<----- 模拟ECP读取的 WAVE 文件号为: ",FI.ID," 为MX25中第: ",sn," 号WAVE文件,文件时间: ",FI.TIME," 文件节数: ",FI.INFONUM," 文件长度: ",FI.LENS," 文件指针: ",FI.READ," ----->");

	xTaskResumeAll();

	return true;
}
/************************************************************!
*Function: DEBUG_PRINT_WAVE
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool DEBUG_PRINT_WAVE(uint32_t read)
{
	DOUBLEBYTE DB;
	uint8_t data[100];
	WAVE_DAT_FORMAT WDF;

	Mx25_Read_U8_Array(read,DB.U8,2,false);

	if(DB.U16==0xFFFF)
		return false;

	read+=2;

	vTaskSuspendAll();

	Mx25_Read_U8_Array(read,data,7,false);
	read+=7;

	DEBUG_PRINT_NISR("KKNSEK","<----- 故障波形文件时间:",data);
	DEBUG_PRINT_NISR("KKNSK","<----- 当前波形文件的CFG文件如下:");
	DEBUG_PRINT_NISR("KKNSDKK","<----- 当前波形文件的CFG文件长度:",DB.U16);

	while(DB.U16>0)
	{
		if(DB.U16>=100)
		{
			Mx25_Read_U8_Array(read,data,100,false);
			DEBUG_PRINT_U8_ARRAY_NISR(data,100);
			read+=100;
			DB.U16-=100;
		}
		else
		{
			Mx25_Read_U8_Array(read,data,DB.U16,false);
			DEBUG_PRINT_U8_ARRAY_NISR(data,DB.U16);
			read+=DB.U16;
			DB.U16=0;
		}
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------------------------- 当前波形文件的DATA文件如下:");

	for(uint32_t i=0;i<MAX_WAVE_LEN;i++)
	{
		Mx25_Read_U8_Array(read,WDF.DATA,WAVE_INFO_LEN,false);

		if(WDF.N==0xFFFFFFFF)
			break;

		DEBUG_PRINT_NISR("KSDSDS","序号 ",WDF.N," 时间点 ",WDF.TIMESTAMP," 数据  ");
		DEBUG_PRINT_NISR("FSFSFSFSF",WDF.AU," ",WDF.BU," ",WDF.CU," ",WDF.ZU," ",WDF.AI);
		DEBUG_PRINT_NISR("SFSFSFSFS"," ",WDF.BI," ",WDF.CI," ",WDF.ZI," ",WDF.YX,"  ");

		DEBUG_PRINT_U8CHAR_ARRAY_NISR(WDF.DATA,WAVE_INFO_LEN);

		read+=WAVE_INFO_LEN;
	}

	DEBUG_PRINT_NISR("KKSKKK","<--------------------------------- 波形文件读取完毕!");

	xTaskResumeAll();

	return true;
}
/************************************************************!
*Function: DEBUG_READ_NOWTIME
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_READ_NOWTIME(void)
{
	uint32_t cnt;
	uint8_t data[6];

	while(RTC->CNT>32700);
	Get_R8025t_RtcTime(data,6);
	cnt=RTC->CNT;

	cnt=cnt*1000/32767;

	DEBUG_PRINT_NISR("KKSK",            "<--------------- 汇集单元系统时间如下 --------------->");
	DEBUG_PRINT_NISR("SDSDSK",          "<--------------- 当前UNIX时间: ",gUNIXTIME.UNIX,".",cnt," --------------->");
	DEBUG_PRINT_NISR("SDSDSDSDSDSDSDSK","<--------------- 当前北京时间: ",data[5],"-",data[4],"-",data[3]," ",data[2],":",data[1],":",data[0],".",cnt," --------------->");
}
/************************************************************!
*Function: DEBUG_READ_NOWTIME
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void DEBUG_ERASE_PARABLOCK(void)
{
	vTaskSuspendAll();

	DEBUG_PRINT_NISR("KKSKK","<--------------- 正在擦除系统参数区...");

	for(uint8_t i=0;i<10;i++)
	{
		Wdog_Feed();
		Mx25_Erase_Sector(i);
	}

	DEBUG_PRINT_NISR("KKSKK","<--------------- 系统参数区已擦除完毕!!!");

	xTaskResumeAll();
}
/************************************************************!
*Function: DEBUG_READ_NOWTIME
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool DEBUG_ECP_START_WAVE(ICPFRAME *IF)
{
	UNIXTIME UT;

	Get_Sys_Time(&UT);

	Trigger_Start_Call_Wave(0,UT.DATA);

	return true;
}
