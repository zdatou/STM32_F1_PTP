#ifndef _timer_h_
#define _timer_h_

#include "stm32f10x.h"
#include "sntp.h"
#include "bds.h"
#include "mktime.h"
#include "ptpd.h"

typedef unsigned long tstamp;


typedef struct ntp_state	/*�˽ṹ�嶨����NTPʱ��ͬ�������״̬*/
{
  uint8_t leap_state;					//����״̬
  uint8_t sync_state;					//ͬ��״̬
	uint8_t com_leap_state;     //��������״̬�ж�
} state;

typedef struct {

	uint16_t Fre;
	uint32_t Num;
  
} FrequencyType; 	

/*�˽ṹ�嶨����������������״̬*/


#define JAN_1900 2208988800u //0X83AA7E80 
#define JAN_2018 1514736000u
#define NULL 0
  tstamp mktime (unsigned int year,   int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec);
void get_sys_timer(time_fp *current);
//void get_sys_time(TimeInternal *time);
void BTimer_Init(void);
void CTimer_Init(void);
void PTimer_Init(void);
void MasterToSlave_Timer(void); //TIM1 TIM3
void tim5_init(void);
void Pulse_Init(void);
void SetFreNum( u16 Fre , u32 Num);
void cal_time_offset(void);
#endif
