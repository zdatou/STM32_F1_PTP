#include "AllHeader.h"

RuTame GPSTame;
extern uint32 systime;
extern STUsart usart1;
extern STUsart usart2;
extern STUsart usart3;
extern msgstate cmd_state;
extern __IO uint32_t LocalTime ;
extern ts_FLASH_DATA_SAVE  TS_FDS;

int  swapInt32(int  value) ;
//static unsigned char fe5650a_cs_calc(unsigned char * pDat, unsigned int len);
void OXCO_Control_Direct(short data);
void OXCO_Control_VloDirect(double Var); 

/**
* @brief			 初始化
* @retval 		空
*/

void Ru_Tame_Init(void)
{
	GPSTame.Offset = 0;
	GPSTame.Output = 0;
	GPSTame.FilterRatio = 0;
	GPSTame.FixedOffset = TS_FDS.sc_FixOffset;
	GPSTame.pid_t.SampleTime = 5000;
	GPSTame.pid_t.SetPoint = 0;
	GPSTame.pid_t.Proportion = 0.0005111;
  GPSTame.pid_t.Integral =  0.0004111;
	GPSTame.pid_t.Derivative = 0;
	GPSTame.pid_t.isMax =  2.5;//不到5V
	GPSTame.pid_t.isMin = -2.5;
	GPSTame.TameFlag = 0;
	GPSTame.iSave = false;
	GPSTame.LockLastTime = 0;   //上一次锁定的时刻
	GPSTame.LockCurrTime = 0;   //锁定时 运行的时刻
	GPSTame.NoiseBand    = 3;  //噪声 5* 7 35ns
	GPSTame.iOKTime      = 200*1000; //1000S 
	GPSTame.state.SyncFlag = FREEDOM;
	GPSTame.MaxNoiseBand = 2000;
  PIDinit(&GPSTame.pid_t);
}

/**
* @brief 			运行函数
* @retval 		空
*/
void Ru_Tame(void)
{
	static u32 LastTime = 0;
	static u8 flag = 0;
	uint32 tmpavr = 0;
	uint8  i = 0;
	
	if(( (LocalTime - LastTime) >	GPSTame.pid_t.SampleTime )&&( GPSTame.state.SyncFlag == TRACKING || GPSTame.state.SyncFlag == LOCKING || GPSTame.state.SyncFlag == LOCKED))
	{
		LastTime = LocalTime ;
		if(GPSTame.TameFlag == TAME_ON)
		{
			if( GPSTame.iSave == false)
			{	
					if( GPSTame.FilterOffset >= 0 )
						{
						 if( ( GPSTame.FilterOffset - GPSTame.LastOffset ) > 0 ) //上一次值
						 {
							 GPSTame.Output =  LocPIDCalcFunc(&GPSTame.pid_t, GPSTame.FilterOffset, GPSTame.LastOffset - GPSTame.FilterOffset);
						   OXCO_Control_VloDirect(GPSTame.Output);
						 }
						}
					else
						{
							 if( ( GPSTame.FilterOffset - GPSTame.LastOffset ) < 0 ) //上一次值
							 {
								 
								 GPSTame.Output =  LocPIDCalcFunc(&GPSTame.pid_t, GPSTame.FilterOffset,GPSTame.LastOffset - GPSTame.FilterOffset);
								 OXCO_Control_VloDirect(GPSTame.Output);
							 }
						}
							if( (  GPSTame.FilterOffset - GPSTame.LastOffset) == 0 )//等于0的情况
						 {
								 if( flag > 5)
								 {
											for( i = 0; i < 20 ; i ++)
									 {
											tmpavr  = GPSTame.OffsetHistory[i] ;    
									 }
									tmpavr  = tmpavr / 20;
								 
								 if(  GPSTame.FilterOffset > 0 )
								 {
										 if( (  GPSTame.FilterOffset - tmpavr ) > 0 ) //上一次值 
										 {
											 
											 GPSTame.Output =  LocPIDCalcFunc(&GPSTame.pid_t, GPSTame.FilterOffset, tmpavr - GPSTame.FilterOffset);
											 OXCO_Control_VloDirect(GPSTame.Output);
										 }
								 }
								 else
								 {
										if( (  GPSTame.FilterOffset - tmpavr ) < 0 ) //上一次值
									 {
										 GPSTame.Output =  LocPIDCalcFunc(&GPSTame.pid_t, GPSTame.FilterOffset, tmpavr - GPSTame.FilterOffset);
										 OXCO_Control_VloDirect(GPSTame.Output);
									 }
									}
								}
							}
						 else
						 {
								flag = 0;
						 }
						  GPSTame.OffsetHistory[GPSTame.OffsetHistoryCnt++] = GPSTame.FilterOffset;
						 if( GPSTame.OffsetHistoryCnt > 19 ) GPSTame.OffsetHistoryCnt = 0;  
							GPSTame.LastOffset = GPSTame.FilterOffset; 
			}
			else
			{

			}
		}
		else
		{
			 OXCO_Control_VloDirect(0);
		}
	}
}

/**
* @brief 			锁定判断函数
* @retval 		空
*/
extern nmea_msg gpsx;
	
void RunStateLock(void)
{
	 static  u8 cnt = 0;
	 static  u8 FilterCnt = 0; 
   static  int FilterTab[5] = {0};
			
		if( GPSTame.PPSDetection > 2)
			{
				GPSTame.TameFlag = TAME_OFF;
				GPSTame.Offset = 0;
				GPSTame.state.SyncFlag = UNPPS;
				return ;
			}
			GPSTame.PPSDetection ++ ;//计数

			if( GPSTame.state.SyncFlag == UNPPS)
			{
				 cnt++;
				if( cnt > 5)
				{
				  cnt = 0;
					if( abs(GPSTame.Offset) > GPSTame.MaxNoiseBand )
			  	  GPSTame.state.SyncFlag = UNLOCK;
					else 
					{
						GPSTame.state.SyncFlag = TRACKING;
					}
				}
				else
				{
				 return ;
				}
			}
	
	if( GPSTame.state.SyncFlag == FREEDOM ) 
		GPSTame.state.SyncFlag = UNLOCK;
	
	    GPSTame.LockCurrTime = LocalTime;
			if( GPSTame.state.SyncUnit > 75e6 )
		{
			GPSTame.Offset = (GPSTame.state.SyncUnit - 150e6);
		}
		else
		{
			GPSTame.Offset = GPSTame.state.SyncUnit;
		}
		GPSTame.Offset = (GPSTame.Offset - GPSTame.FixedOffset) ;//减去固定偏差
		FilterTab[FilterCnt++] = GPSTame.Offset ;
		if( FilterCnt >= 6 ) FilterCnt = 0;
		GPSTame.FilterOffset = AvrValueFiter(FilterTab,5);
		GPSTame.FilterOffset = (int)KalmanFilter(GPSTame.FilterOffset,0.0005,0.0001,0);
		
	 if( GPSTame.state.SyncFlag != UNLOCK )
		{
			GPSTame.TameFlag = TAME_ON;
		}
		else
		{
			GPSTame.TameFlag = TAME_OFF;
			GPSTame.pid_t.Proportion = 0.0005111;
			GPSTame.pid_t.Integral =  0.0004111;
			GPSTame.pid_t.SampleTime = 5000;
		}
	if(abs(GPSTame.Offset) > GPSTame.MaxNoiseBand)
	{
			GPSTame.state.SyncFlag = UNLOCK;
	}
	if( (abs(GPSTame.FilterOffset) < GPSTame.NoiseBand ) && ( GPSTame.state.SyncFlag == TRACKING ))//判断是否锁定 记录时间
	{
		GPSTame.LockLastTime = LocalTime;
		GPSTame.state.SyncFlag = LOCKING;
	}
 if( (abs(GPSTame.FilterOffset ) < GPSTame.NoiseBand ) && ((GPSTame.LockCurrTime -  GPSTame.LockLastTime) > GPSTame.iOKTime) && (GPSTame.state.SyncFlag == LOCKING) )// 判断锁定是否已经稳定
	{
		 GPSTame.pid_t.Proportion = 0.0005111;
		 GPSTame.pid_t.Integral =  0.0000511;
		 GPSTame.pid_t.SampleTime = 5000;
		 GPSTame.MaxNoiseBand = 10000;
		 GPSTame.state.SyncFlag = LOCKED ;
	}
	else if( (GPSTame.state.SyncFlag == LOCKING || GPSTame.state.SyncFlag == LOCKED) && abs(GPSTame.FilterOffset) > GPSTame.NoiseBand )
	{
		GPSTame.state.SyncFlag = TRACKING ;
	}
}



/**
* @brief 	恒温晶振DAC调控
* @param[in]	buf   控制结构体
* @details 		压控晶振
* @retval 		空
*/

void OXCO_Control_Direct(short data)
{
	DAC_SetChannel2Data(DAC_Align_12b_R, data + 2186);  //12位右对齐数据格式设置DAC值
}

void OXCO_Control_VloDirect(double Var)
{
   uint16_t x = 0;
	
	Var = Var + 2.67 ;
  if(  Var > 5.0)
	{
	 Var = 5.0;
	}
	x   = ( Var / 5.0 ) * 4095 ;
	DAC_SetChannel2Data(DAC_Align_12b_R, x );  //12位右对齐数据格式设置DAC值
}




 
/**
* @brief 	         校验和
* @param[in]	pDat 数据输入
* @param[in]	len  字符长度
* @details 		计算校验和取低八位
* @retval 		校验和 低八位
*/
 unsigned char fe5650a_cs_calc(unsigned char * pDat, unsigned int len)
{
	unsigned char tmpXor = 0;
	unsigned int i = 0;

	for(i = 0; i < len; i++)
	{
		tmpXor ^= *(pDat + i); 
	}

	return(tmpXor & 0xFF);

}

/**
* @brief 	高低字节转换
* @param[in]	value  32 数据输入
* @details 		高低字节转换
* @retval 		转换后的数据
*/
int  swapInt32(int  value)  
{  
     return ((value & 0x000000FF) << 24) |  
               ((value & 0x0000FF00) << 8) |  
               ((value & 0x00FF0000) >> 8) |  
               ((value & 0xFF000000) >> 24) ;  
}  

 
