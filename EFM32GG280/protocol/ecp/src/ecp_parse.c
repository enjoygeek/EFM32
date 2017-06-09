/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: ecp_parse.h
*Version: 0.1.0
*Date: 2016-10-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"


/************************************************************!
*Function: Ecp_Parse
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Ecp_Parse(DATACACHE *DC,ECPFRAME *EF)
{
	if(EF->REQ.ISLOCK==true) //101结构体还没有被释放掉,里面还有未处理完的数据
		return false;

	while(DataCacheLength(DC)>0)
	{
		if(EF->REQ.PARSE==false) //解析变长帧的帧头和定长帧
		{
			if(DataCacheLength(DC)<ECP_FIXFRMAE_LEN)
				return false;

			ReadDataCache(DC,EF->REQ.EF.DATA,1); //读取!!!!!!!!队列的第一个字节

			if(!((EF->REQ.EF.FF.STARTBYTE==ECP_FIXFRMAE_STARTBYTE)||(EF->REQ.EF.VF.STARTBYTE1==ECP_VARFRMAE_STARTBYTE)||(EF->REQ.EF.VF.STARTBYTE1==ECP_ESMFRMAE_STARTBYTE))) //起始字节判断
				continue;

			VirtualReadDataCache(DC,DC->READ,EF->REQ.EF.DATA+1,ECP_FIXFRMAE_LEN-1); //查找队列中的帧头,不读取数据!!!!!!!!!

			if(!(((EF->REQ.EF.FF.STARTBYTE==ECP_FIXFRMAE_STARTBYTE)&&(EF->REQ.EF.FF.ENDBYTE==ECP_FIXFRMAE_ENDBYTE))||(((EF->REQ.EF.VF.STARTBYTE1==ECP_VARFRMAE_STARTBYTE)||(EF->REQ.EF.VF.STARTBYTE1==ECP_ESMFRMAE_STARTBYTE))&&(EF->REQ.EF.VF.LEN1==EF->REQ.EF.VF.LEN2))))
				continue;

			ReadDataCache(DC,EF->REQ.EF.DATA+1,ECP_FIXFRMAE_LEN-1); //读取!!!!!!完整的定长帧和变长帧的帧头

			EF->REQ.PARSE=true;
		}

		if(EF->REQ.EF.FF.STARTBYTE==ECP_FIXFRMAE_STARTBYTE) //定长帧帧格式及CS判断
		{
			EF->REQ.PARSE=false;

			if(EF->REQ.EF.FF.CS!=Check_Ecp_Cs(EF->REQ.EF.DATA+1,ECP_FIXFRMAE_LEN-1-1-1))
				continue;

			EF->REQ.ISLOCK=true;
			EF->REQ.FRAMETYPE=FIX_FRAME;

			return true;
		}

		if((EF->REQ.EF.VF.STARTBYTE1==ECP_VARFRMAE_STARTBYTE)||(EF->REQ.EF.VF.STARTBYTE1==ECP_ESMFRMAE_STARTBYTE)) //变长帧帧格式及CS判断
		{
			if(DataCacheLength(DC)<(EF->REQ.EF.VF.LEN1+ECP_VARFRMAE_TAIL-ECP_FIXFRMAE_LEN+ECP_VARFRMAE_HEAD)) //等待变长帧数据接收完毕
				return false;

			EF->REQ.PARSE=false;

			ReadDataCache(DC,EF->REQ.EF.DATA+ECP_FIXFRMAE_LEN,EF->REQ.EF.VF.LEN1-ECP_FIXFRMAE_LEN+ECP_VARFRMAE_HEAD);
			ReadDataCache(DC,EF->REQ.EF.DATA+ECP_VARFRAME_CS_LOC,ECP_VARFRMAE_TAIL);

			if(EF->REQ.EF.VF.ENDBYTE!=ECP_VARFRMAE_ENDBYTE)
				continue;

			if(EF->REQ.EF.VF.CS!=Check_Ecp_Cs(EF->REQ.EF.DATA+ECP_VARFRMAE_HEAD,EF->REQ.EF.VF.LEN1))
				continue;

			EF->REQ.ISLOCK=true;

			if(EF->REQ.EF.VF.STARTBYTE1==ECP_VARFRMAE_STARTBYTE)
				EF->REQ.FRAMETYPE=VAR_FRAME;

			if(EF->REQ.EF.VF.STARTBYTE1==ECP_ESMFRMAE_STARTBYTE)
			{
				EF->REQ.FRAMETYPE=ESM_FRAME;
				EF->REQ.ESMLEN=EF->REQ.EF.VF.LEN1-3;  //1个字节的控制域和2个字节的地址域
			}

			return true;
		}
	}

	return false;
}
/************************************************************!
*Function: Check_Ecp_Cs
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t Check_Ecp_Cs(uint8_t *data,uint32_t lens)
{
	uint8_t cs=0;

	for(uint32_t i=0;i<lens;i++)
		cs+=data[i];

	return cs;
}

