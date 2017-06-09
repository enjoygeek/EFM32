/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: sysdef.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef SYSDEF_H_
#define SYSDEF_H_


#include <stddef.h>
#include <stdint.h>
#include "include.h"


extern SYSFLAG gSYSFLAG;
extern SYSPARA gSYSPARA;
extern ECPPARA gECPPARA;
extern ECPINFO gECPINFO;

extern YAOCEFILE gYCFILE;
extern SOEQUEUE gSOEQUEUE;
extern UNIXTIME gUNIXTIME;
extern FAULTINFO gFAULTINFO;
extern FAULTQUEUE gFAULTQUEUE;

extern ICPFRAME RADIOFRAME;
extern ICPFRAME WEIHUFRAME;
extern ECPFRAME gECPFRAMELIST;
extern ICPFRAME gICPFRAMELIST[MAX_ICPFRAME_NUM];

extern DATACACHE GPRS_RXDC;
extern DATACACHE GPRS_TXDC;
extern DATACACHE RADIO_RXDC;
extern DATACACHE WEIHU_RXDC;
extern DATACACHE WEIHU_TXDC;


/**********ÈÎÎñ¾ä±ú**********/
extern TaskHandle_t xEcp_File_Report;
extern TaskHandle_t xCall_Wave;
extern TaskHandle_t xAu_Timing;
extern TaskHandle_t xData_Send_Reply;
extern TaskHandle_t xYAOCE_Report;
extern TaskHandle_t xData_Report;


#endif

