/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: mid.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef MID_H_
#define MID_H_


void DataCache_Init(void);
bool Radio_Time_Slice(void);
void Radio_Int_Process(void);
void Radio_StartTx_Data(uint8_t *data,uint8_t lens);
bool Radio_ShortPkt_Tx(uint8_t *data,uint8_t lens);
bool Radio_WakeUp_StartTx(uint8_t *data,uint8_t lens);
void Radio_WakeUp(uint8_t *data,uint8_t lens);
void WeiHu_Queue_Rx(void);
void WeiHu_Queue_Tx(void);
bool WeiHu_StartTx(void);
void WeiHu_ContinueTx(void);
void Gps_Queue_Rx(uint8_t data);
void Gprs_Queue_Rx(void);
void Gprs_Queue_Tx(void);
bool Gprs_StartTx(void);
void Gprs_ContinueTx(void);


#endif

