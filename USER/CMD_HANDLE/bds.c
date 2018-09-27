#include "bds.h" 
#include "usart.h" 								   
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "math.h"
 
char *my_strrstr(char *dest,char *src)  
{  
    char *first=NULL;  
    char *second=NULL;  
    char *str1=dest;  
    char *str2=src;  
//    assert(dest);  
//    assert(src);  
    while((*str1 != '\0'))  
    {  
        first=str1;    //记录下一次比较的起始位置  
        while((*str1 != '\0') && (*str2 != '\0') && (*str1 == *str2))  
        {  
            str1++;  
            str2++;  
        }  
        if(*str2 == '\0')  
            second=first;  //从第一次查找到子串的位置后继续查找  
        str1=first+1;  
        str2=src;  
    }  
    if(*str1 == '\0')  
        return second;  
    return 0;  
} 

char *my_strstr(const char *s1, const char *s2,const int s1len,const int s2len)
{
    int l1, l2;

    l2 = s2len;
    if (!l2)
        return (char *)s1;
    l1 = s1len;
    while (l1 >= l2)
    {
        l1 --;
        if (!memcmp(s1, s2, l2))
            return (char *)s1;
        s1 ++;
    }

    return NULL;
}



//从buf里面得到第cx个逗号所在的位置
//返回值:0~0XFE,代表逗号所在位置的偏移.
//       0XFF,代表不存在第cx个逗号							  
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}
//m^n函数
//返回值:m^n次方.
u32 NMEA_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}
//str转换为数字,以','或者'*'结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值
int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		if(*p==','||(*p=='*'))break;//遇到结束了
		if(*p=='.'){mask|=0X01;p++;}//遇到小数点了
		else if(*p>'9'||(*p<'0'))	//有非法字符
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//去掉负号
	for(i=0;i<ilen;i++)	//得到整数部分数据
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//最多取5位小数
	*dx=flen;	 		//小数点位数
	for(i=0;i<flen;i++)	//得到小数部分数据
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	  							 
////分析GPGSV信息
////gpsx:nmea信息结构体
////buf:接收到的GPS数据缓冲区首地址
//void NMEA_GNGSV_Analysis(nmea_msg *gpsx,u8 *buf)
//{
//	u8 *p,*p1,dx;
//	u8 len,i,j,slx=0;
//	u8 posx;   	 
//	p=buf;
//	p1=(u8*)strstr((const char *)p,"GPGSV");
//	len=p1[7]-'0';								//得到GPGSV的条数
//	posx=NMEA_Comma_Pos(p1,3); 					//得到可见卫星总数

//	if(posx!=0XFF)gpsx->gpsvnum=NMEA_Str2num(p1+posx,&dx);
//	for(i=0;i<len;i++)
//	{	 
//		p1=(u8*)strstr((const char *)p,"GNGSV");  
//		for(j=0;j<4;j++)
//		{	  
//			posx=NMEA_Comma_Pos(p1,4+j*4);
//			if(posx!=0XFF)gpsx->slmsg[slx].num=NMEA_Str2num(p1+posx,&dx);	//得到卫星编号
//			else break; 
//			posx=NMEA_Comma_Pos(p1,5+j*4);
//			if(posx!=0XFF)gpsx->slmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx);//得到卫星仰角 
//			else break;
//			posx=NMEA_Comma_Pos(p1,6+j*4);
//			if(posx!=0XFF)gpsx->slmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx);//得到卫星方位角
//			else break; 
//			posx=NMEA_Comma_Pos(p1,7+j*4);
//			if(posx!=0XFF)gpsx->slmsg[slx].sn=NMEA_Str2num(p1+posx,&dx);	//得到卫星信噪比
//			else break;
//			slx++;	   
//		}   
// 		p=p1+1;//切换到下一个GPGSV信息
//	}   
//}

void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p,*p1,dx;
	u8 len,i,j,slx=0;
	u8 posx;   	 
	p=buf;
	p1=(u8*)strstr((const char *)p,"GPGSV");
	len=p1[7]-'0';								//得到GPGSV的条数
	posx=NMEA_Comma_Pos(p1,3); 					//得到可见卫星总数
	
	if(posx!=0XFF)gpsx->gpsvnum=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(u8*)strstr((const char *)p,"GPGSV");  
		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].num=NMEA_Str2num(p1+posx,&dx);	//得到卫星编号
			else break; 
			posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx);//得到卫星仰角 
			else break;
			posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx);//得到卫星方位角
			else break; 
			posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].sn=NMEA_Str2num(p1+posx,&dx);	//得到卫星信噪比
			else break;
			slx++;	   
		}   
 		p=p1+1;//切换到下一个GPGSV信息
	}   
}

void NMEA_BDGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p,*p1,dx;
	u8 len,i,j,slx=0;
	u8 posx;   	 
	p=buf;
	p1=(u8*)strstr((const char *)p,"BDGSV");
	len=p1[7]-'0';								//得到GPGSV的条数
	posx=NMEA_Comma_Pos(p1,3); 					//得到可见卫星总数
	if(posx!=0XFF)gpsx->bdsvnum=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(u8*)strstr((const char *)p,"BDGSV");  
		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->BDslmsg[slx].num=NMEA_Str2num(p1+posx,&dx);	//得到卫星编号
			else break; 
			posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->BDslmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx);//得到卫星仰角 
			else break;
			posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->BDslmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx);//得到卫星方位角
			else break; 
			posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->BDslmsg[slx].sn=NMEA_Str2num(p1+posx,&dx);	//得到卫星信噪比
			else break;
			slx++;	   
		}   
 		p=p1+1;
	}   
}
//分析GPGGA信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)strstr((const char *)buf,"GNGGA");
	posx=NMEA_Comma_Pos(p1,6);								//得到GPS状态
	if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);	
	posx=NMEA_Comma_Pos(p1,7);								//得到用于定位的卫星数
	if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 
	posx=NMEA_Comma_Pos(p1,9);								//得到海拔高度
	if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx);  
}
//分析GPGSA信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx; 
	u8 i;   
	p1=(u8*)strstr((const char *)buf,"GNGSA");
	posx=NMEA_Comma_Pos(p1,2);								//得到定位类型
	if(posx!=0XFF)gpsx->fixmode=NMEA_Str2num(p1+posx,&dx);	
	for(i=0;i<12;i++)										//得到定位卫星编号
	{
		posx=NMEA_Comma_Pos(p1,3+i);					 
		if(posx!=0XFF)gpsx->possl[i]=NMEA_Str2num(p1+posx,&dx);
		else break; 
	}				  
	posx=NMEA_Comma_Pos(p1,15);								//得到PDOP位置精度因子
	if(posx!=0XFF)gpsx->pdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,16);								//得到HDOP位置精度因子
	if(posx!=0XFF)gpsx->hdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,17);								//得到VDOP位置精度因子
	if(posx!=0XFF)gpsx->vdop=NMEA_Str2num(p1+posx,&dx);  
}
//分析GPRMC信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GNRMC_Analysis(nmea_msg *gpsx,u8 *buf)//双星解调
{
	static u8 *p1,dx;			 
	static u8 posx;     
	u32 temp;	   
	p1=(u8*)strstr((const char *)buf,"GNRMC");//"$GPRMC",经常有&和GPRMC分开的情况,故只判断GPRMC.
		if( p1 == NULL)
	{
	  return;
	}
  posx=NMEA_Comma_Pos(p1,1);								//得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//得到UTC时间,去掉ms
		gpsx->utc.hour=temp/10000;
		gpsx->utc.minute=(temp/100)%100;
		gpsx->utc.second=temp%100;	 	 
	}
	posx=NMEA_Comma_Pos(p1,2);							
	if(posx!=0XFF)gpsx->posstate=*(p1+posx);		
	posx=NMEA_Comma_Pos(p1,3);								//得到纬度
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp;	//得到°
	}
	posx=NMEA_Comma_Pos(p1,4);								//南纬还是北纬 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								//得到经度
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp;	
	}
	posx=NMEA_Comma_Pos(p1,6);								//东经还是西经
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	posx=NMEA_Comma_Pos(p1,9);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
		gpsx->utc.day=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
		posx=NMEA_Comma_Pos(p1,12);								
	if(posx!=0XFF)gpsx->posmode=*(p1+posx);		
}

void NMEA_BDRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	static u8 *p1,dx;			 
	static u8 posx;     
	u32 temp;	   
	p1=(u8*)strstr((const char *)buf,"BDRMC");//"$GPRMC",经常有&和GPRMC分开的情况,故只判断GPRMC.
	if( p1 == NULL)
	{
	  return;
	}
  posx=NMEA_Comma_Pos(p1,1);								//得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//得到UTC时间,去掉ms
		gpsx->utc.hour=temp/10000;
		gpsx->utc.minute=(temp/100)%100;
		gpsx->utc.second=temp%100;	 	 
	}
	posx=NMEA_Comma_Pos(p1,2);							
	if(posx!=0XFF)gpsx->posstate=*(p1+posx);		
	posx=NMEA_Comma_Pos(p1,3);								//得到纬度
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp;	//得到°
	}
	posx=NMEA_Comma_Pos(p1,4);								//南纬还是北纬 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								//得到经度
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp;	
	}
	posx=NMEA_Comma_Pos(p1,6);								//东经还是西经
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	posx=NMEA_Comma_Pos(p1,9);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
		gpsx->utc.day=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
		posx=NMEA_Comma_Pos(p1,12);								
	if(posx!=0XFF)gpsx->posmode=*(p1+posx);		
}

void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	static u8 *p1,dx;			 
	static u8 posx;     
	u32 temp;	   
	p1=(u8*)strstr((const char *)buf,"GPRMC");//"$GPRMC",经常有&和GPRMC分开的情况,故只判断GPRMC.
		if( p1 == NULL)
	{
	  return;
	}
  posx=NMEA_Comma_Pos(p1,1);								//得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//得到UTC时间,去掉ms
		gpsx->utc.hour=temp/10000;
		gpsx->utc.minute=(temp/100)%100;
		gpsx->utc.second=temp%100;	 	 
	}
	posx=NMEA_Comma_Pos(p1,2);							
	if(posx!=0XFF)gpsx->posstate=*(p1+posx);		
	posx=NMEA_Comma_Pos(p1,3);								//得到纬度
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp;	//得到°
	}
	posx=NMEA_Comma_Pos(p1,4);								//南纬还是北纬 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								//得到经度
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp;	
	}
	posx=NMEA_Comma_Pos(p1,6);								//东经还是西经
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	posx=NMEA_Comma_Pos(p1,9);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
		gpsx->utc.day=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
		posx=NMEA_Comma_Pos(p1,12);								
	if(posx!=0XFF)gpsx->posmode=*(p1+posx);		
}
//分析GPVTG信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)strstr((const char *)buf,"GNVTG");							 
	posx=NMEA_Comma_Pos(p1,7);								//得到地面速率
	if(posx!=0XFF)
	{
		gpsx->speed=NMEA_Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		//确保扩大1000倍
	}
}  

void NMEA_GNZDA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx; 
	u32 temp;		
	p1=(u8*)strstr((const char *)buf,"GNZDA");							 
	posx=NMEA_Comma_Pos(p1,1);								//得到UTC时间
		if(posx!=0XFF)
		{
			temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//得到UTC时间,去掉ms
			gpsx->utc.hour=temp/10000;
			gpsx->utc.minute=(temp/100)%100;
			gpsx->utc.second=temp%100;
		}			
		posx=NMEA_Comma_Pos(p1,2);								
		if(posx!=0XFF)
		{
			temp=NMEA_Str2num(p1+posx,&dx);		 				
			gpsx->utc.day=temp;
		} 
		posx=NMEA_Comma_Pos(p1,3);								
		if(posx!=0XFF)
		{
			temp=NMEA_Str2num(p1+posx,&dx);		 				
			gpsx->utc.month=temp;
		} 
		posx=NMEA_Comma_Pos(p1,4);								
		if(posx!=0XFF)
		{
			temp=NMEA_Str2num(p1+posx,&dx);		 				
			gpsx->utc.year=temp;
		}
		posx=NMEA_Comma_Pos(p1,5);								
		if(posx!=0XFF)
		{
			temp=NMEA_Str2num(p1+posx,&dx);		 				
			gpsx->time.PPsState = temp;
		}
		posx=NMEA_Comma_Pos(p1,6);								
		if(posx!=0XFF)
		{
			gpsx->time.TimeSource = *(p1+posx);
		}
}

void TOD_Analysis(DateTime *time , u8 *buf , uint16_t len)
{
  	u8 *p1,dx;			 
		u8 posx; 
		u32 temp;		
	
	
  p1=(u8*)my_strstr((const char *)buf,"XHTOD",len,5);//"$GNRMC",经常有&和GNRMC分开的情况,故只判断GPRMC.
	 
	if(p1 == NULL)
	{
		return;
	}
	
   posx=NMEA_Comma_Pos(p1,1);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
		time->year=temp; 
	} 
	posx=NMEA_Comma_Pos(p1,2);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
		time->month=temp; 
	} 
	posx=NMEA_Comma_Pos(p1,3);								//得到UTC日期
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//得到UTC日期
		time->day=temp; 
	} 
	posx=NMEA_Comma_Pos(p1,4);								//得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);	 	
		time->hour=temp;	 
	}
	posx=NMEA_Comma_Pos(p1,5);								//得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);	 	
		time->minute = temp;	 
	}
	posx=NMEA_Comma_Pos(p1,6);								//得到UTC时间
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);	 	
		time->second = temp;	 
	}
}

//提取NMEA-0183信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void GNSS_Analysis(nmea_msg *gpsx,u8 *buf)
{
//	NMEA_BDGSV_Analysis(gpsx,buf);
//	NMEA_GPGSV_Analysis(gpsx,buf);
//	NMEA_GPRMC_Analysis(gpsx,buf);	//GPRMC解析
//	NMEA_GNRMC_Analysis(gpsx,buf);
	NMEA_BDRMC_Analysis(gpsx,buf);
	
}



void Ublox_CheckSum(u8 *buf,u16 len,u8* cka,u8*ckb)
{
	u16 i;
	*cka=0;*ckb=0;
	for(i=0;i<len;i++)
	{
		*cka=*cka+buf[i];
		*ckb=*ckb+*cka;
	}
}
/*
u8 Ublox_Cfg_Ack_Check(void)
{			 
	u16 len=0,i;
	u8 rval=0;
	while((USART3_RX_STA&0X8000)==0 && len<100)
	{
		len++;
		delay_ms(5);
	}		 
	if(len<250)   	
	{
		len=USART3_RX_STA&0X7FFF;	
		for(i=0;i<len;i++)if(USART3_RX_BUF[i]==0XB5)break;
		if(i==len)rval=2;						
		else if(USART3_RX_BUF[i+3]==0X00)rval=3;
		else rval=0;	   						
	}else rval=1;							
    USART3_RX_STA=0;						
	return rval;  
}


u8 Ublox_Cfg_Cfg_Save(void)
{
	u8 i;
	_ublox_cfg_cfg *cfg_cfg=(_ublox_cfg_cfg *)USART3_TX_BUF;
	cfg_cfg->header=0X62B5;		//cfg header
	cfg_cfg->id=0X0906;			//cfg cfg id
	cfg_cfg->dlength=13;		//Êý¾ÝÇø³¤¶ÈÎª13¸ö×Ö½Ú.		 
	cfg_cfg->clearmask=0;		//Çå³ýÑÚÂëÎª0
	cfg_cfg->savemask=0XFFFF; 	//±£´æÑÚÂëÎª0XFFFF
	cfg_cfg->loadmask=0; 		//¼ÓÔØÑÚÂëÎª0 
	cfg_cfg->devicemask=4; 		//±£´æÔÚEEPROMÀïÃæ		 
	Ublox_CheckSum((u8*)(&cfg_cfg->id),sizeof(_ublox_cfg_cfg)-4,&cfg_cfg->cka,&cfg_cfg->ckb);
	Ublox_Send_Date((u8*)cfg_cfg,sizeof(_ublox_cfg_cfg));//·¢ËÍÊý¾Ý¸øNEO-6M     
	for(i=0;i<6;i++)if(Ublox_Cfg_Ack_Check()==0)break;		//EEPROMÐ´ÈëÐèÒª±È½Ï¾ÃÊ±¼ä,ËùÒÔÁ¬ÐøÅÐ¶Ï¶à´Î
	return i==6?1:0;
}
//ÅäÖÃNMEAÊä³öÐÅÏ¢¸ñÊ½
//msgid:Òª²Ù×÷µÄNMEAÏûÏ¢ÌõÄ¿,¾ßÌå¼ûÏÂÃæµÄ²ÎÊý±í
//      00,GPGGA;01,GPGLL;02,GPGSA;
//		03,GPGSV;04,GPRMC;05,GPVTG;
//		06,GPGRS;07,GPGST;08,GPZDA;
//		09,GPGBS;0A,GPDTM;0D,GPGNS;
//uart1set:0,Êä³ö¹Ø±Õ;1,Êä³ö¿ªÆô.	  
//·µ»ØÖµ:0,Ö´ÐÐ³É¹¦;ÆäËû,Ö´ÐÐÊ§°Ü.
u8 Ublox_Cfg_Msg(u8 msgid,u8 uart1set)
{
	_ublox_cfg_msg *cfg_msg=(_ublox_cfg_msg *)USART3_TX_BUF;
	cfg_msg->header=0X62B5;		//cfg header
	cfg_msg->id=0X0106;			//cfg msg id
	cfg_msg->dlength=8;			//Êý¾ÝÇø³¤¶ÈÎª8¸ö×Ö½Ú.	
	cfg_msg->msgclass=0XF0;  	//NMEAÏûÏ¢
	cfg_msg->msgid=msgid; 		//Òª²Ù×÷µÄNMEAÏûÏ¢ÌõÄ¿
	cfg_msg->iicset=1; 			//Ä¬ÈÏ¿ªÆô
	cfg_msg->uart1set=uart1set; //¿ª¹ØÉèÖÃ
	cfg_msg->uart2set=1; 	 	//Ä¬ÈÏ¿ªÆô
	cfg_msg->usbset=1; 			//Ä¬ÈÏ¿ªÆô
	cfg_msg->spiset=1; 			//Ä¬ÈÏ¿ªÆô
	cfg_msg->ncset=1; 			//Ä¬ÈÏ¿ªÆô	  
	Ublox_CheckSum((u8*)(&cfg_msg->id),sizeof(_ublox_cfg_msg)-4,&cfg_msg->cka,&cfg_msg->ckb);
	Ublox_Send_Date((u8*)cfg_msg,sizeof(_ublox_cfg_msg));//·¢ËÍÊý¾Ý¸øNEO-6M    
	return Ublox_Cfg_Ack_Check();
}


u8 Ublox_Cfg_Tp(u32 interval,u32 length,signed char status)
{
	_ublox_cfg_tp *cfg_tp=(_ublox_cfg_tp *)USART3_TX_BUF;
	cfg_tp->header=0X62B5;		//cfg header
	cfg_tp->id=0X0706;			//cfg tp id
	cfg_tp->dlength=20;			//Êý¾ÝÇø³¤¶ÈÎª20¸ö×Ö½Ú.
	cfg_tp->interval=interval;	//Âö³å¼ä¸ô,us
	cfg_tp->length=length;		//Âö³å¿í¶È,us
	cfg_tp->status=status;	   	//Ê±ÖÓÂö³åÅäÖÃ
	cfg_tp->timeref=0;			//²Î¿¼UTC Ê±¼ä
	cfg_tp->flags=0;			//flagsÎª0
	cfg_tp->reserved=0;		 	//±£ÁôÎ»Îª0
	cfg_tp->antdelay=820;    	//ÌìÏßÑÓÊ±Îª820ns
	cfg_tp->rfdelay=0;    		//RFÑÓÊ±Îª0ns
	cfg_tp->userdelay=0;    	//ÓÃ»§ÑÓÊ±Îª0ns
	Ublox_CheckSum((u8*)(&cfg_tp->id),sizeof(_ublox_cfg_tp)-4,&cfg_tp->cka,&cfg_tp->ckb);
	Ublox_Send_Date((u8*)cfg_tp,sizeof(_ublox_cfg_tp));//·¢ËÍÊý¾Ý¸øNEO-6M  
	return Ublox_Cfg_Ack_Check();
}

u8 Ublox_Cfg_Rate(u16 measrate,u8 reftime)
{
	_ublox_cfg_rate *cfg_rate=(_ublox_cfg_rate *)USART3_TX_BUF;
 	if(measrate<200)return 1;	
 	cfg_rate->header=0X62B5;	
	cfg_rate->id=0X0806;	 	//cfg rate id
	cfg_rate->dlength=6;	 	
	cfg_rate->measrate=measrate;//Âö³å¼ä¸ô,us
	cfg_rate->navrate=1;		
	cfg_rate->timeref=reftime; 	
	Ublox_CheckSum((u8*)(&cfg_rate->id),sizeof(_ublox_cfg_rate)-4,&cfg_rate->cka,&cfg_rate->ckb);
	Ublox_Send_Date((u8*)cfg_rate,sizeof(_ublox_cfg_rate));//·¢ËÍÊý¾Ý¸øNEO-6M 
	return Ublox_Cfg_Ack_Check();
}


void Ublox_Send_Date(u8* dbuf,u16 len)
{
	u16 j;
	for(j=0;j<len;j++)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR=dbuf[j];  
	}	
}


*/






