/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: hal_esam.h
*Version: 0.1.0
*Date: 2017-04-12
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef HAL_ESAM_H_
#define HAL_ESAM_H_


#include <stdint.h>
#include <stdbool.h>


#define ESAM_READ_MAX_LEN  300


typedef union
{
	uint8_t DATA[1000];
	struct
	{
		uint16_t LENS;
		uint8_t  CERDATA[1000-2];
	};
}ESAM1161Y_CERDATA;


uint16_t Esam1161y_Get_Chip_Id(uint8_t *data);
uint16_t Esam1161y_Get_Key_Version(uint8_t *data);
uint16_t Esam1161y_Get_Key(uint8_t *data);
uint16_t Esam1161y_Get_Random(uint8_t *data);
uint16_t Esam1161y_Sign_Random(uint8_t *data);
bool Esam_1161y_Verify_Signature(uint8_t *data,uint16_t lens);
bool Esam_1161y_Verify_Tool_Signature(uint8_t *data,uint16_t lens);
uint16_t Esam1161y_Data_Encryption(uint8_t *data,uint16_t lens);
uint16_t Esam1161y_Data_Encryption_Random(uint8_t *data,uint16_t lens);
uint16_t Esam1161y_Tool_Data_Encryption(uint8_t *data,uint16_t lens);
uint16_t Esam1161y_Data_Decryption(uint8_t *data,uint16_t lens);
uint16_t Esam1161y_Tool_Data_Decryption(uint8_t *data,uint16_t lens);
bool Esam1161y_Verify_Signature(uint8_t *data,uint16_t lens);
bool Esam1161y_Symmetric_key_Update(uint8_t *data,bool isUpdate);
bool Esam1161y_Ca_Certificate_Update(uint8_t *data,uint16_t lens);
bool Esam1161y_Set_Ca_Certificate(uint8_t *data,uint16_t lens);
uint16_t Esam1161y_Get_Ca_Certificate(uint8_t *data,uint16_t lens);
uint16_t Esam1161y_Set_Ca_Root_Certificate(uint8_t *data,uint16_t lens);
uint16_t Esam1161y_Set_Gateway_Ca_Certificate(uint8_t *data,uint16_t lens);
uint16_t Esam1161y_Get_Test_Ca_Certificate_Lens(void);
uint16_t Esam1161y_Get_Test_Ca_Certificate(uint8_t *data,uint8_t lens);
uint16_t Esam1161y_Verify_Tool_Cer(uint8_t *data,uint16_t lens);



uint16_t Esam1120a_Get_Chip_Id(uint8_t *data);
uint16_t Esam1120a_Get_Key_Version(uint8_t *data);
uint16_t Esam1120a_Get_Random(uint8_t *data);
uint16_t Esam1120a_Authentication(uint8_t *data);
uint16_t Esam1120a_Public_Key_Verification(uint8_t *data);
uint16_t Esam1120a_Public_Key_Update(uint8_t *data);
bool Esam1120a_Symmetric_key_Update(uint8_t *data);
bool Esam1120a_Key_Consult(uint8_t *data);
uint16_t Esam1120a_Data_Encryption(uint8_t *data,uint16_t datalen);
uint16_t Esam1120a_Verify_Mac_6Byte(uint8_t *data,uint16_t datalen);
uint16_t ESAM_SPI_READ_ARRAY(uint8_t* data);
bool ESAM_SPI_WRITE_ARRAY(uint8_t *data,uint16_t lens);
uint8_t ESAM_SPI_READ_BYTE(void);
void ESAM_SPI_WRITE_BYTE(uint8_t data);
uint8_t ESAM_SPI_READWRITE(uint8_t data);
void ESAM_DELAY_US(uint32_t us);


#endif

