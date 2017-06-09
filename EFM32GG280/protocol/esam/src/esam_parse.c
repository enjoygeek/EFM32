/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: esam_parse.c
*Version: 0.1.0
*Date: 2017-05-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "esam.h"
#include "esam_parse.h"
#include "include.h"


/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Esam_Parse(DATACACHE *DC,ESAMFRAME *EF)
{
	if(EF->REQ.ISLOCK==true)
		return false;

	while(DataCacheLength(DC)>0)
	{
		if(EF->REQ.PARSE==false)
		{
			if(DataCacheLength(DC)<ESAM_FRAMEHEAD_LEN)
				return false;

			ReadDataCache(DC,EF->REQ.EF.DATA,1); //读取!!!!!!!!队列的第一个字节

			if(EF->REQ.EF.STARTBYTE1!=ESAM_STARTBYTE) //起始字节判断
				continue;

			VirtualReadDataCache(DC,DC->READ,EF->REQ.EF.DATA+1,ESAM_STARTBYTE-1); //查找队列中的帧头,不读取数据!!!!!!!!!

			if(!((EF->REQ.EF.STARTBYTE1==EF->REQ.EF.STARTBYTE2)&&(EF->REQ.EF.PROTOLEN<=(ESAM_FRAME_MAX_LEN-6))))
				continue;

			ReadDataCache(DC,EF->REQ.EF.DATA+1,ESAM_FRAMEHEAD_LEN-1); //读取!!!!!!完整的定长帧和变长帧的帧头

			EF->REQ.PARSE=true;
		}

		if(DataCacheLength(DC)<(EF->REQ.EF.PROTOLEN+ESAM_FRAMETAIL_LEN)) //等待帧数据接收完毕
			return false;

		EF->REQ.PARSE=false;

		ReadDataCache(DC,EF->REQ.EF.DATA+ESAM_FRAMEHEAD_LEN,EF->REQ.EF.PROTOLEN);
		ReadDataCache(DC,EF->REQ.EF.DATA+ESAM_FRAME_CS_LOC,ESAM_FRAMETAIL_LEN);

		if(EF->REQ.EF.ENDBYTE!=ESAM_ENDBYTE)
			continue;

		if(EF->REQ.EF.CS!=Check_Esam_Cs(EF->REQ.EF.DATA+ESAM_FRAMEHEAD_LEN,EF->REQ.EF.PROTOLEN))
			continue;

		EF->REQ.ISLOCK=true;

		return true;
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
//bool Ecp101_Parse(ESAMFRAME *EF)
//{
//	if(!(EF->REQ.EF.ENCAPDATA.EF.FF.STARTBYTE==ESAM101_FIXFRAME_STARTBYTE||EF->REQ.EF.ENCAPDATA.EF.VF.STARTBYTE1==ESAM101_VARFRAME_STARTBYTE))
//		return false;
//
//	if(EF->REQ.EF.ENCAPDATA.EF.FF.STARTBYTE==ESAM101_FIXFRAME_STARTBYTE)
//	{
//		if(EF->REQ.EF.ENCAPDATA.EF.FF.ENDBYTE!=ESAM101_FIXFRAME_ENDBYTE)
//			return false;
//
//		if(EF->REQ.EF.ENCAPDATA.EF.FF.CS!=Check_Esam_Cs(EF->REQ.EF.ENCAPDATA.EF.DATA+1,ESAM101_FIXFRAME_LEN-3))
//			return false;
//
//		if(EF->REQ.EF.ENCAPDATA.APPLEN!=ESAM101_FIXFRAME_LEN)
//			return false;
//
//		EF->REQ.ESAM101_FRAMETYPE=ESAM101_FIXFRAME_TYPE;
//	}
//
//	if(EF->REQ.EF.ENCAPDATA.EF.VF.STARTBYTE1==ESAM101_VARFRAME_STARTBYTE)
//	{
//		if((EF->REQ.EF.ENCAPDATA.EF.VF.STARTBYTE1!=EF->REQ.EF.ENCAPDATA.EF.VF.STARTBYTE2)||(EF->REQ.EF.ENCAPDATA.EF.VF.LEN1!=EF->REQ.EF.ENCAPDATA.EF.VF.LEN2))
//			return false;
//
//		if(EF->REQ.EF.ENCAPDATA.APPLEN!=(EF->REQ.EF.ENCAPDATA.EF.VF.LEN1+6))
//			return false;
//
//		if(EF->REQ.EF.ENCAPDATA.EF.DATA[EF->REQ.EF.ENCAPDATA.APPLEN-1]!=ESAM101_VARFRAME_ENDBYTE)
//			return false;
//
//		if(EF->REQ.EF.ENCAPDATA.EF.DATA[EF->REQ.EF.ENCAPDATA.APPLEN-2]!=Check_Esam_Cs(EF->REQ.EF.ENCAPDATA.EF.DATA+ESAM101_VARFRAME_FRAMEHEAD,EF->REQ.EF.ENCAPDATA.EF.VF.LEN1))
//			return false;
//
//		EF->REQ.ESAM101_FRAMETYPE=ESAM101_VARFRAME_TYPE;
//	}
//
//	return true;
//}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t Check_Esam_Cs(uint8_t *data,uint32_t lens)
{
	uint8_t cs=0;

	for(uint32_t i=0;i<lens;i++)
		cs+=data[i];

	return cs;
}
