#ifndef __RTC_H
#define __RTC_H	 
#include "common.h" 
 //////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//RTC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/5
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//�޸�˵��
//V1.1 20140726
//����:RTC_Get_Week����,���ڸ�����������Ϣ,�õ�������Ϣ.
////////////////////////////////////////////////////////////////////////////////// 

	
	
u8 My_RTC_Init(void);						//RTC��ʼ��
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week);
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);

typedef struct Time_Format
{
	RTC_DateTypeDef Date;
	RTC_TimeTypeDef Time;
}time;


#endif

















