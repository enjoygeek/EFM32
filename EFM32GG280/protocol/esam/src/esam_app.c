/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: esam_app.c
*Version: 0.1.0
*Date: 2017-05-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"

/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Esam_App(ESAMFRAME *EF)
{
	switch(EF->REQ.EF.ENCAP.APPTYPE)
	{
		case 0x20: //网关对终端的认证请求
		case 0x50: //主站对终端的认证请求
			break;


		default:
			break;

	}
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void Esam_Certification_Request(ESAMFRAME *EF)
{
	EF->REQ.INFOLEN.U16=EF->REQ.EF.ENCAP.ENCAP2.INFOLEN.U16;

	if(EF->REQ.EF.ENCAP.APPTYPE==0x50)
	{

	}

}
