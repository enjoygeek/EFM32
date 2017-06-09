/*************************************************************************!
*Copyright (c), 2008-2016, Qingdao Topscomm Communication Co .Ltd.
*File name: radio_hal.c
*Version: 0.1.0
*Date: 2016-08-26
*Author: WANGJIANHUI
*Description:
*************************************************************************/
#include "radio_hal.h"
#include "include.h"


/**********RADIO数据收发数组结构体**********/
RADIOPACKET RadioPkt;
uint8_t preamble[64];  //前导填充字节
uint8_t ctsWentHigh=0;
uint8_t Radio_Configuration_Data_Array[]=RADIO_CONFIGURATION_DATA_ARRAY;
tRadioConfiguration RadioConfiguration=RADIO_CONFIGURATION_DATA;
tRadioConfiguration *pRadioConfiguration=&RadioConfiguration;

/************************************************************!
*Function: Radio_PowerUp
*Description: RADIO硬件上电复位
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_PowerUp(void)
{
	si446x_reset();

	for(uint32_t i=0;i<0xFFFF;i++);
}
/************************************************************!
*Function: Radio_Init
*Description: RADIO硬件上电复位，并将radio_config.h生成的寄存器初
*             始化数组发送至RADIO，若初始化失败，重新上电复位初始化。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_Init(void)
{
	Spi_Init();  //SPI驱动

	Radio_PowerUp(); //上电复位

	while(SI446X_SUCCESS!=si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
	{
		for(uint32_t i=0;i<10000;i++);
		Radio_PowerUp();
	}

	si446x_get_int_status(0u,0u,0u);
}
/************************************************************!
*Function: Radio_StartRx
*Description: RADIO开始接收数据函数
*Input: 1.channel：接收数据的信道
*       2.lens：待接收的数据长度
*Output: null
*Return: null
*************************************************************/
void Radio_StartRx(uint8_t channel,uint8_t lens)
{
	#ifdef RADIO_LDC_MODE
	//si446x_set_property(0x00,0x01,0x04,0x42); //开启WUT_LDC
	#endif

	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT); //复位FIFO

	si446x_get_int_status(0u,0u,0u);

	#ifdef RADIO_LDC_MODE
	si446x_start_rx(channel,0u,lens,
			      SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_RX,
				  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_SLEEP,
				  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX);
	#else
	si446x_start_rx(channel,0u,lens,
                  SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_RX,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX);
	#endif
}
/************************************************************!
*Function: Radio_StartTx
*Description: RADIO数据发送函数
*Input: 1.channel：发送数据的信道
*       2.data： 待发送数据的首地址
*       3.lens：待发送数据的长度
*Output: null
*Return: null
*************************************************************/
void Radio_StartTx(uint8_t channel,uint8_t *data,uint8_t lens)
{
	#ifdef RADIO_LDC_MODE
	si446x_set_property(0x00,0x01,0x04,0x02); //关闭LDC使能,不需要关闭WUT
	#endif

	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT); //复位FIFO
	si446x_get_int_status(0u,0u,0u);
	si446x_write_tx_fifo(lens,data);
	si446x_start_tx(channel,0x30,lens);
}
/************************************************************!
*Function: Radio_WakeUp_Tx
*Description: 超长前导唤醒帧发送函数
*Input: 1.channel: 通道
*       2. repeat: 发送时间为repeat*2MS
*       3.data:  发送数据的首地址
*       4.lens: 发送数据的长度
*Output: null
*Return: 函数运行状态
*************************************************************/
void Radio_WakeUp_Tx(uint8_t channel,uint8_t repeat,uint8_t *data,uint8_t lens)
{
	RadioPkt.TXTHRESH=50; //TX_FIFO_ALMOST_EMPTY=50

	memset(preamble,0xAA,64); //填充前导码

	RadioPkt.LENS=lens;
	RadioPkt.ENABLE=true;
	RadioPkt.REPEAT=repeat-1; //一定要减1

	memcpy(RadioPkt.DATA,data,lens);

	#ifdef RADIO_LDC_MODE
	si446x_set_property(0x00,0x01,0x04,0x02); //关闭LDC使能,不需要关闭WUT
	#endif

	Radio_Set_TXFIFOAE_Thresh(RadioPkt.TXTHRESH); //设置门限为50字节,200kbps下为2ms

	Radio_TFAERFAF_Enable(1,0); //打开TX_FIFO_ALMOST_EMPTY中断

	Radio_Change_Syncword(0x5555); //将同步字改为0xAA

	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT);

	si446x_get_int_status(0u,0u,0u);

	si446x_write_tx_fifo(RadioPkt.TXTHRESH,preamble);

	si446x_start_tx(channel,0x30,RadioPkt.TXTHRESH*repeat+lens); //此处的长度一定要与发送的长度一致！！
}
/************************************************************!
*Function: Enter_LDCRx_Mode
*Description: RADIO进入LDC接收数据模式,重新配置LDC各寄存器值。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Enter_LDCRx_Mode(void)
{
	si446x_set_property(0x00,0x01,0x04,0x42); //开启WUT_LDC
}
/************************************************************!
*Function: Exit_LDCRx_Mode
*Description: 退出LDC接收数据模式。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Exit_LDCRx_Mode(void)
{
	si446x_set_property(0x00,0x01,0x04,0x02); //关闭LDC使能,不需要关闭WUT
}
/************************************************************!
*Function: Radio_Set_LDC_Time
*Description: RADIO设置LDC的RX和SLEEP周期。各参数配置请参考数据手册。
*Input:
*Output: null
*Return: null
*************************************************************/
void Radio_Set_LDC_Time(uint8_t wut_m_15_8,uint8_t wut_m_7_0,
		                uint8_t wut_r,uint8_t wut_ldc)
{
	si446x_set_property(0x00,0x01,0x04,0x00);

	si446x_set_property(0x00,0x04,0x05,wut_m_15_8,wut_m_7_0,wut_r,wut_ldc);
}
/************************************************************!
*Function: Radio_Wait_Channel_Idle
*Description: RADIO等待信道是否空闲。GPIO管脚为高电平时判定信道被占
*             用，为低电平时则认为信道空闲，可以发送数据，此函数需要
*             配合Set_RSSI_Thresh函数使用，需要先设置RSSI的阈值。
*             此函数会比对RSSI阈值判断信道是否空闲。
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_Wait_Channel_Idle(void)
{
	while(radio_hal_Gpio1Level()==true);
}
/************************************************************!
*Function: Radio_Set_RSSI_Thresh
*Description: RADIO设置RSSI阈值,用于信道检测的阈值判断。
*Input: thresh：RSSI阈值
*Output: null
*Return: null
*************************************************************/
void Radio_Set_RSSI_Thresh(uint8_t thresh)
{
	si446x_set_property(0x20,0x01,0x4a,thresh);
}
/************************************************************!
*Function: Radio_Set_TXFIFOAE_Thresh
*Description: RADIO设置TX_FIFO_ALMOST_EMPTY阈值。
*Input: thresh：阈值
*Output: null
*Return: null
*************************************************************/
void Radio_Set_TXFIFOAE_Thresh(uint8_t thresh)
{
	si446x_set_property(0x12,0x01,0x0b,thresh);
}
/************************************************************!
*Function: Radio_Set_RXFIFOAF_Thresh
*Description: RADIO设置RX_FIFO_ALMOST_FULL阈值。
*Input: thresh：阈值
*Output: null
*Return: null
*************************************************************/
void Radio_Set_RXFIFOAF_Thresh(uint8_t thresh)
{
	si446x_set_property(0x12,0x01,0x0c,thresh);
}
/************************************************************!
*Function: Radio_Set_Preamble
*Description: RADIO设置前导码标准格式及长度。
*Input: 2.lens：前导码长度,单位byte
*Output: null
*Return: null
*************************************************************/
void Radio_Set_Preamble(uint8_t lens)
{
	si446x_set_property(0x10,0x01,0x00,lens);
}
/************************************************************!
*Function: Radio_TFAERFAF_Enable
*Description: RADIO设置发送FIFO门限空使能和接收FIFO门限满使能
*Input: 1.tfae: 发送FIFO门限使能
*       2.rfaf: 接收FIFO门限使能
*Output: null
*Return: null
*************************************************************/
void Radio_TFAERFAF_Enable(uint8_t tfae,uint8_t rfaf)
{
	uint8_t data=0x38; //使能发送中断,接收中断,CRC错误中断

	data=((tfae==1)?(data|0x02):(data&0xFD));

	data=((rfaf==1)?(data|0x01):(data&0xFE));

	si446x_set_property(0x01,0x01,0x01,data);
}
/************************************************************!
*Function: Radio_Field1_Length
*Description: RADIO设置FIELD1域长度
*Input: 1.lens: FIELD1域长度
*Output: null
*Return: null
*************************************************************/
void Radio_Field1_Length(uint16_t lens)
{
	if(lens>=0x1FFF) //8191字节
		while(1);

	si446x_set_property(0x12,0x02,0x0d,(uint8_t)(lens>>8),(uint8_t)lens);
}
/************************************************************!
*Function: Radio_Change_Syncword
*Description: RADIO设置同步字格式。
*Input: 1.sw: 同步字
*Output: null
*Return: null
*************************************************************/
void Radio_Change_Syncword(uint16_t sw)
{
	si446x_set_property(0x11,0x04,0x01,(uint8_t)((sw>>8)&0xFF),(uint8_t)(sw&0xFF),0x00,0x00);
}
/************************************************************!
*Function: Radio_Cmd
*Description: RADIO命令设置
*Input: 1.data：数据首地址
*       2.lens：数据长度
*Output: null
*Return: 函数运行状态
*************************************************************/
uint8_t Radio_Cmd(uint8_t *data,uint8_t lens)
{
	if (radio_comm_SendCmdGetResp(lens,data,0,0)!=0xFF)
		return false;

	return true;
}
/************************************************************!
*Function: Radio_StartSleep
*Description: RADIO睡眠状态设置
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_StartSleep(void)
{
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
}
/************************************************************!
*Function: Radio_ReadyTx
*Description: RADIO数据发送状态设置
*Input: null
*Output: null
*Return: null
*************************************************************/
void Radio_ReadyTx(void)
{
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);
}
/************************************************************!
*Function: radio_comm_GetResp
*Description: RADIO得到应答数据函数
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_comm_GetResp(uint8_t byteCount, uint8_t* pData)
{
	uint8_t ctsVal = 0;
	uint16_t errCnt = RADIO_CTS_TIMEOUT;

	while (errCnt != 0)      //wait until radio IC is ready with the data
	{
		radio_hal_ClearNsel();
		radio_hal_SpiWriteByte(0x44);    //read CMD buffer
		ctsVal = radio_hal_SpiReadByte();
		if (ctsVal == 0xFF)
		{
			if (byteCount)
			{
				radio_hal_SpiReadData(byteCount, pData);
			}
			radio_hal_SetNsel();
			break;
		}
		radio_hal_SetNsel();
		errCnt--;
	}
	if (errCnt == 0)
	{
		while(1);
	}
	if (ctsVal == 0xFF)
	{
		ctsWentHigh = 1;
	}
	return ctsVal;
}
/************************************************************!
*Function: radio_comm_SendCmd
*Description: RADIO发送命令数据函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_comm_SendCmd(uint8_t byteCount, uint8_t* pData)
{
	/* There was a bug in A1 hardware that will not handle 1 byte commands.
	It was supposedly fixed in B0 but the fix didn't make it at the last minute, so here we go again */
    if (byteCount == 1)
    	byteCount++;

	while (!ctsWentHigh)
    {
		radio_comm_PollCTS();
    }
    radio_hal_ClearNsel();
    radio_hal_SpiWriteData(byteCount, pData);
    radio_hal_SetNsel();
    ctsWentHigh = 0;
}
/************************************************************!
*Function: radio_comm_ReadData
*Description: RADIO 读取命令数据函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
{
	if(pollCts)
	{
		while(!ctsWentHigh)
        {
			radio_comm_PollCTS();
        }
    }
    radio_hal_ClearNsel();
    radio_hal_SpiWriteByte(cmd);
    radio_hal_SpiReadData(byteCount, pData);
    radio_hal_SetNsel();
    ctsWentHigh = 0;
}
/************************************************************!
*Function: radio_comm_WriteData
*Description: RADIO写命令数据函数
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_comm_WriteData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
{
	if(pollCts)
	{
		while(!ctsWentHigh)
        {
			radio_comm_PollCTS();
        }
    }
	radio_hal_ClearNsel();
    radio_hal_SpiWriteByte(cmd);
    radio_hal_SpiWriteData(byteCount, pData);
    radio_hal_SetNsel();
    ctsWentHigh = 0;
}
/************************************************************!
*Function: radio_comm_PollCTS
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_comm_PollCTS(void)
{
	#ifdef RADIO_USER_CFG_USE_GPIO1_FOR_CTS
		while(!radio_hal_Gpio1Level())
		{
			/* Wait...*/
		}
		ctsWentHigh = 1;
		return 0xFF;
	#else
		return radio_comm_GetResp(0, 0);
	#endif
}
/************************************************************!
*Function: radio_comm_ClearCTS
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_comm_ClearCTS()
{
	ctsWentHigh = 0;
}
/************************************************************!
*Function: radio_comm_SendCmdGetResp
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t* pCmdData, uint8_t respByteCount, uint8_t* pRespData)
{
	radio_comm_SendCmd(cmdByteCount, pCmdData);
	return radio_comm_GetResp(respByteCount, pRespData);
}
/************************************************************!
*Function: radio_hal_AssertShutdown
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_AssertShutdown(void)
{
	GPIO_PinOutSet(RF_SDN_PORT, RF_SDN_PIN);
}
/************************************************************!
*Function: radio_hal_DeassertShutdown
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_DeassertShutdown(void)
{
	GPIO_PinOutClear(RF_SDN_PORT, RF_SDN_PIN);
}
/************************************************************!
*Function: radio_hal_ClearNsel
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_ClearNsel(void)
{
    GPIO_PinOutClear(SPI_CS_PORT, SPI_CS_PIN);
}
/************************************************************!
*Function: radio_hal_SetNsel
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_SetNsel(void)
{
	GPIO_PinOutSet(SPI_CS_PORT, SPI_CS_PIN);
}
/************************************************************!
*Function: radio_hal_NirqLevel
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_NirqLevel(void)
{
	return GPIO_PinInGet(RF_NIRQ_PORT, RF_NIRQ_PIN);
}
/************************************************************!
*Function: radio_hal_SpiWriteByte
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_SpiWriteByte(uint8_t byteToWrite)
{
	Spi_ReadWrite(byteToWrite);
}
/************************************************************!
*Function: radio_hal_SpiReadByte
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_SpiReadByte(void)
{
	return Spi_ReadWrite(0x00);
}
/************************************************************!
*Function: radio_hal_SpiWriteData
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData)
{
	Spi_WriteDataBurst(byteCount, pData);
}
/************************************************************!
*Function: radio_hal_SpiReadData
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
void radio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData)
{
	Spi_ReadDataBurst(byteCount, pData);
}
/************************************************************!
*Function: radio_hal_Gpio0Level
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
#ifdef RADIO_DRIVER_EXTENDED_SUPPORT
uint8_t radio_hal_Gpio0Level(void)
{
	return GPIO_PinInGet(RF_GPIO0_PORT, RF_GPIO0_PIN);
}
/************************************************************!
*Function: radio_hal_Gpio1Level
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_Gpio1Level(void)
{
    return GPIO_PinInGet(RF_GPIO1_PORT, RF_GPIO1_PIN);
}
/************************************************************!
*Function: radio_hal_Gpio2Level
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_Gpio2Level(void)
{
	return GPIO_PinInGet(RF_GPIO2_PORT, RF_GPIO2_PIN);
}
/************************************************************!
*Function: radio_hal_Gpio3Level
*Description:
*Input: null
*Output: null
*Return: null
*************************************************************/
uint8_t radio_hal_Gpio3Level(void)
{
	return GPIO_PinInGet(RF_GPIO3_PORT, RF_GPIO3_PIN);
}
#endif

