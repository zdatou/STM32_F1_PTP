#include "AllHeader.h"



#define SYSTEMTICK_PERIOD_MS  1

__IO uint32_t LocalTime = 0; 
uint32_t timingdelay;
extern ts_FLASH_DATA_SAVE  TS_FDS;
extern STUsart usart1;
extern STUsart usart5;
extern STUsart usart2;
extern RuTame GPSTame;
extern b_dc BTime;
extern uint32 systime;//实际时间
extern msgstate cmd_state;
extern nmea_msg gpsx;
extern PtpClock ptpClock;
extern ts_NTPRunTimeOpts   TS_NRTP;
static void RunStateLock_CallBack(void);
static void USART_CallBack(void);
static void KEY_CallBack(void);
static void ADJ_CallBack(void);
static void LED_CallBack(void);
static void UDBG_CallBack(void);
static void DS1302_CallBack(void);
static void ptp_tod_msg_pack(void);
static void Task_Poll(uint32_t time,Task p[],uint8_t Task_Num);
static void ntp_msg_hanlde_packed(void);
static void ptp_msg_hanlde_packed(void);

 

Task List[TaskNum]={
	{0, 1000u 					 , RunStateLock_CallBack    },
	{0, 20  						 , USART_CallBack 					},
	{0, 4000u   				 , KEY_CallBack             },
	{0, 1000u 					 , ADJ_CallBack           },
	{0, 500u             , LED_CallBack             },
	{0, 2000u            , UDBG_CallBack            },
	{0, 60000u           , DS1302_CallBack          }
};


void run_task(void)
{
	while(1)
	{
		Task_Poll(LocalTime,List,TaskNum);
		LwIP_Periodic_Handle(LocalTime);
        ptpd_Periodic_Handle(LocalTime);
	}
}


void Task_Poll(uint32_t time,Task p[],uint8_t Task_Num)
{
	 uint8_t i = 0;
		for(i = 0 ; i < Task_Num ; i++)
		{
			if( (time - p[i].Count ) >=  p[i].Period )
			{
				p[i].Count = time;
			 (p[i].CallBackFuntion)();
			}
		}
}

void RunStateLock_CallBack(void)
{
  	 ptp_tod_msg_pack();
}

void USART_CallBack(void)
{
	 struct ptptime_t next_trigger;
 	receive_str_manage(&usart1,NULL,0);
	receive_str_manage(&usart5,NULL,0);
	receive_str_manage(&usart2,NULL,0);

}


extern void get_sys_time(TimeInternal *time);
void KEY_CallBack(void)
{
	
 if( TS_FDS.PTP_MODE == emPTPMaster)
	{
	    ptp_msg_hanlde_packed();
	}
}

void LED_CallBack(void)
{
  TimeInternal g_time;
  char buf[36];
  getTime(&g_time);
  IWDG_Feed();
  //GPIOB->ODR ^= GPIO_Pin_9;
	sprintf(buf, "%d, %d\r\n", g_time.seconds, g_time.nanoseconds);
  com_send_str(&usart2, buf, strlen(buf));
}

void UDBG_CallBack(void)
{


}

void DS1302_CallBack(void)//1Min写入一次
{

}

TimeInternal g_SYSTIME;
TimeInternal g_PTPTIME;
TimeInternal g_OFFSET;
TimeInternal offset;
extern u8  Adj_Flag;

static void ADJ_CallBack(void)
{
	TimeInternal t_TmpTime;
	if( (ptpClock.portDS.portState == PTP_MASTER && TS_FDS.PTP_MODE == emPTPMaster) ||  TS_FDS.PTP_MODE == emNTPMaster)
	{
			 subTime(&g_OFFSET,&g_PTPTIME,&g_SYSTIME);
			 if(abs(g_OFFSET.nanoseconds) > 900e6 && g_OFFSET.seconds == 0)
			 {
				 g_OFFSET.nanoseconds = g_OFFSET.nanoseconds - 1000e6  ;
				 DBG("g_OFFSET.seconds:%d  g_OFFSET.nanoseconds:%d\r\n",g_OFFSET.seconds,g_OFFSET.nanoseconds);
			 }
			 t_TmpTime.seconds = 0;
			 t_TmpTime.nanoseconds = 640;
			 subTime(&g_OFFSET,&g_OFFSET,&t_TmpTime);
			 ptpClock.currentDS.offsetFromMaster = g_OFFSET;
			 updateClock(&ptpClock);
	}
}

static void ptp_tod_msg_pack(void)
{
	  DateTime Time_t;
		char buff[40] = {0};
		uint8 str_ret = 0;
		
		if( TS_FDS.PTP_MODE == emPTPMaster )
		{
		  Time_t = SecToTime(systime,UTC);
		}
		else if( TS_FDS.PTP_MODE == emPTPSlave)
		{
		  Time_t = SecToTime(ETH->PTPTSHR,UTC);
		}
		
		if( TS_FDS.PTP_MODE == emPTPSlave  )
		{
			str_ret = sprintf(buff,"$ETOD,%04d,%02d,%02d,%02d,%02d,%02d,%01d*\n",Time_t.year,Time_t.month,Time_t.day,Time_t.hour,Time_t.minute,Time_t.second,TS_FDS.PTP_MODE);
		}
		else if(  TS_FDS.PTP_MODE == emPTPMaster) 
		{
	  	str_ret = sprintf(buff,"$ETOD,%04d,%02d,%02d,%02d,%02d,%02d,%01d*\n",0,0,0,0,0,0,TS_FDS.PTP_MODE);
		}
	   com_send_str(&usart5,buff,str_ret);
}

extern struct ptp_c ptpMonitorl[ptpMonitorlConstant];
static void ptp_msg_hanlde_packed(void)
{
		uint8 buff[262] = {0};
	  uint8 i = 0;
	
	  memcpy(buff,"$EONE",4);
	  
  	for( i = 0 ; i < ptpMonitorlConstant ; i ++ )
		{
			buff[i*16 + 7 ] = (ptpMonitorl[i].ipaddr&0xff000000) >> 24;
			buff[i*16 + 6 ] = (ptpMonitorl[i].ipaddr&0x00ff0000) >> 16;
			buff[i*16 + 5 ] = (ptpMonitorl[i].ipaddr&0x0000ff00) >> 8;
			buff[i*16 + 4 ] = (ptpMonitorl[i].ipaddr&0x000000ff) >> 0;
			
			buff[i*16 + 11 ] = (ptpMonitorl[i].Offset.seconds&0xff000000) >> 24;
			buff[i*16 + 10 ] = (ptpMonitorl[i].Offset.seconds&0x00ff0000) >> 16;
			buff[i*16 + 9] =   (ptpMonitorl[i].Offset.seconds&0x0000ff00) >> 8;
			buff[i*16 + 8] =   (ptpMonitorl[i].Offset.seconds&0x000000ff) >> 0;
			
			buff[i*16 + 15] = (ptpMonitorl[i].Offset.nanoseconds&0xff000000) >> 24;
			buff[i*16 + 14] = (ptpMonitorl[i].Offset.nanoseconds&0x00ff0000) >> 16;
			buff[i*16 + 13] = (ptpMonitorl[i].Offset.nanoseconds&0x0000ff00) >> 8;
			buff[i*16 + 12] = (ptpMonitorl[i].Offset.nanoseconds&0x000000ff) >> 0;
			
			buff[i*16 + 19] = (ptpMonitorl[i].Time&0xff000000) >> 24;
			buff[i*16 + 18] = (ptpMonitorl[i].Time&0x00ff0000) >> 16;
			buff[i*16 + 17] = (ptpMonitorl[i].Time&0x0000ff00) >> 8;
			buff[i*16 + 16] = (ptpMonitorl[i].Time&0x000000ff) >> 0;
		}
	
		  buff[261] = fe5650a_cs_calc(buff,261) ;
		
		 com_send_str(&usart5,buff,262);
	
}


/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nCount;  

  /* wait until the desired delay finish */  
  while(timingdelay > LocalTime)
  {     
  }
}

void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}

