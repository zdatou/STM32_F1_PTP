#include "rtc.h"
#include "usart.h" 
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

NVIC_InitTypeDef   NVIC_InitStructure;

//RTCʱ������
//hour,min,sec:Сʱ,����,����
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ�� 
ErrorStatus RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	
}
//RTC��������
//year,month,date:��(0~99),��(1~12),��(0~31)
//week:����(1~7,0,�Ƿ�!)
//����ֵ:SUCEE(1),�ɹ�
//       ERROR(0),�����ʼ��ģʽʧ�� 
ErrorStatus RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
u8 My_RTC_Init(void)
{
		u8 state = 0;
	RTC_InitTypeDef RTC_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ������� 

	
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div25);		//����RTCʱ��(RTCCLK),ѡ��HSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ�� 

    RTC_InitStructure.RTC_AsynchPrediv = (100-1)&0x7f;//RTC�첽��Ƶϵ��(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = (4000-1)&0x7FFF;//RTCͬ����Ƶϵ��(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC����Ϊ,24Сʱ��ʽ
    RTC_Init(&RTC_InitStructure);
	
  //	RTC_ITConfig(RTC_IT_SEC, ENABLE);
 
		 state = RTC_Set_Time(0,0,0,RTC_H12_AM);	//����ʱ��
	#if debug
	  if(state != SUCCESS)
		{
	  	com_send_str(&usart1, "\r\nRTC Time ERROR\r\n", sizeof("RTC ERROR\r\n"));
		}
	#endif
		 state = RTC_Set_Date(18,1,1,1);		//��������
	#if debug
	 	  if(state != SUCCESS)
		{
	  	com_send_str(&usart1, "\r\nRTC Date ERROR\r\n", sizeof("RTC ERROR\r\n"));
		}
  #endif
 
	return 0;
}

//CNT �Զ���װ
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);
	
	RTC_WakeUpClockConfig(wksel);
	
	RTC_SetWakeUpCounter(cnt);
	
	
	RTC_ClearITPendingBit(RTC_IT_WUT); 
  EXTI_ClearITPendingBit(EXTI_Line22);
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	RTC_WakeUpCmd( ENABLE);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
 
 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}




//RTC WAKE UP
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);
	
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);							
}


void get_sys_time(uint32_t RTC_Format,time *p)
{

	  RTC_GetDate(RTC_Format,&(p->Date));
		RTC_GetTime(RTC_Format,&(p->Time));
}

void set_sys_time(time *p)
{
	RTC_Set_Time(p->Time.RTC_Hours,p->Time.RTC_Minutes,p->Time.RTC_Seconds,RTC_H12_AM);
  RTC_Set_Date(p->Date.RTC_Year,p->Date.RTC_Month,p->Date.RTC_Date,p->Date.RTC_WeekDay);
}













