#include "AllHeader.h"
#include "sntp.h"

extern STUsart usart3;
extern nmea_msg gpsx;

__IO tstamp systime = JAN_2018;
__IO DateTime nowdate;
__IO state sntp_state;

extern PtpClock ptpClock;

//static void tim8_start(u16 Pre,u16 arr);

/**************************************************************************
 *
 * 函数功能: sec to date
 *
 * 参数说明: [OUT] current, SNTP 时间.
 *
 * 返 回 值: 成功返回 0, 失败返回 -1.
 *
 **************************************************************************/
 


/**************************************************************************
 *
 * 函数功能: 得到系统当前时间.
 *
 * 参数说明: [OUT] current, SNTP 时间.
 *
 * 返 回 值: 成功返回 0, 失败返回 -1.
 *
 **************************************************************************/
 char str[30];
void get_sys_timer(time_fp *current)
{

	    	current->fp_ui = ETH_GetPTPRegister(ETH_PTPTSHR) + JAN_1900;//时间转换 1970 - 1900 
				current->fp_uf =  ETH_GetPTPRegister(ETH_PTPTSLR) * 2 ;
 
}

void get_sys_time(TimeInternal *time)
{
	time->seconds     = systime;
	time->nanoseconds = (TIM1->CCR1 + TIM2->CNT * (TIM1->ARR+1)) * 6.667;
	if( time->nanoseconds > 1000e6 )
	{
	  time->seconds     =  time->seconds+1;
		time->nanoseconds =  time->nanoseconds - 1000e6;
	}
}



  tstamp mktime (unsigned int year,   int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec)
{
		if ( 0 >= (mon -= 2))
	{ /**//* 1..12 -> 11,12,1..10 */
			mon += 12;	 /**//* Puts Feb last since it has leap day */
			year -= 1;
	}
	
	return ((tstamp)((((year/4 - year/100 + year/400 + 367*mon/12 + day) + year*365 - 719499
	)*24 + hour /**//* now have hours */
	)*60 + min /**//* now have minutes */
	)*60 + sec); /**//* finally seconds */
}

void PTimer_Init(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_GetClocksFreq(&RCC_Clocks);
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
//  
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	 GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

  /* Remap PPS output */
  GPIO_PinRemapConfig(GPIO_Remap_PTP_PPS, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
  GPIO_PinRemapConfig(GPIO_Remap_TIM2ITR1_PTP_SOF, DISABLE);
	
  TIM_TimeBaseStructure.TIM_Period = 10000;
  /* prescaler of SysClk - 1 */
  TIM_TimeBaseStructure.TIM_Prescaler = RCC_Clocks.SYSCLK_Frequency / 100000 - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  /* Because of TIM_OCFast_Enable this value must be greater then 0,
   * but then it does not generate any delay */
  TIM_OCInitStructure.TIM_Pulse = 500;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  /* Configure TIM2 channel 1 as output */
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  //TIM_OC1FastConfig(TIM2, TIM_OCFast_Enable);
	TIM2->CCR1 = 5000;
//	TIM_RemapConfig(TIM2,TIM2_ETH_PTP);

  /* Input Trigger selection */
  TIM_SelectInputTrigger(TIM2, TIM_TS_ITR1);//用户可以将PTP触发信号连接到TIM2的ITR1
  TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single);
	
//  /* Slave Mode selection: Trigger Mode */
  TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Trigger);
}

void CTimer_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //TIM1 上桥臂
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
 
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;  // 选择输入端 IC4映射到TI4上
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM_ICInitStructure.TIM_ICFilter = 0;//IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
  TIM_ICInit(TIM3, &TIM_ICInitStructure);//初始化定时器输入捕获通道
	
	TIM_Cmd(TIM3, ENABLE);  //使能TIM1
	TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);
	TIM_ClearITPendingBit(TIM3,TIM_IT_CC2);  
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;  //从优先级7级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	
}

 
extern RuTame GPSTame;
extern TimeInternal g_SYSTIME;
extern TimeInternal g_PTPTIME;
extern TimeInternal g_OFFSET;
extern RuTame GPSTame;
extern ts_FLASH_DATA_SAVE  TS_FDS;
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_CC2) != RESET )
	{
			systime = systime + 1;
		if(TS_FDS.PTP_MODE== emPTPMaster || TS_FDS.PTP_MODE == emNTPMaster)
		{
				getTime(&g_PTPTIME);
				g_SYSTIME.seconds = systime;
				g_SYSTIME.nanoseconds = 0;
		}
 }

	TIM_ClearITPendingBit(TIM3,TIM_IT_CC2);
}
 



