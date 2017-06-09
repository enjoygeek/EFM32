/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: flashstorage.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef FLASHSTORAGE_H_
#define FLASHSTORAGE_H_


#include <stdarg.h>
#include <stdlib.h>
#include "include.h"
#include "hal_msc.h"


/**********MSC备份程序区宏定义**********/
//MSC划分：BOOTLOADER:12k 主程序:300k 备份程序:200k
#define MSC_PAGE_LEN         4096
#define BACKUP_PRM_KEY       0x0004E000
#define BACKUP_PROGRAM       0x0004E004
#define BACKUP_PAGE_NUM      50
#define BACKUP_PRM_BLOCK     232
/**********MX25数据区宏定义**********/
#define MX25_PAGE_LEN        256
#define MX25_SECTOR_LEN      4096
/**********MX25参数数据区宏定义**********/
#define PARA_SECTOR_NUM      1
#define PARA_FLASH_LEN       (1*4096)
#define SYSPARA_ADDR         0x0000000
#define ECPPARA_ADDR         0x0001000
#define POINTTABLE_ADDR      0x0002000
#define BACKUP1              0x0003000
#define BACKUP2              0x0004000
#define INHERENTPARA_ADDR    0x0005000
#define RUNPARA_ADDR         0x0006000
#define FIXVALUE_PARA        0x0007000
#define BACKUP6              0x0008000
#define BACKUP7              0x0009000
/**********MX25日志数据区宏定义**********/
#define LOG_START_ADDR1      0x0000A000
#define LOG_SECTOR_NUM       40   //40*4096/139=1036条LOG
#define LOG_FLASH_LEN        (LOG_SECTOR_NUM*MX25_SECTOR_LEN)
#define LOG_START_ADDR2      (LOG_START_ADDR1+LOG_FLASH_LEN)
/**********MX25遥信数据区宏定义**********/
#define SOE_START_ADDR1      (LOG_START_ADDR2+LOG_FLASH_LEN)
#define SOE_SECTOR_NUM       1     //4*4096/13=1260条SOE
#define SOE_FLASH_LEN        (SOE_SECTOR_NUM*MX25_SECTOR_LEN)
#define SOE_START_ADDR2      (SOE_START_ADDR1+SOE_FLASH_LEN)
/**********MX25故障数据区宏定义**********/
#define FAULT_START_ADDR1    (SOE_START_ADDR2+SOE_FLASH_LEN)
#define FAULT_SECTOR_NUM     1     //4*4096/11=1489条故障信息
#define FAULT_FLASH_LEN      (FAULT_SECTOR_NUM*MX25_SECTOR_LEN)
#define FAULT_START_ADDR2    (FAULT_START_ADDR1+FAULT_FLASH_LEN)
/**********MX25遥控数据区宏定义**********/
#define YK_START_ADDR1       (FAULT_START_ADDR2+FAULT_FLASH_LEN)
#define YK_SECTOR_NUM        1
#define YK_FLASH_LEN         (YK_SECTOR_NUM*MX25_SECTOR_LEN)
#define YK_START_ADDR2       (YK_START_ADDR1+YK_FLASH_LEN)
/**********MX25遥测数据区宏定义**********/
#define YC_START_ADDR1       (YK_START_ADDR2+YK_FLASH_LEN) //0x60000
#define YC_SECTOR_NUM        120   //120/4=30个遥测文件
#define YC_FLASH_LEN         (YC_SECTOR_NUM*MX25_SECTOR_LEN)
#define YC_START_ADDR2       (YC_START_ADDR1+YC_FLASH_LEN)
#define YC_FILE_SECTOR_NUM   4
#define YC_FILE_LEN          (YC_FILE_SECTOR_NUM*MX25_SECTOR_LEN)
/**********MX25波形数据区宏定义**********/
#define WAVE_START_ADDR1     (YC_START_ADDR2+YC_FLASH_LEN)
#define WAVE_SECTOR_NUM      576
#define WAVE_FLASH_LEN       (WAVE_SECTOR_NUM*MX25_SECTOR_LEN)
#define WAVE_START_ADDR2     (WAVE_START_ADDR1+WAVE_FLASH_LEN)
#define WAVE_FILE_SECTOR_NUM 9
#define WAVE_FILE_LEN        (WAVE_FILE_SECTOR_NUM*MX25_SECTOR_LEN)
/**********MX25信息记录长度宏定义**********/
#define LOG_INFO_HEAD        34
#define SOE_INFO_HEAD        38
#define FAULT_INFO_HEAD      40
#define WAVE_INFO_HEAD       40
#define YK_INFO_HEAD         37
#define YC_FILEHEAD_LEN      42   //遥测文件头长度
#define LOG_INFO_LEN         139  //每条日志长度
#define SOE_INFO_LEN         13   //每条SOE长度
#define SOE_INFO_NUM         256    //遥测定点信息条数
#define YC_INFO_LEN          149  //遥测定点信息长度
#define YC_INFO_NUM          96    //遥测定点信息条数
#define YK_INFO_LEN          15
#define WAVE_INFO_LEN        26//6B(地址) 8B(时间戳) 81B(每周波81个点) 50(50个周波) 3(A,B,C三相)
#define FAULT_INFO_LEN       12
/**********MX25信息读数据长度参数**********/
#define TERMINAL_PARA_LEN    20
#define READ_LOG_NUM         1024
#define READ_SOE_NUM         256
#define READ_FAULT_NUM       64
#define READ_YK_NUM          64
#define READ_WAVE_DATA_LEN   120
#define ECP_DATA_SEND_LEN    230
#define ECP_MULTIPARA_LEN    2

void FlashPara_Init(void);
void SysPara_Init(void);
void EcpPara_Init(void);
void SysFlag_Init(void);
bool PointTable_Init(void);
void Program_Storage(uint16_t sn,uint8_t *data,uint32_t lens);
uint16_t Get_Program_Cs(uint32_t lens);
uint32_t Locate_Log_Position(void);
uint32_t Locate_Soe_Position(void);
uint32_t Locate_Yc_Position(void);
uint32_t Locate_Fault_Position(void);
uint32_t Locate_Yk_Position(void);
uint32_t Locate_Wave_Position(void);
bool Log_Data_Record(uint8_t type,char *ind, ...);
bool Check_Log_Lens(uint32_t lens,uint32_t size);
void Soe_Data_Record(ECPDATA_YX *YX,uint8_t soe,bool updatetime,uint8_t *time);
uint8_t Yc_Data_Record(void);
void Fill_YaoCeFile(uint8_t sn,uint8_t n);
void Switch_YaoCeFile(void);
void Yk_Data_Record(P101YK YK);
void Fault_Data_Record(uint8_t fault,uint8_t *wavetime,uint16_t addr);
void Wave_Data_Record(uint8_t *data,uint32_t lens);
uint32_t Switch_NextWave_Position(void);
void Locate_Log_FileInfo(FILEINFO *FI);
bool Locate_Soe_FileInfo(FILEINFO *FI);
bool Locate_Yc_FileInfo(FILEINFO *FI);
bool Locate_Fault_FileInfo(FILEINFO *FI);
bool Locate_Yk_FileInfo(FILEINFO *FI);
void Locate_Wave_FileInfo(FILEINFO *FI);
void Get_Log_Data(uint32_t read,uint8_t *data,uint32_t lens);
void Get_Soe_Data(uint32_t read,uint8_t *data,uint32_t lens);
void Get_Yk_Data(uint32_t read,uint8_t *data,uint32_t lens);
void Get_Yc_Data(uint32_t read,uint8_t *data,uint32_t lens);
void Get_Fault_Data(uint32_t read,uint8_t *data,uint32_t lens);
void Get_Wave_Data(uint32_t read,uint8_t *data,uint32_t lens);
void Mx25_Data_Storage(uint32_t read,uint8_t *data,uint32_t lens);
uint8_t Get_U32_Hex(uint32_t u32,uint8_t *data,uint8_t lens);
uint8_t Get_U32_Dec(uint32_t u32,uint8_t *data,uint8_t lens);
uint8_t Get_U32_Time(uint32_t u32,uint8_t *data,uint8_t lens);
uint8_t Get_P101_Time(uint8_t *time,uint8_t *data);
void Print_Sys_Para(void);


#endif

