
#include "lock.h"
//#inlcude "bds.h"

//#define abs(x)   x>=0? x:-x


//#define  NOISE 4 //噪声值
//#define  MAX_Threshold  100//检测锁定门限值 100s 标志着驯服完成

//LOCKSTATE Tame_Detection(int value)//检测驯服是否驯服完成
//{
//	static unsigned int  Flag = 0;
//	
//   if(abs(value) <= NOISE)
//	 {
//		 Flag++;
//		 if( Flag >= MAX_Threshold )
//		 {
//				return LOCK;
//		 }
//	 }
//	 else
//	 {
//		  Flag = 0;
//	 }
//		 return TRACKING;
//}
