#include "AllHeader.h"
#include "stm32_eth.h"

static int  uart1_call_back(void * str, Usart_uint16 len, void * store, Usart_uint8 count);
static int  uart5_call_back(void * str, Usart_uint16 len, void * store, Usart_uint8 count);
static int  uart2_call_back(void * str, Usart_uint16 len, void * store, Usart_uint8 count);
static void main_com_init(void);
static void EEPROM_FLASH_SAVE_Init(void);
static void TargetTime_Init(void);

STUsart usart1;
STUsart usart5;
STUsart usart2;
nmea_msg gpsx;
ts_FLASH_DATA_SAVE  TS_FDS;

extern struct ptp_c ptpMonitorl[ptpMonitorlConstant];
extern struct netif netif;
extern msgstate cmd_state;//cmd state
extern RunTimeOpts rtOpts; 
extern PtpClock ptpClock;
extern ForeignMasterRecord ptpForeignRecords[DEFAULT_MAX_FOREIGN_RECORDS];

void system_init(void)
{
    	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	ETH_BSP_Config();     //DP83848相关IO初始化与ETH相关初始化
	LED_Init();
	
	 ETH_SetPTPTargetTime(10,0);
/* - unmask timestamp interrupt (9) ETH_MACIMR */
  ETH_MACITConfig(ETH_MAC_IT_TST, ENABLE);
/* - set TSCR bit 4 */
  ETH_EnablePTPTimeStampInterruptTrigger();
	
	
	
	LwIP_Init();          //LWIP初始化
	IWDG_Init(6,625);
 	main_com_init();        //串口初始化
	TS_FDS.PTP_MODE = emPTPSlave;
	//TS_FDS.PTP_MODE = emPTPMaster;
	PTimer_Init();
	CTimer_Init();
	EEPROM_FLASH_SAVE_Init();
	TargetTime_Init();
	cmd_state.tod_state = 1;
}

static void EEPROM_FLASH_SAVE_Init(void)
{

}


static void TargetTime_Init(void)
{
	
	PTPd_Init();
	if(TS_FDS.PTP_MODE == emPTPMaster )
	{
     rtOpts.slaveOnly = 0;
	 rtOpts.clockQuality.clockAccuracy = 200;
	 ptpdStartup(&ptpClock, &rtOpts, ptpForeignRecords);
	}
	else if( TS_FDS.PTP_MODE == emPTPSlave )
	{
	   rtOpts.slaveOnly = 1;
	   ptpdStartup(&ptpClock, &rtOpts, ptpForeignRecords);
	}
}

static void main_com_init(void)
{
	usart1.bound = 115200;
	usart1.partity = EUSART_Parity_No;
	usart1.name = EUsart_1;
	usart1.receive_count = 0;
	usart1.state = 0;
	memset(usart1.buff, 0, USART_RECEIVE_MAX);
	usart1.func = uart1_call_back;
	com_init(&usart1);
	
	usart5.bound = 115200;
	usart5.partity = EUSART_Parity_No;
	usart5.name = EUsart_5;
	usart5.receive_count = 0;
	usart5.state = 0;
	memset(usart5.buff, 0, USART_RECEIVE_MAX);
	usart5.func = uart5_call_back;
	
	com_init(&usart5);
	
  usart2.bound = 115200;
	usart2.partity = EUSART_Parity_No;
	usart2.name = EUsart_2;
	usart2.receive_count = 0;
	usart2.state = 0;
	memset(usart2.buff, 0, USART_RECEIVE_MAX);
	usart2.func = uart2_call_back;
	com_init(&usart2);
}

/**********************************串口数据处理函数*******************************

**********************************************************************************/
static int uart1_call_back(void * str, Usart_uint16 len, void * store, Usart_uint8 count)
{
	int ret = 0;
	
	com_send_str(&usart2,str,len);
  ret = cmd_handle_usart1(str, len);

	return ret;
}

static int uart2_call_back(void * str, Usart_uint16 len, void * store, Usart_uint8 count)
{
	int ret = 0;
	
    cmd_handle_usart2(str, len);
	  cmd_handle_usart5(str, len);
  	com_send_str(&usart5,str,len);
	return ret;
}


static int uart5_call_back(void * str, Usart_uint16 len, void * store, Usart_uint8 count)
{
	int ret = 0;
//	char flashret = 0;
	char p[100];

 
  ret = cmd_handle_usart5(str, len);
	
	if(ret == CONNECT)//连接命令
	{
	  
	}
	else if(ret == PTP_MASTER_BACK) 
	{
		
	}
	else if(ret == PTP_SLAVE_BACK) 
	{

	}
	else if(ret == UDP_CONNET)//系统查询命令 设置本机IP
	{

	}
		else if(ret == IPSHOW)//查询本机IP
	{

	}
		else if(ret == IP_ERROR)
	{
		memset(p, 0, 100);
		sprintf(p, "IPAddr ERROR\r\n");
		com_send_str(&usart5, p, strlen(p));
	}
 
 
	return ret;
}
