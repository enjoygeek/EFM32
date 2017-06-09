/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: database.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef DATABASE_H_
#define DATABASE_H_


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "include.h"


/**********汇集单元带载的最大线数(采集单元组数)**********/
#define LINEGROUP_MAXNUM     3
#define SYSPARA_NUM          (((1+3*6)*LINEGROUP_MAXNUM+1)+18+1)
#define ECPPARA_NUM          15
#define ECPYX_NUM            (10+((3*8)*LINEGROUP_MAXNUM))  //此处注意修改总召遥信排序函数!!!!!!!!!
#define ECPYC_NUM            (2+((3*5)*LINEGROUP_MAXNUM))  //此处注意修改总召遥测排序函数!!!!!!!!!
#define ECPINFO_LEN          (2*ECPYX_NUM+3*ECPYC_NUM)
/**********射频发送数组**********/
#define MAX_ICPFRAME_NUM     5
#define MAX_RADIO_FRAME      5
#define MAX_SOE_NUM          10
/**********采集单元录波数据的最大长度(字节)**********/
#define AU_ADC_SAMPLE        82
#define MAX_WAVE_NUM         16
#define MAX_WAVE_LEN         (AU_ADC_SAMPLE*MAX_WAVE_NUM)
#define WAVE_FRAME_LEN       104
/**********故障信息结构体的最大长度(字节)**********/
#define FAULT_DATA_LEN       ((16*MAX_WAVE_LEN)+8+9)


#pragma pack(1)
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA;
	struct
	{
		uint8_t U03:4;
		uint8_t U47:4;
	};
}SINGLEBYTE;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint16_t   U16;
	uint8_t    U8[2];
	SINGLEBYTE U4[2];
}DOUBLEBYTE;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint32_t   U32;
	uint8_t    U8[4];
	SINGLEBYTE U4[4];
	DOUBLEBYTE U16[2];
}FOURBYTE;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[8];
	struct
	{
		uint32_t CNT;
		uint32_t UNIX;
	};
}UNIXTIME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t ADDR[6]; //采集单元地址
}AU_ADDR;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t LINENO;  //采集单元所在的线号
	AU_ADDR AU[3];   //三相采集单元
}LINE_UNIT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t   LINENUM;                 //有效的采集单元组数
	LINE_UNIT LINE[LINEGROUP_MAXNUM];  //采集单元组
}LINE_GROUP;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[SYSPARA_NUM];
	struct
	{
		LINE_GROUP LINEGROUP;        //线组地址参数,共3*(3*6+1)+1=58字节
		uint8_t    BENJI[6];         //本机地址
		uint8_t    WEIHU[6];         //维护地址
		uint8_t    BROADCAST[6];     //广播地址
		uint8_t    RADIO_CHANNEL;    //射频通道
	};
}SYSPARA;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[ECPPARA_NUM*2];
	struct
	{
		uint16_t YC_REPORT_CYCLE;              //遥测主动上报周期
		uint16_t GPS_TIMING_MODE;              //GPS校时模式
		uint16_t GPS_TIMING_CYCLE;             //GPS校时周期
		uint16_t BATT_VOL_THRESHOLD;           //汇集单元电池电压阈值
		uint16_t SUN_VOL_REPORT_THRESHOLD;     //太阳能电压上报阈值
		uint16_t BATT_VOL_REPORT_THRESHOLD;    //汇集单元电池电压上报阈值
		uint16_t I_REPORT_THRESHOLD;           //电流上报阈值
		uint16_t U_REPORT_THRESHOLD;           //电压上报阈值
		uint16_t T_REPORT_THRESHOLD;           //温度上报阈值
		uint16_t B_REPORT_THRESHOLD;           //采集单元电池电压上报阈值
		DOUBLEBYTE ZIZHAN;                     //子站地址,2字节
		uint16_t FAULT_WAVE_NUM;               //故障波形数量
		uint16_t YC_REPORT_MODE;               //遥测上报模式
		uint16_t RADIO_CHANNEL;                //射频通道号
		uint16_t AU_OFFLINE_TIME;              //采集单元掉线时间
	};
}ECPPARA;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[27];
	struct
	{
		uint16_t TFMIN;
		uint16_t TFMAX;
		uint16_t IFAULTSHORT;
		uint16_t IFAULTEARTH;
		uint16_t TEND;
		uint16_t IMIN;
		uint16_t UDOWNRATIO;
		uint8_t  FAULT_RESET_TIME;
		uint8_t  LINEPHASE;
		uint8_t  YC_PERIOD;
		uint8_t  FAULT_RESETTIME_HH;
		uint8_t  FAULT_RESETTIME_MM;
		uint8_t  CUADDR[6];
		DOUBLEBYTE MSGADDR;
	};
}AU_PARA;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t  SYS_STATUS;
	uint8_t  POWER_SWITCH;   //电源切换标志
	uint8_t  ISCALLWAVE;     //召唤波形标志
	uint8_t  SYS_EMEN;       //系统EM模式标志
	uint8_t  UPDATING;       //程序更新标志
	uint8_t  FAULTWRG;       //启动录波标志
	uint8_t  CHARGE_STATUS;
	uint8_t  RUN_LED_STATUS; //运行指示灯状态
	uint32_t RADIOTICKS;     //射频时间片计数
	uint32_t RADIORECEIVECNT;//射频接收中断计数
	uint32_t ICPSTATUS;
	uint32_t ECPSTATUS;
	uint32_t ICPRUNCOUNT;
	uint32_t ECPRUNCOUNT;
}CU_WORK_STATUS;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint16_t SUN;  //太阳能电池电压
	uint16_t XDC;  //蓄电池电池电压
	uint16_t CAP;  //超级电容电池电压
}CU_SYS_VOL;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t  GPRS;    //GPRS端口状态标志
	uint8_t  WEIHU;   //维护端口状态标志
	uint8_t  RADIO;   //射频端口状态标志
}CU_INTERFACE_STATUS;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint32_t  AU_TIMING_ACK;      //采集单元校时应答统计
	uint32_t  AU_TIMING_ASYNC;    //采集单元校时异步统计
	uint32_t  AU_ENTER_LOW_POWER; //采集单元
    uint32_t  AU_WORKMODE_ACK;    //采集单元工作模式应答统计
    uint32_t  WORKMODE;           //采集单元组的工作模
    uint8_t   WORKMODE_SWITCH;
	uint8_t   AU_YC_REPORT;       //采集单元遥测上报统计
	uint8_t   AU_WORKMODE[3];
	uint32_t  AU_OFFLINE[3];
}AU_STATUS_FLAG;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	AU_STATUS_FLAG  ASF[LINEGROUP_MAXNUM]; //采集单元状态标志
}AU_WORK_STATUS;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint32_t  AU_OFFLINE_MARK[3];    //采集单元掉线时间
	uint32_t  AU_WORKMODE_MARK[3];   //采集单元工作模式切换接收时间
	uint32_t  AU_SOE_TIME[3];        //采集单元SOE接收时间
}SYS_TIME_COUNT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint32_t       YC_RECEIVE;          //遥测接收时间
	uint32_t       AU_TIMING_TIME;      //采集单元校时时刻的计时
	uint32_t       YC_RECORD_TIME;      //采集单元遥测数据纪录时刻
	uint32_t       YC_REPORT_TIME;      //遥测上报记录时间
	uint32_t       GPS_TIMING_TIME;     //GPS校时时间计时
	uint32_t       SYSFLAG_PRINT_TIME;  //系统标志打印时间
	uint32_t       CU_STOP_CHARGE_TIME; //
	SYS_TIME_COUNT STC[LINEGROUP_MAXNUM];
}SYS_TIME_MARK;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint32_t  LOG;   //日志写数据指针
	uint32_t  SOE;   //遥信写数据指针
	uint32_t  YC;    //遥测写数据指针
    uint32_t  YK;    //遥控写数据指针
    uint32_t  FAULT; //故障写数据指针
    uint32_t  WAVE;  //波形写数据指针
}MX25FLASH_WRITE;

typedef struct
{
	uint8_t REMOTE_RANDOM[16];
	uint8_t TOOL_RANDOM[16];
	uint8_t  REMOTE_VERI;
    uint8_t  TOOLID[8];
	uint8_t  TOOL_VERI;
}ESAM1161Y_PARA;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	CU_WORK_STATUS         CWS;   //汇集单元工作状态
	CU_SYS_VOL             CBV;   //汇集单元系统电压
	CU_INTERFACE_STATUS    CIS;   //汇集单元接口状态
	AU_WORK_STATUS         AWS;   //采集单元工作状态
	MX25FLASH_WRITE        MFW;   //MX25写数据指针
	SYS_TIME_MARK          STM;   //系统计时标志
	ESAM1161Y_PARA         EYP;   //ESAM随机数
}SYSFLAG;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	DOUBLEBYTE ADDR; //信息体地址
	DOUBLEBYTE MSG;  //信息数据
}ECPDATA_YX;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	ECPDATA_YX  CUR_OVER[3];        //过流
	ECPDATA_YX  COMM_FAULT[3];      //通信故障
	ECPDATA_YX  SHUNSHI_FAULT[3];   //瞬时性短路
	ECPDATA_YX  YONGJIU_FAULT[3];   //永久性短路
	ECPDATA_YX  AU_LOW_POWER[3];    //采集单元电池欠压
	ECPDATA_YX  AU_POWER_SWITCH[3]; //采集单元电池切换
	ECPDATA_YX  FUHE_OVER[3];       //负荷越限
	ECPDATA_YX  ZHONGZAI[3];        //重载
}ECP_YX;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint16_t DATA[ECPYX_NUM*2];
	struct
	{
		ECPDATA_YX  STATUS;                  //汇集单元状态
		ECPDATA_YX  LOW_POWER;               //汇集单元低电压
		ECPDATA_YX  POWER_SWITCH;            //后备电源投入
		ECPDATA_YX  RF_FAULT;                //小无线模块故障
		ECPDATA_YX  GPS_LOCK;                //GPS锁定标志
		ECPDATA_YX  WAVEFILE;                //新录波文件生成
		ECPDATA_YX  ALL_FAULT;               //相线故障
		ECPDATA_YX  LINE_WORKMODE[3];

		ECP_YX      ECPYX[LINEGROUP_MAXNUM]; //采集单元组遥信数据
	};
}ECP_YX_DATA;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	DOUBLEBYTE ADDR; //信息体地址
	DOUBLEBYTE MSG;  //信息数据
	DOUBLEBYTE QDS;  //品质描述词
}ECPDATA_YC;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	ECPDATA_YC CUR[3];         //采集单元电流
	ECPDATA_YC VOL[3];         //采集单元电压
	ECPDATA_YC TEM[3];         //采集单元温度
	ECPDATA_YC AU_BAT_VOL[3];  //采集单元电池电压
	ECPDATA_YC AU_GET_VOL[3];  //采集单元取电电压
}ECP_YC;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint16_t DATA[ECPYC_NUM*3];
	struct
	{
		ECPDATA_YC SUN_VOL;                 //太阳能板电压
		ECPDATA_YC BAT_VOL;                 //电池电压
		ECP_YC     ECPYC[LINEGROUP_MAXNUM]; //采集单元组遥测数据
	};
}ECP_YC_DATA;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint16_t DATA[ECPINFO_LEN];
	struct
	{
		ECP_YX_DATA YX;  //遥信数据
		ECP_YC_DATA YC;  //遥测数据
	};
}ECPINFO;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA;
	struct
	{
		uint8_t DAY:5;  //日
		uint8_t WEEK:3;	//星期
	};
}P101DAY;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[7];
	struct
	{
		uint16_t MS;
		uint8_t  MINUTE;
		uint8_t  HOUR;
		P101DAY  DAY;
		uint8_t  MONTH;
		uint8_t  YEAR;
	};
}P101TIME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[139];
	struct
	{
		uint8_t   LENS;
		uint8_t   TYPE;
		P101TIME  TIME;
		char      ASCII[130]; //协议规定ASCII最长不超过128字节+CRLF
	};
}LOGRECORD;
/************************************************************!
*Function: WAVE_DAT_FORMAT结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[26];
	struct
	{
		uint32_t N;
		uint32_t TIMESTAMP;
		uint16_t AU;
		uint16_t BU;
		uint16_t CU;
		uint16_t ZU;
		uint16_t AI;
		uint16_t BI;
		uint16_t CI;
		uint16_t ZI;
		uint16_t YX;
	};
}WAVE_DAT_FORMAT;
/************************************************************!
*Function: FAULTWAVE结构体
*Description:采集单元组故障波形
*************************************************************/
typedef union
{
	uint8_t DATA[4*MAX_WAVE_LEN];
	struct
	{
		int16_t  VOL[MAX_WAVE_LEN];  //电压波形
		int16_t  CUR[MAX_WAVE_LEN];  //电流波形
	};
}FAULTWAVE;
/************************************************************!
*Function: FAULTINFO结构体
*Description:采集单元故障信息
*************************************************************/
typedef union
{
	uint8_t DATA[FAULT_DATA_LEN];
	struct
	{
		uint32_t    CNT;
		uint32_t    UNIXTIME;

		uint8_t     LINE;     //故障所属的线号
		uint8_t     PHASE;    //故障所属的相号
		uint8_t     TYPE;     //故障类型,当故障为疑似接地故障时,此值为1,其他情况下都为0
		uint8_t     FRAMESEQ; //故障录波的数据帧序号
		uint8_t     FRAMENUM; //故障录波的数据帧数
		uint32_t    WAVELEN;  //故障录波点数(周期数*采样速率)
		SINGLEBYTE  WAVENUM;  //高4位为故障前波形数,低4位为故障后波形数
		FAULTWAVE   WAVE[3]; //波形信息
		int16_t     ZEROU[MAX_WAVE_LEN];
		int16_t     ZEROI[MAX_WAVE_LEN];
	};
}FAULTINFO;
/************************************************************!
*Function: P101SOE结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[15];
	struct
	{
		uint8_t    TYPE;        //类型标识
		DOUBLEBYTE MSGADDR;     //信息体地址
		uint8_t    SOE;         //带品质描述的单点信息
		uint8_t    TIME[7];     //CP56时间
		uint16_t   CRLF;        //回车换行
		uint8_t    ISLOCK;      //帧锁标志
		uint8_t    STATUS;
	};
}P101SOE;
/************************************************************!
*Function: SOEQUEUE结构体
*Description:
*************************************************************/
typedef struct
{
	uint8_t WRITE;
	P101SOE SOE[MAX_SOE_NUM];
}SOEQUEUE;
/************************************************************!
*Function: P101FAULT结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[13];
	struct
	{
		DOUBLEBYTE MSGADDR;   //信息体地址
		uint8_t    FAULT;     //故障信息
		uint8_t    TIME[7];   //CP56时间
		uint16_t   CRLF;
		uint8_t    ISLOCK;    //帧锁标志
	};
}P101FAULT;
/************************************************************!
*Function: FAULTQUEUE结构体
*Description:
*************************************************************/
typedef struct
{
	uint8_t   WRITE;
	P101FAULT FAULT[20];
}FAULTQUEUE;
/************************************************************!
*Function: P101SCO
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA;
	struct
	{
		uint8_t SCS:1; //0:开,1:合
		uint8_t BS1:1;
		uint8_t QU:5;
		uint8_t SE:1;  //0:执行1:选择
	};
}P101SCO;
/************************************************************!
*Function: P101YK结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[20];
	struct
	{
		uint8_t    TYPE;      //类型标识
		DOUBLEBYTE MSGADDR;   //信息体地址
		DOUBLEBYTE COT;       //传送原因
		P101SCO    SCO;       //单点命令
		uint8_t    TIME[7];   //CP56时间
		uint16_t   CRLF;      //回车换行
		uint32_t   TIMING;    //命令计时
		uint8_t    STATE;     //命令状态
	};
}P101YK;
/************************************************************!
*Function: YCDATA结构体
*Description:
*************************************************************/
typedef struct
{
	uint8_t     COMMA;
	DOUBLEBYTE  ADDR;
	uint16_t    DATA;
}YCDATA;
/************************************************************!
*Function: YAOCEFILE结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[164];
	struct
	{
		uint8_t  LENS;
		uint8_t  SN;       //节名
		uint8_t  COMMA1;   //逗号
		uint8_t  N;        //定点数据数目
		uint8_t  COMMA2;   //逗号
		P101TIME TIME;     //7字节
		YCDATA   YC[27];   //5*27字节
		uint16_t CRLF;     //回车换行

		uint32_t TIMEOUT;  //遥测超时管理
		uint32_t OLDTIME;
		uint32_t NEWTIME;
		uint8_t  CONNECTION[3];
	};
}YAOCEFILE;
/************************************************************!
*Function: YAOCEFILEHEAD结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[77]; //规范最长76字节加一个字节的LENS
	struct
	{
		char     FILENAME[20];
		uint8_t  COMMA1;
		char     FILEVERSION[20];
		uint16_t CRLF1;
		char     TERMINALNAME[20];
		uint8_t  COMMA2;
		uint8_t  DATE[3];           //CP56年月日
		uint8_t  COMMA3;
		uint8_t  K;
		uint8_t  COMMA4;
		uint8_t  ADDRLENS[2];
		uint8_t  COMMA5;
		uint8_t  TYPE;
		uint16_t CRLF2;
		uint8_t  LENS;
	};
}YAOCEFILEHEAD;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[22];
	struct
	{
		uint16_t   ID;      //文件名称
		uint8_t    CS;      //文件校验和
		uint16_t   LENS;    //文件长度
		uint32_t   READ;    //文件读指针
		uint8_t    STATE;   //文件状态
		uint32_t   INFONO;  //文件段序号
		uint8_t    SECTION; //节号
		uint16_t   SENDNUM;
		uint16_t   INFONUM; //文件段数量
		uint8_t    TIME[7]; //文件时间
		DOUBLEBYTE MSGADDR; //信息体地址
	};
}FILEINFO;
/************************************************************!
*Function: RESETCAUSE结构体
*Description:
*************************************************************/
typedef union
{
	uint32_t DATA;
	struct
	{
		uint8_t POR:1;  //上电复位
		uint8_t UBOR:1; //异常掉电复位
		uint8_t BOR:1;  //正常掉电复位
		uint8_t ETR:1;  //外部复位(外狗)
		uint8_t WDR:1;  //看门狗复位
		uint8_t LUR:1;  //M3锁复位
		uint8_t SRR:1;  //系统请求复位
	};
}RMU_RESETCAUSE;
/************************************************************!
*Function: 归一化值
*Description:
*************************************************************/
typedef union
{
	uint16_t F16;
	struct
	{
		uint16_t DATA:15;
		uint8_t  SIGN:1;
	};
}NVA;
#pragma pack()
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef enum
{
	JIEDI=1,         //接地
	DIANLIANGDI=2,   //电池电量低
	DUANLU=3,        //短路
}SOE_TYPE_t;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef enum
{
	SLEEP=1,  //休眠
	RX=2,     //接收
	RXC=3,    //接收完成
	TX=4,     //发送
	TXC=5,    //发送完成
}CU_INTERFACE_STATUS_ENUM;


uint8_t Get_LinePara_Num(void);
void YaoCeFile_Init(YAOCEFILE *YCF);
void YaoCeFile_Clear(YAOCEFILE *YCF);
void YaoCeFileHead_Init(YAOCEFILEHEAD *YCFH);
void Wave_Dat_Format_Init(WAVE_DAT_FORMAT *WDF);


#endif

