/*
//
	Project 	: driver
	File Name 	: usart.h
	Author 		: songlaifeng
	Copyright		:	长沙光河信息科技有限公司版权所有
	Created on	: 2016-3-2
	Destribe	:
		
	modify :

//
*/

#ifndef _USART_H_
#define _USART_H_

typedef signed char boolen;
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

//<<<<<<<<<<<<<<<<宏定义<<<<<<<<<<<<<<
typedef  unsigned char      Usart_boolean;     /* Boolean value type. */ 
typedef  unsigned long int  Usart_uint32;      /* Unsigned 32 bit value */ 
typedef  unsigned short     Usart_uint16;      /* Unsigned 16 bit value */ 
typedef  unsigned char      Usart_uint8;       /* Unsigned 8  bit value */ 
typedef  signed long int    Usart_int32;       /* Signed 32 bit value */ 
typedef  signed short       Usart_int16;       /* Signed 16 bit value */ 
typedef  signed char        Usart_int8;        /* Signed 8  bit value */ 

#define USART_RECEIVE_MAX  1024
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<枚举定义<<<<<<<<<<<<
//串口编号
typedef enum
{
	EUsart_1,
	EUsart_2,
	EUsart_3,
	EUsart_4,
	EUsart_5
}EUsart;

//奇偶校验位
typedef enum
{
	EUSART_Parity_No,
	EUSART_Parity_Even,
	EUSART_Parity_Odd
}EUSART_Parity;

//中断开关状态
typedef enum
{
	EUSART_Interrupt_State_On,
	EUSART_Interrupt_State_Off,
}EUsart_Interrupt_State;

void mystrncpy(void * des, void * souce, uint32 len);
//功能函数
typedef int (*USART_Func)(void *, Usart_uint16, void *, Usart_uint8);
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<结构定义<<<<<<<<<<<<

typedef struct _st_usart
{
		EUsart name;
		Usart_uint8 state;
		Usart_uint32 bound;
		EUSART_Parity partity;
		Usart_uint16 receive_count;
		Usart_uint8 buff[USART_RECEIVE_MAX];
		USART_Func func;
}STUsart, *PSTUsart;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<变量定义<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
extern STUsart usart1;

//<<<<<<<<<<<<<<<<函数申明<<<<<<<<<<<<

#ifdef __cplusplus
extern "C" {
#endif
	
/***********************************************
	Description: void usart_init(PSTUsart com);
		串口初始化
	para:
		串口配置信息
	return:
	
***********************************************/
void com_init(PSTUsart com);
	
/***********************************************
	Description: void usart_send_str(PSTUsart com,void* str,Usart_uint16 len);
		发送字符函数
	para:
		(PSTUsart, 		  void*		,				Usart_uint16)
		串口选择 	发送字符串首地址	发送字符串长度
	return:
	
***********************************************/
void com_send_str(PSTUsart com,void* str,Usart_uint16 len);

/***********************************************
	Description: void set_usart_interrupt_state(PSTUsart com, EUsart_Interrupt_State usart_interrupt_state);
		设定串口中断状态
	para:
		( PSTUsart,        EUSART_Interrupt_State)
		  串口选择 	        中断状态(on or off)
	return:
	
***********************************************/
void set_com_interrupt_state(PSTUsart com, EUsart_Interrupt_State usart_interrupt_state);

/***********************************************
	Description: 	Usart_int8 receive_str_manage(PSTUsart com, void *store, Usart_uint8 count);
		串口数组数据处理函数
	para:
		(		PSTUsart,	void *store, Usart_uint8 count);
			 串口选择     存取地址        备用
	return:
		返回值为function 函数的返回值
***********************************************/
Usart_int8 receive_str_manage(PSTUsart com, void *store, Usart_uint8 count);

/***********************************************
	Description: 	void clear_usart_buff(PSTUsart com);
		清除缓存
	para:
		(		PSTUsart	);
			 串口选择 
	return:
	
***********************************************/
void clear_com_buff(PSTUsart com);


/***********************************************
	Description: 	void usart_send_byte(PSTUsart com,Usart_uint8 byte);
		发送一个字符
	para:
		(		PSTUsart,					Usart_uint8);
			 串口选择           数据
	return:
	
***********************************************/
void com_send_byte(PSTUsart com,Usart_uint8 byte);

/***********************************************
	Description: 	void usart_send_pstr(PSTUsart com,void *p);
		发送一串数据
	para:
		(		PSTUsart,					void *);
			 串口选择     要发送的数据首地址
	return:
	
***********************************************/
void com_send_pstr(PSTUsart com,void *p);

#ifdef __cplusplus
}
#endif

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//----------------测试函数 start----------------
#if defined(TEST_usart_driver) && defined (TEST_SWITCH)
	int _usart_test_(void);
#endif
//----------------测试函数 end------------------

//
//..头文件其他内容
//

#endif/*_USART_H*/
