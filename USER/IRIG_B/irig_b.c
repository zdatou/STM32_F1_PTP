#include "irig_b.h"

//unsigned char Bcode[100];
b_dc b_code;

void irig_b_init(void)
{
  b_code.sec = 0;
	b_code.hour = 12;
  b_code.min = 12;
	b_code.day = 365;
	b_code.daysec = 86399;
}
/*
  时间信息转换流程
  1. DecToBcd
  2. shift_fun
  3. tobcode
*/

unsigned short DecToBcd(unsigned short  Dec)
{
  unsigned short  tmp = 0;
		if( 0 == Dec)
		return 0 ;
  if(Dec > 99) tmp = (Dec/100) << 8 | ((Dec%100)/10) << 4 | (Dec % 10);
  else 
   tmp = (Dec/10)<<4 | (Dec % 10);
	return tmp ;
}

unsigned short BcdToDec(unsigned short  Bcd)
{
	unsigned short  tmp = 0;
 
	tmp = (((Bcd & 0x0f) + ((Bcd & 0x00f0 )>> 4) * 10 ) + ((Bcd & 0x0f00)>>8)*100);
	return tmp ;
}


unsigned char shift_fun(unsigned char data) //蝶式交换 高低位交换
{  
		if( 0 == data)
		return 0 ;
    data=(data<<4)|(data>>4);  
    data=((data<<2)&0xcc)|((data>>2)&0x33);  
    data=((data<<1)&0xaa)|((data>>1)&0x55);    
    return data;  
}  

unsigned char ToBcode(unsigned char data)//时分秒天转换
{
	unsigned char tmp = 0;
	
	if( 0 == data)
		return 0 ;
	tmp  = (data & 0x0f) >> 1;
	data = data & 0xf0;
	data = data | tmp;
	return data;
}


void irig_b_out(unsigned char pos,unsigned char state)
{
  switch(state)
	{
		case zero_code:  { TIM2->CCR1 = TIM2->ARR*0.2;}  	break;
		case one_code:   { TIM2->CCR1 = TIM2->ARR*0.5;}   break;	
		case p_code:     { TIM2->CCR1 = TIM2->ARR*0.8;}   break;
		default : break;
	}
}

unsigned char  BCodeStartFlag  = 0;
unsigned char  BCodeStartCnt   = 0;
unsigned char  BCodeRuningCnt  = 0;
unsigned char  Bcode[100] = { 0 };
b_dc BTime;

void irig_b_in(unsigned short Dval)
{
 
	if( (Dval > 1000 && Dval < 3000) )   //2ms  0
	{
			BCodeStartCnt = 0;
	 if(  BCodeStartFlag == 1 )
		{
			Bcode[BCodeRuningCnt++] = zero_code;
		}
	}
	else if(( Dval > 4000 && Dval < 6000 )) //5ms 1
	{
			BCodeStartCnt = 0;
    if(  BCodeStartFlag == 1 )
		{
			Bcode[BCodeRuningCnt++] = one_code;
		}		
	}
  else if( Dval > 7000 && Dval < 9000  )//8ms p
	{
		 BCodeStartCnt ++ ;				
			if( BCodeStartCnt == 2)
		 {
			 BCodeStartFlag = 1;
			 BCodeStartCnt  = 0;
			 BCodeRuningCnt = 0;
			 Bcode[BCodeRuningCnt++] = p_code;
		 }
		 else
		 {
			Bcode[BCodeRuningCnt++] = p_code;
		 }
		 if( BCodeRuningCnt >= 99 ) 
		 {
			 	BTime.sec  = (Bcode[8] << 6 | Bcode[7] << 5 | Bcode[6] << 4 | Bcode[4] << 3 | Bcode[3] << 2 | Bcode[2] << 1 | Bcode[1]) ;
		    BCD_TO_DEC(BTime.sec);
			  BTime.min  = (Bcode[17] << 6 | Bcode[16] << 5 | Bcode[15] << 4 | Bcode[13] << 3 | Bcode[12] << 2 | Bcode[11] << 1 | Bcode[10]) ;
			  BCD_TO_DEC(BTime.min);
			  BTime.hour = (Bcode[26] << 5 | Bcode[25] << 4 | Bcode[23] << 3 | Bcode[22] << 2 | Bcode[21] << 1 | Bcode[20]);
			  BCD_TO_DEC(BTime.hour);
			  BTime.day  =  (Bcode[41] << 9 | Bcode[40] << 8 | Bcode[38] << 7 | Bcode[37] << 6 | Bcode[36] << 5 | Bcode[35] << 4 | Bcode[33] << 3 | Bcode[32] << 2 | Bcode[31] << 1 | Bcode[30]);
			  BTime.day = BcdToDec(BTime.day);
			  BTime.leap = Bcode[60] << 1 | Bcode[61] ;
				BTime.year = (Bcode[58]<<7 | Bcode[57]<<6 | Bcode[56]<<5 | Bcode[55] << 4| Bcode[53] << 3| Bcode[52] << 2|Bcode[51] << 1 | Bcode[50]);
			  BCD_TO_DEC(BTime.year);

				BCodeRuningCnt = 0;
			  BCodeStartFlag = 0;
		 }
	}
	else
	{
		 BCodeStartCnt  = 0;
	}
}


void step_irig_b_out(void)
{
	static unsigned char pos = 0;
	unsigned char state = p_code;
	if(pos == 0 || pos == 9 || pos == 19 || pos == 29 ||\
		 pos == 39|| pos == 49|| pos == 59 || pos == 69 ||\
	   pos == 79|| pos == 89|| pos == 99)
	{
	 state = p_code;
	}
	else
	{
		if( pos < 9)
		  state =  ToBcode((shift_fun(DecToBcd(b_code.sec))) >> pos)&0x01;//秒
		else if( pos > 9  && pos < 19)
		  state =  ToBcode((shift_fun(DecToBcd(b_code.min))) >> (pos - 10))&0x01;//分
		else if( pos > 19 && pos < 29)
		  state =  ToBcode((shift_fun(DecToBcd(b_code.hour))) >> (pos - 20))&0x01;//时 6位 后2位补0
		else if( pos > 29 && pos < 39)
	   state =  ToBcode((shift_fun(DecToBcd(b_code.day%100))) >> (pos - 30))&0x01;
		else if( pos > 39 && pos < 49)
		 state =  ToBcode((shift_fun(DecToBcd(b_code.day/100))) >> (pos - 40))&0x01;
		else if( pos > 49 && pos < 59)
		  state = zero_code;
		else if( pos > 59 && pos < 69)
		 state = zero_code;
		else if( pos > 69 && pos < 79)
		state =  zero_code;
		else if( pos > 79 && pos < 89)
		 state =  (shift_fun(b_code.daysec%512)>> (pos - 80))&0x01;
		else if( pos > 89 && pos < 99)
		 state =  (shift_fun(b_code.daysec/512) >> (pos - 90))&0x01;
	}
	irig_b_out(pos,state);
	pos++;
	if(pos >= 100)
	{ pos = 0;}
}
