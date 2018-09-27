#ifndef _IWDG_H
#define _IWDG_H
#include "common.h"
#include "stm32f10x.h"


void IWDG_Init(u8 prer,u16 rlr);//IWDG³õÊ¼»¯
void IWDG_Feed(void);  //Î¹¹·º¯Êý
#endif
