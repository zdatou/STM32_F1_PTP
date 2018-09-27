#include "Allheader.h"
 
extern nmea_msg gpsx;
extern STUsart usart2;
extern STUsart usart3;
extern STUsart usart5;
extern DateTime time;
extern RuTame GPSTame; 
extern ts_FLASH_DATA_SAVE  TS_FDS;
 
extern RunTimeOpts rtOpts;
extern uint32 systime;
extern PtpClock ptpClock;
 u8 remote_ip_addr1;
 u8 remote_ip_addr2;
 u8 remote_ip_addr3;
 u8 remote_ip_addr4;
 
 u8 netif_ip_addr1;
 u8 netif_ip_addr2;
 u8 netif_ip_addr3;
 u8 netif_ip_addr4;

 u8 netif_ip_gw1;
 u8 netif_ip_gw2;
 u8 netif_ip_gw3;
 u8 netif_ip_gw4;

 u8 netif_ip_netmask1;
 u8 netif_ip_netmask2;
 u8 netif_ip_netmask3;
 u8 netif_ip_netmask4;



uint8 TOD_Comma_Pos(uint8 *buf,uint8 cx);
int TOD_Str2num(uint8 *buf,uint8*dx);
char TOD_CheckXOR(char *buf, short len);

msgstate cmd_state;

extern struct ptp_c ptpMonitorl[ptpMonitorlConstant];
extern struct netif netif;
extern msgstate cmd_state;//cmd state
extern RunTimeOpts rtOpts; 
extern PtpClock ptpClock;
extern ForeignMasterRecord ptpForeignRecords[DEFAULT_MAX_FOREIGN_RECORDS];
/**************************************************************************
 *
 * 函数功能: AT串口命令查询
 *
 * 参数说明: 字符串，长度
 *
 * 返 回 值: 命令代码
 *
 **************************************************************************/

uint8 cmd_handle_usart5(void *str, uint16 len)
{
	
	struct ip_addr netif_ipaddr;
  struct ip_addr netif_netmask;
  struct ip_addr netif_gw;
	int ret = 10;
	uint16 tmpcnt = 0;
	uint8 count = 0;
	uint8 ip_count = 0;
	uint8 flag = 0;
	char buff[5] = {0};
	char *cpos = 0;
	static char p[512] = {0};
	memcpy(p, str,512);
 
		if((cpos = strstr(p, "E1 Master") )!= NULL  )
	{
		TS_FDS.PTP_MODE = emPTPMaster;
		memset(ptpMonitorl,0,sizeof(ptpMonitorl));
		PTPd_Init();
		rtOpts.slaveOnly = 0;
		rtOpts.clockQuality.clockAccuracy = 200;
		STMFLASH_Write(FLASH_SAVE_Data,(u16*)&TS_FDS,20);
		ptpdStartup(&ptpClock, &rtOpts, ptpForeignRecords);
		ret = PTP_MASTER_BACK;
	}
	if((cpos = strstr(p, "E1 Slave") )!= NULL  )
	{
		TS_FDS.PTP_MODE = emPTPSlave;
		PTPd_Init();
		memset(ptpMonitorl,0,sizeof(ptpMonitorl));
		rtOpts.slaveOnly = 1;
		rtOpts.clockQuality.clockAccuracy = 0xFE;
	  STMFLASH_Write(FLASH_SAVE_Data,(u16*)&TS_FDS,20);
	 
  	com_send_str(&usart2, p, strlen(p));
		 ptpdStartup(&ptpClock, &rtOpts, ptpForeignRecords);
		ret = PTP_SLAVE_BACK;
	}
//系统命令	
	if(strstr(p, "AT\r\n") != NULL)
	{		
		ret = SYSTEM;
	}
	//UDP sntp 处理
		if((cpos = strstr(p, "AT+SETP=")) != NULL)
	{
		//判断数据
	   	flag = 1;
      tmpcnt = (cpos - (char*)p);
		//数据处理 IP以及端口处理
		if(flag == 1)
		{
			while(p[ tmpcnt + count + 8] != ',')//ip addr
			{
				buff[ip_count] = p[ tmpcnt + 8 + count];
				count ++;
				ip_count++;
			  if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_addr1 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[tmpcnt + count + 8] != ',')
			{
				buff[ip_count] = p[ tmpcnt + 8 + count];
				count ++;
				ip_count++;
			 if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_addr2 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[ tmpcnt + count + 8] != ',')
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
				if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_addr3 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[ tmpcnt + count + 8] != '/')
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
				if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_addr4 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[tmpcnt + count + 8] != ',')//netmask
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
			if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_netmask1 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[tmpcnt + count + 8] != ',')
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
				if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_netmask2 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[tmpcnt + count + 8] != ',')
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
				if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_netmask3 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[tmpcnt + count + 8] != '/')
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
				if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_netmask4 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[tmpcnt + count + 8] != ',') //netif_ip_gw
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
				if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_gw1 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[tmpcnt + count + 8] != ',')
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
				if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_gw2 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[tmpcnt + count + 8] != ',')
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
			  if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_gw3 = atoi(buff);
			count++;
			ip_count = 0;
			memset(buff, 0, 5);
			
			while(p[tmpcnt + count + 8] != '/')
			{
				buff[ip_count] = p[tmpcnt + 8 + count];
				count ++;
				ip_count++;
				if(ip_count >= 5) return ret = IP_ERROR;
			}
			netif_ip_gw4 = atoi(buff);
			count = count + 3;
			ip_count = 0;
			memset(buff, 0, 5);
			
			
			memset(p, 0, 512);
			sprintf(p, "AT+ETBK=%d,%d,%d,%d/%d,%d,%d,%d/%d,%d,%d,%d/\r\n", netif_ip_addr1, netif_ip_addr2, netif_ip_addr3, netif_ip_addr4,\
																								 netif_ip_netmask1,netif_ip_netmask2,netif_ip_netmask3,netif_ip_netmask4,\
																								 netif_ip_gw1,netif_ip_gw2,netif_ip_gw3,netif_ip_gw4);

			com_send_str(&usart5, p, strlen(p));
			IP4_ADDR(&netif_ipaddr,netif_ip_addr1,netif_ip_addr2,netif_ip_addr3,netif_ip_addr4);
			IP4_ADDR(&netif_netmask,netif_ip_netmask1,netif_ip_netmask2,netif_ip_netmask3,netif_ip_netmask4);
			IP4_ADDR(&netif_gw,netif_ip_gw1,netif_ip_gw2,netif_ip_gw3,netif_ip_gw4);
					
			TS_FDS.ts_local_ipaddr.ui_ip       =   ((netif_ip_addr1&0xff) << 24 ) | ((netif_ip_addr2&0xff) << 16 ) |\
																									((netif_ip_addr3&0xff) << 8 ) | ((netif_ip_addr4&0xff));
			TS_FDS.ts_local_ipaddr.ui_gatway   =    ((netif_ip_gw1&0xff) << 24 ) | ((netif_ip_gw2&0xff) << 16 ) |\
																									((netif_ip_gw3&0xff) << 8 ) | ((netif_ip_gw4&0xff)); ;
			TS_FDS.ts_local_ipaddr.ui_mask     =    ((netif_ip_netmask1&0xff) << 24 ) | ((netif_ip_netmask2&0xff) << 16 ) |\
																									((netif_ip_netmask3&0xff) << 8 ) | ((netif_ip_netmask4&0xff));;
					
			STMFLASH_Write(FLASH_SAVE_Data,(u16*)&TS_FDS,20);
	 
			netif_set_addr(&netif,&netif_ipaddr,&netif_netmask,&netif_gw);
			ret = UDP_CONNET;
		}
	}
	
	if(strstr(p, "AT+E1SHOW=") != NULL)
	{
		memset(p,0,512);
		sprintf(p, "AT+ETBK=%d,%d,%d,%d/%d,%d,%d,%d/%d,%d,%d,%d/\r\n", ip4_addr1(&netif.ip_addr), ip4_addr2(&netif.ip_addr), ip4_addr3(&netif.ip_addr),ip4_addr4(&netif.ip_addr),\
																					                         ip4_addr1(&netif.netmask),ip4_addr2(&netif.netmask),ip4_addr3(&netif.netmask),ip4_addr4(&netif.netmask),\
																					                         ip4_addr1(&netif.gw), ip4_addr2(&netif.gw),ip4_addr3(&netif.gw),ip4_addr4(&netif.gw));
		com_send_str(&usart5, p, strlen(p));
		
		ret = IPSHOW;
	}
 
	
 
	return ret;
}


extern PtpClock ptpClock;

uint8 cmd_handle_usart1(void *str, uint16 len)
{
	uint8 ret = 0;
  DateTime tmpTime;
	static u8 p[512] = {0};

	memcpy(p, str,512);
	TOD_Analysis(&tmpTime , p,len);
	memset(p, 0, 512);
/*
	$xhtime,year,mon,day,hour,min,sec,pos,pos,high,syncunit*
	*/
	 
	//systime = mktime(tmpTime.year,tmpTime.month,tmpTime.day,tmpTime.hour,tmpTime.minute,tmpTime.second);
	if( ptpClock.portDS.portState == PTP_MASTER && TS_FDS.PTP_MODE == emNTPMaster)
	{
		systime = mktime(tmpTime.year,tmpTime.month,tmpTime.day,tmpTime.hour,tmpTime.minute,tmpTime.second);
	}

	return ret;
}

uint8 cmd_handle_usart2(void *str, uint16 len)
{
	uint8 ret = 0;
  DateTime tmpTime;
	static u8 p[512] = {0};

	memcpy(p, str,512);
	
	
 
	memset(p, 0, 512);
 

	return ret;
}

char TOD_CheckXOR(char *buf, short len)
{
   char i = 0;
   char cr = 0;

	buf++;
	for( i = 0 ; i < len ; i++)
	{
		cr = cr^*buf;
		buf++;
	}
	return cr ;
}


uint8 TOD_Comma_Pos(uint8 *buf,uint8 cx)
{	 		    
	uint8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}


//str转换为数字,以','或者'*'结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值
int TOD_Str2num(uint8 *buf,uint8*dx)
{
	uint8 *p=buf;
	uint32 ires=0,fres=0;
	uint8 ilen=0,flen=0,i;
	uint8 mask=0;
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
