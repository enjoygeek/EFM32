/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: ecp.h
*Version: 0.1.0
*Date: 2016-10-12
*Author: WANGJIANHUI
*Description: External communication protocol
*******************************************************************/
#ifndef ECP_H_
#define ECP_H_


/**********101规约层COT定义**********/
//#define SINGLE_COT_CTRL         1    //单字节COT
/**********101最大优先级**********/
#define MAX_PRIORITY_NUM        11
/**********101最大应答帧数**********/
#define MAX_REPLY_NUM           10
/**********101遥控取消倒计时**********/
#define ECP_YK_TIME             30
/**********101固定帧宏定义**********/
#define ECP_FIXFRMAE_LEN        6
#define ECP_FIXFRMAE_STARTBYTE  0x10
#define ECP_FIXFRMAE_ENDBYTE    0x16
/**********101可变帧宏定义**********/
#define ECP_VARFRMAE_LEN        261
#define ECP_VARFRMAE_STARTBYTE  0x68
#define ECP_ESMFRMAE_STARTBYTE  0x69
#define ECP_VARFRMAE_ENDBYTE    0x16
#define ECP_VARFRAME_CS_LOC     259
#define ECP_VARFRMAE_HEAD       4      //帧头4个字节
#define ECP_VARFRMAE_TAIL       2      //帧尾2个字节
#define ECP_ESM_MAXLEN          252
/**********101功能码宏定义**********/
#define ECP_RST_LINK            0       //复位链路
#define ECP_RST_PROCESS         1       //复位远动终端的用户进程
#define ECP_TICKS_TEST          2       //心跳测试
#define ECP_CALL_DATA           3       //传送数据
#define ECP_CALL_LINKSTATE      9       //召唤链路状态
#define ECP_MASTER_LINKSTATE    11
#define ECP_SET_PARA            48
#define ECP_REMOTE_CONTROL      45
#define ECP_ALL_CALL            100     //总召唤
#define ECP_READ_DATA           102
#define ECP_READ_MULTIPARA      202
#define ECP_TIMING              103     //主站对时
#define ECP_TEST_LINK           104     //用于平衡式传输的测试链路功能
#define ECP_RESET_PROCESS       105
#define ECP_GET_DELAY           106
#define ECP_ACK                 0
#define ECP_NOACK               1
#define ECP_CALL_DIR            5
#define ECP_CALL_FILE           13
#define ECP_CALL_DIRFILE        122     //
#define ECP_REQUEST_FILE        121
#define ECP_FILE_ACK            124

#define ECP_SWITCH_FIXED_AREA   200
#define READ_FIXED_AREA         201
#define ECP_CALL_FILEDIR        210
/**********ESAM功能码宏定义**********/
#define ECP_READ_SAFE_INFO            0xF0
#define ECP_AUTHENTICATION            0xF1
#define ECP_PUBLIC_KEY_VERIFICATION   0xF2
#define ECP_PUBLIC_KEY_UPDATE         0xF3
#define ECP_SYMMETRIC_KEY_UPDATE      0xF4
#define ECP_KEY_CONSULT               0xF5
#define ECP_GET_RANDOM                0xF6


#pragma pack(1)
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t U16;
}ECP_SINGLE_COT;
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
}ECP_SINGLEBYTE;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint16_t       U16;
	uint8_t        U8[2];
	ECP_SINGLEBYTE U4[2];
}ECP_DOUBLEBYTE;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA;
	struct
	{
		uint8_t FC:4;  //功能码
		uint8_t FCV:1; //帧计数有效位
		uint8_t FCB:1; //帧计数位
		uint8_t PRM:1; //启动标志
		uint8_t DIR:1; //传输方向
	};
}ECP_CTRL;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t        STARTBYTE;  //起始字符
	ECP_CTRL       CTRL;       //控制位
	ECP_DOUBLEBYTE LINKADDR;   //链路地址
	uint8_t        CS;         //校验和
	uint8_t        ENDBYTE;    //结束字符
}ECP_FIXFRAME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[252];                  //101协议规定ASDU最大长度为255
	struct
	{
		uint8_t             TI;         //类型标识
		uint8_t             VSQ;        //可变限定
		#ifdef SINGLE_COT_CTRL
		    ECP_SINGLE_COT  COT;        //单字节传送原因
		#else
			ECP_DOUBLEBYTE  COT;        //双字节传送原因
		#endif

		ECP_DOUBLEBYTE      CAOA;       //公共地址
		ECP_DOUBLEBYTE      MSGADDR;    //信息体地址
		#ifdef SINGLE_COT_CTRL
			uint8_t         MSG[245];   //信息体元素
		#else
			uint8_t         MSG[244];   //信息体元素
		#endif
	};
}ECP_ASDU;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t        STARTBYTE1;   //启动字符     0
	uint8_t        LEN1;         //长度0-255  1
	uint8_t        LEN2;         //长度0-255  2
	uint8_t        STARTBYTE2;   //启动字符    3
	ECP_CTRL       CTRL;         //控制位      4
	ECP_DOUBLEBYTE LINKADDR;     //链路地址   5 6
	ECP_ASDU       ASDU;         //链路用户数据
	uint8_t        CS;           //校验和
	uint8_t        ENDBYTE;      //结束字符
}ECP_VARFRAME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t       DATA[261];  //帧数组
	ECP_FIXFRAME  FF;         //固定长度帧
	ECP_VARFRAME  VF;         //可变长度帧
}ECP_FRAME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	ECP_FRAME EF;         //101数据帧

	uint8_t   PARSE;      //数据解析标志
	uint8_t   ISLOCK;     //数据帧占用标志
	uint8_t   STATUS;     //数据帧状态标志
	uint8_t   PRIORITY;   //优先级标志
	uint8_t   ESMLEN;     //ESAM加密长度
	uint8_t   FRAMETYPE;  //是固定帧还是变长帧
}ECP_DATAFRAME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t        SN;
	ECP_DATAFRAME  ETQ[MAX_REPLY_NUM];
}ECP_TXQUEUE;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[11];
	struct
	{
		uint8_t        TYPE;
		ECP_DOUBLEBYTE MSGADDR;
		ECP_DOUBLEBYTE COT;
		P101SCO        SCO;
		uint32_t       TIMING;
		uint8_t        STATE;
	};
}P101REMOTECONTROL;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[9];
	struct
	{
		uint8_t        STATUS;
		ECP_DOUBLEBYTE MSGADDR;
		ECP_DOUBLEBYTE PARA;
		uint32_t       TIMEOUT;
	};
}TERMINAL_PARA;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t        DATA[140];
	ECP_DOUBLEBYTE PARA[70];
}READPARA;
/************************************************************!
*Function: ECPFRAME
*Description:
*************************************************************/
typedef struct
{
	uint8_t       M_FCB;        //主站FCB
	uint8_t       S_FCB;        //从站FCB
	uint8_t       INITRESET;    //链路复位标志
	uint8_t       ESMSTATE;
	uint8_t       LINKRESET;
	uint8_t       LINKSTATE;    //链路状态

	uint8_t       SN;
    P101YK        YK;
    AU_PARA       AP;
	uint8_t       ISLOCK;
	FILEINFO      FILE;
    TERMINAL_PARA TP;
	ECP_DATAFRAME REQ;                    //数据请求帧
	ECP_TXQUEUE   RES[MAX_PRIORITY_NUM];  //数据应答帧
	uint8_t       PRIORITY[MAX_PRIORITY_NUM];
}ECPFRAME;
/************************************************************!
*Function: FLASHPARAARRAY
*Description:
*************************************************************/
typedef	struct
	{
		uint32_t PARAADDR;
		uint16_t MSGADDR;
		uint8_t TAG;
		uint8_t LEN;
}FLASHPARAARRAY;


#pragma pack()
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef enum
{
	FIX_FRAME=0,    //固定帧
	VAR_FRAME=1,    //可变帧
	ESM_FRAME=2,    //ESAM加密帧
}ECP_FRAME_FORMAT;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef enum
{
	LINKSTATE_START=0,    //链路未连接
	LINKSTATE_CONNECT=1,  //链路已链接
}ECP_LINKSTATE;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef enum
{
	YKINIT=0,    //初始化
	YKSELECT=1,  //选择
	YKEXEC=2,    //执行
}ECP_RC_TYPE;


#endif

