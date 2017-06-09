/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: flashstorage.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "flashstorage.h"


/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void FlashPara_Init(void)
{
	gSYSFLAG.MFW.LOG=Locate_Log_Position();

	gSYSFLAG.MFW.SOE=Locate_Soe_Position();

	gSYSFLAG.MFW.YC=Locate_Yc_Position();

	gSYSFLAG.MFW.FAULT=Locate_Fault_Position();

	gSYSFLAG.MFW.YK=Locate_Yk_Position();

	gSYSFLAG.MFW.WAVE=Locate_Wave_Position();

	#ifdef PRINT_SYS_INIT_DEBUG    //打印初始化指针
	DEBUG_PRINT("NSHSHK","<----- 初始 日志 指针: ",LOG_START_ADDR1,  " ----- 当前指针 ----- ",gSYSFLAG.MFW.LOG);
	DEBUG_PRINT("NSHSHK","<----- 初始 遥信 指针: ",SOE_START_ADDR1,  " ----- 当前指针 ----- ",gSYSFLAG.MFW.SOE);
	DEBUG_PRINT("NSHSHK","<----- 初始 遥测 指针: ",YC_START_ADDR1,   " ----- 当前指针 ----- ",gSYSFLAG.MFW.YC);
	DEBUG_PRINT("NSHSHK","<----- 初始 故障 指针: ",FAULT_START_ADDR1," ----- 当前指针 ----- ",gSYSFLAG.MFW.FAULT);
	DEBUG_PRINT("NSHSHK","<----- 初始 遥控 指针: ",YK_START_ADDR1,   " ----- 当前指针 ----- ",gSYSFLAG.MFW.YK);
	DEBUG_PRINT("NSHSHK","<----- 初始 波形 指针: ",WAVE_START_ADDR1, " ----- 当前指针 ----- ",gSYSFLAG.MFW.WAVE);
	#endif
}
/************************************************************!
*Function:
*Description:
*Output: null
*Return: null
*************************************************************/
void SysPara_Init(void)
{
	Get_R8025t_RtcTime(gUNIXTIME.DATA,6);                 //获得R8025T时钟
	gUNIXTIME.UNIX=LocalTime2UnixTime(gUNIXTIME.DATA,6);  //更新软系统时钟


	Mx25_Read_U8_Array(SYSPARA_ADDR,gSYSPARA.DATA,SYSPARA_NUM,false);

	if((gSYSPARA.LINEGROUP.LINE[0].LINENO!=0xFF)&&gSYSPARA.LINEGROUP.LINE[0].LINENO!=0x00)  //有MX25参数更新系统参数
	{
		gSYSPARA.LINEGROUP.LINENUM=0;  //先清空

		for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)  //更新采集单元组数目
		{
			if((gSYSPARA.LINEGROUP.LINE[line].LINENO!=0xFF)&&(gSYSPARA.LINEGROUP.LINE[line].LINENO!=0))
				gSYSPARA.LINEGROUP.LINENUM++;
		}
	}
	else
	{
		//无MX25参数,则以下设置默认的系统参数

		memset(gSYSPARA.DATA,0,SYSPARA_NUM); //清空参数区

		gSYSPARA.LINEGROUP.LINENUM=1;  //默认采集单元组数
		gSYSPARA.LINEGROUP.LINE[0].LINENO=1; //默认采集单元组的线号,无效采集单元组的线号应该设置为0
		memset(gSYSPARA.LINEGROUP.LINE[0].AU[0].ADDR,0x00,6); //默认A相地址
		memset(gSYSPARA.LINEGROUP.LINE[0].AU[1].ADDR,0x00,6); //默认B相地址
		memset(gSYSPARA.LINEGROUP.LINE[0].AU[2].ADDR,0x00,6); //默认C相地址
		gSYSPARA.LINEGROUP.LINE[0].AU[0].ADDR[5]=0x01;
		gSYSPARA.LINEGROUP.LINE[0].AU[1].ADDR[5]=0x02;
		gSYSPARA.LINEGROUP.LINE[0].AU[2].ADDR[5]=0x03;

		gSYSPARA.LINEGROUP.LINE[1].LINENO=0; //默认采集单元组的线号,无效采集单元组的线号应该设置为0
		memset(gSYSPARA.LINEGROUP.LINE[1].AU[0].ADDR,0x00,6); //默认A相地址
		memset(gSYSPARA.LINEGROUP.LINE[1].AU[1].ADDR,0x00,6); //默认B相地址
		memset(gSYSPARA.LINEGROUP.LINE[1].AU[2].ADDR,0x00,6); //默认C相地址
		gSYSPARA.LINEGROUP.LINE[1].AU[0].ADDR[5]=0x11;
		gSYSPARA.LINEGROUP.LINE[1].AU[1].ADDR[5]=0x12;
		gSYSPARA.LINEGROUP.LINE[1].AU[2].ADDR[5]=0x13;

		memset(gSYSPARA.BENJI,0x01,6);      //默认本机地址
		memset(gSYSPARA.WEIHU,0x00,6);      //默认维护地址
		memset(gSYSPARA.BROADCAST,0xFF,6);  //默认广播地址
		gSYSPARA.RADIO_CHANNEL=1;           //默认射频信道

		//此处补充系统默认参数的默认值,注意修改SYSPARA_NUM宏定义!!!!!!!!
	}
}
/************************************************************!
*Function:
*Description:
*Output: null
*Return: null
*************************************************************/
void EcpPara_Init(void)
{
	Mx25_Read_U8_Array(ECPPARA_ADDR,gECPPARA.DATA,(ECPPARA_NUM*2),false);

	gECPPARA.YC_REPORT_CYCLE=(gECPPARA.YC_REPORT_CYCLE!=0xFFFF)?gECPPARA.YC_REPORT_CYCLE:60;                                 //设置遥测上报周期
	gECPPARA.GPS_TIMING_MODE=(gECPPARA.GPS_TIMING_MODE!=0xFFFF)?gECPPARA.GPS_TIMING_MODE:0;                                  //设置汇集单元校时模式
	gECPPARA.GPS_TIMING_CYCLE=(gECPPARA.GPS_TIMING_CYCLE!=0xFFFF)?gECPPARA.GPS_TIMING_CYCLE:14400;                           //设置GPS校时周期
	gECPPARA.BATT_VOL_THRESHOLD=(gECPPARA.BATT_VOL_THRESHOLD!=0xFFFF)?gECPPARA.BATT_VOL_THRESHOLD:12100;                     //设置汇集单元电池低电压阈值
	gECPPARA.SUN_VOL_REPORT_THRESHOLD=(gECPPARA.SUN_VOL_REPORT_THRESHOLD!=0xFFFF)?gECPPARA.SUN_VOL_REPORT_THRESHOLD:500;     //设置太阳能电池电压上报阈值
	gECPPARA.BATT_VOL_REPORT_THRESHOLD=(gECPPARA.BATT_VOL_REPORT_THRESHOLD!=0xFFFF)?gECPPARA.BATT_VOL_REPORT_THRESHOLD:500;  //设置蓄电池电压上报阈值
	gECPPARA.I_REPORT_THRESHOLD=(gECPPARA.I_REPORT_THRESHOLD!=0xFFFF)?gECPPARA.I_REPORT_THRESHOLD:10;                        //设置电流上报阈值
	gECPPARA.U_REPORT_THRESHOLD=(gECPPARA.U_REPORT_THRESHOLD!=0xFFFF)?gECPPARA.U_REPORT_THRESHOLD:10;                        //设置电压上报阈值
	gECPPARA.T_REPORT_THRESHOLD=(gECPPARA.T_REPORT_THRESHOLD!=0xFFFF)?gECPPARA.T_REPORT_THRESHOLD:1;                         //设置温度上报阈值
	gECPPARA.B_REPORT_THRESHOLD=(gECPPARA.B_REPORT_THRESHOLD!=0xFFFF)?gECPPARA.B_REPORT_THRESHOLD:1;                         //设置电池上报阈值
	gECPPARA.ZIZHAN.U16=(gECPPARA.ZIZHAN.U16!=0xFFFF)?gECPPARA.ZIZHAN.U16:0x0001;                                            //设置汇集单元子站地址
	gECPPARA.FAULT_WAVE_NUM=(gECPPARA.FAULT_WAVE_NUM!=0xFFFF)?gECPPARA.FAULT_WAVE_NUM:0x0509;                                //设置故障波形录波模式
	gECPPARA.YC_REPORT_MODE=(gECPPARA.YC_REPORT_MODE!=0xFFFF)?gECPPARA.YC_REPORT_MODE:0;                                     //设置遥测上报模式
	gECPPARA.RADIO_CHANNEL=(gECPPARA.RADIO_CHANNEL!=0xFFFF)?gECPPARA.RADIO_CHANNEL:0;
	gECPPARA.AU_OFFLINE_TIME=(gECPPARA.AU_OFFLINE_TIME!=0xFFFF)?gECPPARA.AU_OFFLINE_TIME:60;


	//以上位置添加ECPPARA参数默认值,注意修改ECPPARA_NUM!!!!!!!!!!!!!!!
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void SysFlag_Init(void)
{
	RADIOFRAME.CHANNEL=RADIO_CHANNEL; //通道初始化
	WEIHUFRAME.CHANNEL=WEIHU_CHANNEL;


	gSYSFLAG.CIS.GPRS=TXC;
	gSYSFLAG.CIS.RADIO=RX;
	gSYSFLAG.CIS.WEIHU=TXC;

	for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)
	{
		gSYSFLAG.AWS.ASF[line].AU_TIMING_ASYNC=AU_TIMING_ASYNC_BUFFER;  //系统初始化直接校时,跳过异步缓冲

		for(uint8_t phase=0;phase<3;phase++)
		{
			gSYSFLAG.STM.STC[line].AU_OFFLINE_MARK[phase]=gUNIXTIME.UNIX;
		}
	}
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool PointTable_Init(void)
{
	DOUBLEBYTE DB;
	uint32_t yxmaxspan=0;    //采集单元组遥信数据的信息体地址的最大跨度
	uint32_t ycmaxspan=0;    //采集单元组遥测数据的信息体地址的最大跨度
	uint32_t yxmaxnum=34-10;
	uint32_t ycmaxnum=17-2;
	uint32_t yxoffset=0x0000; //遥信点表信息体地址偏移
	uint32_t ycoffset=0x4000; //遥测点表信息体地址偏移

	memset(gECPINFO.DATA,0,ECPINFO_LEN); //清空遥测遥信数据

	Mx25_Read_U8_Array(POINTTABLE_ADDR,DB.U8,2,false);  //读取MX25中的点表

	if(DB.U16!=0xFFFF)
	{
		for(uint32_t num=0;num<ECPYX_NUM;num++)
		{
			Mx25_Read_U8_Array(POINTTABLE_ADDR+2*num,DB.U8,2,false);
			gECPINFO.YX.DATA[2*num]=DB.U16;
		}

		for(uint32_t num=0;num<ECPYC_NUM;num++)
		{
			Mx25_Read_U8_Array(POINTTABLE_ADDR+2*ECPYX_NUM+2*num,DB.U8,2,false);
			gECPINFO.YC.DATA[3*num]=DB.U16;
		}

		return true;
	}

	gECPINFO.YX.STATUS.ADDR.U16=yxoffset+1;
	gECPINFO.YX.LOW_POWER.ADDR.U16=yxoffset+2;
	gECPINFO.YX.POWER_SWITCH.ADDR.U16=yxoffset+3;
	gECPINFO.YX.RF_FAULT.ADDR.U16=yxoffset+4;
	gECPINFO.YX.GPS_LOCK.ADDR.U16=yxoffset+5;
	gECPINFO.YX.WAVEFILE.ADDR.U16=yxoffset+6;
	gECPINFO.YX.ALL_FAULT.ADDR.U16=yxoffset+7;
	gECPINFO.YX.LINE_WORKMODE[0].ADDR.U16=yxoffset+8;
	gECPINFO.YX.LINE_WORKMODE[1].ADDR.U16=yxoffset+9;
	gECPINFO.YX.LINE_WORKMODE[2].ADDR.U16=yxoffset+10;

	for(uint8_t line=0;line<gSYSPARA.LINEGROUP.LINENUM;line++)
	{
		yxmaxspan=yxoffset+(yxmaxnum*line);

		gECPINFO.YX.ECPYX[line].CUR_OVER[0].ADDR.U16=yxmaxspan+11;
		gECPINFO.YX.ECPYX[line].CUR_OVER[1].ADDR.U16=yxmaxspan+12;
		gECPINFO.YX.ECPYX[line].CUR_OVER[2].ADDR.U16=yxmaxspan+13;
		gECPINFO.YX.ECPYX[line].COMM_FAULT[0].ADDR.U16=yxmaxspan+14;
		gECPINFO.YX.ECPYX[line].COMM_FAULT[1].ADDR.U16=yxmaxspan+15;
		gECPINFO.YX.ECPYX[line].COMM_FAULT[2].ADDR.U16=yxmaxspan+16;
		gECPINFO.YX.ECPYX[line].SHUNSHI_FAULT[0].ADDR.U16=yxmaxspan+17;
		gECPINFO.YX.ECPYX[line].SHUNSHI_FAULT[1].ADDR.U16=yxmaxspan+18;
		gECPINFO.YX.ECPYX[line].SHUNSHI_FAULT[2].ADDR.U16=yxmaxspan+19;
		gECPINFO.YX.ECPYX[line].YONGJIU_FAULT[0].ADDR.U16=yxmaxspan+20;
		gECPINFO.YX.ECPYX[line].YONGJIU_FAULT[1].ADDR.U16=yxmaxspan+21;
		gECPINFO.YX.ECPYX[line].YONGJIU_FAULT[2].ADDR.U16=yxmaxspan+22;
		gECPINFO.YX.ECPYX[line].AU_LOW_POWER[0].ADDR.U16=yxmaxspan+23;
		gECPINFO.YX.ECPYX[line].AU_LOW_POWER[1].ADDR.U16=yxmaxspan+24;
		gECPINFO.YX.ECPYX[line].AU_LOW_POWER[2].ADDR.U16=yxmaxspan+25;
		gECPINFO.YX.ECPYX[line].AU_POWER_SWITCH[0].ADDR.U16=yxmaxspan+26;
		gECPINFO.YX.ECPYX[line].AU_POWER_SWITCH[1].ADDR.U16=yxmaxspan+27;
		gECPINFO.YX.ECPYX[line].AU_POWER_SWITCH[2].ADDR.U16=yxmaxspan+28;
		gECPINFO.YX.ECPYX[line].FUHE_OVER[0].ADDR.U16=yxmaxspan+29;
		gECPINFO.YX.ECPYX[line].FUHE_OVER[1].ADDR.U16=yxmaxspan+30;
		gECPINFO.YX.ECPYX[line].FUHE_OVER[2].ADDR.U16=yxmaxspan+31;
		gECPINFO.YX.ECPYX[line].ZHONGZAI[0].ADDR.U16=yxmaxspan+32;
		gECPINFO.YX.ECPYX[line].ZHONGZAI[1].ADDR.U16=yxmaxspan+33;
		gECPINFO.YX.ECPYX[line].ZHONGZAI[2].ADDR.U16=yxmaxspan+34;
	}

	gECPINFO.YC.SUN_VOL.ADDR.U16=ycoffset+1;
	gECPINFO.YC.BAT_VOL.ADDR.U16=ycoffset+2;

	for(uint8_t line=0;line<gSYSPARA.LINEGROUP.LINENUM;line++)
	{
		ycmaxspan=ycoffset+(ycmaxnum*line);

		gECPINFO.YC.ECPYC[line].CUR[0].ADDR.U16=ycmaxspan+3;
		gECPINFO.YC.ECPYC[line].CUR[1].ADDR.U16=ycmaxspan+4;
		gECPINFO.YC.ECPYC[line].CUR[2].ADDR.U16=ycmaxspan+5;
		gECPINFO.YC.ECPYC[line].VOL[0].ADDR.U16=ycmaxspan+6;
		gECPINFO.YC.ECPYC[line].VOL[1].ADDR.U16=ycmaxspan+7;
		gECPINFO.YC.ECPYC[line].VOL[2].ADDR.U16=ycmaxspan+8;
		gECPINFO.YC.ECPYC[line].TEM[0].ADDR.U16=ycmaxspan+9;
		gECPINFO.YC.ECPYC[line].TEM[1].ADDR.U16=ycmaxspan+10;
		gECPINFO.YC.ECPYC[line].TEM[2].ADDR.U16=ycmaxspan+11;
		gECPINFO.YC.ECPYC[line].AU_BAT_VOL[0].ADDR.U16=ycmaxspan+12;
		gECPINFO.YC.ECPYC[line].AU_BAT_VOL[1].ADDR.U16=ycmaxspan+13;
		gECPINFO.YC.ECPYC[line].AU_BAT_VOL[2].ADDR.U16=ycmaxspan+14;
		gECPINFO.YC.ECPYC[line].AU_GET_VOL[0].ADDR.U16=ycmaxspan+15;
		gECPINFO.YC.ECPYC[line].AU_GET_VOL[1].ADDR.U16=ycmaxspan+16;
		gECPINFO.YC.ECPYC[line].AU_GET_VOL[2].ADDR.U16=ycmaxspan+17;
	}


	gECPINFO.YX.GPS_LOCK.MSG.U16=gECPPARA.GPS_TIMING_MODE; //将GPS校时状态赋值给遥信



	//以下代码用于调试遥信遥测点表

	for(uint32_t i=0;i<(7+3*8*gSYSPARA.LINEGROUP.LINENUM);i++) //遥信点表测试
	{
		//gECPINFO.YX.DATA[2*i]=i+1;
		//gECPINFO.YX.DATA[2*i+1]=i+1;
	}

	for(uint32_t i=0;i<(2+3*5*gSYSPARA.LINEGROUP.LINENUM);i++) //遥测点表测试
	{
		//gECPINFO.YC.DATA[3*i]=i+1;
		//gECPINFO.YC.DATA[3*i+1]=i+1;
		//gECPINFO.YC.DATA[3*i+2]=i+1;
	}

	return true;
}
/************************************************************!
*Function:
*Description:
*Output: null
*Return: null
*************************************************************/
void Program_Storage(uint16_t sn,uint8_t *data,uint32_t lens)
{
	FOURBYTE FB;
	uint8_t len1,len2;

	len1=lens%4;
	len2=lens-len1;

	for(uint8_t i=0;i<len2;i+=4)
	{
		memcpy(FB.U8,data+i,4);

    	Msc_WriteWord(BACKUP_PROGRAM+(sn-1)*BACKUP_PRM_BLOCK+i,FB.U32);
	}

	if(len1>0) //不为4的整数倍,有且只能有最后一帧数据不为4的整数倍
	{
		FB.U32=0;

		memcpy(FB.U8,data+len2,len1);

    	Msc_WriteWord(BACKUP_PROGRAM+(sn-1)*BACKUP_PRM_BLOCK+len2,FB.U32);
	}
}
/************************************************************!
*Function:
*Description:
*Output: null
*Return: null
*************************************************************/
uint16_t Get_Program_Cs(uint32_t lens)
{
	FOURBYTE FB;
	uint16_t cs=0;

	for(uint32_t i=0;i<lens;i+=4)
	{
		FB.U32=Msc_ReadWord(BACKUP_PROGRAM+i);

		for(uint8_t j=0;j<4;j++)
			cs+=FB.U8[j];
	}

	return cs;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
uint32_t Locate_Log_Position(void)
{
	FOURBYTE FB;
	uint32_t pointer;

	pointer=((LOG_FLASH_LEN/LOG_INFO_LEN)-1)*LOG_INFO_LEN;  //块的最后一条记录数据空间相对地址

	Mx25_Read_U8_Array(LOG_START_ADDR1+pointer,FB.U8,4,false);

	if(FB.U32==0xFFFFFFFF) //第一个块最后一条记录数据空间没有数据,则数据指针在第一个块中
	{
		for(uint32_t i=0;i<(LOG_FLASH_LEN/LOG_INFO_LEN);i++)
		{
			Mx25_Read_U8_Array(LOG_START_ADDR1+i*LOG_INFO_LEN,FB.U8,4,false);

			if(FB.U32==0xFFFFFFFF)
				return (LOG_START_ADDR1+i*LOG_INFO_LEN);
		}
	}

	Mx25_Read_U8_Array(LOG_START_ADDR2+pointer,FB.U8,4,false);

	if(FB.U32==0xFFFFFFFF) //第二个块最后一条记录数据空间没有数据,则数据指针在第二个块中
	{
		for(uint32_t i=0;i<(LOG_FLASH_LEN/LOG_INFO_LEN);i++)
		{
			Mx25_Read_U8_Array(LOG_START_ADDR2+i*LOG_INFO_LEN,FB.U8,4,false);

			if(FB.U32==0xFFFFFFFF)
				return (LOG_START_ADDR2+i*LOG_INFO_LEN);
		}
	}

	for(uint32_t i=0;i<(2*LOG_SECTOR_NUM);i++) //当两个块的最后一条记录空间都满了的时候,就强制认为该写第一个块了
		Mx25_Erase_Sector((LOG_START_ADDR1/MX25_SECTOR_LEN)+i); //此处是极小概率有问题的,先这样处理

	return LOG_START_ADDR1;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
uint32_t Locate_Soe_Position(void)
{
	FOURBYTE FB;
	uint32_t pointer;

	pointer=((SOE_FLASH_LEN/SOE_INFO_LEN)-1)*SOE_INFO_LEN;  //块的最后一条记录数据空间相对地址(偏移)

	Mx25_Read_U8_Array(SOE_START_ADDR1+pointer,FB.U8,4,false);

	if(FB.U32==0xFFFFFFFF) //第一个块最后一条记录数据空间没有数据,则数据指针在第一个块中
	{
		for(uint32_t i=0;i<(SOE_FLASH_LEN/SOE_INFO_LEN);i++)
		{
			Mx25_Read_U8_Array(SOE_START_ADDR1+i*SOE_INFO_LEN,FB.U8,4,false);

			if(FB.U32==0xFFFFFFFF)
				return (SOE_START_ADDR1+i*SOE_INFO_LEN);
		}
	}

	Mx25_Read_U8_Array(SOE_START_ADDR2+pointer,FB.U8,4,false);

	if(FB.U32==0xFFFFFFFF) //第二个块最后一条记录数据空间没有数据,则数据指针在第二个块中
	{
		for(uint32_t i=0;i<(SOE_FLASH_LEN/SOE_INFO_LEN);i++)
		{
			Mx25_Read_U8_Array(SOE_START_ADDR2+i*SOE_INFO_LEN,FB.U8,4,false);

			if(FB.U32==0xFFFFFFFF)
				return (SOE_START_ADDR2+i*SOE_INFO_LEN);
		}
	}

	for(uint32_t i=0;i<SOE_SECTOR_NUM;i++) //当两个块的最后一条记录空间都满了的时候,就强制认为该写第一个块了
		Mx25_Erase_Sector((SOE_START_ADDR1/MX25_SECTOR_LEN)+i); //此处是极小概率有问题的,先这样处理

	return SOE_START_ADDR1;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
uint32_t Locate_Yc_Position(void)
{
	FOURBYTE FB;

	for(uint32_t i=0;i<((2*YC_SECTOR_NUM)/YC_FILE_SECTOR_NUM);i++)
	{
		Mx25_Read_U8_Array((YC_START_ADDR1+i*YC_FILE_LEN),FB.U8,4,false);

		if(i==0&&FB.U32==0xFFFFFFFF) //第一个文件无数据
			return YC_START_ADDR1;

		if(i==(((2*YC_SECTOR_NUM)/YC_FILE_SECTOR_NUM)-1)&&FB.U32!=0xFFFFFFFF) //最后一个文件有数据
			return YC_START_ADDR1+i*YC_FILE_LEN;

		if(FB.U32!=0xFFFFFFFF) //中间的文件有数据
			continue;

		return YC_START_ADDR1+(i-1)*YC_FILE_LEN;
	}

	for(uint32_t i=0;i<(2*YC_SECTOR_NUM);i++)   //防护,无用
		Mx25_Erase_Sector((YC_START_ADDR1/MX25_SECTOR_LEN)+i);

	return YC_START_ADDR1;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
uint32_t Locate_Fault_Position(void)
{
	FOURBYTE FB;
	uint32_t pointer;

	pointer=((FAULT_FLASH_LEN/FAULT_INFO_LEN)-1)*FAULT_INFO_LEN;  //块的最后一条记录数据空间相对地址(偏移)

	Mx25_Read_U8_Array(FAULT_START_ADDR1+pointer,FB.U8,4,false);

	if(FB.U32==0xFFFFFFFF) //第一个块最后一条记录数据空间没有数据,则数据指针在第一个块中
	{
		for(uint32_t i=0;i<(FAULT_FLASH_LEN/FAULT_INFO_LEN);i++)
		{
			Mx25_Read_U8_Array(FAULT_START_ADDR1+i*FAULT_INFO_LEN,FB.U8,4,false);

			if(FB.U32==0xFFFFFFFF)
				return (FAULT_START_ADDR1+i*FAULT_INFO_LEN);
		}
	}

	Mx25_Read_U8_Array(FAULT_START_ADDR2+pointer,FB.U8,4,false);

	if(FB.U32==0xFFFFFFFF) //第二个块最后一条记录数据空间没有数据,则数据指针在第二个块中
	{
		for(uint32_t i=0;i<(FAULT_FLASH_LEN/FAULT_INFO_LEN);i++)
		{
			Mx25_Read_U8_Array(FAULT_START_ADDR2+i*FAULT_INFO_LEN,FB.U8,4,false);

			if(FB.U32==0xFFFFFFFF)
				return (FAULT_START_ADDR2+i*FAULT_INFO_LEN);
		}
	}

	for(uint32_t i=0;i<FAULT_SECTOR_NUM;i++) //当两个块的最后一条记录空间都满了的时候,就强制认为该写第一个块了
		Mx25_Erase_Sector((FAULT_START_ADDR1/MX25_SECTOR_LEN)+i); //此处是极小概率有问题的,先这样处理

	return FAULT_START_ADDR1;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
uint32_t Locate_Yk_Position(void)
{
	FOURBYTE FB;
	uint32_t pointer;

	pointer=((YK_FLASH_LEN/YK_INFO_LEN)-1)*YK_INFO_LEN;  //块的最后一条记录数据空间相对地址(偏移)

	Mx25_Read_U8_Array(YK_START_ADDR1+pointer,FB.U8,4,false);

	if(FB.U32==0xFFFFFFFF) //第一个块最后一条记录数据空间没有数据,则数据指针在第一个块中
	{
		for(uint32_t i=0;i<(YK_FLASH_LEN/YK_INFO_LEN);i++)
		{
			Mx25_Read_U8_Array(YK_START_ADDR1+i*YK_INFO_LEN,FB.U8,4,false);

			if(FB.U32==0xFFFFFFFF)
				return (YK_START_ADDR1+i*YK_INFO_LEN);
		}
	}

	Mx25_Read_U8_Array(YK_START_ADDR2+pointer,FB.U8,4,false);

	if(FB.U32==0xFFFFFFFF) //第二个块最后一条记录数据空间没有数据,则数据指针在第二个块中
	{
		for(uint32_t i=0;i<(YK_FLASH_LEN/YK_INFO_LEN);i++)
		{
			Mx25_Read_U8_Array(YK_START_ADDR2+i*YK_INFO_LEN,FB.U8,4,false);

			if(FB.U32==0xFFFFFFFF)
				return (YK_START_ADDR2+i*YK_INFO_LEN);
		}
	}

	for(uint32_t i=0;i<(2*YK_SECTOR_NUM);i++) //当两个块的最后一条记录空间都满了的时候,就强制认为该写第一个块了
		Mx25_Erase_Sector((YK_START_ADDR1/MX25_SECTOR_LEN)+i); //此处是极小概率有问题的,先这样处理

	return YK_START_ADDR1;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
uint32_t Locate_Wave_Position(void)
{
	FOURBYTE FB;

	for(uint32_t i=0;i<((2*WAVE_FLASH_LEN)/WAVE_FILE_LEN);i++) //查找块1块2中波形文件首地址数据
	{
		Mx25_Read_U8_Array(WAVE_START_ADDR1+i*WAVE_FILE_LEN,FB.U8,4,false);

		if(i==0&&FB.U32==0xFFFFFFFF)
			return WAVE_START_ADDR1;

		if(FB.U32==0xFFFFFFFF)
			return (WAVE_START_ADDR1+(i-1)*WAVE_FILE_LEN);
	}

	for(uint32_t i=0;i<WAVE_SECTOR_NUM;i++) //强制擦除块1数据,指针指向块1首地址(极小概率)
		Mx25_Erase_Sector((WAVE_START_ADDR1/MX25_SECTOR_LEN)+i);

	return WAVE_START_ADDR1;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
bool Log_Data_Record(uint8_t type,char *ind, ...)
{
	char *para;
	va_list vl;
	uint32_t U32;
	LOGRECORD LOG;
	uint8_t strlens;
	uint8_t data[23];

	LOG.LENS=0;
	LOG.TYPE=type;

	vTaskSuspendAll();
	Wdog_Feed();

	NowTime2P101Time(LOG.TIME.DATA,7);

	if(((gSYSFLAG.MFW.LOG-LOG_START_ADDR1)/LOG_FLASH_LEN)==0) //指针在第一个块中
	{
		if((LOG_START_ADDR1+LOG_FLASH_LEN-gSYSFLAG.MFW.LOG)<LOG_INFO_LEN)
		{
			gSYSFLAG.MFW.LOG=LOG_START_ADDR2;

			for(uint32_t i=0;i<LOG_SECTOR_NUM;i++)
				Mx25_Erase_Sector((LOG_START_ADDR2/MX25_SECTOR_LEN)+i);
		}
	}

	if(((gSYSFLAG.MFW.LOG-LOG_START_ADDR1)/LOG_FLASH_LEN)==1) //指针在第二个块中
	{
		if((LOG_START_ADDR2+LOG_FLASH_LEN-gSYSFLAG.MFW.LOG)<LOG_INFO_LEN)
		{
			gSYSFLAG.MFW.LOG=LOG_START_ADDR1;

			for(uint32_t i=0;i<LOG_SECTOR_NUM;i++)
				Mx25_Erase_Sector((LOG_START_ADDR1/MX25_SECTOR_LEN)+i);
		}
	}

	va_start(vl,ind);

	while(*ind!='\0')
	{
		switch(*(ind++))
		{
			case 'E':  //2017-03-14 12:31:45.123
				U32=va_arg(vl,uint32_t);
				strlens=Get_P101_Time((uint8_t *)U32,data);

				if(Check_Log_Lens(strlens,LOG.LENS)==false)
				{
					va_end(vl);
					xTaskResumeAll();

					return false;
				}

				memcpy(LOG.ASCII+LOG.LENS,data,strlens);
				LOG.LENS+=strlens;
				break;

			case 'S':  //字符串
				para=va_arg(vl,char*);
				strlens=strlen(para);

				if(Check_Log_Lens(strlens,LOG.LENS)==false)
				{
					va_end(vl);
					xTaskResumeAll();

					return false;
				}

				strcpy(LOG.ASCII+LOG.LENS,para);
				LOG.LENS+=strlens;
				break;

			case 'H':  //十六进制
				U32=va_arg(vl,uint32_t);
				strlens=Get_U32_Hex(U32,data,11);

				if(Check_Log_Lens(strlens,LOG.LENS)==false)
				{
					va_end(vl);
					xTaskResumeAll();

					return false;
				}

				memcpy(LOG.ASCII+LOG.LENS,data,strlens);
				LOG.LENS+=strlens;
				break;

			case 'D':  //十进制
				U32=va_arg(vl,uint32_t);
				strlens=Get_U32_Dec(U32,data,10);

				if(Check_Log_Lens(10-strlens,LOG.LENS)==false)
				{
					va_end(vl);
					xTaskResumeAll();

					return false;
				}

				memcpy(LOG.ASCII+LOG.LENS,data+strlens,10-strlens);
				LOG.LENS+=10-strlens;
				break;

			case 'T': //2017-03-14
				U32=va_arg(vl,uint32_t);
				strlens=Get_U32_Time(U32,data,8);

				if(Check_Log_Lens(strlens,LOG.LENS)==false)
				{
					va_end(vl);
					xTaskResumeAll();

					return false;
				}

				memcpy(LOG.ASCII+LOG.LENS,data,strlens);
				LOG.LENS+=strlens;
				break;

			default :
				break;
		}
	}

	va_end(vl);

	strcpy(LOG.ASCII+LOG.LENS,"\r\n");

	LOG.LENS+=10; //1字节类型+7字节时间+2字节CRLF

	Mx25_Data_Storage(gSYSFLAG.MFW.LOG,LOG.DATA,LOG.LENS+1); //数据存储,LENS长度本身未包含自己的这一个字节大小

	gSYSFLAG.MFW.LOG+=LOG_INFO_LEN;

	#ifdef PRINT_MX25_LOG_DEBUG //打印日志信息
	DEBUG_PRINT_LOG(gSYSFLAG.MFW.LOG-LOG_INFO_LEN);
	#endif

	Wdog_Feed();
	xTaskResumeAll();

	return true;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
bool Check_Log_Lens(uint32_t lens,uint32_t size)
{
	if((size+lens)>128)
	{
		#ifdef PRINT_MX25_LOG_DEBUG //打印日志信息
		DEBUG_PRINT_NISR("KKNS","<----- 日志数组越界!!!!!!!!!!!!!!!!!!!!!! ----->\r\n\r\n");
		#endif

		return false;
	}

	return true;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Soe_Data_Record(ECPDATA_YX *YX,uint8_t soe,bool updatetime,uint8_t *soetime)
{
	P101SOE PS;

	PS.TYPE=30; //单点信息
	PS.CRLF=0x0A0D; //回车换行

	YX->MSG.U16=soe;
	PS.SOE=YX->MSG.U16;
	PS.MSGADDR.U16=YX->ADDR.U16;

	vTaskSuspendAll();
	Wdog_Feed();

	if(((gSYSFLAG.MFW.SOE-SOE_START_ADDR1)/SOE_FLASH_LEN)==0) //指针在第一个块中
	{
		if((SOE_START_ADDR1+SOE_FLASH_LEN-gSYSFLAG.MFW.SOE)<SOE_INFO_LEN) //第一块空间已满
		{
			gSYSFLAG.MFW.SOE=SOE_START_ADDR2; //切换至第二块

			for(uint32_t i=0;i<SOE_SECTOR_NUM;i++) //擦除第二块
				Mx25_Erase_Sector((SOE_START_ADDR2/MX25_SECTOR_LEN)+i);
		}
	}

	if(((gSYSFLAG.MFW.SOE-SOE_START_ADDR1)/SOE_FLASH_LEN)==1) //指针在第二个块中
	{
		if((SOE_START_ADDR2+SOE_FLASH_LEN-gSYSFLAG.MFW.SOE)<SOE_INFO_LEN) //第二块空间已满
		{
			gSYSFLAG.MFW.SOE=SOE_START_ADDR1; //切换至第一块

			for(uint32_t i=0;i<SOE_SECTOR_NUM;i++) ////擦除第一块
				Mx25_Erase_Sector((SOE_START_ADDR1/MX25_SECTOR_LEN)+i);
		}
	}

	if(updatetime==false) //更新系统时间
		memcpy(PS.TIME,soetime,7);
	else
		NowTime2P101Time(PS.TIME,7);

	Mx25_Data_Storage(gSYSFLAG.MFW.SOE,PS.DATA,SOE_INFO_LEN);
	gSYSFLAG.MFW.SOE+=SOE_INFO_LEN;

	Wdog_Feed();
	xTaskResumeAll();

	Add_P101SoeQueue(&gSOEQUEUE,&PS);

	#ifdef PRINT_MX25_SOE_DEBUG //打印SOE信息
	DEBUG_PRINT_SOE(gSYSFLAG.MFW.SOE-SOE_INFO_LEN);
	#endif
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
uint8_t Yc_Data_Record(void)
{
	FOURBYTE FB;

	if(((gUNIXTIME.UNIX-10)%900)!=0)
		return false;

	if(gUNIXTIME.UNIX==gSYSFLAG.STM.YC_RECORD_TIME)
		return false;

	gSYSFLAG.STM.YC_RECORD_TIME=gUNIXTIME.UNIX;

//	if(gYCFILE.TIMEOUT==0) //无遥测数据上报
//		return false;

//	if((gUNIXTIME.UNIX-gYCFILE.TIMEOUT)<5) //超时120秒,认为是下一组遥测数据
//		return false;

	gYCFILE.TIMEOUT+=28800; //加上东八区时差8个小时

	//gYCFILE.SN=((gYCFILE.TIMEOUT%(15*YC_INFO_NUM))/15)+1;
	gYCFILE.SN=((gUNIXTIME.UNIX%86400)/900)+1; //86400:24小时  //900:15分钟

	gYCFILE.N=gSYSPARA.LINEGROUP.LINENUM*9;  //统计该节定点数据个数
	gYCFILE.LENS=11+(5*gYCFILE.N)+2;
	gYCFILE.DATA[11+(5*gYCFILE.N)+1]=0x0D;
	gYCFILE.DATA[11+(5*gYCFILE.N)+2]=0x0A;

	for(uint8_t i=0;i<YC_INFO_NUM;i++) //获得有效的定点数据文件头时间
	{
		Mx25_Read_U8_Array(gSYSFLAG.MFW.YC+i*YC_INFO_LEN+8,FB.U8,4,false); //+8:读取节数据的年月日的时间位置

		if(FB.U32==0xFFFFFFFF||FB.U32!=0x00000000)
			break;
	}

	gYCFILE.OLDTIME=((FB.U32&0xFFFF1F00)>>8);
	gYCFILE.NEWTIME=(((uint32_t)gYCFILE.TIME.YEAR)<<16)+(((uint32_t)gYCFILE.TIME.MONTH)<<8)+(((uint32_t)gYCFILE.TIME.DAY.DAY)<<0);

	if(gYCFILE.NEWTIME!=gYCFILE.OLDTIME) //当前指针的文件头时间与当前遥测时间不匹配
	{
		if(gYCFILE.OLDTIME!=0x00FFFF1F) //当前文件有文件头
		{
			#ifdef PRINT_YC_FILE_DEBUG //打印完整的遥测文件
			DEBUG_PRINT_YC_FILE(gSYSFLAG.MFW.YC);
			#endif

			Switch_YaoCeFile();
		}

		#ifdef PRINT_YC_FILE_DEBUG //打印文件信息
		uint32_t sn;
		sn=((gSYSFLAG.MFW.YC-YC_START_ADDR1)/YC_FILE_LEN)+1;
		if(sn!=1)
			DEBUG_PRINT("NSDSTSDSTS","新的一天,已由第 ",sn-1," 号遥测文件(",gYCFILE.OLDTIME,"),已切换至第 ",sn," 号遥测文件(",gYCFILE.NEWTIME,")!\r\n");
		#endif
	}

	Mx25_Read_U8_Array(gSYSFLAG.MFW.YC+(gYCFILE.SN-1)*YC_INFO_LEN,FB.U8,4,false);

	if(FB.U32!=0xFFFFFFFF) //防重写,切换文件
	{
		#ifdef PRINT_YC_FILE_DEBUG
		DEBUG_PRINT("NSDSDS","<----- 当前第 ",((gSYSFLAG.MFW.YC-YC_START_ADDR1)/YC_FILE_LEN)+1," 号遥测文件的第 ",gYCFILE.SN," 定点数据已重复,已切换至下一文件!\r\n");
		DEBUG_PRINT_YC_FILE(gSYSFLAG.MFW.YC);
		#endif

		Switch_YaoCeFile();
	}

	Mx25_Data_Storage(gSYSFLAG.MFW.YC+(gYCFILE.SN-1)*YC_INFO_LEN,gYCFILE.DATA,YC_INFO_LEN);

	Fill_YaoCeFile(gYCFILE.SN,gYCFILE.N);

	#ifdef PRINT_YC_FILE_DEBUG //打印当前遥测文件节信息
	DEBUG_PRINT_YC_FILE_SECTION(gSYSFLAG.MFW.YC,gYCFILE.SN);
	#endif

	YaoCeFile_Init(&gYCFILE);

	return true;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Fill_YaoCeFile(uint8_t sn,uint8_t n)
{
	FOURBYTE FB;
	YAOCEFILE YCF;

	YaoCeFile_Init(&YCF);

	YCF.N=n;
	YCF.LENS=11+(5*YCF.N)+2;
	YCF.DATA[11+(5*YCF.N)+1]=0x0D;
	YCF.DATA[11+(5*YCF.N)+2]=0x0A;

	for(uint32_t i=0;i<sn;i++)
	{
		Mx25_Read_U8_Array(gSYSFLAG.MFW.YC+i*YC_INFO_LEN,FB.U8,4,false);

		if(FB.U32==0xFFFFFFFF)
		{
			YCF.SN=i+1;
			//YCF.TIME.DATA          //空的定点数据时间未作处理,暂为0
			Mx25_Data_Storage(gSYSFLAG.MFW.YC+i*YC_INFO_LEN,YCF.DATA,YC_INFO_LEN);
		}
	}
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Switch_YaoCeFile(void)
{
	gSYSFLAG.MFW.YC+=YC_FILE_LEN;

	if(gSYSFLAG.MFW.YC==(YC_START_ADDR1+YC_FLASH_LEN))
	{
		for(uint32_t i=0;i<YC_SECTOR_NUM;i++)
			Mx25_Erase_Sector((YC_START_ADDR2/MX25_SECTOR_LEN)+i);
	}

	if(gSYSFLAG.MFW.YC==(YC_START_ADDR2+YC_FLASH_LEN))
	{
		gSYSFLAG.MFW.YC=YC_START_ADDR1;

		for(uint32_t i=0;i<YC_SECTOR_NUM;i++)
			Mx25_Erase_Sector((YC_START_ADDR1/MX25_SECTOR_LEN)+i);
	}
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Yk_Data_Record(P101YK YK)
{
	YK.CRLF=0x0A0D; //回车换行
	NowTime2P101Time(YK.TIME,7);

	if(((gSYSFLAG.MFW.YK-YK_START_ADDR1)/YK_FLASH_LEN)==0) //指针在第一个块中
	{
		if((YK_START_ADDR1+YK_FLASH_LEN-gSYSFLAG.MFW.YK)<YK_INFO_LEN) //当第一块空间已满
		{
			gSYSFLAG.MFW.YK=YK_START_ADDR2; //切换至第二块中

			for(uint32_t i=0;i<YK_SECTOR_NUM;i++) //擦除第二块
				Mx25_Erase_Sector(((YK_START_ADDR2/MX25_SECTOR_LEN)+i));
		}
	}

	if(((gSYSFLAG.MFW.YK-YK_START_ADDR1)/YK_FLASH_LEN)==1) //指针在第二个块中
	{
		if((YK_START_ADDR2+YK_FLASH_LEN-gSYSFLAG.MFW.YK)<YK_INFO_LEN) //当第二块空间已满
		{
			gSYSFLAG.MFW.YK=YK_START_ADDR1; //切换至第一块中

			for(uint32_t i=0;i<YK_SECTOR_NUM;i++) //擦除第一块
				Mx25_Erase_Sector(((YK_START_ADDR1/MX25_SECTOR_LEN)+i));
		}
	}

	Mx25_Data_Storage(gSYSFLAG.MFW.YK,YK.DATA,YK_INFO_LEN);
	gSYSFLAG.MFW.YK+=YK_INFO_LEN;

	#ifdef PRINT_MX25_YK_DEBUG //打印YK信息
	DEBUG_PRINT_YK(gSYSFLAG.MFW.YK-YK_INFO_LEN);
	#endif
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
void Fault_Data_Record(uint8_t fault,uint8_t *wavetime,uint16_t addr)
{
	UNIXTIME UT;
	P101FAULT PF;

	PF.CRLF=0x0A0D;
	memcpy(UT.DATA,wavetime,8);
	UnixTime2P101Time(&UT,PF.TIME,7);

	if(((gSYSFLAG.MFW.FAULT-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==0) //指针在第一个块中
	{
		if((FAULT_START_ADDR1+FAULT_FLASH_LEN-gSYSFLAG.MFW.FAULT)<FAULT_INFO_LEN) //当第一块空间已满
		{
			gSYSFLAG.MFW.FAULT=FAULT_START_ADDR2; //切换至第2块中

			for(uint32_t i=0;i<FAULT_SECTOR_NUM;i++)
				Mx25_Erase_Sector((FAULT_START_ADDR2/MX25_SECTOR_LEN)+i);
		}
	}

	if(((gSYSFLAG.MFW.FAULT-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==1) //指针在第二个块中
	{
		if((FAULT_START_ADDR2+FAULT_FLASH_LEN-gSYSFLAG.MFW.FAULT)<FAULT_INFO_LEN) //当第二块空间已满
		{
			gSYSFLAG.MFW.FAULT=FAULT_START_ADDR1; //切换至第1块中

			for(uint32_t i=0;i<FAULT_SECTOR_NUM;i++)
				Mx25_Erase_Sector((FAULT_START_ADDR1/MX25_SECTOR_LEN)+i);
		}
	}

	Mx25_Data_Storage(gSYSFLAG.MFW.FAULT,PF.DATA,FAULT_INFO_LEN);

	gSYSFLAG.MFW.FAULT+=FAULT_INFO_LEN;

	Add_P101FaultQueue(&gFAULTQUEUE,&PF);

	#ifdef PRINT_MX25_FAULT_DEBUG
	DEBUG_PRINT_FAULT(gSYSFLAG.MFW.FAULT-FAULT_INFO_LEN);
	#endif
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: 函数运行状态
*************************************************************/
void Wave_Data_Record(uint8_t *data,uint32_t lens)
{
	Mx25_Data_Storage(gSYSFLAG.MFW.WAVE,data,lens);

	gSYSFLAG.MFW.WAVE+=lens;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
uint32_t Switch_NextWave_Position(void)
{
	FOURBYTE FB;

	gSYSFLAG.MFW.WAVE=gSYSFLAG.MFW.WAVE-((gSYSFLAG.MFW.WAVE-WAVE_START_ADDR1)%WAVE_FILE_LEN);

	Mx25_Read_U8_Array(gSYSFLAG.MFW.WAVE,FB.U8,4,false);

	if(FB.U32==0xFFFFFFFF)
		return gSYSFLAG.MFW.WAVE;

	gSYSFLAG.MFW.WAVE+=WAVE_FILE_LEN;

	if(gSYSFLAG.MFW.WAVE==(WAVE_START_ADDR1+WAVE_FLASH_LEN))
	{
		gSYSFLAG.MFW.WAVE=WAVE_START_ADDR2; //强迫癌

		for(uint32_t i=0;i<WAVE_SECTOR_NUM;i++)
			Mx25_Erase_Sector((WAVE_START_ADDR2/MX25_SECTOR_LEN)+i);
	}

	if(gSYSFLAG.MFW.WAVE==(WAVE_START_ADDR2+WAVE_FLASH_LEN))
	{
		gSYSFLAG.MFW.WAVE=WAVE_START_ADDR1;

		for(uint32_t i=0;i<WAVE_SECTOR_NUM;i++)
			Mx25_Erase_Sector((WAVE_START_ADDR1/MX25_SECTOR_LEN)+i);
	}

	return gSYSFLAG.MFW.WAVE;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Locate_Log_FileInfo(FILEINFO *FI)
{
	FOURBYTE FB;
	LOGRECORD LOG;

	FI->MSGADDR.U16=0x6807;

	if(((gSYSFLAG.MFW.LOG-LOG_START_ADDR1)/LOG_FLASH_LEN)==0) //写指针在块1
	{
		FI->INFONUM=((gSYSFLAG.MFW.LOG-LOG_START_ADDR1)/LOG_INFO_LEN);

		if(FI->INFONUM>=READ_LOG_NUM) //截取最新的n条记录数据
		{
			FI->READ=LOG_START_ADDR1+((FI->INFONUM-READ_LOG_NUM)*LOG_INFO_LEN);

			for(uint32_t i=0;i<READ_LOG_NUM;i++)
			{
				Mx25_Read_U8_Array(FI->READ+i*LOG_INFO_LEN,FB.U8,4,false);

				if(FB.U32==0xFFFFFFFF) //防护,还需调整
					continue;

				FI->LENS+=FB.U8[0];
			}

			FI->INFONUM=READ_LOG_NUM;
		}

		if(FI->INFONUM<READ_LOG_NUM)
		{
			Mx25_Read_U8_Array(LOG_START_ADDR2,FB.U8,4,false); //判断块2是否有数据

			if(FB.U32==0xFFFFFFFF) //块2无数据
			{
				FI->READ=LOG_START_ADDR1;

				for(uint32_t i=0;i<FI->INFONUM;i++)
				{
					Mx25_Read_U8_Array(LOG_START_ADDR1+i*LOG_INFO_LEN,FB.U8,4,false);

					if(FB.U32==0xFFFFFFFF) //防护,还需调整
						continue;

					FI->LENS+=FB.U8[0];
				}
			}
			else
			{
				for(uint32_t i=0;i<FI->INFONUM;i++) //获取块1中数据长度
				{
					Mx25_Read_U8_Array(LOG_START_ADDR1+i*LOG_INFO_LEN,FB.U8,4,false);

					if(FB.U32==0xFFFFFFFF) //防护,还需调整
						continue;

					FI->LENS+=FB.U8[0];
				}

				FI->INFONUM=READ_LOG_NUM-FI->INFONUM;

				FI->READ=LOG_START_ADDR2+(((LOG_FLASH_LEN/LOG_INFO_LEN)-FI->INFONUM)*LOG_INFO_LEN);

				for(uint32_t i=0;i<FI->INFONUM;i++) //获取块2中数据长度
				{
					Mx25_Read_U8_Array(FI->READ+i*LOG_INFO_LEN,FB.U8,4,false);

					if(FB.U32==0xFFFFFFFF) //防护,还需调整
						continue;

					FI->LENS+=FB.U8[0];
				}

				FI->INFONUM=READ_LOG_NUM;
			}
		}
	}

	if(((gSYSFLAG.MFW.LOG-LOG_START_ADDR1)/LOG_FLASH_LEN)==1) //写指针在块2
	{
		FI->INFONUM=((gSYSFLAG.MFW.LOG-LOG_START_ADDR2)/LOG_INFO_LEN);

		if(FI->INFONUM>=READ_LOG_NUM) //截取最新的n条记录数据
		{
			FI->READ=LOG_START_ADDR2+((FI->INFONUM-READ_LOG_NUM)*LOG_INFO_LEN);

			for(uint32_t i=0;i<READ_LOG_NUM;i++)
			{
				Mx25_Read_U8_Array(FI->READ+i*LOG_INFO_LEN,FB.U8,4,false);

				if(FB.U32==0xFFFFFFFF) //防护,还需调整
					continue;

				FI->LENS+=FB.U8[0];
			}

			FI->INFONUM=READ_LOG_NUM;
		}

		if(FI->INFONUM<READ_LOG_NUM)
		{
			Mx25_Read_U8_Array(LOG_START_ADDR1,FB.U8,4,false); //判断块1是否有数据

			if(FB.U32==0xFFFFFFFF) //块1无数据
			{
				FI->READ=LOG_START_ADDR2;

				for(uint32_t i=0;i<FI->INFONUM;i++)
				{
					Mx25_Read_U8_Array(LOG_START_ADDR2+i*LOG_INFO_LEN,FB.U8,4,false);

					if(FB.U32==0xFFFFFFFF) //防护,还需调整
						continue;

					FI->LENS+=FB.U8[0];
				}
			}
			else
			{
				for(uint32_t i=0;i<FI->INFONUM;i++) //获取块2中数据长度
				{
					Mx25_Read_U8_Array(LOG_START_ADDR2+i*LOG_INFO_LEN,FB.U8,4,false);

					if(FB.U32==0xFFFFFFFF) //防护,还需调整
						continue;

					FI->LENS+=FB.U8[0];
				}

				FI->INFONUM=READ_LOG_NUM-FI->INFONUM;

				FI->READ=LOG_START_ADDR1+(((LOG_FLASH_LEN/LOG_INFO_LEN)-FI->INFONUM)*LOG_INFO_LEN);

				for(uint32_t i=0;i<FI->INFONUM;i++) //获取块1中数据长度
				{
					Mx25_Read_U8_Array(FI->READ+i*LOG_INFO_LEN,FB.U8,4,false);

					if(FB.U32==0xFFFFFFFF) //防护,还需调整
						continue;

					FI->LENS+=FB.U8[0];
				}

				FI->INFONUM=READ_LOG_NUM;
			}
		}
	}

	Mx25_Read_U8_Array(FI->READ,LOG.DATA,LOG_INFO_LEN,false);
	memcpy(FI->TIME,LOG.TIME.DATA,7);

	if(FI->INFONUM!=0)
		FI->LENS+=LOG_INFO_HEAD+1;
	else
		FI->LENS=0;

	FI->SENDNUM=FI->INFONUM;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
bool Locate_Soe_FileInfo(FILEINFO *FI)
{
	P101SOE PS;
	FOURBYTE FB;

	FI->MSGADDR.U16=0x6808;

	if(((gSYSFLAG.MFW.SOE-SOE_START_ADDR1)/SOE_FLASH_LEN)==0) //写指针在块1
	{
		FI->INFONUM=((gSYSFLAG.MFW.SOE-SOE_START_ADDR1)/SOE_INFO_LEN);

		if(FI->INFONUM>=READ_SOE_NUM) //截取最新的n条记录数据
		{
			FI->READ=SOE_START_ADDR1+((FI->INFONUM-READ_SOE_NUM)*SOE_INFO_LEN);

			FI->INFONUM=READ_SOE_NUM;
			FI->LENS+=READ_SOE_NUM*SOE_INFO_LEN;
		}

		if(FI->INFONUM<READ_SOE_NUM)
		{
			Mx25_Read_U8_Array(SOE_START_ADDR2,FB.U8,4,false);//判断块2是否有数据

			if(FB.U32==0xFFFFFFFF) //块2无数据
			{
				FI->READ=SOE_START_ADDR1;
				FI->LENS+=FI->INFONUM*SOE_INFO_LEN;
			}

			if(FB.U32!=0xFFFFFFFF)
			{
				FI->INFONUM=READ_SOE_NUM-FI->INFONUM;

				FI->READ=SOE_START_ADDR2+(((SOE_FLASH_LEN/SOE_INFO_LEN)-FI->INFONUM)*SOE_INFO_LEN);

				FI->INFONUM=READ_SOE_NUM;
				FI->LENS+=READ_SOE_NUM*SOE_INFO_LEN;
			}
		}
	}

	if(((gSYSFLAG.MFW.SOE-SOE_START_ADDR1)/SOE_FLASH_LEN)==1) //写指针在块1
	{
		FI->INFONUM=((gSYSFLAG.MFW.SOE-SOE_START_ADDR2)/SOE_INFO_LEN);

		if(FI->INFONUM>=READ_SOE_NUM) //截取最新的n条记录数据
		{
			FI->READ=SOE_START_ADDR2+((FI->INFONUM-READ_SOE_NUM)*SOE_INFO_LEN);

			FI->INFONUM=READ_SOE_NUM;
			FI->LENS+=READ_SOE_NUM*SOE_INFO_LEN;
		}

		if(FI->INFONUM<READ_SOE_NUM)
		{
			Mx25_Read_U8_Array(SOE_START_ADDR1,FB.U8,4,false);//判断块2是否有数据

			if(FB.U32==0xFFFFFFFF) //块2无数据
			{
				FI->READ=SOE_START_ADDR2;
				FI->LENS+=FI->INFONUM*SOE_INFO_LEN;
			}
			else
			{
				FI->INFONUM=READ_SOE_NUM-FI->INFONUM;

				FI->READ=SOE_START_ADDR1+(((SOE_FLASH_LEN/SOE_INFO_LEN)-FI->INFONUM)*SOE_INFO_LEN);

				FI->INFONUM=READ_SOE_NUM;
				FI->LENS+=READ_SOE_NUM*SOE_INFO_LEN;
			}
		}
	}

	Mx25_Read_U8_Array(FI->READ,PS.DATA,SOE_INFO_LEN,false);
	memcpy(FI->TIME,PS.TIME,7);

	if(FI->INFONUM==0)
		FI->LENS=0;
	else
	{
		if((FI->LENS%ECP_DATA_SEND_LEN)==0)
			FI->SENDNUM=FI->LENS/ECP_DATA_SEND_LEN;
		else
			FI->SENDNUM=(FI->LENS/ECP_DATA_SEND_LEN)+1;

		FI->LENS+=SOE_INFO_HEAD+1;
	}

	return true;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
bool Locate_Yc_FileInfo(FILEINFO *FI)
{
	FOURBYTE FB;
	YAOCEFILE YCF;
	uint8_t sn,block11,block12,block21,block22;

	FI->MSGADDR.U16=0x6806;

	if(((gSYSFLAG.MFW.YC-YC_START_ADDR1)/YC_FLASH_LEN)==0) //写指针在块1
	{
		sn=((gSYSFLAG.MFW.YC-YC_START_ADDR1)/YC_FILE_LEN)+1;

		if(sn==30)
		{
			FI->READ=YC_START_ADDR1+(FI->ID-22)*YC_FILE_LEN;
		}
		else
		{
			block11=51-sn+1;
			block12=51;
			block21=22;
			block22=51-sn;

			if(FI->ID>=block11&&FI->ID<=block12)
			{
				FI->READ=YC_START_ADDR1+(FI->ID-block11)*YC_FILE_LEN;
			}

			if(FI->ID>=block21&&FI->ID<=block22)
			{
				FI->READ=YC_START_ADDR2+(FI->ID-22+sn)*YC_FILE_LEN;
			}
		}
	}

	if(((gSYSFLAG.MFW.YC-YC_START_ADDR1)/YC_FLASH_LEN)==1) //写指针在块2
	{
		sn=((gSYSFLAG.MFW.YC-YC_START_ADDR2)/YC_FILE_LEN)+1;

		if(sn==30)
		{
			FI->READ=YC_START_ADDR2+(FI->ID-22)*YC_FILE_LEN;
		}
		else
		{
			block11=51-sn+1;
			block12=51;
			block21=22;
			block22=51-sn;

			if(FI->ID>=block11&&FI->ID<=block12)
			{
				FI->READ=YC_START_ADDR2+(FI->ID-block11)*YC_FILE_LEN;
			}

			if(FI->ID>=block21&&FI->ID<=block22)
			{
				FI->READ=YC_START_ADDR1+(FI->ID-22+sn)*YC_FILE_LEN;
			}
		}
	}

	for(uint32_t i=0;i<YC_INFO_NUM;i++)
	{
		Mx25_Read_U8_Array(FI->READ+i*YC_INFO_LEN,FB.U8,4,false);

		if(FB.U32!=0xFFFFFFFF)
			break;

		if(i==(YC_INFO_NUM-1))
		{
			FI->LENS=0;
			FI->INFONUM=0;
			memset(FI->TIME,0,7);

			return false;
		}
	}

	for(uint32_t i=0;i<YC_INFO_NUM;i++) //获得有效的定点数据文件头时间
	{
		Mx25_Read_U8_Array(gSYSFLAG.MFW.YC+i*YC_INFO_LEN,YCF.DATA,YC_INFO_LEN+1,false); //+4:读取节数据的年月日的时间位置

		if(YCF.TIME.YEAR!=0)
		{
			memcpy(FI->TIME,YCF.TIME.DATA,7);
			break;
		}
	}

	for(uint8_t i=0;i<YC_INFO_NUM;i++)
	{
		Mx25_Read_U8_Array(FI->READ+i*YC_INFO_LEN,YCF.DATA,YC_INFO_LEN+1,false);

		if(YCF.LENS==0xFF)
			continue;

		FI->INFONUM++;
		FI->LENS+=YCF.LENS;
	}

	FI->SENDNUM=FI->INFONUM;
	FI->LENS+=YC_FILEHEAD_LEN+1;

	return true;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
bool Locate_Fault_FileInfo(FILEINFO *FI)
{
	FOURBYTE FB;
	P101FAULT PF;

	FI->MSGADDR.U16=0x680B;

	if(((gSYSFLAG.MFW.FAULT-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==0) //写指针在块1
	{
		FI->INFONUM=((gSYSFLAG.MFW.FAULT-FAULT_START_ADDR1)/FAULT_INFO_LEN);

		if(FI->INFONUM>=READ_FAULT_NUM) //截取最新的n条记录数据
		{
			FI->READ=FAULT_START_ADDR1+((FI->INFONUM-READ_FAULT_NUM)*FAULT_INFO_LEN);

			FI->INFONUM=READ_FAULT_NUM;
			FI->LENS+=READ_FAULT_NUM*FAULT_INFO_LEN;
		}

		if(FI->INFONUM<READ_FAULT_NUM)
		{
			Mx25_Read_U8_Array(FAULT_START_ADDR2,FB.U8,4,false); //判断块2是否有数据

			if(FB.U32==0xFFFFFFFF) //块2无数据
			{
				FI->READ=FAULT_START_ADDR1;
				FI->LENS+=FI->INFONUM*FAULT_INFO_LEN;
			}

			if(FB.U32!=0xFFFFFFFF)
			{
				FI->INFONUM=READ_FAULT_NUM-FI->INFONUM;

				FI->READ=FAULT_START_ADDR2+(((FAULT_FLASH_LEN/FAULT_INFO_LEN)-FI->INFONUM)*FAULT_INFO_LEN);

				FI->INFONUM=READ_FAULT_NUM;
				FI->LENS+=READ_FAULT_NUM*FAULT_INFO_LEN;
			}
		}
	}

	if(((gSYSFLAG.MFW.FAULT-FAULT_START_ADDR1)/FAULT_FLASH_LEN)==1) //写指针在块1
	{
		FI->INFONUM=((gSYSFLAG.MFW.FAULT-FAULT_START_ADDR2)/FAULT_INFO_LEN);

		if(FI->INFONUM>=READ_FAULT_NUM) //截取最新的n条记录数据
		{
			FI->READ=FAULT_START_ADDR2+((FI->INFONUM-READ_FAULT_NUM)*FAULT_INFO_LEN);

			FI->INFONUM=READ_FAULT_NUM;
			FI->LENS+=READ_FAULT_NUM*FAULT_INFO_LEN;
		}

		if(FI->INFONUM<READ_FAULT_NUM)
		{
			Mx25_Read_U8_Array(FAULT_START_ADDR1,FB.U8,4,false); //判断块2是否有数据

			if(FB.U32==0xFFFFFFFF) //块2无数据
			{
				FI->READ=FAULT_START_ADDR2;
				FI->LENS+=FI->INFONUM*FAULT_INFO_LEN;
			}
			else
			{
				FI->INFONUM=READ_FAULT_NUM-FI->INFONUM;

				FI->READ=FAULT_START_ADDR1+(((FAULT_FLASH_LEN/FAULT_INFO_LEN)-FI->INFONUM)*FAULT_INFO_LEN);

				FI->INFONUM=READ_FAULT_NUM;
				FI->LENS+=READ_FAULT_NUM*FAULT_INFO_LEN;
			}
		}
	}

	Mx25_Read_U8_Array(FI->READ,PF.DATA,FAULT_INFO_LEN,false);
	memcpy(FI->TIME,PF.TIME,7);

	if(FI->INFONUM==0)
		FI->LENS=0;
	else
	{
		if((FI->LENS%ECP_DATA_SEND_LEN)==0)
			FI->SENDNUM=FI->LENS/ECP_DATA_SEND_LEN;
		else
			FI->SENDNUM=(FI->LENS/ECP_DATA_SEND_LEN)+1;

		FI->LENS+=FAULT_INFO_HEAD+1;
	}

	return true;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
bool Locate_Yk_FileInfo(FILEINFO *FI)
{
	P101YK YK;
	FOURBYTE FB;

	FI->MSGADDR.U16=0x6809;

	if(((gSYSFLAG.MFW.YK-YK_START_ADDR1)/YK_FLASH_LEN)==0) //写指针在块1
	{
		FI->INFONUM=((gSYSFLAG.MFW.YK-YK_START_ADDR1)/YK_INFO_LEN);

		if(FI->INFONUM>=READ_YK_NUM) //截取最新的n条记录数据
		{
			FI->READ=YK_START_ADDR1+((FI->INFONUM-READ_YK_NUM)*YK_INFO_LEN);

			FI->INFONUM=READ_YK_NUM;
			FI->LENS+=READ_YK_NUM*YK_INFO_LEN;
		}

		if(FI->INFONUM<READ_YK_NUM)
		{
			Mx25_Read_U8_Array(YK_START_ADDR2,FB.U8,4,false); //判断块2是否有数据

			if(FB.U32==0xFFFFFFFF) //块2无数据
			{
				FI->READ=YK_START_ADDR1;
				FI->LENS+=FI->INFONUM*YK_INFO_LEN;
			}

			if(FB.U32!=0xFFFFFFFF)
			{
				FI->INFONUM=READ_YK_NUM-FI->INFONUM;

				FI->READ=YK_START_ADDR2+(((YK_FLASH_LEN/YK_INFO_LEN)-FI->INFONUM)*YK_INFO_LEN);

				FI->INFONUM=READ_YK_NUM;
				FI->LENS+=READ_YK_NUM*YK_INFO_LEN;
			}
		}
	}

	if(((gSYSFLAG.MFW.YK-YK_START_ADDR1)/YK_FLASH_LEN)==1) //写指针在块1
	{
		FI->INFONUM=((gSYSFLAG.MFW.YK-YK_START_ADDR2)/YK_INFO_LEN);

		if(FI->INFONUM>=READ_YK_NUM) //截取最新的n条记录数据
		{
			FI->READ=YK_START_ADDR2+((FI->INFONUM-READ_YK_NUM)*YK_INFO_LEN);

			FI->INFONUM=READ_YK_NUM;
			FI->LENS+=READ_YK_NUM*YK_INFO_LEN;
		}

		if(FI->INFONUM<READ_YK_NUM)
		{
			Mx25_Read_U8_Array(YK_START_ADDR1,FB.U8,4,false); //判断块2是否有数据

			if(FB.U32==0xFFFFFFFF) //块2无数据
			{
				FI->READ=YK_START_ADDR2;
				FI->LENS+=FI->INFONUM*YK_INFO_LEN;
			}
			else
			{
				FI->INFONUM=READ_YK_NUM-FI->INFONUM;

				FI->READ=YK_START_ADDR1+(((YK_FLASH_LEN/YK_INFO_LEN)-FI->INFONUM)*YK_INFO_LEN);

				FI->INFONUM=READ_YK_NUM;
				FI->LENS+=READ_YK_NUM*YK_INFO_LEN;
			}
		}
	}

	Mx25_Read_U8_Array(FI->READ,YK.DATA,YK_INFO_LEN,false);
	memcpy(FI->TIME,YK.TIME,7);

	if(FI->INFONUM==0)
		FI->LENS=0;
	else
	{
		if((FI->LENS%ECP_DATA_SEND_LEN)==0)
			FI->SENDNUM=FI->LENS/ECP_DATA_SEND_LEN;
		else
			FI->SENDNUM=(FI->LENS/ECP_DATA_SEND_LEN)+1;

		FI->LENS+=YK_INFO_HEAD+1;
	}

	return true;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Locate_Wave_FileInfo(FILEINFO *FI)
{
	FOURBYTE FB;
	DOUBLEBYTE DB;

	uint8_t sn,block11,block12,block21,block22;

	FI->INFONUM=0;
	FI->MSGADDR.U16=0x680A;

	if(((gSYSFLAG.MFW.WAVE-WAVE_START_ADDR1)/WAVE_FLASH_LEN)==0) //写指针在块1
	{
		sn=((gSYSFLAG.MFW.WAVE-WAVE_START_ADDR1)/WAVE_FILE_LEN)+1;

		if(sn==64)
		{
			FI->READ=WAVE_START_ADDR1+(FI->ID-52)*WAVE_FILE_LEN;
		}
		else
		{
			block11=115-sn+1;
			block12=115;
			block21=52;
			block22=115-sn;

			if(FI->ID>=block11&&FI->ID<=block12)
			{
				FI->READ=WAVE_START_ADDR1+(FI->ID-block11)*WAVE_FILE_LEN;
			}

			if(FI->ID>=block21&&FI->ID<=block22)
			{
				FI->READ=WAVE_START_ADDR2+(FI->ID-52+sn)*WAVE_FILE_LEN;
			}
		}
	}

	if(((gSYSFLAG.MFW.WAVE-WAVE_START_ADDR1)/WAVE_FLASH_LEN)==1) //写指针在块2
	{
		sn=((gSYSFLAG.MFW.WAVE-WAVE_START_ADDR2)/WAVE_FILE_LEN)+1;

		if(sn==64)
		{
			FI->READ=WAVE_START_ADDR2+(FI->ID-52)*WAVE_FILE_LEN;
		}
		else
		{
			block11=115-sn+1;
			block12=115;
			block21=52;
			block22=115-sn;

			if(FI->ID>=block11&&FI->ID<=block12)
			{
				FI->READ=WAVE_START_ADDR2+(FI->ID-block11)*WAVE_FILE_LEN;
			}

			if(FI->ID>=block21&&FI->ID<=block22)
			{
				FI->READ=WAVE_START_ADDR1+(FI->ID-52+sn)*WAVE_FILE_LEN;
			}
		}
	}

	Mx25_Read_U8_Array(FI->READ,DB.U8,2,false);

	if(DB.U16==0xFFFF)
	{
		FI->LENS=0;
		FI->INFONUM=0;
		memset(FI->TIME,0,7);
	}
	else
	{
		for(uint32_t i=0;i<MAX_WAVE_LEN;i++)
		{
			Mx25_Read_U8_Array(FI->READ+9+DB.U16+i*WAVE_INFO_LEN,FB.U8,4,false);

			if(FB.U32==0xFFFFFFFF)
				break;

			FI->INFONUM++;
		}

		FI->LENS=DB.U16+FI->INFONUM*WAVE_INFO_LEN+1;

		Mx25_Read_U8_Array(FI->READ+2,FI->TIME,7,false);

		if((FI->LENS-DB.U16)%ECP_DATA_SEND_LEN==0)
			FI->SENDNUM=(FI->LENS-DB.U16)/ECP_DATA_SEND_LEN;
		else
			FI->SENDNUM=((FI->LENS-DB.U16)/ECP_DATA_SEND_LEN)+1;
	}
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Get_Log_Data(uint32_t read,uint8_t *data,uint32_t lens)
{
	if(((read-LOG_START_ADDR1)/LOG_FLASH_LEN)==0) //块1
	{
		if((LOG_START_ADDR1+LOG_FLASH_LEN-read)<LOG_INFO_LEN)
			read=LOG_START_ADDR2;
	}

	if(((read-LOG_START_ADDR1)/LOG_FLASH_LEN)==1)
	{
		if((LOG_START_ADDR2+LOG_FLASH_LEN-read)<LOG_INFO_LEN)
			read=LOG_START_ADDR1;
	}

	Mx25_Read_U8_Array(read,data,lens,false);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Get_Soe_Data(uint32_t read,uint8_t *data,uint32_t lens)
{
	if(((read-SOE_START_ADDR1)/SOE_FLASH_LEN)==0) //块1
	{
		if((SOE_START_ADDR1+SOE_FLASH_LEN-read)<SOE_INFO_LEN)
			read=SOE_START_ADDR2;
	}

	if(((read-SOE_START_ADDR1)/SOE_FLASH_LEN)==1)
	{
		if((SOE_START_ADDR2+SOE_FLASH_LEN-read)<SOE_INFO_LEN)
			read=SOE_START_ADDR1;
	}

	Mx25_Read_U8_Array(read,data,lens,false);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Get_Yk_Data(uint32_t read,uint8_t *data,uint32_t lens)
{
	if(((read-YK_START_ADDR1)/YK_FLASH_LEN)==0) //块1
	{
		if((YK_START_ADDR1+YK_FLASH_LEN-read)<YK_INFO_LEN)
			read=YK_START_ADDR2;
	}

	if(((read-YK_START_ADDR1)/YK_FLASH_LEN)==1)
	{
		if((YK_START_ADDR2+YK_FLASH_LEN-read)<YK_INFO_LEN)
			read=YK_START_ADDR1;
	}

	Mx25_Read_U8_Array(read,data,lens,false);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return:
*************************************************************/
void Get_Yc_Data(uint32_t read,uint8_t *data,uint32_t lens)
{
	Mx25_Read_U8_Array(read,data,lens,false);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
void Get_Fault_Data(uint32_t read,uint8_t *data,uint32_t lens)
{
	Mx25_Read_U8_Array(read,data,lens,false);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
void Get_Wave_Data(uint32_t read,uint8_t *data,uint32_t lens)
{
	Mx25_Read_U8_Array(read,data,lens,false);
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Mx25_Data_Storage(uint32_t read,uint8_t *data,uint32_t lens)
{
	Mx25_Write_U8_Array(read,data,lens,false);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
uint8_t Get_U32_Hex(uint32_t u32,uint8_t *data,uint8_t lens)
{
	FOURBYTE FB;

	if(lens!=11)  //暂时这样处理
		while(1);

	FB.U32=u32;

	for(uint8_t i=0;i<4;i++)
	{
		if(FB.U4[i].U03>9)
			data[9-3*i]=FB.U4[i].U03+0x37;
		else
			data[9-3*i]=FB.U4[i].U03+0x30;

		if(FB.U4[i].U47>9)
			data[10-3*i]=FB.U4[i].U47+0x37;
		else
			data[10-3*i]=FB.U4[i].U47+0x30;

		if(i!=0)
			data[3*i-1]=0x20;
	}

	return 0x0B;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
uint8_t Get_U32_Dec(uint32_t u32,uint8_t *data,uint8_t lens)
{
	uint32_t U32=u32;

	if(lens!=10)  //暂时这样处理
		while(1);

	memset(data,0,10*sizeof(uint8_t));

	for(uint8_t i=10;i>0;i--)
	{
		data[i-1]=(U32%10)+0x30;
		U32/=10;
	}

	for(uint8_t i=0;i<10;i++)
	{
		if(data[i]!=0x30)
			return i;

		if(i==9)
			return i;
	}

	return 0;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
uint8_t Get_U32_Time(uint32_t u32,uint8_t *data,uint8_t lens)
{
	FOURBYTE FB;

	FB.U32=u32;

	for(uint8_t i=0;i<3;i++)
	{
		FB.U4[3].DATA=FB.U4[i].DATA;
		FB.U4[i].DATA=((FB.U4[3].DATA/10)<<4)+(FB.U4[3].DATA%10);
	}

	data[0]=FB.U4[2].U47+0x30;
	data[1]=FB.U4[2].U03+0x30;
	data[2]=0x2D;
	data[3]=FB.U4[1].U47+0x30;
	data[4]=FB.U4[1].U03+0x30;
	data[5]=0x2D;
	data[6]=FB.U4[0].U47+0x30;
	data[7]=FB.U4[0].U03+0x30;

	return 0x08;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
uint8_t Get_P101_Time(uint8_t *time,uint8_t *data)
{
	uint8_t timestr[8];
	DOUBLEBYTE DB;

	memcpy(timestr+1,time,7);

	memcpy(DB.U8,timestr+1,2*sizeof(uint8_t));

	timestr[5]&=0x1F;           //屏蔽掉星期
	timestr[2]=DB.U16/1000;    //秒
	DB.U16%=1000;
	timestr[1]=DB.U16/100;     //毫秒高位
	timestr[0]=DB.U16%100;     //毫秒低位

	for(uint8_t i=0;i<8;i++)
		timestr[i]=Byte2Bcd(timestr[i]);

	DB.U4[0].DATA=timestr[7];   //年
	data[0]=DB.U4[0].U47+0x30;
	data[1]=DB.U4[0].U03+0x30;
	data[2]=0x2D;
	DB.U4[0].DATA=timestr[6];
	data[3]=DB.U4[0].U47+0x30;
	data[4]=DB.U4[0].U03+0x30;
	data[5]=0x2D;
	DB.U4[0].DATA=timestr[5];
	data[6]=DB.U4[0].U47+0x30;
	data[7]=DB.U4[0].U03+0x30;
	data[8]=0x20;
	DB.U4[0].DATA=timestr[4];
	data[9]=DB.U4[0].U47+0x30;
	data[10]=DB.U4[0].U03+0x30;
	data[11]=0x3A;
	DB.U4[0].DATA=timestr[3];
	data[12]=DB.U4[0].U47+0x30;
	data[13]=DB.U4[0].U03+0x30;
	data[14]=0x3A;
	DB.U4[0].DATA=timestr[2];
	data[15]=DB.U4[0].U47+0x30;
	data[16]=DB.U4[0].U03+0x30;
	data[17]=0x2E;
	DB.U4[0].DATA=timestr[1];
	data[18]=DB.U4[0].U03+0x30;
	DB.U4[0].DATA=timestr[0];
	data[19]=DB.U4[0].U47+0x30;
	data[20]=DB.U4[0].U03+0x30;

	return 21;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
void Print_Sys_Para(void)
{
	#ifdef PRINT_SYS_INIT_DEBUG

	DEBUG_PRINT("KKNSSK","<-------------------- 汇集单元系统当前版本号: ",SYS_VERSION);
	DEBUG_PRINT("KKNSK","<-------------------- 汇集单元系统参数区参数如下:");
	DEBUG_PRINT("KSDSK","<----- 当前汇集单元共有 ",gSYSPARA.LINEGROUP.LINENUM," 组采集单元组:");

	for(uint8_t line=0;line<LINEGROUP_MAXNUM;line++)  //更新采集单元组数目
	{
		if(gSYSPARA.LINEGROUP.LINE[line].LINENO==0||gSYSPARA.LINEGROUP.LINE[line].LINENO==0xFF)
			continue;

		DEBUG_PRINT("KSDSK","第 ",gSYSPARA.LINEGROUP.LINE[line].LINENO," 采集单元组采集单元地址:");

		for(uint8_t phase=0;phase<3;phase++)
				DEBUG_PRINT_U8CHAR_ARRAY(gSYSPARA.LINEGROUP.LINE[line].AU[phase].ADDR,6);
	}

	DEBUG_PRINT("KS","<----- 汇集单元本机地址:  ");
	DEBUG_PRINT_U8CHAR_ARRAY(gSYSPARA.BENJI,6);
	DEBUG_PRINT("KS","<----- 汇集单元维护地址:  ");
	DEBUG_PRINT_U8CHAR_ARRAY(gSYSPARA.WEIHU,6);
	DEBUG_PRINT("KS","<----- 汇集单元广播地址:  ");
	DEBUG_PRINT_U8CHAR_ARRAY(gSYSPARA.BROADCAST,6);

	DEBUG_PRINT("KSHK","<----- 子站地址: ",gECPPARA.ZIZHAN.U16);
	DEBUG_PRINT("KSDK","<----- 射频信道: ",gECPPARA.RADIO_CHANNEL);

	DEBUG_PRINT("KS","<----- 遥测上报模式: ");
	if(gECPPARA.YC_REPORT_MODE==0)
		DEBUG_PRINT("SK","遥测突发上报");
	if(gECPPARA.YC_REPORT_MODE==1)
		DEBUG_PRINT("SK","遥测周期上报");
	DEBUG_PRINT("KS","<----- 系统校时模式: ");
	if(gECPPARA.GPS_TIMING_MODE==0)
		DEBUG_PRINT("SKKK","ECP规约校时");
	if(gECPPARA.GPS_TIMING_MODE==1)
		DEBUG_PRINT("SKKK","GPS周期校时");

	#endif
}

















