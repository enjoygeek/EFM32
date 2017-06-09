/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: esam.h
*Version: 0.1.0
*Date: 2017-05-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef ESAM_H_
#define ESAM_H_


#include <stdint.h>
#include <stdbool.h>


#define ESAM_FRAME_MAX_LEN  1000    //ESAM数据帧的最大长度
#define ESAM_RES_MAX_NUM    5       //ESAM应答帧的最大数量


#define ESAM_STARTBYTE      0xEB       //ESAM帧起始字节
#define ESAM_ENDBYTE        0xD7       //ESAM帧结束字节


#define ESAM_FRAMEHEAD_LEN  4     //ESAM数据帧帧头长度
#define ESAM_FRAMETAIL_LEN  2     //ESAM数据帧帧尾长度
#define ESAM_FRAME_CS_LOC   260   //ESAM帧中ECP101数据帧的CS标志位索引


#define ESAM101_FIXFRAME_STARTBYTE   0x10
#define ESAM101_FIXFRAME_ENDBYTE     0x16
#define ESAM101_FIXFRAME_LEN         6
#define ESAM101_VARFRAME_STARTBYTE   0x68
#define ESAM101_VARFRAME_ENDBYTE     0x16
#define ESAM101_VARFRAME_FRAMEHEAD   4
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t U16;
}ESAM101_SINGLE_COT;
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
}ESAM_SINGLEBYTE;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint16_t          U16;
	uint8_t           U8[2];
	ESAM_SINGLEBYTE   U4[2];
}ESAM_DOUBLEBYTE;
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
}ESAM101_CTRL;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t           STARTBYTE;  //起始字符
	ESAM101_CTRL      CTRL;       //控制位
	ESAM_DOUBLEBYTE   LINKADDR;   //链路地址
	uint8_t           CS;         //校验和
	uint8_t           ENDBYTE;    //结束字符
}ESAM101_FIXFRAME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[246];                  //101协议规定ASDU最大长度为255
	struct
	{
		uint8_t            TI;         //类型标识
		uint8_t            VSQ;        //可变限定
		#ifdef SINGLE_COT_CTRL
		ESAM101_SINGLE_COT COT;        //单字节传送原因
		#else
		ESAM_DOUBLEBYTE    COT;        //双字节传送原因
		#endif
		ESAM_DOUBLEBYTE    CAOA;       //公共地址
		ESAM_DOUBLEBYTE    MSGADDR;    //信息体地址
		#ifdef SINGLE_COT_CTRL
		uint8_t            MSG[239];   //信息体元素
		#else
		uint8_t            MSG[238];   //信息体元素
		#endif
	};
}ECP101_ASDU;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t          STARTBYTE1;   //启动字符     0
	uint8_t          LEN1;         //长度0-255  1
	uint8_t          LEN2;         //长度0-255  2
	uint8_t          STARTBYTE2;   //启动字符    3
	ESAM101_CTRL     CTRL;         //控制位      4
	ESAM_DOUBLEBYTE  LINKADDR;     //链路地址   5 6
	ECP101_ASDU      ASDU;         //链路用户数据
	uint8_t          CS;           //校验和
	uint8_t          ENDBYTE;      //结束字符
}ESAM101_VARFRAME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t           DATA[255];  //帧数组
	ESAM101_FIXFRAME  FF;         //固定长度帧
	ESAM101_VARFRAME  VF;         //可变长度帧
}ESAM101_FRAME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint16_t DATA;
	struct
	{
		uint8_t KEYMARK:3;     //对称密钥标识
		uint8_t ENCRYPTION:1;  //是否加密
		uint8_t RES1:1;        //备用
		uint8_t RES2:1;        //备用
		uint8_t OBJECT:2;      //相对终端连接对象
		uint8_t RES3:8;        //备用
	};
}PROTOCOL_TYPE;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[ESAM_FRAME_MAX_LEN-9];
	struct
	{
		uint8_t       APPLEN;     //报文长度
		ESAM101_FRAME EF;         //报文数据
		uint8_t       EXTEN[ESAM_FRAME_MAX_LEN-265];  //信息安全内容
	};
}ENCAPSULATE_DATA1;

typedef union
{
	ESAM_DOUBLEBYTE MSG;
	uint8_t         SAFEINFO[ESAM_FRAME_MAX_LEN-11];

	struct                     //证书更新包结构体
	{
		uint8_t CERFLAG;
		uint8_t FRAMENUM;
		uint8_t FRAMESEQ;
		uint8_t CIPHERTEXT[ESAM_FRAME_MAX_LEN-12];
	};

	struct
	{
		uint8_t FRAMENUM1;
		uint8_t FRAMESEQ1;
		uint8_t CIPHERTEXT1[ESAM_FRAME_MAX_LEN-11];
	};
}ENCAPSULATE_DATA2_UNION;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[ESAM_FRAME_MAX_LEN-9];
	struct
	{
		ESAM_DOUBLEBYTE          INFOLEN;   //安全信息区长度
		ENCAPSULATE_DATA2_UNION  EDU;
	};
}ENCAPSULATE_DATA2;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[ESAM_FRAME_MAX_LEN-8];
	struct
	{
		uint8_t            APPTYPE;    //应用类型
		ENCAPSULATE_DATA1  ENCAP1;     //包含应用数据区
		ENCAPSULATE_DATA2  ENCAP2;     //只含信息安全扩展区
	};
}ENCAPSULATE_DATA;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef union
{
	uint8_t DATA[ESAM_FRAME_MAX_LEN];
	struct
	{
		uint8_t          STARTBYTE1; //起始字节
		uint16_t         PROTOLEN;   //报文长度
		uint8_t          STARTBYTE2; //起始字节
		PROTOCOL_TYPE    PROTOTYPE;  //报文类型
		ENCAPSULATE_DATA ENCAP;      //封装数据域
        uint8_t          CS;         //校验和
        uint8_t          ENDBYTE;    //结束字节
	};
}ESAM_FRAME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t         PARSE;                //数据解析标志
	uint8_t         ISLOCK;               //数据帧锁标志
	uint8_t         ESAM101_FRAMETYPE;    //ECP101数据帧类型
	ESAM_DOUBLEBYTE INFOLEN;              //信息拓展区长度

	ESAM_FRAME EF;    //ESAM数据帧
}ECP101_REQ;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint16_t POS;
	uint8_t HAS_ESAM101;
	uint8_t ESAM101_FRAMELEN;
	uint8_t ESAM101_FRAMETYPE;
	uint8_t STATUS;
	uint8_t ISLOCK;
	uint16_t INFOLEN;      //信息安全拓展区长度
	uint8_t  ESAM101LEN;   //应用数据区长度
	uint8_t ENCRYTYPE;
	ESAM_FRAME EF;    //ESAM数据帧
}ECP101RES;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t   SN;
	uint8_t   REQSN;
	uint8_t   RESSN;
	ECP101RES ETQ[ESAM_RES_MAX_NUM];
}ECP101_RES;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef struct
{
	uint8_t LINKSTATE;
	uint8_t MFCB;
	uint8_t SFCB;

	ECP101_REQ REQ;  //数据请求帧
	ECP101_RES RES;  //数据应答帧
}ESAMFRAME;
/************************************************************!
*Function:
*Description:
*************************************************************/
typedef enum
{
	ESAM_MAINSITE=0,        //主站
	ESAM_MAINTENANCETOOL=1, //现场运维工具
	ESAM_GATEWAY=2,          //网关
}ESAM_CONNECT_OBJECT_ENUM;

typedef enum
{
	ESAM101_FIXFRAME_TYPE=0,
	ESAM101_VARFRAME_TYPE=1,
}ESAM101_FRAMETYPE_ENUM;




#endif
