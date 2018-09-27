#ifndef _Rubidium_H
#define _Rubidium_H

#include "common.h"
#include "pid.h"
#include "stdbool.h"


#define fe5650a_run_time 1000u

__packed typedef struct{
	
	unsigned char CommandID;
	unsigned char MessageLengthLow;
	unsigned char MessageLengthHigh;
	unsigned char CommandCheckSum;
	unsigned char Data[4];
	unsigned char DataCheckSum;

}RuCmdFormat;

 typedef struct {
 
	u8  	 SyncFlag ;
	u8  	 SyncCnt ;
	u32 LastSyncUnit ;
	u32 SyncUnit ;

 }Sync;//同步状态记录
 
 enum  SYNC_Type{
	 FREEDOM,
	 UNLOCK ,
	 LOCKING,
	 LOCKED,
	 TRACKING,
	 UNPPS
 };
 
 enum TAME_Type{
	 TAME_OFF,
	 TAME_ON
 };
 
 typedef struct{

	double Offset ;
	double FilterOffset ;
	double LastOffset;
	s32 FixedOffset ;
	double OffsetHistory[20];
	u8 		OffsetHistoryCnt;
	s32 	ControlHistory[255];
	u8 		ControlHistoryCnt;
	u8  	TameFlag;
	u8  	PPSDetection;
	bool  iSave;
	bool  RisingEdge;
	float FilterRatio;
	PIDTypedef pid_t;
	double Output;
	s32 Du;//上次偏移相对于当前偏移的差值
	u32 LockLastTime; //记录上一次锁定状态时间
	u32 LockCurrTime; //记录这次锁定时间
	u32 NoiseBand;
	u32 MaxNoiseBand;//大噪声门限
	u32 iOKTime;
	u8	TOD_Flag;
	RuCmdFormat cmd; 
	Sync state;
}RuTame;

 


void RunStateLock(void);
void Ru_Tame(void);
void Ru_Tame_Init(void);
unsigned char fe5650a_cs_calc(unsigned char * pDat, unsigned int len);


#endif 

