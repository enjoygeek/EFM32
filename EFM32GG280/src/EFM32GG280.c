/******************************************************************!
*Copyright (c), 2008-2017, Qingdao Topscomm Communication Co .Ltd.
*File name: EFM32GG280.c
*Version: 0.1.0
*Date: 2016-09-20
*Author: WANGJIANHUI
*Description: 1.XS3:115200:  维护:维护端口
*             2.XS5:115200:  GPRS:GPRS端口
*             3.XS7:9600:    GPS:输入GPS数据
*******************************************************************/
#include "include.h"

int main(void)
{
	BSP_Init();
	APP_Init();

	xTaskCreate(Test_Task,(const char *)"task0",400,NULL,1,NULL);
    xTaskCreate(Power_Monitor,(const char *)"task1",300,NULL,1,NULL);
    xTaskCreate(Icp_Protocol_Comm,(const char *)"task2",990,NULL,1,NULL);
	xTaskCreate(Ecp_Protocol_Comm,(const char *)"task3",990,NULL,1,NULL);
	xTaskCreate(Call_Wave,(const char *)"task4",700,NULL,1,&xCall_Wave);
	xTaskCreate(Data_Report,(const char *)"task5",600,NULL,1,&xData_Report);
	xTaskCreate(Data_Send_Reply,(const char *)"task6",900,NULL,1,&xData_Send_Reply);
    xTaskCreate(Ecp_File_Report,(const char *)"task7",900,NULL,1,&xEcp_File_Report);

	vTaskStartScheduler();

	while(1);

	return 0;
}


