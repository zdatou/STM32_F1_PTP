#ifndef _irig_b_h_
#define _irig_b_h_

#include "stm32f10x.h"

#define zero_code 0
#define one_code  1
#define p_code		2

#define BDC_IN  PDin(12)

#define doysec(x,y,z)   x*3600+y*60+z 

#define BCD_TO_DEC(bcd) (bcd = ((bcd & 0x0f) + (bcd >> 4) * 10 ) + ((bcd & 0x0f00)>>8)*100)

typedef struct irig_b{
 unsigned char  sec;//BCD 8BIT
 unsigned char  min ;   // 8BIT
 unsigned char  hour; //8BIT
 unsigned short day; //16bit
 unsigned int daysec;
 unsigned char year;
 unsigned char leap;
}b_dc;


void step_irig_b_out(void);
void irig_b_init(void);
void irig_b_in(unsigned short Dval);

#endif
