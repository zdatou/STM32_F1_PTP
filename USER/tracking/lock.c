
#include "lock.h"
//#inlcude "bds.h"

//#define abs(x)   x>=0? x:-x


//#define  NOISE 4 //����ֵ
//#define  MAX_Threshold  100//�����������ֵ 100s ��־��ѱ�����

//LOCKSTATE Tame_Detection(int value)//���ѱ���Ƿ�ѱ�����
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
