/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: icp.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description: Internal communication protocol
*******************************************************************/
#ifndef ICP_H_
#define ICP_H_


#include <stdint.h>


/**********ICP协议相关宏定义(规约V1.0.3)**********/
#define ICP_STARTBYTE       0x68
#define ICP_ENDBYTE         0x16

#define ICP_ADDRLEN         6
#define ICP_FRAMELEN        261
#define ICP_FRAMEHEAD       4     //帧头(68+lens+lens+68)
#define ICP_FRAMETAIL       2     //帧尾(crc+16)
#define ICP_MINPDULEN       16
#define ICP_MAXPDULEN       255
#define ICP_MINFRAMELEN     22
/************************************************************!
*Function:
*Description:
*************************************************************/
#pragma pack(1)
typedef union
{
	uint8_t DATA;        //控制域
	struct
	{
		uint8_t CC:4;    //功能码
		uint8_t RES1:1;  //
		uint8_t RES2:1;  //
		uint8_t PRM:1;   //启动标志
		uint8_t RES3:1;  //
	};
}ICP_CTRL;

/************************************************************!
*Function: YAOXIN结构体
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA;
	struct
	{
		uint8_t COMMUNTION:1;  //通信状态
		uint8_t LOWPOWER:1;    //电池欠压
		uint8_t IOVERRUN:1;    //电流超限
		uint8_t TOVERRUN:1;    //温度超限
		uint8_t JIEDI:1;       //疑似接地
		uint8_t YONGJIU:1;     //永久性短路
		uint8_t SHUNSHI:1;     //瞬时性短路
		uint8_t FUGUI:1;       //故障复归
	};
}YAOXIN;

typedef union
{
	uint8_t  DATA[ICP_MAXPDULEN-ICP_MINPDULEN];

	struct                   //遥测数据帧
	{
		uint16_t I;          //电流
		uint16_t U;          //电场
		uint16_t T;          //线温
		uint16_t B;          //电池电压
		uint8_t  WORKMODE;   //工作模式
	};

	struct                   //SOE数据帧
	{
		UNIXTIME TIME;
		YAOXIN  SOE;
	};

	struct                   //遥信数据帧
	{
		YAOXIN  YX;
	};

}ICP_FRAME_DATA;
/************************************************************!
*Function: ICPFRAME联合体
*Description:
*************************************************************/
typedef union
{
	uint8_t FRAME[ICP_FRAMELEN+6];
	struct
	{
		uint8_t         STARTBYTE1;            //起始字节
		uint8_t         LEN1;    		        //长度1
		uint8_t         LEN2;    		        //长度2
		uint8_t         STARTBYTE2;            //起始字节
		ICP_CTRL        CTRL;                  //控制域
		uint8_t         SRCADDR[ICP_ADDRLEN];  //源地址
		uint8_t         DSTADDR[ICP_ADDRLEN];	//目的地址
		uint8_t         TYPE;                  //数据类型
		uint8_t         FRAMENUM;              //帧数量
		uint8_t         FRAMESEQ;              //帧序号
		ICP_FRAME_DATA  IFD;
		uint8_t         CS;                    //校验码
		uint8_t         ENDBYTE;               //结束字符

		uint8_t         LINE;                  //数据所属的线号
		uint8_t         PHASE;                 //数据所属的相号

		uint8_t         PARSE;                 //数据解析标志
		uint8_t         ISLOCK;                //结构体占用标志
		uint8_t         STATUS;
		uint8_t         CHANNEL;               //数据帧类型
	};
}ICPFRAME;
#pragma pack()
/************************************************************!
*Function: ICP_PRM枚举
*Description:
*************************************************************/
typedef enum
{
	PRMSEC=0, //发起者
	PRMPRI=1, //从动者
}ICP_PRM;
/************************************************************!
*Function: 从动者功能码定义PRM=0
*Description:
*************************************************************/
typedef enum
{
	FCCON=0,     //肯定确认
	FCNOCON=1,   //否定确认
	FCRESDATA=2, //数据
	FCNODATA=3,  //否定认可
}ICP_CC0;
/************************************************************!
*Function: 从动者功能码定义PRM=1
*Description:
*************************************************************/
typedef enum
{
	FCRESETUSE=0,  //发送/无应答
	FCSENDDATA=1,  //发送/确认
	FCREQUESTD=2,  //请求/响应
}ICP_CC1;
/************************************************************!
*Function: 类型标识定义
*Description:
*************************************************************/
typedef enum
{
	AUTICKS=0,
	TIMESTAMP=1,    //时标
	DBYC=2,         //遥测
	DBSOE=3,        //SOE
	DBWAVE=4,       //电流录波数据
	DBPARA=5,       //参数
	DBPROG=6,       //程序
	FAULTWRG=7,     //启动录波
	LED=8,          //点灯测试
	WAKEUP=10,
	DBYX=11,        //无时标遥信
	RSTPROCESS=12,  //复位进程
	WORKMODE=14,    //工作模式
	ERASEFLASH=15,
	READLOG=16,
	ECPREADLOG=17,
	ECPREADSOE=19,
	NOWTIME=18,
	READSOE=20,
	READYC=21,
	READFAULT=22,
	ECPREADFAULT=23,
	ECPREADYC=24,
	READYK=25,
	ECPREADYK=26,
	READWAVE=27,
	ECPREADWAVE=28,
	ERASEPARA=29,
	ECPSTARTWAVE=30,
}ICP_TYPE;
/************************************************************!
*Function: ICP_CHANNEL
*Description:
*************************************************************/
typedef enum
{
	RADIO_CHANNEL=0,
	WEIHU_CHANNEL=1,
}ICP_CHANNEL;


#endif

