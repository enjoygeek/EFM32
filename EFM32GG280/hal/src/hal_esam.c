/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_esam.c
*Version: 0.1.0
*Date: 2017-04-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#include "hal_esam.h"
#include "include.h"


uint8_t MASTER_RANDOM[16]; //主站随机数,用于终端的数据加密
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Get_Chip_Id(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x00;
	ESMCMD[1]=0xb0;
	ESMCMD[2]=0x99;
	ESMCMD[3]=0x05;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x02;
	ESMCMD[6]=0x00;
	ESMCMD[7]=0x08;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,8);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

	#ifdef PRINT_ESAM_INFO_DEBUG
    DEBUG_PRINT_NISR("KKNSDSK","<-------- ESAM芯片 ID 长度 ",relens," ID:");
    DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Get_Key_Version(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x1A;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x00;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x00;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

	#ifdef PRINT_ESAM_INFO_DEBUG
    DEBUG_PRINT_NISR("KKNSDSK","<-------- ESAM芯片密钥版本长度 ",relens," 密钥版本:");
    DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Get_Key(uint8_t *data)
{
	uint16_t relens=0;

	return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Get_Random(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x00;
	ESMCMD[1]=0x84;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x08;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x00;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

	#ifdef PRINT_ESAM_INFO_DEBUG
    DEBUG_PRINT_NISR("KKNSDSK","<-------- ESAM芯片随机数长度 ",relens," 随机数:");
    DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Sign_Random(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x16;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x80;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x08;

	memcpy(ESMCMD+6,data,8);

//	if(isMaster==true)  //拷贝主站随机数
//	{
//		memcpy(MASTER_RANDOM,data,8);
//
//		for(uint8_t i=0;i<8;i++)
//			MASTER_RANDOM[8+i]=~MASTER_RANDOM[i];
//	}

    ESAM_SPI_WRITE_ARRAY(ESMCMD,8);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

	#ifdef PRINT_ESAM_INFO_DEBUG
	DEBUG_PRINT_NISR("KKNSDSK","<-------- ESAM随机数签名长度 ",relens," 数据:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Esam_1161y_Verify_Signature(uint8_t *data,uint16_t lens)
{
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x08;
	ESMCMD[2]=0x00;
	ESMCMD[3]=data[lens-1]; //ASKID
	ESMCMD[4]=(lens-1)>>8;
	ESMCMD[5]=lens-1;

	memcpy(ESMCMD+6,data+1,lens-1);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens+5);
    ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00))
    	return false;

	#ifdef PRINT_ESAM_INFO_DEBUG
    DEBUG_PRINT_NISR("KKNSKK","<-------- ESAM认证主站签名成功!");
	#endif

    return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Data_Encryption(uint8_t *data,uint16_t lens)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x26;
	ESMCMD[2]=0x60;
	ESMCMD[3]=0x01;
	ESMCMD[4]=(lens+16)>>8;
	ESMCMD[5]=(lens+16);

	memcpy(ESMCMD+6,MASTER_RANDOM,16);
	memcpy(ESMCMD+22,data,lens);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens+16+6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Data_Encryption_Random(uint8_t *data,uint16_t lens)
{
	uint16_t relens;
	uint8_t RANDOM[8];
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x00;
	ESMCMD[1]=0x84;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x08;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x00;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    memcpy(RANDOM,ESMCMD+4,8);

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x26;
	ESMCMD[2]=0x60;
	ESMCMD[3]=0x01;
	ESMCMD[4]=(lens+16+8)>>8;
	ESMCMD[5]=(lens+16+8);

	memcpy(ESMCMD+6,MASTER_RANDOM,16);
	memcpy(ESMCMD+22,data,lens);
	memcpy(ESMCMD+22+lens,RANDOM,8);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens+16+6+8);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Data_Decryption(uint8_t *data,uint16_t lens)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x2C;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x01;
	ESMCMD[4]=(lens+16)>>8;
	ESMCMD[5]=(lens+16);

	memcpy(ESMCMD+6,MASTER_RANDOM,16);
	memcpy(ESMCMD+22,data,lens);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens+16+6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Tool_Data_Decryption(uint8_t *data,uint16_t lens)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x2C;
	ESMCMD[2]=0x60;
	ESMCMD[3]=0x01;
	ESMCMD[4]=(lens+16)>>8;
	ESMCMD[5]=(lens+16);

	memcpy(ESMCMD+6,MASTER_RANDOM,16);
	memcpy(ESMCMD+22,data,lens);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens+16+6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Esam1161y_Verify_Signature(uint8_t *data,uint16_t lens)
{
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x08;
	ESMCMD[2]=0x00;
	ESMCMD[3]=data[lens-1];
	ESMCMD[4]=(lens-1)>>8;
	ESMCMD[5]=(lens-1);

	memcpy(ESMCMD+6,data,lens-1);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens-1+6);
    ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00))
    	return false;

    return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Esam1161y_Symmetric_key_Update(uint8_t *data,bool isUpdate)
{
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x1C;
	ESMCMD[2]=(data[0]>0?0x00:0x01);
	ESMCMD[3]=data[245];
	ESMCMD[4]=0x00;
	ESMCMD[5]=245;  //1+5*36+64

	memcpy(ESMCMD+6,data,245);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,245+6);
    ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00))
    	return false;

    return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Esam1161y_Ca_Certificate_Update(uint8_t *data,uint16_t lens)
{
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x22;
	ESMCMD[2]=data[0];
	ESMCMD[3]=0x00;
	ESMCMD[4]=(lens-1)>>8;
	ESMCMD[5]=(lens-1);

	memcpy(ESMCMD+6,data+1,lens-1);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens-1+6);
    ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00))
    	return false;

    return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Esam1161y_Set_Ca_Certificate(uint8_t *data,uint16_t lens)
{
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x24;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x00;
	ESMCMD[4]=lens>>8;
	ESMCMD[5]=lens;

	memcpy(ESMCMD+6,data,lens);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens+6);
    ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00))
    	return false;

    return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Get_Ca_Certificate(uint8_t *data,uint16_t lens)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x30;
	ESMCMD[2]=0x01;
	ESMCMD[3]=0x00;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x00;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Set_Ca_Root_Certificate(uint8_t *data,uint16_t lens)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x22;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x00;
	ESMCMD[4]=lens>>8;
	ESMCMD[5]=lens;

	memcpy(ESMCMD,data,lens);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens+6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Set_Gateway_Ca_Certificate(uint8_t *data,uint16_t lens)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x22;
	ESMCMD[2]=0x05;
	ESMCMD[3]=0x00;
	ESMCMD[4]=lens>>8;
	ESMCMD[5]=lens;

	memcpy(ESMCMD,data,lens);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,lens+6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Get_Test_Ca_Certificate_Lens(void)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x00;
	ESMCMD[1]=0xB0;
	ESMCMD[2]=0x81;
	ESMCMD[3]=0x00;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x02;
	ESMCMD[6]=0x00;
	ESMCMD[7]=0x02;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,8);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1161y_Get_Test_Ca_Certificate(uint8_t *data,uint8_t lens)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x00;
	ESMCMD[1]=0xB0;
	ESMCMD[2]=0x81;
	ESMCMD[3]=0x02;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x02;
	ESMCMD[6]=lens>>8;
	ESMCMD[7]=lens;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,8);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1120a_Get_Chip_Id(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x00;
	ESMCMD[1]=0xb0;
	ESMCMD[2]=0x99;
	ESMCMD[3]=0x05;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x08;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens); //拷贝8个字节的ID

	#ifdef PRINT_ESAM_INFO_DEBUG
    DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片 ID 长度 ",relens," ID:");
    DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1120a_Get_Key_Version(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x5E;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x00;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x00;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

	#ifdef PRINT_ESAM_INFO_DEBUG
	DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片密钥版本长度 ",relens," 密钥版本:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1120a_Get_Random(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x00;
	ESMCMD[1]=0x84;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x00;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x08;

    ESAM_SPI_WRITE_ARRAY(ESMCMD,6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

	#ifdef PRINT_ESAM_INFO_DEBUG
	DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片随机数长度 ",relens," 随机数:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1120a_Authentication(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x5C;
	ESMCMD[2]=0x01;
	ESMCMD[3]=0x00;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x08;

    memcpy(ESMCMD+6,data,8);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,14);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

	#ifdef PRINT_ESAM_INFO_DEBUG
	DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片身份认证长度 ",relens," 身份认证数据:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1120a_Public_Key_Verification(uint8_t *data) //OK
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0] = 0x80;
	ESMCMD[1] = 0x4E;
	ESMCMD[2] = data[8];
	ESMCMD[3] = 0x00;
	ESMCMD[4] = 0x00;
	ESMCMD[5] = 0x08;

    memcpy(ESMCMD+6,data,8);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,14);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);  //此处长度都应该为64???

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,104);  //relens

	#ifdef PRINT_ESAM_INFO_DEBUG
	DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片公钥验证数据长度 ",relens," 公钥验证数据:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

	return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1120a_Public_Key_Update(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x00;
	ESMCMD[1]=0xE7;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x01;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x8A; //138字节

    memcpy(ESMCMD+6,data,138);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,144);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);  //返回长度为138明文

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

	#ifdef PRINT_ESAM_INFO_DEBUG
    DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片公钥升级明文长度 ",relens," 公钥升级明文:");
    DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    memcpy(data,ESMCMD+4,relens); //拷贝出明文数据，暂存到data!!!!

    ESMCMD[0]=0x80;
    ESMCMD[1]=0x56;
    ESMCMD[2]=data[1]; //FID
    ESMCMD[3]=0x00;
    ESMCMD[4]=0x00;
    ESMCMD[5]=0x80; //64公钥+64签名

    memcpy(ESMCMD+6,data+2,128); //拷贝新公钥和签名

    ESAM_SPI_WRITE_ARRAY(ESMCMD,134);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    ESMCMD[0]=0x80;                     //公钥写入
    ESMCMD[1]=0x52;
    ESMCMD[2]=data[1]; //FID
    ESMCMD[3]=0x00;
    ESMCMD[4]=0x00;
    ESMCMD[5]=0x40; //64字节的公钥

    memcpy(ESMCMD+6,data+2,64); //拷贝新公钥

    ESAM_SPI_WRITE_ARRAY(ESMCMD,70);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    ESMCMD[0]=0x80;
    ESMCMD[1]=0x4E;
    ESMCMD[2]=data[1]; //FID
    ESMCMD[3]=0x00;
    ESMCMD[4]=0x00;
    ESMCMD[5]=0x08; //8字节随机数

    memcpy(ESMCMD+6,data+130,8); //拷贝随机数

    ESAM_SPI_WRITE_ARRAY(ESMCMD,14);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD); //此处得到的应该是104字节的验证数据，放入data!!!!

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

	#ifdef PRINT_ESAM_INFO_DEBUG
	DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片公钥验证密文长度 ",relens," 公钥验证密文:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Esam1120a_Symmetric_key_Update(uint8_t *data)
{
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x5A;
	ESMCMD[2]=data[0];
	ESMCMD[3]=0x00;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0xC4; //196字节

    memcpy(ESMCMD+6,data+1,196); //196字节

    ESAM_SPI_WRITE_ARRAY(ESMCMD,202);
    ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)) //对称密钥更新失败
    	return false;

    ESMCMD[0]=0x84;
    ESMCMD[1]=0xD4;
    ESMCMD[2]=0x01;
    ESMCMD[3]=0xFF;
    ESMCMD[4]=0x00;
    ESMCMD[5]=0x84; //132字节

    memcpy(ESMCMD+6,data+1,132);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,138);
    ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)) //对称密钥更新失败
    	return false;

	#ifdef PRINT_ESAM_INFO_DEBUG
    DEBUG_PRINT_NISR("KKNSKK","<-------- 当前ESAM芯片对称密钥更新成功!");
	#endif

    return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool Esam1120a_Key_Consult(uint8_t *data)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x60;
	ESMCMD[2]=data[0];
	ESMCMD[3]=0x01;
	ESMCMD[4]=0x00;
	ESMCMD[5]=0x48; //72字节

	memcpy(MASTER_RANDOM,data+1,8); //拷贝主站随机数,用于后期终端的数据加密

    memcpy(ESMCMD+6,data+1,72); //72字节

    ESAM_SPI_WRITE_ARRAY(ESMCMD,78);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

	#ifdef PRINT_ESAM_INFO_DEBUG
	DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片会话协商密文长度 ",relens," 会话协商密文:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1120a_Data_Encryption(uint8_t *data,uint16_t datalen)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x80;
	ESMCMD[1]=0x62;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x00;
	ESMCMD[4]=(datalen+8+1)>>8;   //+8: 主站随机数 +1：ASDU长度
	ESMCMD[5]=(datalen+8+1);

	memcpy(ESMCMD+6,MASTER_RANDOM,8);  //拷贝会话密钥协商中的主站随机数

	ESMCMD[14]=datalen;   //此处封装明文需要按照 1字节ASDU长度+ASDU内容进行！！！！！！

	memcpy(ESMCMD+15,data,datalen);

    ESAM_SPI_WRITE_ARRAY(ESMCMD,datalen+6+8+1);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+4,relens);

	#ifdef PRINT_ESAM_INFO_DEBUG
	DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片数据加密明文长度 ",datalen," 明文数据:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(data,datalen);
	DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片数据加密密文长度 ",relens," 密文数据:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t Esam1120a_Verify_Mac_6Byte(uint8_t *data,uint16_t datalen)
{
	uint16_t relens;
	uint8_t ESMCMD[ESAM_READ_MAX_LEN];

	ESMCMD[0]=0x00;
	ESMCMD[1]=0xE7;
	ESMCMD[2]=0x00;
	ESMCMD[3]=0x01;
	ESMCMD[4]=datalen>>8;
	ESMCMD[5]=datalen;

    memcpy(ESMCMD+6,data,datalen);   //此处没有验证主站数据及签名的正确性

    ESAM_SPI_WRITE_ARRAY(ESMCMD,datalen+6);
    relens=ESAM_SPI_READ_ARRAY(ESMCMD);

    if((ESMCMD[0]!=0x90)||(ESMCMD[1]!=0x00)||(relens==0))
    	return 0;

    memcpy(data,ESMCMD+5,relens-1);  //去掉湖南ESAM解密后的ASDU长度,一个字节

	#ifdef PRINT_ESAM_INFO_DEBUG
	DEBUG_PRINT_NISR("KKNSDSK","<-------- 当前ESAM芯片数据解密明文长度 ",relens," 明文数据:");
	DEBUG_PRINT_U8CHAR_ARRAY_NISR(ESMCMD,relens+4);
	#endif

    return relens;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint16_t ESAM_SPI_READ_ARRAY(uint8_t* data)
{
	uint32_t timer;
	uint8_t status=0;
    uint16_t datalen;
    uint8_t lrc2,recvlrc2;

    ESAM_DELAY_US(10);

    ENABLE_ESAM_SPI_CS;

    ESAM_DELAY_US(100); //cs拉低后,至少50us

    timer=gUNIXTIME.UNIX;

    while(status!=0x55)
    {
        status=ESAM_SPI_READ_BYTE();

        ESAM_DELAY_US(1);

        if((gUNIXTIME.UNIX-timer)>3) //超时3秒
            goto EASM_READ_END;
    };

    data[0]=ESAM_SPI_READ_BYTE();  //读取CLA和INS
    data[1]=ESAM_SPI_READ_BYTE();

    if((data[0]!=0x90)||(data[1]!=0x00))
        goto EASM_READ_END;

    lrc2=data[0]^data[1];  //计算校验

    data[2]=ESAM_SPI_READ_BYTE(); //读取长度Len1和Len2，Len1代表长度的高字节，Len2代表长度的低字节。
    data[3]=ESAM_SPI_READ_BYTE();

    datalen=(data[2]<<8)+data[3];

    if((datalen>2048)||(datalen>ESAM_READ_MAX_LEN))  //长度最大不能超过2K 或 长度不能超出接收缓冲区的长度
    {
        datalen=0;
        goto EASM_READ_END;
    }

    lrc2^=data[2]^data[3]; //计算校验

    for(uint16_t i=4;i<(datalen+4);i++)
    {
        data[i]=ESAM_SPI_READ_BYTE();
        lrc2^=data[i];
    }

    lrc2=~lrc2;

    recvlrc2=ESAM_SPI_READ_BYTE();

    if(lrc2!=recvlrc2)
    {
        datalen=0;
        goto EASM_READ_END;
    }

EASM_READ_END:

    DISABLE_ESAM_SPI_CS;

    ESAM_DELAY_US(15);  //cs拉高后,至少10us

    return datalen;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
bool ESAM_SPI_WRITE_ARRAY(uint8_t *data,uint16_t lens)
{
    uint8_t lrc1=0;

    ESAM_DELAY_US(10); //置高后至少10微秒才能再次拉低

    ENABLE_ESAM_SPI_CS;

    ESAM_DELAY_US(100);  //置低后至少50微秒后才能发送数据

    ESAM_SPI_WRITE_BYTE(0x55);

    ESAM_DELAY_US(30);

    for(uint16_t i=0;i<lens;i++)
    {
    	ESAM_SPI_WRITE_BYTE(data[i]);
        lrc1^=data[i];
    }

    lrc1=~lrc1;

    ESAM_SPI_WRITE_BYTE(lrc1);

   // ESAM_DELAY_US(10); //发送完数据后10微秒内拉高

    DISABLE_ESAM_SPI_CS;

   // ESAM_DELAY_US(15); //cs拉高后，最少10us

    return true;
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t ESAM_SPI_READ_BYTE(void)
{
	return ESAM_SPI_READWRITE(0xFF);  //模式3空闲为高电平
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void ESAM_SPI_WRITE_BYTE(uint8_t data)
{
	ESAM_SPI_READWRITE(data);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
uint8_t ESAM_SPI_READWRITE(uint8_t data)
{
	return Usart0_ReadWrite(data);
}
/************************************************************!
*Function:
*Description:
*Input:
*Output: null
*Return: null
*************************************************************/
void ESAM_DELAY_US(uint32_t us)
{
	for(uint32_t i=0;i<us*7*2;i++); //系统主频暂定7M,不考虑M3的指令周期,2：冗余
}
