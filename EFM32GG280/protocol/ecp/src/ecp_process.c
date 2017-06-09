/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: ecp_process.c
*Version: 0.1.0
*Date: 2016-10-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"


/************************************************************!
*Function: Ecp_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Ecp_Process(ECPFRAME *EF)
{
	if(EF->REQ.ISLOCK==false) //没有需要解析的101帧
		return false;

	EF->REQ.ISLOCK=false;

	gSYSFLAG.CWS.ECPRUNCOUNT++;

	if(Check_Ecp_Rtu(EF)==false)  //判断子站地址是否正确
		return false;

	#ifdef DEBUG_ECP_RECEIVE_FRAME_PRINT
	PRINT_ECP_RECEIVE_FRAME(EF);
	#endif

	if(EF->REQ.FRAMETYPE==ESM_FRAME) //进行ESAM解密处理
	{
		EF->REQ.FRAMETYPE=VAR_FRAME;

		if(Esam1120a_Verify_Mac_6Byte(EF->REQ.EF.VF.ASDU.DATA,EF->REQ.ESMLEN)==0)
			return false;
	}

	if(EF->LINKSTATE==LINKSTATE_START)
	{
		if(EF->REQ.FRAMETYPE!=FIX_FRAME)
			return false;

		switch(EF->REQ.EF.FF.CTRL.FC)
		{
			case ECP_RST_LINK:
				App_Send_Ack(EF);
				App_Reset_Link(EF);

				#ifdef HUNAN_101INIT
				App_Call_Master_Linkstate(EF);
				#else
				App_Init_End(EF);
				#endif

				break;

			 case ECP_CALL_LINKSTATE:
				 App_Send_Linkstate(EF);
				 break;

			 default:
				 break;
		}

		return true;
	}

	if(EF->LINKSTATE==LINKSTATE_CONNECT)
	{
		if(EF->REQ.FRAMETYPE==FIX_FRAME)
		{
			if(EF->REQ.EF.FF.CTRL.PRM==1) //只接收主站主发
			{
				switch(EF->REQ.EF.FF.CTRL.FC)
				{
					case ECP_RST_LINK:
						App_Send_Ack(EF);
						App_Reset_Link(EF);

						#ifdef HUNAN_101INIT
						App_Call_Master_Linkstate(EF);
						#else
						App_Init_End(EF);
						#endif

						break;

					case ECP_TICKS_TEST:
						App_Send_Ack(EF);
						break;

					case ECP_CALL_LINKSTATE:
						App_Send_Linkstate(EF);
						break;

					default:
						break;
				}
			}

			if(EF->REQ.EF.FF.CTRL.PRM==0) //只接收主站应答
			{
				switch(EF->REQ.EF.FF.CTRL.FC)
				{
					case ECP_RST_LINK:

						xTaskNotifyGive(xData_Send_Reply);

						#ifdef HUNAN_101INIT
						if(EF->LINKRESET==true)
						{
							App_Init_End(EF);
							EF->LINKRESET=false;
						}
						#endif
						break;

					case ECP_MASTER_LINKSTATE:
						App_Reset_Master_Link(EF);
						xTaskNotifyGive(xData_Send_Reply);
						break;

					default:
						break;
				}
			}

			return true;
		}

		if(EF->REQ.FRAMETYPE==VAR_FRAME)
		{
			switch(EF->REQ.EF.VF.CTRL.FC)
			{
				case ECP_CALL_DATA:
					App_Send_Ack(EF);
					App_Send_Data(EF);
					break;

				default:
					break;
			}
		}
	}

	return false;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Send_Ack(ECPFRAME *EF)
{
	uint8_t sn,priority;

	priority=0;
	sn=Req_Empty_ResFrame(EF,priority);

	EF->RES[priority].ETQ[sn].EF.FF.CTRL.DATA=0x80;

	Fill_Ecp_Frame(EF,FIX_FRAME,priority,sn,0);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Send_NoAck(ECPFRAME *EF)
{
	uint8_t sn,priority;

	priority=0;
	sn=Req_Empty_ResFrame(EF,priority);

	EF->RES[priority].ETQ[sn].EF.FF.CTRL.DATA=0x01;

	Fill_Ecp_Frame(EF,FIX_FRAME,priority,sn,0);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Send_Linkstate(ECPFRAME *EF)
{
	uint8_t priority,sn;

	priority=0;
	sn=Req_Empty_ResFrame(EF,priority);

	EF->RES[priority].ETQ[sn].EF.FF.CTRL.DATA=0x8B; //B:链路完好

	Fill_Ecp_Frame(EF,FIX_FRAME,priority,sn,0);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Reset_Link(ECPFRAME *EF)
{
	EF->SN=0;
	EF->S_FCB=0;
	EF->FILE.STATE=0;
	EF->INITRESET=true;
	EF->LINKSTATE=LINKSTATE_CONNECT;

	for(uint8_t i=1;i<MAX_PRIORITY_NUM;i++)
	{
		EF->PRIORITY[i]=0;

		for(uint8_t j=0;j<MAX_REPLY_NUM;j++) //消除待发送的报文
		{
			EF->RES[i].ETQ[j].ISLOCK=false;
			EF->RES[i].ETQ[j].STATUS=false;
		}
	}

	#ifdef PRINT_LINKSTATE_DEBUG
	DEBUG_PRINT_NISR("KKNSKK","<----- 收到主站的链路复位请求,ECP链路正在复位...");
	#endif
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Call_Master_Linkstate(ECPFRAME *EF)
{
	uint8_t priority,sn;

	priority=1;
	sn=Req_Empty_ResFrame(EF,priority);

	EF->RES[priority].ETQ[sn].EF.FF.CTRL.DATA=0xC9; //请求链路状态

	Fill_Ecp_Frame(EF,FIX_FRAME,priority,sn,0);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Reset_Master_Link(ECPFRAME *EF)
{
	uint8_t priority,sn; //应答帧的帧序号

	EF->LINKRESET=true;

	priority=1;
	sn=Req_Empty_ResFrame(EF,priority);

	EF->RES[priority].ETQ[sn].EF.FF.CTRL.DATA=0xC0; //复位主站链路

	Fill_Ecp_Frame(EF,FIX_FRAME,priority,sn,0);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void App_Init_End(ECPFRAME *EF)
{
	uint8_t priority,sn;

	priority=1;
	sn=Req_Empty_ResFrame(EF,priority);

	EF->RES[priority].ETQ[sn].EF.VF.CTRL.DATA=0xD3;
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.TI=70;  //初始化结束
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.VSQ=1;
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.COT.U16=4;  //初始化
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSGADDR.U16=0;
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSG[0]=00; //复位限定词

	Fill_Ecp_Frame(EF,VAR_FRAME,priority,sn,1);

	#ifdef STATUS_LED2_ECP_LINKSTATE_CONTROL
	Status_Led2_Clear();
	#endif
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
bool Check_Ecp_Rtu(ECPFRAME *EF)
{
	if(EF->REQ.FRAMETYPE==FIX_FRAME)
	{
		if(EF->REQ.EF.FF.LINKADDR.U16==gECPPARA.ZIZHAN.U16||EF->REQ.EF.FF.LINKADDR.U16==0xFFFF)
			return true;
	}

	if(EF->REQ.FRAMETYPE==VAR_FRAME||EF->REQ.FRAMETYPE==ESM_FRAME)
	{
		if(EF->REQ.EF.VF.LINKADDR.U16==gECPPARA.ZIZHAN.U16||EF->REQ.EF.VF.LINKADDR.U16==0xFFFF)
			return true;
	}

	return false;
}
/************************************************************!
*Function: Is_Sys_Full
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Is_Sys_Full(void)
{
	return false;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t Get_Empty_ResFrame(ECPFRAME *EF,uint8_t priority)
{
	uint8_t OLDSN=EF->RES[priority].SN;

	if(priority>(MAX_PRIORITY_NUM-1))
		return 0xFF;

	if(EF->LINKSTATE==LINKSTATE_START)
		return 0;

	for(uint8_t sn=OLDSN;sn<(MAX_REPLY_NUM+OLDSN);sn++)
	{
		EF->RES[priority].SN=sn%MAX_REPLY_NUM;

		if(EF->RES[priority].ETQ[EF->RES[priority].SN].STATUS==true)
			continue;

		EF->RES[priority].ETQ[EF->RES[priority].SN].STATUS=true;

		return EF->RES[priority].SN;
	}

	return 0xFF;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t Req_Empty_ResFrame(ECPFRAME *EF,uint8_t priority)
{
	uint8_t sn=0xFF; //帧序号

	while(sn==0xFF)
	{
		sn=Get_Empty_ResFrame(EF,priority);

		if(sn!=0xFF)
			break;

		taskYIELD();
	}

	return sn;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Fill_Ecp_Frame(ECPFRAME *EF,uint8_t frametype,uint8_t priority,uint8_t sn,uint8_t lens)
{
	EF->RES[priority].ETQ[sn].FRAMETYPE=frametype;

	if(EF->RES[priority].ETQ[sn].FRAMETYPE==FIX_FRAME)
	{
		EF->RES[priority].ETQ[sn].EF.FF.STARTBYTE=ECP_FIXFRMAE_STARTBYTE;
		EF->RES[priority].ETQ[sn].EF.FF.ENDBYTE=ECP_FIXFRMAE_ENDBYTE;

		EF->RES[priority].ETQ[sn].EF.FF.CTRL.DATA|=Is_Sys_Full();

		EF->RES[priority].ETQ[sn].EF.FF.LINKADDR.U16=gECPPARA.ZIZHAN.U16;
		EF->RES[priority].ETQ[sn].EF.FF.CS=Check_Ecp_Cs(EF->RES[priority].ETQ[sn].EF.DATA+1,3);
	}

	if(EF->RES[priority].ETQ[sn].FRAMETYPE==VAR_FRAME)
	{
		EF->RES[priority].ETQ[sn].EF.VF.STARTBYTE1=ECP_VARFRMAE_STARTBYTE;
		EF->RES[priority].ETQ[sn].EF.VF.STARTBYTE2=ECP_VARFRMAE_STARTBYTE;
		EF->RES[priority].ETQ[sn].EF.VF.ENDBYTE=ECP_VARFRMAE_ENDBYTE;
		#ifdef SINGLE_COT_CTRL
		EF->RES[priority].ETQ[sn].EF.VF.LEN1=10+lens;
		#else
		EF->RES[priority].ETQ[sn].EF.VF.LEN1=11+lens;
		#endif

		EF->RES[priority].ETQ[sn].EF.VF.LEN2=EF->RES[priority].ETQ[sn].EF.VF.LEN1;
		EF->RES[priority].ETQ[sn].EF.VF.CTRL.DATA|=Is_Sys_Full();
		EF->RES[priority].ETQ[sn].EF.VF.LINKADDR.U16=gECPPARA.ZIZHAN.U16;
		EF->RES[priority].ETQ[sn].EF.VF.ASDU.CAOA.U16=gECPPARA.ZIZHAN.U16;
	}

	if(EF->RES[priority].ETQ[sn].FRAMETYPE==ESM_FRAME)
	{
		EF->RES[priority].ETQ[sn].EF.VF.STARTBYTE1=ECP_ESMFRMAE_STARTBYTE;
		EF->RES[priority].ETQ[sn].EF.VF.STARTBYTE2=ECP_ESMFRMAE_STARTBYTE;
		EF->RES[priority].ETQ[sn].EF.VF.ENDBYTE=ECP_VARFRMAE_ENDBYTE;

		#ifdef SINGLE_COT_CTRL
			EF->RES[priority].ETQ[sn].EF.VF.LEN1=Esam_Data_Encryption(EF->RES[priority].ETQ[sn].EF.VF.ASDU.DATA,7+lens)+3;
		#else
			EF->RES[priority].ETQ[sn].EF.VF.LEN1=Esam1120a_Data_Encryption(EF->RES[priority].ETQ[sn].EF.VF.ASDU.DATA,8+lens)+3;
		#endif

		EF->RES[priority].ETQ[sn].EF.VF.LEN2=EF->RES[priority].ETQ[sn].EF.VF.LEN1;
		EF->RES[priority].ETQ[sn].EF.VF.CTRL.DATA|=Is_Sys_Full();
		EF->RES[priority].ETQ[sn].EF.VF.LINKADDR.U16=gECPPARA.ZIZHAN.U16;
		EF->RES[priority].ETQ[sn].EF.VF.ASDU.CAOA.U16=gECPPARA.ZIZHAN.U16;          //修改7+lens!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	if(priority==0) //没有优先级的数据,直接发送,此处未进行FCB处理
	{
		Fill_Ecp_DataCache(EF,priority,sn);
		EF->RES[priority].ETQ[sn].STATUS=false; //释放占用的数据帧状态

		#ifdef DEBUG_ECP_SEND_FRAME_PRINT
		PRINT_ECP_SEND_FRAME(EF,priority,sn);
		#endif
	}

	if(priority!=0)
	{
		#ifndef ECP_LINKSTATE_CONTROL
		EF->ISLOCK=true;
		EF->RES[priority].ETQ[sn].ISLOCK=true; //此代码要放在组帧函数的最后,防止发送函数出错
		#else  //没有优先级的数据,直接发送
		Fill_Ecp_DataCache(EF,priority,sn);
		EF->RES[priority].ETQ[sn].STATUS=false; //释放占用的数据帧状态
		#ifdef DEBUG_ECP_SEND_FRAME_PRINT
		PRINT_ECP_SEND_FRAME(EF,priority,sn);
		#endif
		#endif
	}
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Fill_Ecp_DataCache(ECPFRAME *EF,uint8_t priority,uint8_t sn)
{
	if(EF->RES[priority].ETQ[sn].FRAMETYPE==FIX_FRAME)
	{
		taskENTER_CRITICAL();
		WriteDataCache(&GPRS_TXDC,EF->RES[priority].ETQ[sn].EF.DATA,ECP_FIXFRMAE_LEN);
		taskEXIT_CRITICAL();
	}

	if(EF->RES[priority].ETQ[sn].FRAMETYPE!=FIX_FRAME)
	{
		taskENTER_CRITICAL();
		WriteDataCache(&GPRS_TXDC,EF->RES[priority].ETQ[sn].EF.DATA,EF->RES[priority].ETQ[sn].EF.VF.LEN1+ECP_VARFRMAE_HEAD);
		WriteDataCache(&GPRS_TXDC,&EF->RES[priority].ETQ[sn].EF.VF.CS,1);
		WriteDataCache(&GPRS_TXDC,&EF->RES[priority].ETQ[sn].EF.VF.ENDBYTE,1);
		taskEXIT_CRITICAL();
	}
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Ecp_Fcb_Flip(ECPFRAME *EF,uint8_t priority,uint8_t sn)
{
	if(EF->RES[priority].ETQ[sn].FRAMETYPE==FIX_FRAME)
	{
		if(EF->RES[priority].ETQ[sn].EF.FF.CTRL.FCV==1)
		{
			if(EF->S_FCB==0)
				EF->RES[priority].ETQ[sn].EF.FF.CTRL.FCB=1;

			EF->S_FCB^=1;
		}

		EF->RES[priority].ETQ[sn].EF.FF.CS=Check_Ecp_Cs(EF->RES[priority].ETQ[sn].EF.DATA+1,3);
	}

	if(EF->RES[priority].ETQ[sn].FRAMETYPE!=FIX_FRAME)
	{
		if(EF->RES[priority].ETQ[sn].EF.VF.CTRL.FCV==1)
		{
			if(EF->S_FCB==0)
				EF->RES[priority].ETQ[sn].EF.VF.CTRL.FCB=1;

			EF->S_FCB^=1;
		}

		EF->RES[priority].ETQ[sn].EF.VF.CS=Check_Ecp_Cs(EF->RES[priority].ETQ[sn].EF.DATA+ECP_VARFRMAE_HEAD,EF->RES[priority].ETQ[sn].EF.VF.LEN1);
	}
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Add_P101SoeQueue(SOEQUEUE *SQ,P101SOE *PS)
{
	portTICK_TYPE_ENTER_CRITICAL();

	SQ->SOE[SQ->WRITE].ISLOCK=true;

	memcpy(SQ->SOE[SQ->WRITE++].DATA,PS->DATA,SOE_INFO_LEN);

	SQ->WRITE%=MAX_SOE_NUM; //队列满,从0开始循环写

	portTICK_TYPE_EXIT_CRITICAL();
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t Get_P101SoeQueue(SOEQUEUE *SQ)
{
	for(uint8_t i=0;i<MAX_SOE_NUM;i++)
	{
		if(SQ->SOE[i].ISLOCK==true)
			return i;
	}

	return 0xFF;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool App_Send_P101SoeQueue(ECPFRAME *EF,SOEQUEUE *SQ)
{
	uint8_t priority,sn,cn; //应答帧的帧序号

	#ifndef ECP_LINKSTATE_CONTROL
	if(EF->LINKSTATE!=LINKSTATE_CONNECT)
		return false;
	#endif

	#ifndef ESAMC_FUNCTION_CTRL
	if(EF->ESMSTATE==false)
		return false;
	#endif

	cn=Get_P101SoeQueue(SQ);

	if(cn==0xFF)
		return false;

	DEBUG_PRINT_NISR("KKNSKK","<-------------- 正在发送SOE数据帧---------------->");

//	priority=4;
//	sn=Req_Empty_ResFrame(EF,priority);
//
//	EF->RES[priority].ETQ[sn].EF.VF.CTRL.DATA=0xD3;
//
//	EF->RES[priority].ETQ[sn].EF.VF.ASDU.TI=1; //单点信息
//	EF->RES[priority].ETQ[sn].EF.VF.ASDU.VSQ=1;
//	EF->RES[priority].ETQ[sn].EF.VF.ASDU.COT.U16=3; //突发
//	EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSGADDR.U16=SQ->SOE[cn].MSGADDR.U16;
//	EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSG[0]=SQ->SOE[cn].SOE;
//
//	SQ->SOE[cn].ISLOCK=false;
//
//	#ifdef ESAMC_FUNCTION_CTRL
//	Fill_Ecp_Frame(EF,VAR_FRAME,priority,sn,1);
//	#else
//	Fill_Ecp_Frame(EF,ESM_FRAME,priority,sn,1);
//	#endif

	priority=4;
	sn=Req_Empty_ResFrame(EF,priority);

	EF->RES[priority].ETQ[sn].EF.VF.CTRL.DATA=0xD3;
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.TI=0x1E; //带时标的单点信息
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.VSQ=1;
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.COT.U16=3; //突发
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSGADDR.U16=SQ->SOE[cn].MSGADDR.U16;
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSG[0]=SQ->SOE[cn].SOE;
	memcpy(EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSG+1,&SQ->SOE[cn].TIME,7);

	SQ->SOE[cn].ISLOCK=false;

	#ifdef ESAMC_FUNCTION_CTRL
	Fill_Ecp_Frame(EF,VAR_FRAME,priority,sn,8);
	#else
	Fill_Ecp_Frame(EF,ESM_FRAME,priority,sn,8);
	#endif

	#ifdef ECP_SOE_REPORT_DEBUG //打印上报的SOE信息
	vTaskSuspendAll();
	DEBUG_PRINT_NISR("KKNSK","<---------- 已上报主站SOE数据!SOE数据如下: ---------->");
	DEBUG_PRINT_NISR("SESHSHKK","<---------- SOE时间: ",EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSG+1," SOE地址: ",EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSGADDR.U16,"SOE内容: ",EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSG[0]);
	xTaskResumeAll();
	#endif

	return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Add_P101FaultQueue(FAULTQUEUE *FQ,P101FAULT *PS)
{
	portTICK_TYPE_ENTER_CRITICAL();

	FQ->FAULT[FQ->WRITE].ISLOCK=true;

	memcpy(FQ->FAULT[FQ->WRITE++].DATA,PS->DATA,10);

	FQ->WRITE%=20;

	portTICK_TYPE_EXIT_CRITICAL();
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t Get_P101FaultQueue(FAULTQUEUE *FQ)
{
	for(uint8_t i=0;i<20;i++)
	{
		if(FQ->FAULT[i].ISLOCK==true)
			return i;
	}

	return 0xFF;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool App_Send_P101FaultQueue(ECPFRAME *EF,FAULTQUEUE *FQ)
{
	uint8_t priority,sn,cn; //应答帧的帧序号

	#ifndef ECP_LINKSTATE_CONTROL
	if(EF->LINKSTATE!=LINKSTATE_CONNECT)
		return false;
	#endif

	cn=Get_P101FaultQueue(FQ);

	if(cn==0xFF)
		return false;

	priority=6;
	sn=Req_Empty_ResFrame(EF,priority);

	EF->RES[priority].ETQ[sn].EF.VF.CTRL.DATA=0xD3;

	EF->RES[priority].ETQ[sn].EF.VF.ASDU.TI=0x1E; //带时标的单点信息
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.VSQ=1;
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.COT.U16=4; //激活确认
	EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSGADDR.U16=FQ->FAULT[cn].MSGADDR.U16;
	memcpy(EF->RES[priority].ETQ[sn].EF.VF.ASDU.MSG,FQ->FAULT[cn].DATA+2,8);

	FQ->FAULT[cn].ISLOCK=false;

	Fill_Ecp_Frame(EF,VAR_FRAME,priority,sn,8);

	return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void PRINT_ECP_RECEIVE_FRAME(ECPFRAME *EF)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_NISR("KKNSKK",">>>>>>>>>>>>>>>>>>>> 汇集单元收到主站的数据帧:");

	if(EF->REQ.FRAMETYPE==FIX_FRAME)
		DEBUG_PRINT_U8CHAR_ARRAY_NISR(EF->REQ.EF.DATA,ECP_FIXFRMAE_LEN);

	if(EF->REQ.FRAMETYPE!=FIX_FRAME)
	{
		DEBUG_PRINT_U8CHAR_ARRAY_NISR(EF->REQ.EF.DATA,EF->REQ.EF.VF.LEN1+4);
		DEBUG_PRINT_U8CHAR_ARRAY_NISR(EF->REQ.EF.DATA+ECP_VARFRAME_CS_LOC,ECP_VARFRMAE_TAIL);
	}

	DEBUG_PRINT_NISR("KK");

	Wdog_Feed();
	xTaskResumeAll();
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void PRINT_ECP_SEND_FRAME(ECPFRAME *EF,uint8_t priority,uint8_t sn)
{
	vTaskSuspendAll();
	Wdog_Feed();

	DEBUG_PRINT_NISR("KNSKK","<<<<<<<<<<<<<<<<<<<< 汇集单元正在发送数据帧:");

	if(EF->RES[priority].ETQ[sn].FRAMETYPE==FIX_FRAME)
		DEBUG_PRINT_U8CHAR_ARRAY_NISR(EF->RES[priority].ETQ[sn].EF.DATA,ECP_FIXFRMAE_LEN);

	if(EF->RES[priority].ETQ[sn].FRAMETYPE!=FIX_FRAME)
	{
		DEBUG_PRINT_U8CHAR_ARRAY_NISR(EF->RES[priority].ETQ[sn].EF.DATA,EF->RES[priority].ETQ[sn].EF.VF.LEN1+4);
		DEBUG_PRINT_U8CHAR_ARRAY_NISR(EF->RES[priority].ETQ[sn].EF.DATA+ECP_VARFRAME_CS_LOC,ECP_VARFRMAE_TAIL);
	}

	DEBUG_PRINT_NISR("KK");

	Wdog_Feed();
	xTaskResumeAll();
}

