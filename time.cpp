#include "stdio.h" 
#include "time.h"

#define EPOCH 1900


static  unsigned long mktime (unsigned int year, unsigned int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec);
int	 main(void)
{
	int localtime;
	localtime = mktime(2017,12,10,10,10,10);
	printf("%d,%x\r\n",localtime,localtime);
	return 0;
}

static  unsigned long mktime (unsigned int year, unsigned int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec)
{
		if (0 >= (int) (mon -= 2))
	{ /**//* 1..12 -> 11,12,1..10 */
			mon += 12;	 /**//* Puts Feb last since it has leap day */
			year -= 1;
	}
	
	return ((((unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) + year*365 - 719499
	)*24 + hour /**//* now have hours */
	)*60 + min /**//* now have minutes */
	)*60 + sec; /**//* finally seconds */
}

unsigned long change_datetime_to_seconds(DateTime *p) 
{
	unsigned long  seconds=0;
	uint32_t total_day=0;
	uint16_t i=0,run_year_cnt=0,l=0;
	
	l = nowdate.year[0];//high
	
	l = (l<<8);
	
	l = l + nowdate.year[1];//low
	
	
	for(i=EPOCH;i<l;i++)
	{
		if((i%400==0) || ((i%100!=0) && (i%4==0))) 
		{
			run_year_cnt += 1;
		}
	}
	
	total_day=(l-EPOCH-run_year_cnt)*365+run_year_cnt*366;
	
	for(i=1;i<=nowdate.month;i++)
	{
		if(i==5 || i==7 || i==10 || i==12)
		{
			total_day += 30;
		}
		if (i==3)
		{
			if (l%400==0 && l%100!=0 && l%4==0) 
			{
				total_day += 29;
			}
			else 
			{
				total_day += 28;
			}
		} 
		if(i==2 || i==4 || i==6 || i==8 || i==9 || i==11)
		{
			total_day += 31;
		}
	}
	
	seconds = (total_day+nowdate.day-1)*24*3600;  
	seconds += nowdate.second;
	seconds += nowdate.minute*60;
	seconds += nowdate.hour*3600;
	
	return seconds;
}

