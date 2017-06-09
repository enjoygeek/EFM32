/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: ecp_app.h
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description:
*******************************************************************/
#ifndef ECP_APP_H_
#define ECP_APP_H_


void App_Send_Data(ECPFRAME *EF);
void App_Send_AllCall(ECPFRAME *EF);
bool App_Remote_Control(ECPFRAME *EF);
bool App_Protocol_Timing(ECPFRAME *EF);
void App_Send_Test_Link(ECPFRAME *EF);
void App_Reset_Zizhan_Process(ECPFRAME *EF);
void App_Get_Delay(ECPFRAME *EF);
void App_Call_DirFile(ECPFRAME *EF);
void App_Call_Dir(ECPFRAME *EF);
void App_Call_2017Dir(ECPFRAME *EF);
bool App_Call_File(ECPFRAME *EF);
void App_File_Ready(ECPFRAME *EF);
void App_Call_FileDir(ECPFRAME *EF);
void App_Request_File(ECPFRAME *EF);
void App_Section_Ready(ECPFRAME *EF);
void App_Switch_Fixed_Area(ECPFRAME *EF);
void App_Read_Fixed_Area(ECPFRAME *EF);
void App_Send_FileEnd(ECPFRAME *EF);
void App_Send_Last_Section(ECPFRAME *EF);
void App_Send_Segment(ECPFRAME *EF);
void App_Send_FileHead(ECPFRAME *EF);
void App_Get_Segment(ECPFRAME *EF,uint8_t priority,uint8_t sn);
void App_Send_Last_Segment(ECPFRAME *EF);
bool App_Set_Para(ECPFRAME *EF);
void App_Read_Data(ECPFRAME *EF);
void App_Read_MultiPara(ECPFRAME *EF);
void App_Read_Safe_Info(ECPFRAME *EF);
void App_Authentication(ECPFRAME *EF);
void App_Public_Key_Verification(ECPFRAME *EF);
void App_Public_Key_Update(ECPFRAME *EF);
void App_Symmetric_key_Update(ECPFRAME *EF);
void App_Key_Consult(ECPFRAME *EF);
void App_Get_Random(ECPFRAME *EF);
uint16_t Get_AllCall_Yx(uint8_t *data);
uint16_t Get_AllCall_Yc(uint8_t *data);
void Locate_File_Info(ECPFRAME *EF);
bool Ecp_File_Send(ECPFRAME *EF);
bool App_Send_Reply(ECPFRAME *EF);
bool Ecp_Yc_Timing_Report(ECPFRAME *EF);
bool Yc_Sudden_Report(ECPDATA_YC *YC,uint16_t value,uint16_t threshold);
uint16_t Ecp_Get_MultiPara(uint8_t vsq,uint8_t *addr, uint8_t *para);
uint32_t Search_MultiPara_Pos(uint8_t *para,uint16_t lens,uint16_t pos);


#endif

