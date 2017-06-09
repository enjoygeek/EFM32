/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: icp_process.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"


/************************************************************!
*Function: Icp_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Icp_Process(ICPFRAME *IF)
{
	if(IF->ISLOCK==false) //未组成一个完整的数据帧，则不进行数据帧的处理
		return false;

	IF->ISLOCK=false; //解析完此帧，继续解析下一帧

	gSYSFLAG.CWS.ICPRUNCOUNT++;

	if(Is_Valid_Icp_Address(IF)==false) //判断源地址和目的地址是否合法
		return false;

	switch(IF->CTRL.PRM) //判断主从者关系
	{
		case PRMSEC: //从动者
			Icp_Prmsec_Process(IF);
			break;

		case PRMPRI: //发起者
			Icp_Prmpri_Process(IF);
			break;

		default:
			break;
	}

	return true;
}
/************************************************************!
*Function: Icp_Prmsec_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_Prmsec_Process(ICPFRAME *IF) //从动者
{
	switch(IF->CTRL.CC)
	{
		case FCCON: //确认,肯定确认
			Icp_Fccon_Process(IF);
			break;

		case FCNOCON: //确认,否定确认
			Icp_Fcnocon_Process(IF);
			break;

		case FCRESDATA: //响应帧,数据
			Icp_Fcresdata_Process(IF);
			break;

		case FCNODATA: //响应帧,否定认可，无数据
			Icp_Fcnodata_Process(IF);
			break;

		default:
			break;
	}
}
/************************************************************!
*Function: Icp_Prmpri_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_Prmpri_Process(ICPFRAME *IF) //发起者
{
	switch(IF->CTRL.CC)
	{
		case FCRESETUSE: //发送/无应答
			Icp_Fcresetuse_Process(IF);
			break;

		case FCSENDDATA: //发送/确认
			Icp_Fcsenddata_Process(IF);
			break;

		case FCREQUESTD: //请求/响应
			Icp_Fcrequestd_Process(IF);
			break;

		default:
			break;
	}
}
/************************************************************!
*Function: Icp_Fccon_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_Fccon_Process(ICPFRAME *IF)
{
	Icp_PduType_Process(IF); //处理数据
}
/************************************************************!
*Function: Icp_Fcnocon_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_Fcnocon_Process(ICPFRAME *IF)
{
	//此函数暂时未用到
}
/************************************************************!
*Function: Icp_Fcresdata_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_Fcresdata_Process(ICPFRAME *IF)
{
	Icp_PduType_Process(IF); //处理数据
}
/************************************************************!
*Function: Icp_Fcnodata_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_Fcnodata_Process(ICPFRAME *IF)
{
	//此函数暂时未用到
}
/************************************************************!
*Function: Icp_Fcresetuse_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_Fcresetuse_Process(ICPFRAME *IF)
{
	Icp_PduType_Process(IF); //处理数据
}
/************************************************************!
*Function: Icp_Fcsenddata_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_Fcsenddata_Process(ICPFRAME *IF)
{
	Icp_PduType_Process(IF);
}
/************************************************************!
*Function: Icp_Fcrequestd_Process
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Icp_Fcrequestd_Process(ICPFRAME *IF)
{
	Icp_PduType_Process(IF); //处理数据
}
/************************************************************!
*Function: Fill_Icp_Frame
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Fill_Icp_Frame(uint8_t channel,uint8_t *addr,uint8_t ctrl,uint8_t type,uint8_t num,uint8_t seq,uint8_t *data,uint8_t lens)
{
	uint8_t sn=0xFF;

	while(sn==0xFF)
	{
		sn=Get_Empty_IcpFrame();

		if(sn!=0xFF)
			break;

		taskYIELD();
	}

	gICPFRAMELIST[sn].FRAME[0]=ICP_STARTBYTE;
	gICPFRAMELIST[sn].FRAME[1]=ICP_MINPDULEN+lens;
	gICPFRAMELIST[sn].FRAME[2]=ICP_MINPDULEN+lens;
	gICPFRAMELIST[sn].FRAME[3]=ICP_STARTBYTE;
	gICPFRAMELIST[sn].FRAME[4]=ctrl;

	memcpy(gICPFRAMELIST[sn].FRAME+5,gSYSPARA.BENJI,ICP_ADDRLEN);
	memcpy(gICPFRAMELIST[sn].FRAME+11,addr,ICP_ADDRLEN);

	gICPFRAMELIST[sn].FRAME[17]=type;
	gICPFRAMELIST[sn].FRAME[18]=num;
	gICPFRAMELIST[sn].FRAME[19]=seq;

	if(lens!=0)
		memcpy(gICPFRAMELIST[sn].FRAME+20,data,lens);

	gICPFRAMELIST[sn].FRAME[ICP_FRAMEHEAD+ICP_MINPDULEN+lens]=Check_Icp_Cs(gICPFRAMELIST[sn].FRAME+ICP_FRAMEHEAD,ICP_MINPDULEN+lens);
	gICPFRAMELIST[sn].FRAME[ICP_FRAMEHEAD+ICP_MINPDULEN+lens+1]=ICP_ENDBYTE;

	if(channel==RADIO_CHANNEL)
		gICPFRAMELIST[sn].ISLOCK=true;

	if(channel==WEIHU_CHANNEL)
	{
		taskENTER_CRITICAL();

		WriteDataCache(&WEIHU_TXDC,gICPFRAMELIST[sn].FRAME,ICP_MINFRAMELEN+lens);//写入MID，等待发送
		gICPFRAMELIST[sn].STATUS=false;

		taskEXIT_CRITICAL();
	}
}
/************************************************************!
*Function: Get_Empty_IcpFrame
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t Get_Empty_IcpFrame(void)
{
	for(uint8_t i=2;i<MAX_ICPFRAME_NUM;i++)
	{
		if(gICPFRAMELIST[i].STATUS==true)
			continue;

		gICPFRAMELIST[i].STATUS=true;

		return i;
	}

	return 0xFF;
}
/************************************************************!
*Function: Is_Valid_Icp_Address
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Is_Valid_Icp_Address(ICPFRAME *IF)
{
	IF->LINE=0xFF;
	IF->PHASE=0xFF; //此处用于判断采集单元是否在线

	if(!(memcmp(gSYSPARA.BROADCAST,IF->DSTADDR,ICP_ADDRLEN)==0||memcmp(gSYSPARA.BENJI,IF->DSTADDR,ICP_ADDRLEN)==0))
		return false;  //确认目的地址是否为本机地址或者广播地址

	for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)//判断源地址是否为采集单元地址
	{
		if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0) //判断线号不为0,确认该线号为有效的采集单元组
			continue;

		for(uint8_t phase=0;phase<3;phase++)
		{
			if(memcmp(gSYSPARA.LINEGROUP.LINE[line].AU[phase].ADDR,IF->SRCADDR,ICP_ADDRLEN)!=0)
				continue;

			IF->LINE=line;
			IF->PHASE=phase;

			return true;
		}
	}

	if(memcmp(gSYSPARA.WEIHU,IF->SRCADDR,ICP_ADDRLEN)==0) //判断源地址是否为维护地址
		return true;

	return false;
}

