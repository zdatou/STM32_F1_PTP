#ifndef _mk_time_h
#define _mk_time_h
#include "stm32f10x.h"
#include "common.h"

__packed typedef struct _DateTime	/*此结构体定义了NTP时间同步的相关变量*/
{
	u16 year;
  u8 month;						/*	月 */
  u8 day;							/*	天 */
  u8 hour;							/*	时 */
  u8 minute;						/*	分 */
  u8 second;						/*	秒 */
} DateTime;

#define SECS_PERDAY     	86400UL             	// seconds in a day = 60*60*24
#define EPOCH             1970                  // NTP start year
#define CST 							39  //中国标准时间
#define UTC								21  //世界协调时


 unsigned long TimeToSec (DateTime p);

DateTime SecToTime(unsigned int seconds ,uint8_t time_zone);//秒 转 日期时间

#endif
