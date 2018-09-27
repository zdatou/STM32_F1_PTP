#ifndef _mk_time_h
#define _mk_time_h
#include "stm32f10x.h"
#include "common.h"

__packed typedef struct _DateTime	/*�˽ṹ�嶨����NTPʱ��ͬ������ر���*/
{
	u16 year;
  u8 month;						/*	�� */
  u8 day;							/*	�� */
  u8 hour;							/*	ʱ */
  u8 minute;						/*	�� */
  u8 second;						/*	�� */
} DateTime;

#define SECS_PERDAY     	86400UL             	// seconds in a day = 60*60*24
#define EPOCH             1970                  // NTP start year
#define CST 							39  //�й���׼ʱ��
#define UTC								21  //����Э��ʱ


 unsigned long TimeToSec (DateTime p);

DateTime SecToTime(unsigned int seconds ,uint8_t time_zone);//�� ת ����ʱ��

#endif
