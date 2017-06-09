/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: include.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef INCLUDE_H_
#define INCLUDE_H_


#define SYS_VERSION "SOFTWARE_VERSION: 0.7.11_201706021823"

//射频工作模式
#define RADIO_LDC_MODE 1
//射频发送加密函数,开启:加密,关闭:不加密
//#define RADIO_AES_ENCRYPTION 1
//定义湖南初始化链路部分程序
//#define HUNAN_101INIT 1

//ESAM功能开关,开启:关闭ESAM,关闭:开启ESAM功能
#define ESAMC_FUNCTION_CTRL

#define DEFAULT_FAULT_WAVE_NUM 0x6A
//系统调试信息宏定义
#define ABOS_DEBUG 1

#ifdef ABOS_DEBUG

//打印系统初始化调试信息,开启:打印,关闭:不打印
#define PRINT_SYS_INIT_DEBUG 1

//默认参数设置,开启:默认参数,关闭:MX25参数
#define DEFAULT_PARA_DEBUG 1

//打印ICP数据帧解析信息,开启:打印,关闭:不打印
#define PRINT_ICP_FRAME_PARSE_DEBUG 1

//打印擦除MX25信息,开启:打印,关闭:不打印
#define PRINT_MX25_ERASE_DEBUG 1

//打印ESAM调试信息,开启:打印,关闭:不打印
//#define PRINT_ESAM_INFO_DEBUG 1

//RTOS任务堆栈空间监控,开启:监控,关闭:不监控
#define TASKSTACK_HIGHWATERMARK  1

//LED1作为射频发送显示,开启:显示,关闭:不显示
#define STATUS_LED1_RADIO_STATE_DEBUG 1

//LED1作为系统内狗复位显示,开启:显示,关闭:不显示
#define STATUS_LED1_SYS_RESET_DEBUG 1

//LED2作为链路状态显示,开启:显示,关闭:不显示
//#define STATUS_LED2_ECP_LINKSTATE_CONTROL 1

//LED2作为系统内狗复位显示,开启:显示,关闭:不显示
#define STATUS_LED2_SYS_RESET_DEBUG 1

//LED2作为采集单元掉线显示,开启:显示,关闭:不显示
//#define STATUS_LED2_AU_TIMING_ASYNC 1

//LED2作为GPS校时成功翻转显示,开启:显示,关闭:不显示
//#define STATUS_LED2_GPS_TIMING_DEBUG 2

//打印采集单元心跳信息,开启:打印,关闭:不打印
#define PRINT_AU_ONLINE_OFFLINE_DEBUG 1

//统计采集单元校时应答,开启:统计,关闭:不统计
#define PRINT_AU_TIMING_ASYNC_DEBUG 1

//打印采集单元时间戳时标,开启:打印,关闭:不打印
//#define PRINT_AUTIMING_TIMESTAMP_DEBUG 1

//打印采集单元工作模式切换信息,开启:打印,关闭:不打印
#define PRINT_AU_WORKMODE_DEBUG 1

//打印系统电压信息,开启:打印,关闭:不打印
#define PRINT_SYS_VOL_DEBUG 1

//打印触发录波调试信息,开启:打印,关闭:不打印
#define PRINT_TRIGGER_CALL_WAVE_DEBUG 1

//打印采集单元遥测数据掉线统计,开启:打印,关闭:不打印
#define AU_YC_LOST_STATS_DEBUG 1

//打印遥测上报信息,开启:打印,关闭:不打印
//#define PRINT_YC_REPORT_DEBUG 1

//打印采集单元遥测数据,开启:打印,关闭:不打印
#define PRINT_AU_YC_DATA_DEBUG 1

//打印MX25 日志信息,开启:打印,关闭:不打印
#define PRINT_MX25_LOG_DEBUG

//打印MX25 SOE信息,开启:打印,关闭:不打印
#define PRINT_MX25_SOE_DEBUG 1

//打印MX25 SOE信息,开启:打印,关闭:不打印
#define PRINT_MX25_YK_DEBUG 1

//打印MX25 FAULT信息,开启:打印,关闭:不打印
#define PRINT_MX25_FAULT_DEBUG 1

//打印ECP遥控信息,开启:打印,关闭:不打印
#define PRINT_ECP_YK_DEBUG 1

//打印采集单元录波数据帧,开启:打印,关闭:不打印
//#define PRINT_AU_WAVE_DATA_DEBUG 1

//打印故障电压电流波形数据,开启:打印,关闭:不打印
//#define PRINT_WAVE_DEBUG 1

//打印波形文件信息,开启:打印,关闭:不打印
//#define PRINT_WAVE_FILE_DEBUG 1

//打印遥测文件数据,开启:打印,关闭:不打印
//#define PRINT_YC_FILE_DEBUG 1

//打印遥测上报数据,开启:打印,关闭:不打印
#define PRINT_YC_DEPORT_DEBUG 1

//打印编程调试信息,开启:打印,关闭:不打印
#define PRINT_PROGRAM_DEBUG 1

//GPS校时测试,开启:测试,关闭:不测试
//#define PRINT_GPS_TIMMING_INFO_DEBUG 1

//打印矫正ABC三相电流矫正调试信息,开启:打印,关闭:不打印
#define PRINT_CORRECT_PHASE_DEBUG 1

//打印ECP接收数据帧,开启:打印,关闭:不打印
//#define DEBUG_ECP_RECEIVE_FRAME_PRINT 1

//打印ECP发送数据帧,开启:打印,关闭:不打印
//#define DEBUG_ECP_SEND_FRAME_PRINT 1

//ECP链路控制,开启:关闭链路控制,关闭:开启链路控制
//#define ECP_LINKSTATE_CONTROL 1

//打印链路状态信息,开启:打印,关闭:不打印
#define PRINT_LINKSTATE_DEBUG 1

//打印SOE上报主站信息,开启:打印,关闭:不打印
//#define ECP_SOE_REPORT_DEBUG 1

//打印主站校时信息,开启:打印,关闭:不打印
#define PRINT_ECP_TIMING_DEBUG 1

//打印文件上报信息,开启:打印,关闭:不打印
#define PRINT_ECP_READ_FILE_DEBUG 1

#endif


/***************************系统参数宏定义***********************************/
#define AU_TIMING_ASYNC_BUFFER      2      //校时异步缓冲:只有连续N次没有收到校时ACK才对该线进行唤醒
#define AU_ASYNC_ENTER_LPOWER_MODE  12     //连续丢失N次校时应答,该线进入低功耗模式


/**********标准C文件及库函数**********/
#include <stddef.h>
#include <string.h>
#include "em_chip.h"
#include "em_device.h"
/**********操作系统头文件**********/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
/**********TASK层头文件**********/
#include "init.h"
#include "mytask.h"
/**********MID层头文件**********/
#include "datacache.h"
#include "database.h"
#include "flashstorage.h"
#include "fault_algorithm.h"
#include "unixtime.h"
#include "systime.h"
#include "mid.h"
#include "print.h"
/**********PROCOTOL层头文件**********/
#include "icp.h"
#include "icp_parse.h"
#include "icp_process.h"
#include "icp_app.h"
#include "ecp.h"
#include "ecp_parse.h"
#include "ecp_process.h"
#include "ecp_app.h"
#include "esam.h"
#include "esam_parse.h"
#include "esam_process.h"
#include "esam_app.h"
/**********SYSDEF头文件**********/
#include "sysdef.h"
/**********射频驱动头文件**********/
#include "radio_hal.h"
#include "rf_spi.h"
/**********底层驱动头文件**********/
#include "hal_esam.h"
#include "hal_adc.h"
#include "hal_cmu.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "hal_gps.h"
#include "hal_math.h"
#include "hal_mx25.h"
#include "hal_r8025t.h"
#include "hal_timer.h"
#include "hal_usart.h"
#include "hal_wdog.h"
#include "hal_msc.h"
#include "hal_i2c.h"
#include "hal_rtc.h"
#include "hal_aes.h"
#include "hal_emu.h"
#include "em_rmu.h"


#endif

