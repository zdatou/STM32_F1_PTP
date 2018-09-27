/*
//
	Project 	: driver
	File Name 	: usart.h
	Author 		: songlaifeng
	Copyright		:	��ɳ�����Ϣ�Ƽ����޹�˾��Ȩ����
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

//<<<<<<<<<<<<<<<<�궨��<<<<<<<<<<<<<<
typedef  unsigned char      Usart_boolean;     /* Boolean value type. */ 
typedef  unsigned long int  Usart_uint32;      /* Unsigned 32 bit value */ 
typedef  unsigned short     Usart_uint16;      /* Unsigned 16 bit value */ 
typedef  unsigned char      Usart_uint8;       /* Unsigned 8  bit value */ 
typedef  signed long int    Usart_int32;       /* Signed 32 bit value */ 
typedef  signed short       Usart_int16;       /* Signed 16 bit value */ 
typedef  signed char        Usart_int8;        /* Signed 8  bit value */ 

#define USART_RECEIVE_MAX  1024
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<ö�ٶ���<<<<<<<<<<<<
//���ڱ��
typedef enum
{
	EUsart_1,
	EUsart_2,
	EUsart_3,
	EUsart_4,
	EUsart_5
}EUsart;

//��żУ��λ
typedef enum
{
	EUSART_Parity_No,
	EUSART_Parity_Even,
	EUSART_Parity_Odd
}EUSART_Parity;

//�жϿ���״̬
typedef enum
{
	EUSART_Interrupt_State_On,
	EUSART_Interrupt_State_Off,
}EUsart_Interrupt_State;

void mystrncpy(void * des, void * souce, uint32 len);
//���ܺ���
typedef int (*USART_Func)(void *, Usart_uint16, void *, Usart_uint8);
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<�ṹ����<<<<<<<<<<<<

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

//<<<<<<<<<<<<<<<<��������<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
extern STUsart usart1;

//<<<<<<<<<<<<<<<<��������<<<<<<<<<<<<

#ifdef __cplusplus
extern "C" {
#endif
	
/***********************************************
	Description: void usart_init(PSTUsart com);
		���ڳ�ʼ��
	para:
		����������Ϣ
	return:
	
***********************************************/
void com_init(PSTUsart com);
	
/***********************************************
	Description: void usart_send_str(PSTUsart com,void* str,Usart_uint16 len);
		�����ַ�����
	para:
		(PSTUsart, 		  void*		,				Usart_uint16)
		����ѡ�� 	�����ַ����׵�ַ	�����ַ�������
	return:
	
***********************************************/
void com_send_str(PSTUsart com,void* str,Usart_uint16 len);

/***********************************************
	Description: void set_usart_interrupt_state(PSTUsart com, EUsart_Interrupt_State usart_interrupt_state);
		�趨�����ж�״̬
	para:
		( PSTUsart,        EUSART_Interrupt_State)
		  ����ѡ�� 	        �ж�״̬(on or off)
	return:
	
***********************************************/
void set_com_interrupt_state(PSTUsart com, EUsart_Interrupt_State usart_interrupt_state);

/***********************************************
	Description: 	Usart_int8 receive_str_manage(PSTUsart com, void *store, Usart_uint8 count);
		�����������ݴ�����
	para:
		(		PSTUsart,	void *store, Usart_uint8 count);
			 ����ѡ��     ��ȡ��ַ        ����
	return:
		����ֵΪfunction �����ķ���ֵ
***********************************************/
Usart_int8 receive_str_manage(PSTUsart com, void *store, Usart_uint8 count);

/***********************************************
	Description: 	void clear_usart_buff(PSTUsart com);
		�������
	para:
		(		PSTUsart	);
			 ����ѡ�� 
	return:
	
***********************************************/
void clear_com_buff(PSTUsart com);


/***********************************************
	Description: 	void usart_send_byte(PSTUsart com,Usart_uint8 byte);
		����һ���ַ�
	para:
		(		PSTUsart,					Usart_uint8);
			 ����ѡ��           ����
	return:
	
***********************************************/
void com_send_byte(PSTUsart com,Usart_uint8 byte);

/***********************************************
	Description: 	void usart_send_pstr(PSTUsart com,void *p);
		����һ������
	para:
		(		PSTUsart,					void *);
			 ����ѡ��     Ҫ���͵������׵�ַ
	return:
	
***********************************************/
void com_send_pstr(PSTUsart com,void *p);

#ifdef __cplusplus
}
#endif

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//----------------���Ժ��� start----------------
#if defined(TEST_usart_driver) && defined (TEST_SWITCH)
	int _usart_test_(void);
#endif
//----------------���Ժ��� end------------------

//
//..ͷ�ļ���������
//

#endif/*_USART_H*/
