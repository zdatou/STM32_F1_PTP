#ifndef _lcok_h_
#define _lcok_h_



typedef  unsigned char  LOCKSTATE;

enum{
	FREERUN,
	TRACKING,
	LOCK,
	UNLOCK,
  HOLD,
}OXCO_FLAG;


LOCKSTATE Tame_Detection(int value);//检测驯服是否驯服完成




#endif
