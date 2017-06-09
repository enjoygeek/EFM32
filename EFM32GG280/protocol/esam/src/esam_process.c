/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: esam_process.c
*Version: 0.1.0
*Date: 2017-05-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "esam_process.h"
#include "include.h"


ESAM1161Y_CERDATA ECD;
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Esam_Process(ESAMFRAME *EF)
{
	uint8_t sn;

	if(EF->REQ.ISLOCK==false)
		return true;

	EF->REQ.ISLOCK=false;

	if(EF->REQ.EF.PROTOTYPE.ENCRYPTION==true) //若加密,则解密
	{
		if(EF->REQ.EF.PROTOTYPE.OBJECT==0x40) //现场运维工具
			EF->REQ.EF.PROTOLEN=Esam1161y_Tool_Data_Decryption(EF->REQ.EF.ENCAP.DATA+2,EF->REQ.EF.PROTOLEN-2); //减去2个字节的报文类型,从封装数据域开始
		else
			EF->REQ.EF.PROTOLEN=Esam1161y_Data_Decryption(EF->REQ.EF.ENCAP.DATA+2,EF->REQ.EF.PROTOLEN-2); //减去2个字节的报文类型,从封装数据域开始

		if(EF->REQ.EF.PROTOLEN==0) //解密失败,错误代码
		{
			sn=Req_Empty_EsamFrame(EF);

			EF->RES.ETQ[sn].HAS_ESAM101=0; //无101

			EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=0x1F;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9103;

			EF->RES.ETQ[sn].INFOLEN=2;
			EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
	}

	if((EF->REQ.EF.ENCAP.APPTYPE<=7)&&((EF->REQ.EF.ENCAP.APPTYPE%2)==1)) //应用类型1 3 5 7
	{
		memcpy(EF->REQ.INFOLEN.U8,EF->REQ.EF.ENCAP.ENCAP1.DATA+1+EF->REQ.EF.ENCAP.ENCAP1.APPLEN,2); //提取信息安全区长度

		memcpy(EF->REQ.EF.ENCAP.ENCAP1.DATA+1+EF->REQ.EF.ENCAP.ENCAP1.APPLEN,EF->REQ.EF.ENCAP.ENCAP1.DATA+1+EF->REQ.EF.ENCAP.ENCAP1.APPLEN+2,EF->REQ.INFOLEN.U16); //覆盖信息安全区长度的两个字节,验签不需要

		if(!(((EF->REQ.EF.ENCAP.APPTYPE==1)&&(EF->REQ.INFOLEN.U16==(65)))||((EF->REQ.EF.ENCAP.APPTYPE==3)&&(EF->REQ.INFOLEN.U16==(65+8)))||((EF->REQ.EF.ENCAP.APPTYPE==5)&&(EF->REQ.INFOLEN.U16==(65+6)))||((EF->REQ.EF.ENCAP.APPTYPE==7)&&(EF->REQ.INFOLEN.U16==(65+6+8)))))
		{
			sn=Req_Empty_EsamFrame(EF);

			EF->RES.ETQ[sn].HAS_ESAM101=0; //无101

			EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=0x1F;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9101;

			EF->RES.ETQ[sn].INFOLEN=2;
			EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

			EF->RES.ETQ[sn].ISLOCK=true;

			return false;
		}

		if(Esam1161y_Verify_Signature(EF->REQ.EF.ENCAP.ENCAP1.DATA+1,EF->REQ.EF.PROTOLEN-6)==false)
		{
			sn=Req_Empty_EsamFrame(EF);

			EF->RES.ETQ[sn].HAS_ESAM101=0; //无101

			EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=0x1F;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9102;

			EF->RES.ETQ[sn].INFOLEN=2;
			EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

			EF->RES.ETQ[sn].ISLOCK=true;

			return false;
		}
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x20||EF->REQ.EF.ENCAP.APPTYPE==0x50) //网关对于终端及主站对于终端的认证请求
	{
		sn=Req_Empty_EsamFrame(EF);

		if(EF->REQ.EF.ENCAP.APPTYPE==0x50) //拷贝主站随机数
		{
			memcpy(gSYSFLAG.EYP.REMOTE_RANDOM,EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO,8);

			for(uint8_t i=0;i<8;i++)
				gSYSFLAG.EYP.REMOTE_RANDOM[8+i]=~gSYSFLAG.EYP.REMOTE_RANDOM[i];
		}

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101

		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Sign_Random(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO)+1; //1:密钥签名标识
		EF->RES.ETQ[sn].INFOLEN=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16;
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.SAFEINFO[EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-1]=6; //密钥签名标识

		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x22||EF->REQ.EF.ENCAP.APPTYPE==0x52) //网关对于终端及主站对于终端的认证请求的响应
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101

		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;

		EF->RES.ETQ[sn].INFOLEN=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16;

		if(EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16<64)
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9102; //值不对

		if(Esam_1161y_Verify_Signature(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO,65)==true)
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9102; //值不对
		else
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9102; //值不对

		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x38||EF->REQ.EF.ENCAP.APPTYPE==0x54) //运维工具或主站获取芯片ID
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101

		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Get_Chip_Id(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO);

		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x34||EF->REQ.EF.ENCAP.APPTYPE==0x60) //运维工具或主站获取芯片密钥版本+随机数
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101

		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Get_Key_Version(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO);
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16+=Esam1161y_Get_Random(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO+1);

		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x62||EF->REQ.EF.ENCAP.APPTYPE==0x64) //主站密钥更新或恢复
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		if(EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16<0x00F6||EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16==0)
		{
			if(EF->REQ.EF.ENCAP.APPTYPE==0x62)
				EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9091; //密钥更新失败
			if(EF->REQ.EF.ENCAP.APPTYPE==0x64)
				EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9092; //密钥恢复失败

			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(EF->REQ.EF.ENCAP.APPTYPE==0x62)
		{
			if(Esam1161y_Symmetric_key_Update(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO,1)==true)
				EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			else
				EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9091; //密钥更新失败
		}

		if(EF->REQ.EF.ENCAP.APPTYPE==0x64)
		{
			if(Esam1161y_Symmetric_key_Update(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO,0)==true)
				EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			else
				EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9091; //密钥恢复失败
		}

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x46) //运维工具密钥恢复
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		if(gSYSFLAG.EYP.TOOL_VERI!=1||Esam1161y_Symmetric_key_Update(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO,0)==false)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9091; //密钥更新失败
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
		else
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x70) //更新CA/主站证书
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		if(gSYSFLAG.EYP.REMOTE_VERI!=1)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9093;

			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMESEQ==1)
			ECD.LENS=0;

		memcpy(ECD.DATA+ECD.LENS,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CIPHERTEXT,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-3);

		ECD.LENS+=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-3;

		if(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMESEQ!=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMENUM)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		ECD.LENS=Esam1161y_Data_Decryption(ECD.DATA,ECD.LENS);

		if(ECD.LENS==0)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9093;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(Esam1161y_Verify_Signature(ECD.DATA,ECD.LENS)==false)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9093;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		ECD.LENS-=1+6+64;

		if(Esam1161y_Ca_Certificate_Update(ECD.DATA,ECD.LENS)==false)  //此处不对,需要修改
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9093;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
		else
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x72) //更新终端证书
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		if(gSYSFLAG.EYP.REMOTE_VERI!=1)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9093;

			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMESEQ==1)
			ECD.LENS=0;

		memcpy(ECD.DATA+ECD.LENS,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CIPHERTEXT,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-3);

		ECD.LENS+=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-3;

		if(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMESEQ!=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMENUM)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(Esam1161y_Set_Ca_Certificate(ECD.DATA,ECD.LENS)==false)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9093;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
		else
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x42||EF->REQ.EF.ENCAP.APPTYPE==0x74) //运维工具/主站提取终端证书
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Get_Ca_Certificate(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CIPHERTEXT,0);

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CERFLAG=6;
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMENUM=1;
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMESEQ=1;

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16+=3;

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x76) //主站向终端返回证书提取结果
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		if(EF->REQ.EF.ENCAP.ENCAP2.EDU.MSG.U8[0]==0)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Get_Test_Ca_Certificate(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CIPHERTEXT,0);
		}
		else
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Get_Ca_Certificate(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CIPHERTEXT,0);
		}

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CERFLAG=EF->REQ.EF.ENCAP.ENCAP2.EDU.MSG.U8[0];
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMENUM=1;
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMESEQ=1;
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16+=3;

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x3A) //运维工具获取公钥
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Get_Key(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.SAFEINFO);

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Tool_Data_Encryption(EF->RES.ETQ[sn].EF.ENCAP.DATA,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16+3);

		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=true;

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x80) //工具获取公钥
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Get_Key(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.SAFEINFO);

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x30) //运维工具信息认证
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		if(EF->REQ.EF.ENCAP.ENCAP2.EDU.FRAMESEQ==1)
		{
			ECD.LENS=0;
			memcpy(gSYSFLAG.EYP.TOOLID,EF->REQ.EF.ENCAP.ENCAP2.DATA+EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16-8,8);
		}

		memcpy(ECD.DATA+ECD.LENS,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CIPHERTEXT,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-11);

		ECD.LENS+=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-11;

		if(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMESEQ!=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMENUM)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9000;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(Esam1161y_Verify_Tool_Cer(ECD.DATA,ECD.LENS))
		{
			Esam1161y_Get_Random(gSYSFLAG.EYP.TOOL_RANDOM);

			memcpy(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.SAFEINFO,gSYSFLAG.EYP.TOOL_RANDOM,8);

			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=8;

			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
		else
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9000; //值不对
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x32) //运维工具签名认证
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		if(EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16<64)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9000; //值不对
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(Esam_1161y_Verify_Tool_Signature(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO,64)==false)
		{
			gSYSFLAG.EYP.TOOL_VERI=1;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9000;
		}
		else
		{
			gSYSFLAG.EYP.TOOL_VERI=0;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9000; //值不对
		}

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x32) //运维工具签名认证
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101

		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		//获取终端序列号

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=24;

		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x3C) //运维工具对证书请求文件进行签名
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		if(gSYSFLAG.EYP.TOOL_VERI!=1)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9093;

			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(EF->REQ.EF.ENCAP.ENCAP2.EDU.FRAMESEQ1==1)
		{
			ECD.LENS=0;
			memcpy(gSYSFLAG.EYP.TOOLID,EF->REQ.EF.ENCAP.ENCAP2.DATA+EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16-8,8);
		}

		memcpy(ECD.DATA+ECD.LENS,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CIPHERTEXT,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-11);

		ECD.LENS+=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-2;

		if(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMESEQ1!=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMENUM1)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}


		ECD.LENS=Esam_1161y_Verify_Tool_Signature(ECD.DATA,ECD.LENS);
		memcpy(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.SAFEINFO,ECD.DATA,ECD.LENS);

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=0x40;
		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x3E) //运维工具更新CA\主站\终端证书
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		if(gSYSFLAG.EYP.TOOL_VERI!=1)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x9093;

			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(EF->REQ.EF.ENCAP.ENCAP2.EDU.FRAMESEQ==1)
		{
			ECD.LENS=0;
			memcpy(gSYSFLAG.EYP.TOOLID,EF->REQ.EF.ENCAP.ENCAP2.DATA+EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16-8,8);
		}

		memcpy(ECD.DATA+ECD.LENS,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.CIPHERTEXT,EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-11);

		ECD.LENS+=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16-11;

		if(EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMESEQ1!=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.FRAMENUM1)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}

		if(EF->REQ.EF.ENCAP.ENCAP2.EDU.CERFLAG==6)
			EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Set_Ca_Certificate(ECD.DATA,ECD.LENS);
		else
			EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Ca_Certificate_Update(ECD.DATA,ECD.LENS);

		if(EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16==0)
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
		else
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0;
			EF->RES.ETQ[sn].ISLOCK=true;

			return true;
		}
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x40) //运维工具回写证书
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Get_Test_Ca_Certificate(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO,0);
		Esam1161y_Set_Ca_Certificate(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO,EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16);

		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.EDU.MSG.U16=0x41;
		EF->RES.ETQ[sn].ISLOCK=true;

		return true;
	}

	if(EF->REQ.EF.ENCAP.APPTYPE==0x48) //运维工具数据传输
	{
		sn=Req_Empty_EsamFrame(EF);

		EF->RES.ETQ[sn].HAS_ESAM101=0; //无101
		EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16=2;
		EF->RES.ETQ[sn].EF.ENCAP.APPTYPE=EF->REQ.EF.ENCAP.APPTYPE+1;
		EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION=false;

		EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16=Esam1161y_Tool_Data_Encryption(EF->REQ.EF.ENCAP.ENCAP2.EDU.SAFEINFO,EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16);

		//未完,待续
	}


	Esam_App(EF);

	return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Fill_Esam_Frame(ESAMFRAME *EF,uint8_t sn)
{
	EF->RES.ETQ[sn].EF.STARTBYTE1=ESAM_STARTBYTE;
	EF->RES.ETQ[sn].EF.STARTBYTE2=ESAM_STARTBYTE;

	if(EF->RES.ETQ[sn].HAS_ESAM101==1) //有应用数据区
	{
		if(EF->RES.ETQ[sn].ESAM101_FRAMETYPE==ESAM101_FIXFRAME_TYPE) //固定帧
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.FF.STARTBYTE=ESAM101_FIXFRAME_STARTBYTE;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.FF.ENDBYTE=ESAM101_FIXFRAME_ENDBYTE;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.FF.LINKADDR.U16=gECPPARA.ZIZHAN.U16;

			if(EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.FF.CTRL.FCV==1)
			{
				if(EF->SFCB==0)
					EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.FF.CTRL.FCB=0;

				if(EF->SFCB==1)
					EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.FF.CTRL.FCB=1;

				EF->SFCB^=1;
			}

			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.FF.CS=Check_Esam_Cs(EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.DATA+1,3); //1:起始字节

			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.APPLEN=ESAM101_FIXFRAME_LEN;
			EF->RES.ETQ[sn].ESAM101_FRAMELEN=EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.APPLEN;
		}

		if(EF->RES.ETQ[sn].ESAM101_FRAMETYPE==ESAM101_VARFRAME_TYPE) //变长帧
		{
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.STARTBYTE1=ESAM101_VARFRAME_STARTBYTE;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.STARTBYTE2=ESAM101_VARFRAME_STARTBYTE;

			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.LEN1=11+EF->RES.ETQ[sn].ESAM101LEN; //2个字节COT
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.LEN2=EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.LEN1;

			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.CTRL.DATA|=Is_Sys_Full();
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.LINKADDR.U16=gECPPARA.ZIZHAN.U16;
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.ASDU.CAOA.U16=gECPPARA.ZIZHAN.U16;

			if(EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.CTRL.FCV==1)
			{
				if(EF->SFCB==0)
					EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.CTRL.FCB=0;

				if(EF->SFCB==1)
					EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.CTRL.FCB=1;

				EF->SFCB^=1;
			}

			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.DATA[ESAM_FRAMEHEAD_LEN+EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.LEN1]=Check_Esam_Cs(EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.DATA+ESAM_FRAMEHEAD_LEN,EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.LEN1);
			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.DATA[ESAM_FRAMEHEAD_LEN+EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.LEN1+1]=ESAM101_FIXFRAME_ENDBYTE;

			EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.APPLEN=EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.EF.VF.LEN1+ESAM_FRAMEHEAD_LEN+ESAM_FRAMETAIL_LEN; //6:帧头加帧尾
			EF->RES.ETQ[sn].ESAM101_FRAMELEN=EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.APPLEN;
		}
	}
	else
	{
		EF->RES.ETQ[sn].EF.PROTOLEN=EF->RES.ETQ[sn].EF.ENCAP.ENCAP2.INFOLEN.U16+1+2;
	}

	if(EF->RES.ETQ[sn].EF.ENCAP.APPTYPE==1) //签名结果,汇集单元不会主发
	{

	}

	if(EF->RES.ETQ[sn].EF.ENCAP.APPTYPE==2) //随机数
	{
		Esam1161y_Get_Random(EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.DATA+1+EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.APPLEN);
		EF->RES.ETQ[sn].EF.PROTOLEN=1+1+2+EF->RES.ETQ[sn].EF.ENCAP.ENCAP1.APPLEN+8;
	}

	if(EF->RES.ETQ[sn].EF.ENCAP.APPTYPE==3) //随机数+签名结果
	{

	}

	if(EF->RES.ETQ[sn].EF.ENCAP.APPTYPE==4) //时间
	{

	}

	if(EF->RES.ETQ[sn].EF.ENCAP.APPTYPE==5) //时间+签名结果
	{

	}

	if(EF->RES.ETQ[sn].EF.ENCAP.APPTYPE==6) //时间+随机数
	{

	}

	if(EF->RES.ETQ[sn].EF.ENCAP.APPTYPE==7) //时间+随机数+签名结果
	{

	}

	if(EF->RES.ETQ[sn].EF.PROTOTYPE.ENCRYPTION==true)
	{
		if(EF->RES.ETQ[sn].ENCRYTYPE==0) //无随机数加密
			EF->RES.ETQ[sn].EF.PROTOLEN=Esam1161y_Data_Encryption(EF->RES.ETQ[sn].EF.ENCAP.DATA,EF->RES.ETQ[sn].EF.PROTOLEN+1+1+2); //1:应用类型 1:报文长度 2:信息安全区长度
		else                             //有随机数加密
			EF->RES.ETQ[sn].EF.PROTOLEN=Esam1161y_Data_Encryption_Random(EF->RES.ETQ[sn].EF.ENCAP.DATA,EF->RES.ETQ[sn].EF.PROTOLEN+1+1+2);
	}

	EF->RES.ETQ[sn].EF.PROTOLEN+=2; //2:报文类型

	EF->RES.ETQ[sn].EF.DATA[ESAM_FRAMEHEAD_LEN+EF->RES.ETQ[sn].EF.PROTOLEN]=Check_Esam_Cs(EF->RES.ETQ[sn].EF.DATA+ESAM_FRAMEHEAD_LEN,EF->RES.ETQ[sn].EF.PROTOLEN);
	EF->RES.ETQ[sn].EF.DATA[ESAM_FRAMEHEAD_LEN+EF->RES.ETQ[sn].EF.PROTOLEN+1]=ESAM_ENDBYTE;

	return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Fill_Esam_DataCache(ESAMFRAME *EF,uint8_t sn)
{



	return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t Get_Empty_EsamFrame(ESAMFRAME *EF)
{
	uint8_t OLDSN=EF->RES.SN;

	if(EF->LINKSTATE==LINKSTATE_START)
		return 0;

	portTICK_TYPE_ENTER_CRITICAL();

	for(uint8_t sn=OLDSN;sn<(ESAM_RES_MAX_NUM+OLDSN);sn++)
	{
		EF->RES.SN=sn%MAX_REPLY_NUM;

		if(EF->RES.ETQ[EF->RES.SN].STATUS==true)
			continue;

		EF->RES.ETQ[sn].POS=0;
		EF->RES.ETQ[EF->RES.SN].STATUS=true;
		EF->RES.ETQ[sn].EF.PROTOTYPE=EF->REQ.EF.PROTOTYPE;

		portTICK_TYPE_EXIT_CRITICAL();

		return EF->RES.SN;
	}

	portTICK_TYPE_EXIT_CRITICAL();

	return 0xFF;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t Req_Empty_EsamFrame(ESAMFRAME *EF)
{
	uint8_t sn=0xFF; //帧序号

	while(sn==0xFF)
	{
		sn=Get_Empty_EsamFrame(EF);

		if(sn!=0xFF)
			break;

		taskYIELD();
	}

	return sn;
}
