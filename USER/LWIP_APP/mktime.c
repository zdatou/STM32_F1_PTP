#include "stdio.h" 
#include "mktime.h"
#include <string.h>



static DateTime calc_date_time(unsigned int seconds);



 unsigned long TimeToSec (DateTime p)
{
		DateTime *tmp = &p;
	
	
		if (0 >= (int) (tmp->month -= 2))
	{ /**//* 1..12 -> 11,12,1..10 */
			tmp->month += 12;	 /**//* Puts Feb last since it has leap day */
			tmp->year -= 1;
	}
	
	return ((((unsigned long) (tmp->year/4 - tmp->year/100 + tmp->year/400 + 367*tmp->month/12 + tmp->day) + tmp->year*365 - 719499
	)*24 + tmp->hour /**//* now have hours */
	)*60 + tmp->minute /**//* now have minutes */
	)*60 + tmp->second; /**//* finally seconds */
}





DateTime SecToTime( unsigned int seconds ,uint8_t time_zone) //秒 转 日期时间
{
        switch (time_zone)
        {
                case 0:
                        seconds -=  12*3600;
                break;
                case 1:
                        seconds -=  11*3600;
                break;
                case 2:
                        seconds -=  10*3600;
                break;
                case 3:
                        seconds -=  (9*3600+30*60);
                break;
                case 4:
                        seconds -=  9*3600;
                break;
                case 5:
                case 6:
                        seconds -=  8*3600;
                break;
                case 7:
                case 8:
                        seconds -=  7*3600;
                break;
                case 9:
                case 10:
                seconds -=  6*3600;
                break;
                case 11:
                case 12:
                case 13:
                        seconds -= 5*3600;
                break;
                case 14:
                        seconds -=  (4*3600+30*60);
                break;
                case 15:
                case 16:
                        seconds -=  4*3600;
                break;
                case 17:
                        seconds -=  (3*3600+30*60);
                break;
                case 18:
                        seconds -=  3*3600;
                break;
                case 19:
                        seconds -=  2*3600;
                break;
                case 20:
                        seconds -=  1*3600;
                break;
                case 21:                           
                case 22:
                break;
                case 23:
                case 24:
                case 25:
                        seconds +=  1*3600;
                break;
                case 26:
                case 27:
                        seconds +=  2*3600;
                break;
                case 28:
                case 29:
                        seconds +=  3*3600;
                break;
                case 30:
                        seconds +=  (3*3600+30*60);
                break;
                case 31:
                        seconds +=  4*3600;
                break;
                case 32:
                        seconds +=  (4*3600+30*60);
                break;
                case 33:
                        seconds +=  5*3600;
                break;
                case 34:
                        seconds +=  (5*3600+30*60);
                break;
                case 35:
                        seconds +=  (5*3600+45*60);
                break;
                case 36:
                        seconds +=  6*3600;
                break;
                case 37:
                        seconds +=  (6*3600+30*60);
                break;
                case 38:
                        seconds +=  7*3600;
                break;
                case 39:
                        seconds +=  8*3600;
                break;
                case 40:
                        seconds +=  9*3600;
                break;
                case 41:
                        seconds +=  (9*3600+30*60);
                break;
                case 42:
                        seconds +=  10*3600;
                break;
                case 43:
                        seconds +=  (10*3600+30*60);
                break;
                case 44:
                        seconds +=  11*3600;
                break;
                case 45:
                        seconds +=  (11*3600+30*60);
                break;
                case 46:
                        seconds +=  12*3600;
                break;
                case 47:
                        seconds +=  (12*3600+45*60);
                break;
                case 48:
                        seconds +=  13*3600;
                break;
                case 49:
                        seconds +=  14*3600;
                break;
        }
         
       return (calc_date_time(seconds));                                                                                                                                                                                                         /*?UTC??????*/
}
 



 
static DateTime calc_date_time(unsigned int seconds)
{
		   	DateTime nowdate;
        uint8_t yf=0;
        uint32_t p_year_total_sec;
        uint32_t r_year_total_sec;
        unsigned  long long n=0,d=0,total_d=0,rz=0;
         static uint16_t y=0,r=0,yr=0;
        signed long long yd=0;
				u8 tmp[2] ={0};
         
	
	
				y=0,r=0,yr=0;
	
        n = seconds;
        total_d = seconds/(SECS_PERDAY);
        d=0;
        p_year_total_sec=SECS_PERDAY*365;
        r_year_total_sec=SECS_PERDAY*366;
        while(n>=p_year_total_sec) 
        {
                if((EPOCH+r)%400==0 || ((EPOCH+r)%100!=0 && (EPOCH+r)%4==0))
                {
                        n = n -(r_year_total_sec);
                        d = d + 366;
                }
                else
                {
                        n = n - (p_year_total_sec);
                        d = d + 365;
                }
                r+=1;
                y+=1;
 
        }
 
        y += EPOCH;
			
        tmp[0] = (uint8_t)((y & 0xff00)>>8);
        tmp[1] = (uint8_t)(y & 0xff);
				nowdate.year = ((tmp[0]<<8)+tmp[1]),		
        yd=0;
        yd = total_d - d;
 
        yf=1;
        while(yd>=28) 
        {
         
                if(yf==1 || yf==3 || yf==5 || yf==7 || yf==8 || yf==10 || yf==12)
                {
                        yd -= 31;
                        if(yd<0)break;
                        rz += 31;
                }
                 
                if (yf==2)
                {
                        if (y%400==0 || (y%100!=0 && y%4==0)) 
                        {
                                yd -= 29;
                                if(yd<0)break;
                                rz += 29;
                        }
                        else
                        {
                                yd -= 28;
                                if(yd<0)break;
                                rz += 28;
                        }
                } 
                if(yf==4 || yf==6 || yf==9 || yf==11 )
                {
                        yd -= 30;
                        if(yd<0)break;
                        rz += 30;
                }
                yf += 1;
 
        }
        nowdate.month=yf;
        yr = total_d-d-rz;
         
        yr += 1;
         
        nowdate.day=yr;
         
        //calculation for time
        seconds = seconds%SECS_PERDAY;
        nowdate.hour = seconds/3600;
        nowdate.minute = (seconds%3600)/60;
        nowdate.second = (seconds%3600)%60;
				
				return nowdate;
}
