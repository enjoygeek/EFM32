/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: mid.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "mid.h"


/************************************************************!
*Function: DataCache_Init
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void DataCache_Init(void)
{
	CreateDataCache(&GPRS_RXDC,1000);  //GPRS串口接收缓存
	CreateDataCache(&GPRS_TXDC,1000);  //GPRS串口发送缓存
	CreateDataCache(&RADIO_RXDC,1000); //RADIO接收缓存
	CreateDataCache(&WEIHU_RXDC,1000); //维护端口接收缓存
	CreateDataCache(&WEIHU_TXDC,1000); //维护端口发送缓存
}
/************************************************************!
*Function: Radio_Time_Slice
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Radio_Time_Slice(void)
{
	gSYSFLAG.CWS.RADIOTICKS++;

	//DEBUG_PRINT("KKNSDSKK","第  ",gSYSFLAG.CWS.RADIOTICKS," 时间片");

	gSYSFLAG.CWS.RADIOTICKS%=32;

//	if(gSYSFLAG.CWS.RADIOTICKS==10||gSYSFLAG.CWS.RADIOTICKS==20||gSYSFLAG.CWS.RADIOTICKS==30)
//		Status_Led2_Set();
//	else
//		Status_Led2_Clear();

	if((gUNIXTIME.UNIX%5==0)&&(RTC->CNT<16384)&&(gSYSFLAG.CWS.RADIOTICKS==10))  //每隔5S的第一个汇集单元时间片发送校时数据
	{
		Au_Timing_Frame();

		return true;
	}

	if(((gSYSFLAG.CWS.RADIOTICKS%10)!=0||gSYSFLAG.CWS.RADIOTICKS==0)&&(gSYSFLAG.CWS.RADIOTICKS<=32))
		return false;

	//DEBUG_PRINT("KKNSDSKK","第  ",gSYSFLAG.CWS.RADIOTICKS," 发送时间片");

	for(uint8_t i=2;i<MAX_RADIO_FRAME;i++)
	{
		if(gICPFRAMELIST[i].ISLOCK==false)
			continue;

		gICPFRAMELIST[i].STATUS=0;
		gICPFRAMELIST[i].ISLOCK=false;

		Radio_ShortPkt_Tx(gICPFRAMELIST[i].FRAME,gICPFRAMELIST[i].LEN1+6);

		#ifdef PRINT_AU_TIMESTAMP_TIME_DEBUG //维护端口打印测试时标
		if(RTC->CNT<16384)
			DEBUG_PRINT("KSDSDSDSK","<------ 射频已于第 ",gSYSFLAG.RADIOTICKS," 时间片发送数据帧,当前RTC计数值: ",RTC->CNT," 时间为: ",RTC->CNT*1000/32768," MS ----->");
		else
			DEBUG_PRINT("KSDSDSDSK","<------ 射频已于第 ",gSYSFLAG.RADIOTICKS+32," 时间片发送数据帧,当前RTC计数值: ",RTC->CNT," 时间为: ",RTC->CNT*1000/32768," MS ----->");
		#endif
	}

	return false;
}
/************************************************************!
*Function: Radio_Int_Process
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_Int_Process(void)
{
	#ifdef RADIO_AES_ENCRYPTION
	uint8_t data[64];
	#endif

	si446x_get_int_status(0u,0u,0u);

	if (Si446xCmd.GET_INT_STATUS.PH_PEND&SI446X_CMD_GET_PH_STATUS_REP_PACKET_RX_PEND_BIT)
	{
		si446x_fifo_info(0u);    //读取RX_FIFO数据长度

		gSYSFLAG.CWS.RADIORECEIVECNT++;

		RadioPkt.LENS=Si446xCmd.FIFO_INFO.RX_FIFO_COUNT;

		si446x_read_rx_fifo(RadioPkt.LENS,&RadioPkt.DATA[0]);

		//DEBUG_PRINT_U8CHAR_ARRAY(RadioPkt.DATA,RadioPkt.LENS);

		#ifdef RADIO_AES_ENCRYPTION
		AES128_CBC_Decryption(RadioPkt.DATA,data,64);
		WriteDataCache(&RADIO_RXDC,data,RadioPkt.LENS);
		#else
		WriteDataCache(&RADIO_RXDC,RadioPkt.DATA,RadioPkt.LENS);
		#endif
	}

	if(Si446xCmd.GET_INT_STATUS.PH_PEND&SI446X_CMD_GET_PH_STATUS_REP_PACKET_SENT_PEND_BIT)
	{
		#ifdef RADIO_LDC_MODE
		si446x_set_property(0x00,0x01,0x04,0x42);
		#endif

		gSYSFLAG.CIS.RADIO=RX;
		Radio_StartRx(gECPPARA.RADIO_CHANNEL,0x40);

		#ifdef STATUS_LED1_RADIO_STATE_DEBUG
		Status_Led1_Clear();
		#endif

		if(RadioPkt.ENABLE==false)
			return;

		RadioPkt.ENABLE=false;
		Radio_TFAERFAF_Enable(0,0); //关闭TX_FIFO_ALMOST_EMPTY中断
		Radio_Change_Syncword(0xB42B); //将同步字改为0x2DD4
	}

	if(Si446xCmd.GET_INT_STATUS.PH_PEND&SI446X_CMD_GET_PH_STATUS_REP_TX_FIFO_ALMOST_EMPTY_PEND_BIT)
	{
		if(RadioPkt.REPEAT>0)
		{
			RadioPkt.REPEAT--;

			if(RadioPkt.REPEAT==0)
			{
				preamble[RadioPkt.TXTHRESH-2]=0x2D; //填充同步字1
				preamble[RadioPkt.TXTHRESH-1]=0xD4; //填充同步字2
			}

			si446x_write_tx_fifo(RadioPkt.TXTHRESH,preamble);
		}
		else
		{
			#ifdef RADIO_AES_ENCRYPTION
			AES128_CBC_Encryption(RadioPkt.DATA,data,64);
			si446x_write_tx_fifo(RadioPkt.LENS,data);
			#else
			si446x_write_tx_fifo(RadioPkt.LENS,RadioPkt.DATA);
			#endif

			RadioPkt.LENS=0;
		}
	}
}
/************************************************************!
*Function:  Radio_StartTx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_StartTx_Data(uint8_t *data,uint8_t lens)
{
	#ifdef RADIO_AES_ENCRYPTION
	uint8_t encrypt1[64];
	uint8_t encrypt2[64];

	memcpy(encrypt1,data,lens);
	AES128_CBC_Encryption(encrypt1,encrypt2,64);

	Radio_StartTx(gECPPARA.RADIO_CHANNEL,encrypt2,64);
	#else
	Radio_StartTx(gECPPARA.RADIO_CHANNEL,data,lens);
	#endif

	#ifdef STATUS_LED1_RADIO_STATE_DEBUG
	Status_Led1_Set();
	#endif
}
/************************************************************!
*Function:  Radio_ShortPkt_Tx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Radio_ShortPkt_Tx(uint8_t *data,uint8_t lens)
{
	if(gSYSFLAG.CIS.RADIO==TX)
		return false;

	gSYSFLAG.CIS.RADIO=TX;

	Radio_StartTx_Data(data,lens); //填充RADIO BUFFER 启动发送

	return true;
}
/************************************************************!
*Function:  Radio_WakeUp_StartTx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Radio_WakeUp_StartTx(uint8_t *data,uint8_t lens)
{
	while(gSYSFLAG.CIS.RADIO==TX);

	gSYSFLAG.CIS.RADIO=TX; //置发送标志位

	Radio_StartTx_Data(data,lens);

	return true;
}
/************************************************************!
*Function: Radio_WakeUp
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_WakeUp(uint8_t *data,uint8_t lens)
{
	while(gSYSFLAG.CIS.RADIO==TX); //等待RADIO数据发送完毕

	gSYSFLAG.CIS.RADIO=TX; //置发送标志位

	Radio_WakeUp_Tx(gECPPARA.RADIO_CHANNEL,90,data,lens); //发送时长为330MS
}
/************************************************************!
*Function: WeiHu_Queue_Rx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void WeiHu_Queue_Rx(void)
{
	uint8_t data=USART_Rx(UART1);

	WriteDataCache(&WEIHU_RXDC,&data,1);
}
/************************************************************!
*Function: WeiHu_Queue_Tx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void WeiHu_Queue_Tx(void)
{
	uint8_t data;

	while(DataCacheLength(&WEIHU_TXDC)>0)
	{
		ReadDataCache(&WEIHU_TXDC,&data,1);

		USART_Tx(UART1,data);
	}
}
/************************************************************!
*Function: WeiHu_StartTx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool WeiHu_StartTx(void)
{
	uint8_t data;

	if(gSYSFLAG.CIS.WEIHU==TX)
		return false;

	if(DataCacheLength(&WEIHU_TXDC)>0)
	{
		ReadDataCache(&WEIHU_TXDC,&data,1);

		USART_Tx(UART1,data);

		gSYSFLAG.CIS.WEIHU=TX;
	}

	return true;
}
/************************************************************!
*Function: WeiHu_ContinueTx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void WeiHu_ContinueTx(void)
{
	uint8_t data;

	if(DataCacheLength(&WEIHU_TXDC)>0)
	{
		ReadDataCache(&WEIHU_TXDC,&data,1);

		USART_Tx(UART1,data);
	}
	else
		gSYSFLAG.CIS.WEIHU=TXC;
}
/************************************************************!
*Function:
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gps_Queue_Rx(uint8_t data)
{
	if(Gps_Abstime_Process(data)==true) //如果接收到GPS数据,且校时成功
	{
		Gps_Power_Off();

		#ifdef STATUS_LED2_GPS_TIMING_DEBUG
		Status_Led2_Toggle();
		#endif
	}

	if((gUNIXTIME.UNIX-gSYSFLAG.STM.GPS_TIMING_TIME)>=120) //超时120S校时失败,取消校时
		Gps_Power_Off();
}
/************************************************************!
*Function: Gprs_Queue_Rx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gprs_Queue_Rx(void)
{
	uint8_t data;

	data=USART_Rx(UART0);

	WriteDataCache(&GPRS_RXDC,&data,1);
}
/************************************************************!
*Function: Gprs_Queue_Tx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gprs_Queue_Tx(void)
{
	uint8_t data;

	while(DataCacheLength(&GPRS_TXDC)>0)
	{
		ReadDataCache(&GPRS_TXDC,&data,1);

		USART_Tx(UART0,data);

		gSYSFLAG.CIS.GPRS=TX;
	}
}
/************************************************************!
*Function: Gprs_StartTx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
bool Gprs_StartTx(void)
{
	uint8_t data;

	if(gSYSFLAG.CIS.GPRS==TX)
		return false;

	if(DataCacheLength(&GPRS_TXDC)>0)
	{
		ReadDataCache(&GPRS_TXDC,&data,1);

		USART_Tx(UART0,data);

		gSYSFLAG.CIS.GPRS=TX;
	}

	return true;
}
/************************************************************!
*Function: Gprs_ContinueTx
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void Gprs_ContinueTx(void)
{
//	uint8_t data;
//
//	if(DataCacheLength(&GPRS_TXDC)>0)
//	{
//		ReadDataCache(&GPRS_TXDC,&data,1);
//
//		USART_Tx(UART0,data);
//	}
//	else
		gSYSFLAG.CIS.GPRS=TXC;
}

