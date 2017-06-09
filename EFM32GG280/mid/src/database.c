/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: database.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "include.h"
#include "database.h"


/************************************************************!
*Function: YaoCeFileHead_Init
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
void YaoCeFileHead_Init(YAOCEFILEHEAD *YCFH)
{
	P101TIME PT;
	uint8_t lens;

	YCFH->LENS=16;
	YCFH->COMMA1=0x2C;
	YCFH->COMMA2=0x2C;
	YCFH->COMMA3=0x2C;
	YCFH->COMMA4=0x2C;
	YCFH->COMMA5=0x2C;
	YCFH->CRLF1=0x0D0A;
	YCFH->CRLF2=0x0D0A;

	YCFH->TYPE=03;          //uint16_t
	YCFH->K=96;    //96节
	YCFH->ADDRLENS[0]=2;    //信息对象地址长度2字节
	YCFH->ADDRLENS[1]=0;

	NowTime2P101Time(PT.DATA,7);

	YCFH->DATE[0]=PT.YEAR;
	YCFH->DATE[1]=PT.MONTH;
	YCFH->DATE[2]=PT.DAY.DAY;

	strcpy(YCFH->FILENAME,"FIXPT_20");
	lens=strlen("FIXPT_20");
	YCFH->LENS+=lens;

	YCFH->FILENAME[lens+0]=(PT.YEAR/10)+0x30;
	YCFH->FILENAME[lens+1]=(PT.YEAR%10)+0x30;
	YCFH->FILENAME[lens+2]=(PT.MONTH/10)+0x30;
	YCFH->FILENAME[lens+3]=(PT.MONTH%10)+0x30;
	YCFH->FILENAME[lens+4]=(PT.DAY.DAY/10)+0x30;
	YCFH->FILENAME[lens+5]=(PT.DAY.DAY%10)+0x30;

	strcpy(YCFH->FILEVERSION,"1.00");
	lens=strlen("1.00");
	YCFH->LENS+=lens;

	strcpy(YCFH->TERMINALNAME,"HUIJIDANYUAN");
	lens=strlen("HUIJIDANYUAN");
	YCFH->LENS+=lens;
}
/************************************************************!
*Function: YaoCeFile_Init
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
void YaoCeFile_Init(YAOCEFILE *YCF)
{
	YCF->N=0;
	YCF->SN=0;
	YCF->TIMEOUT=0;

	YCF->COMMA1=0x2C;
	YCF->COMMA2=0x2C;
	YCF->CRLF=0x0D0A;

	memset(YCF->TIME.DATA,0,7);

	for(uint8_t i=0;i<27;i++)
		YCF->YC[i].COMMA=0x2C;

	for(uint8_t i=0;i<27;i++)
		YCF->YC[i].DATA=0;

	YCF->YC[0].ADDR.U16=0x4011;
	YCF->YC[1].ADDR.U16=0x4012;
	YCF->YC[2].ADDR.U16=0x4013;

	YCF->YC[3].ADDR.U16=0x4016;
	YCF->YC[4].ADDR.U16=0x4017;
	YCF->YC[5].ADDR.U16=0x4018;

	YCF->YC[6].ADDR.U16=0x401A;
	YCF->YC[7].ADDR.U16=0x401B;
	YCF->YC[8].ADDR.U16=0x401C;

	YCF->YC[9].ADDR.U16=0x4021;
	YCF->YC[10].ADDR.U16=0x4022;
	YCF->YC[11].ADDR.U16=0x4023;

	YCF->YC[12].ADDR.U16=0x4025;
	YCF->YC[13].ADDR.U16=0x4026;
	YCF->YC[14].ADDR.U16=0x4027;

	YCF->YC[15].ADDR.U16=0x4009;
	YCF->YC[16].ADDR.U16=0x400A;
	YCF->YC[17].ADDR.U16=0x400B;

	YCF->YC[18].ADDR.U16=0x4031;
	YCF->YC[19].ADDR.U16=0x4032;
	YCF->YC[20].ADDR.U16=0x4033;

	YCF->YC[21].ADDR.U16=0x4035;
	YCF->YC[22].ADDR.U16=0x4036;
	YCF->YC[23].ADDR.U16=0x4037;

	YCF->YC[24].ADDR.U16=0x4039;
	YCF->YC[25].ADDR.U16=0x403A;
	YCF->YC[26].ADDR.U16=0x403B;
}
/************************************************************!
*Function: YaoCeFile_Clear
*Description:
*Input:
*Output: null
*Return:
*************************************************************/
void YaoCeFile_Clear(YAOCEFILE *YCF)
{
	YCF->TIMEOUT=0;

	memset(YCF->TIME.DATA,0,7);

	for(uint8_t i=0;i<27;i++)
		YCF->YC[i].DATA=0;
}

