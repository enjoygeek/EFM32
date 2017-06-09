/*************************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: radio_hal.h
*Version: 0.1.0
*Date: 2016-08-26
*Author: WANGJIANHUI
*Description:
*************************************************************************/
#ifndef RADIO_HAL_H
#define RADIO_HAL_H


#include "include.h"


/**********RADIO数据收发最大长度宏**********/
#define RADIO_MAX_PACKET_LENGTH     64
/**********CTS计数宏**********/
#define RADIO_CTS_TIMEOUT           10000


#define SILABS_RADIO_SI446X
#undef  SILABS_RADIO_SI4455

#define RADIO_DRIVER_EXTENDED_SUPPORT
#define RADIO_DRIVER_FULL_SUPPORT


#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "em_gpio.h"
#include "rf_spi.h"

#ifdef RADIO_LDC_MODE
#include "radio_config_ldc.h"
#else
#include "radio_config.h"
#endif

#include "si446x_defs.h"
#include "si446x_api_lib.h"
#include "si446x_patch.h"
/************************************************************!
*Function: RADIO数据接收结构体
*Description:此结构体是RADIO驱动层对外唯一的数据接收接口
*************************************************************/
typedef struct
{
	uint8_t LENS;   //接收到的数据长度
	uint8_t ENABLE;
	uint8_t REPEAT; //唤醒包发送时长
	uint8_t TXTHRESH;
	uint8_t DATA[RADIO_MAX_PACKET_LENGTH];
}RADIOPACKET;
/************************************************************!
*Function: tRadioConfiguration
*Description:此结构体用于RADIO初始化寄存器配置
*************************************************************/
typedef struct
{
    uint8_t   *Radio_ConfigurationArray;
    uint8_t   Radio_ChannelNumber;
    uint8_t   Radio_PacketLength;
    uint8_t   Radio_State_After_Power_Up;
    uint16_t  Radio_Delay_Cnt_After_Reset;
} tRadioConfiguration;

/************************************************************!
*Function: RADIO对外数据接口
*Description:
*************************************************************/
extern RADIOPACKET RadioPkt;
extern uint8_t preamble[64];
extern uint8_t Radio_Configuration_Data_Array[];
extern uint8_t radioCmd[16u];


void Radio_PowerUp(void);
void Radio_Init(void);
void Radio_StartRx(uint8_t channel,uint8_t lens);
void Radio_StartTx(uint8_t channel,uint8_t *data,uint8_t lens);
void Radio_WakeUp_Tx(uint8_t channel,uint8_t repeat,uint8_t *data,uint8_t lens);
void Enter_LDCRx_Mode(void);
void Exit_LDCRx_Mode(void);
void Radio_Set_LDC_Time(uint8_t wut_m_15_8,uint8_t wut_m_7_0,
		                uint8_t wut_r,uint8_t wut_ldc);
void Radio_Wait_Channel_Idle(void);
void Radio_Set_RSSI_Thresh(uint8_t thresh);
void Radio_Set_TXFIFOAE_Thresh(uint8_t thresh);
void Radio_Set_RXFIFOAF_Thresh(uint8_t thresh);
void Radio_Set_Preamble(uint8_t lens);
void Radio_TFAERFAF_Enable(uint8_t tfae,uint8_t rfaf);
void Radio_Field1_Length(uint16_t lens);
void Radio_Change_Syncword(uint16_t sw);
uint8_t Radio_Cmd(uint8_t *data,uint8_t lens);
void Radio_StartSleep(void);
void Radio_ReadyTx(void);
uint8_t radio_comm_GetResp(uint8_t byteCount, uint8_t* pData);
void radio_comm_SendCmd(uint8_t byteCount, uint8_t* pData);
void radio_comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData);
void radio_comm_WriteData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData);
uint8_t radio_comm_PollCTS(void);
void radio_comm_ClearCTS();
uint8_t radio_comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t* pCmdData, uint8_t respByteCount, uint8_t* pRespData);
void radio_hal_AssertShutdown(void);
void radio_hal_DeassertShutdown(void);
void radio_hal_ClearNsel(void);
void radio_hal_SetNsel(void);
uint8_t radio_hal_NirqLevel(void);
void radio_hal_SpiWriteByte(uint8_t byteToWrite);
uint8_t radio_hal_SpiReadByte(void);
void radio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData);
void radio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData);
uint8_t radio_hal_Gpio0Level(void);
uint8_t radio_hal_Gpio1Level(void);
uint8_t radio_hal_Gpio2Level(void);
uint8_t radio_hal_Gpio3Level(void);


#endif

