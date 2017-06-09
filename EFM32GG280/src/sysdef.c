/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: sysdef.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "sysdef.h"


SYSFLAG gSYSFLAG;
SYSPARA gSYSPARA;
ECPPARA gECPPARA;
ECPINFO gECPINFO;

YAOCEFILE gYCFILE;
SOEQUEUE gSOEQUEUE;
UNIXTIME gUNIXTIME;
FAULTINFO gFAULTINFO;
FAULTQUEUE gFAULTQUEUE;

ICPFRAME RADIOFRAME;
ICPFRAME WEIHUFRAME;
ECPFRAME gECPFRAMELIST;
ICPFRAME gICPFRAMELIST[MAX_ICPFRAME_NUM];

DATACACHE GPRS_RXDC;
DATACACHE GPRS_TXDC;
DATACACHE RADIO_RXDC;
DATACACHE WEIHU_RXDC;
DATACACHE WEIHU_TXDC;

/**********ÈÎÎñ¾ä±ú**********/
TaskHandle_t xEcp_File_Report=NULL;
TaskHandle_t xCall_Wave=NULL;
TaskHandle_t xAu_Timing=NULL;
TaskHandle_t xData_Send_Reply=NULL;
TaskHandle_t xYAOCE_Report=NULL;
TaskHandle_t xData_Report=NULL;

