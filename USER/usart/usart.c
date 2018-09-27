/*
//
	Project 	: mbus
	File Name 	: usart.c
	Author 		: songlaifeng wanghui
	Copyright		:	��ɳ�����Ϣ�Ƽ����޹�˾��Ȩ����
	Created on	: 2016-3-3
	Destribe	:
		
	modify :
		2016-9-26 ���ڼܹ��޸ĳ�com��ʽ
//
*/

//<<<<<<<<<<<<<<<<ͷ�ļ�<<<<<<<<<<<<<<<<
#include <stdio.h>
#include "stm32f10x.h"
#include "usart.h"
#include "stm32_eth.h"
#include "string.h"
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
//<<<<<<<<<<<<<<<<�궨��<<<<<<<<<<<<<<<<

#define USART1_485_SEND()		
#define USART1_485_RECEIVE()		

#define USART2_485_SEND()
#define USART2_485_RECEIVE()

#define USART6_485_SEND()	
#define USART6_485_RECEIVE() 

#define USART3_485_GPIO		GPIOB
#define USART3_485_PIN		GPIO_Pin_12
#define USART3_485_RCC		RCC_AHB1Periph_GPIOB
#define USART3_485_SEND()		GPIO_SetBits(USART3_485_GPIO, USART3_485_PIN)
#define USART3_485_RECEIVE()		GPIO_ResetBits(USART3_485_GPIO, USART3_485_PIN)

#define USART5_485_SEND()
#define USART5_485_RECEIVE()
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<ö�ٶ���<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<�ṹ����<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<��������<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<��������<<<<<<<<<<<<<<<<
static int usart1_init(Usart_uint32,EUSART_Parity);

static int usart2_init(Usart_uint32,EUSART_Parity);

static int usart3_init(Usart_uint32,EUSART_Parity);

static int usart4_init(Usart_uint32,EUSART_Parity);

static int usart5_init(Usart_uint32,EUSART_Parity);


static int usart1_send_str(void*,Usart_uint16);

static int usart2_send_str(void*,Usart_uint16);

static int usart3_send_str(void*,Usart_uint16);

static int usart4_send_str(void* str,Usart_uint16 len);

static int usart5_send_str(void*,Usart_uint16);


static int usart1_send_pstr(void *p);

static int usart2_send_pstr(void *p);

static int usart3_send_pstr(void *p);

static int usart4_send_pstr(void *p);

static int usart5_send_pstr(void *p);

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<��������<<<<<<<<<<<<<<<<
static PSTUsart g_usart1_irq, g_usart2_irq, g_usart3_irq, g_usart5_irq, g_usart4_irq;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//<<<<<<<<<<<<<<<<��������<<<<<<<<<<<<<<<<

void mystrncpy(void * des, void * souce, uint32 len)
{
	uint8 * pdes = (uint8 *)des;
	uint8 * psouce = (uint8 *)souce;
	u32 i = 0;
	
	for(i = 0; i < len; i++)
	{
		pdes[i] = psouce[i];
	}
}

//��ʼ������
void com_init(PSTUsart com)
{
	if(com->name == EUsart_1)
	{
		g_usart1_irq = com;
		usart1_init(com->bound, com->partity);
	}
	else if(com->name == EUsart_2)
	{
		g_usart2_irq = com;
		usart2_init(com->bound, com->partity);
	}
	else if(com->name == EUsart_3)
	{
		g_usart3_irq = com;
		usart3_init(com->bound, com->partity);
	}
	else if(com->name == EUsart_5)
	{
		g_usart5_irq = com;
		usart5_init(com->bound, com->partity);
	}	
	else if(com->name == EUsart_4)
	{
		g_usart4_irq = com;
		usart4_init(com->bound, com->partity);
	}
}

//�����ַ�����
void com_send_str(PSTUsart com,void* str,Usart_uint16 len)
{
	if(com->name == EUsart_1)
	{
		usart1_send_str(str,len);
	}
	else if(com->name == EUsart_2)
	{
		usart2_send_str(str,len);
	}
	else if(com->name == EUsart_3)
	{
		usart3_send_str(str,len);
	}
	else if(com->name == EUsart_4)
	{
		usart4_send_str(str,len);
	}
	else if(com->name == EUsart_5)
	{
		usart5_send_str(str,len);
	}	
}

//�趨�����ж�״̬
void set_com_interrupt_state(PSTUsart com, EUsart_Interrupt_State usart_interrupt_state)
{
	(void)com;
	(void)usart_interrupt_state;
	if(com->name == EUsart_1)
	{
		(usart_interrupt_state == EUSART_Interrupt_State_On)\
		?( USART_ITConfig(USART1, USART_IT_RXNE, ENABLE) )\
		:( USART_ITConfig(USART1, USART_IT_RXNE, DISABLE));
	}
	else if(com->name == EUsart_2)
	{
		(usart_interrupt_state == EUSART_Interrupt_State_On)\
		?( USART_ITConfig(USART2, USART_IT_RXNE, ENABLE) )\
		:( USART_ITConfig(USART2, USART_IT_RXNE, DISABLE));
	}
	else if(com->name == EUsart_3)
	{
		(usart_interrupt_state == EUSART_Interrupt_State_On)\
		?( USART_ITConfig(USART3, USART_IT_RXNE, ENABLE) )\
		:( USART_ITConfig(USART3, USART_IT_RXNE, DISABLE));
	}	
	else if(com->name == EUsart_4)
	{
		(usart_interrupt_state == EUSART_Interrupt_State_On)\
		?( USART_ITConfig(UART4, USART_IT_RXNE, ENABLE) )\
		:( USART_ITConfig(UART4, USART_IT_RXNE, DISABLE));
	}
	else if(com->name == EUsart_5)
	{
		(usart_interrupt_state == EUSART_Interrupt_State_On)\
		?( USART_ITConfig(UART5, USART_IT_RXNE, ENABLE) )\
		:( USART_ITConfig(UART5, USART_IT_RXNE, DISABLE));
	}	
}

//		�����������ݴ�����
Usart_int8 receive_str_manage(PSTUsart com, void *store, Usart_uint8 count)
{
		static Usart_uint32 usart1_tmp = 0;
		static Usart_uint32 usart2_tmp = 0;
		static Usart_uint32 usart3_tmp = 0;
		static Usart_uint32 usart5_tmp = 0;
	    static Usart_uint32 usart4_tmp = 0;
	
		static Usart_uint8 usart1_state = 0;
		static Usart_uint8 usart2_state = 0;
		static Usart_uint8 usart3_state = 0;
		static Usart_uint8 usart5_state = 0;
	    static Usart_uint8 usart4_state = 0;

		unsigned char buff[USART_RECEIVE_MAX] = {0};
		unsigned int len = 0;
		Usart_int8 tmp = 0;
		
		if(com->name == EUsart_1)
		{
				if(com->receive_count != usart1_tmp)
				{	
					usart1_tmp = com->receive_count;
					usart1_state++;
					return tmp;
				}
				if(usart1_state > 0)
				{
								mystrncpy((char *)buff, (char *)com->buff, com->receive_count);
								len = com->receive_count;
							
								com->receive_count = 0;
								memset(com->buff, 0, USART_RECEIVE_MAX);
								usart1_tmp = 0;
								usart1_state = 0;
							
								tmp = com->func(buff, len, store, count);			
				}
		}
		else if(com->name == EUsart_2)
		{
				if(com->receive_count != usart2_tmp)
				{	
					usart2_tmp = com->receive_count;
					usart2_state++;
					return tmp;
				}
				if(usart2_state > 0)
				{
								mystrncpy((char *)buff, (char *)com->buff, com->receive_count);
								len = com->receive_count;
							
								com->receive_count = 0;
								memset(com->buff, 0, USART_RECEIVE_MAX);
								usart2_tmp = 0;
								usart2_state = 0;
							
								tmp = com->func(buff, len, store, count);			
				}
		}
		else if(com->name == EUsart_3)
		{
				if(com->receive_count != usart3_tmp)
				{	
					usart3_tmp = com->receive_count;
					usart3_state++;
					return tmp;
				}
				if(usart3_state > 0)
				{
								mystrncpy((char *)buff, (char *)com->buff, com->receive_count);
								len = com->receive_count;
							
								com->receive_count = 0;
								memset(com->buff, 0, USART_RECEIVE_MAX);
								usart3_tmp = 0;
								usart3_state = 0;
							
								tmp = com->func(buff, len, store, count);			
				}
		}
		else if(com->name == EUsart_5)
		{
				if(com->receive_count != usart5_tmp)
				{	
					usart5_tmp = com->receive_count;
					usart5_state++;
					return tmp;
				}
				if(usart5_state > 0)
				{
								mystrncpy((char *)buff, (char *)com->buff, com->receive_count);
								len = com->receive_count;
							
								com->receive_count = 0;
								memset(com->buff, 0, USART_RECEIVE_MAX);
								usart5_tmp = 0;
								usart5_state = 0;
							
								tmp = com->func(buff, len, store, count);			
				}
		}
		else if(com->name == EUsart_4)
		{
				if(com->receive_count != usart4_tmp)
				{	
					usart4_tmp = com->receive_count;
					usart4_state++;
					return tmp;
				}
				if(usart4_state > 0)
				{
								mystrncpy((char *)buff, (char *)com->buff, com->receive_count);
								len = com->receive_count;
							
								com->receive_count = 0;
								memset(com->buff, 0, USART_RECEIVE_MAX);
								usart4_tmp = 0;
								usart4_state = 0;
							
								tmp = com->func(buff, len, store, count);			
				}
		}
		return tmp;		
}

//�������
void clear_com_buff(PSTUsart com)
{
		memset(com->buff, 0 , USART_RECEIVE_MAX);
		com->receive_count = 0;
}


//����һ���ַ�
void com_send_byte(PSTUsart com,Usart_uint8 byte)
{
	if(com->name == EUsart_1)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
		USART_SendData(USART1 ,byte);//�����ַ�
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
	}
	else if(com->name == EUsart_2)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); 
		USART_SendData(USART2 ,byte);//�����ַ�
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); 
	}
	else if(com->name == EUsart_3)
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); 
		USART_SendData(USART3 ,byte);//�����ַ�
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); 
	}
	else if(com->name == EUsart_4)
	{
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET); 
		USART_SendData(UART4 ,byte);//�����ַ�
		while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET); 
	}
	else if(com->name == EUsart_5)
	{
		while(USART_GetFlagStatus(UART5, USART_FLAG_TC)==RESET); 
		USART_SendData(UART5 ,byte);//�����ַ�
		while(USART_GetFlagStatus(UART5, USART_FLAG_TC)==RESET); 
	}		
}


void com_send_pstr(PSTUsart com,void *p)
{
	if(com->name == EUsart_1)
	{
		usart1_send_pstr(p);
	}
	else if(com->name == EUsart_2)
	{
		usart2_send_pstr(p);
	}
	else if(com->name == EUsart_3)
	{
		usart3_send_pstr(p);
	}
	else if(com->name == EUsart_4)
	{
		usart4_send_pstr(p);
	}
	else if(com->name == EUsart_5)
	{
		usart5_send_pstr(p);
	}
}
//----------------�������� start----------------
//��ʼ������1
static int usart1_init(Usart_uint32 bound,EUSART_Parity parity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	(void)parity;
	
	//485ʹ�ܳ�ʼ��
	//
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	
	
	
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��PA10



   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	
	if(parity != EUSART_Parity_No)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ9λ���ݸ�ʽ
	}
	else
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	
	if(parity == EUSART_Parity_Even)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	}
	else if(parity == EUSART_Parity_Odd)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Odd;//��У��λ
	}
	else
	{
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	}
	
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1  
	

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	//485
	USART1_485_RECEIVE();
	
	return 0;
}
//��ʼ������2
static int usart2_init(Usart_uint32 bound,EUSART_Parity parity)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	(void)parity;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	
	GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
	
	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOD, &GPIO_InitStructure); //��ʼ��PA9
   
	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��PA3

   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	
	if(parity != EUSART_Parity_No)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	else
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	
	if(parity == EUSART_Parity_Even)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	}
	else if(parity == EUSART_Parity_Odd)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Odd;//��У��λ
	}
	else
	{
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	}
	
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
  USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	USART2_485_RECEIVE();
	
	return 0;
}

//��ʼ������3
static int usart3_init(Usart_uint32 bound,EUSART_Parity parity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	(void)parity;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	
	
	//USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ�� PB.10
   
	//USART3_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��PB11
	
   //USART3 ��ʼ������
	 
	USART_InitStructure.USART_BaudRate = bound;//����������
	
	if(parity != EUSART_Parity_No)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	else
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	
	if(parity == EUSART_Parity_Even)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	}
	else if(parity == EUSART_Parity_Odd)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Odd;//��У��λ
	}
	else
	{
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	}
	
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
	
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�
	
	 USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3

	//Usart3 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	
  //USART3_485_RECEIVE();
	return 0;
}

static int usart4_init(Usart_uint32 bound,EUSART_Parity parity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	(void)parity;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	
	
	//USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ�� PB.10
   
	//USART3_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��PB11
	
   //USART3 ��ʼ������
	 
	USART_InitStructure.USART_BaudRate = bound;//����������
	
	if(parity != EUSART_Parity_No)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	else
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	
	if(parity == EUSART_Parity_Even)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	}
	else if(parity == EUSART_Parity_Odd)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Odd;//��У��λ
	}
	else
	{
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	}
	
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure); //��ʼ������3
	
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//��������ж�
	
	 USART_Cmd(UART4, ENABLE);  //ʹ�ܴ���3

	 
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
 
	return 0;
}

/**********************************************************************************************/
//��ʼ������5
static int usart5_init(Usart_uint32 bound,EUSART_Parity parity)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	(void)parity;
	  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	

	

	//USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; // PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ�� PB.10
   
	//USART3_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��PB11



   //USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	
	if(parity != EUSART_Parity_No)
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	else
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	}
	
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	
	if(parity == EUSART_Parity_Even)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	}
	else if(parity == EUSART_Parity_Odd)
	{
		USART_InitStructure.USART_Parity = USART_Parity_Odd;//��У��λ
	}
	else
	{
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	}
	
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART5, &USART_InitStructure); //��ʼ������5
	
  USART_Cmd(UART5, ENABLE);  //ʹ�ܴ���5
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//����6�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	return 0;
}
/**************************************************************************************************************/
//����1���ͺ���
static int usart1_send_str(void* str,Usart_uint16 len)
{
	Usart_uint16 i;
	Usart_int8 * s;
	
	USART1_485_SEND();
	
	s = (Usart_int8 *)str;
	
	for(i = 0; i< len; i++)
	{
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
			USART_SendData(USART1 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET){}
	
	USART1_485_RECEIVE();
	
	return 0;
}

//����2���ͺ���
static int usart2_send_str(void* str,Usart_uint16 len)
{
	Usart_uint16 i;
	Usart_int8 * s;
	
	
	s = (Usart_int8 *)str;
	
	for(i = 0; i< len; i++)
	{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); 
			USART_SendData(USART2 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); 
	
	return 0;
}

//����3���ͺ���
static int usart3_send_str(void* str,Usart_uint16 len)
{
	Usart_uint16 i;
	Usart_int8 * s;
	
	USART3_485_SEND();
	
	s = (Usart_int8 *)str;
	
	for(i = 0; i< len; i++)
	{
			while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); 
			USART_SendData(USART3 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET){}
	
	USART3_485_RECEIVE();
	
	return 0;
}

static int usart4_send_str(void* str,Usart_uint16 len)
{
	Usart_uint16 i;
	Usart_int8 * s;
	
 
	
	s = (Usart_int8 *)str;
	
	for(i = 0; i< len; i++)
	{
			while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET); 
			USART_SendData(UART4 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET){}
	
 
	
	return 0;
}

//����5���ͺ���
static int usart5_send_str(void* str,Usart_uint16 len)
{
	Usart_uint16 i;
	Usart_int8 * s;
	
//	USART5_485_SEND();
	
	s = (Usart_int8 *)str;
	
	for(i = 0; i< len; i++)
	{
			while(USART_GetFlagStatus(UART5, USART_FLAG_TC)==RESET); 
			USART_SendData(UART5 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC)==RESET){} 
	
//	USART5_485_RECEIVE();
	
	return 0;
}


//����1���ͺ���
static int usart1_send_pstr(void* str)
{
	Usart_int8 * s;
	
	USART1_485_SEND();
	
	s = (Usart_int8 *)str;
	
	while(*s)
	{
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
			USART_SendData(USART1 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET){}
	
	USART1_485_RECEIVE();
	
	return 0;
}

//����2���ͺ���
static int usart2_send_pstr(void* str)
{
	Usart_int8 * s;	
	
	s = (Usart_int8 *)str;
	
	USART2_485_SEND();
	
	while(*s)
	{
			while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET); 
			USART_SendData(USART2 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET){} 
	
	USART2_485_RECEIVE();
	
	return 0;
}

//����3���ͺ���
static int usart3_send_pstr(void* str)
{
	Usart_int8 * s;
	
	USART3_485_SEND();
	
	s = (Usart_int8 *)str;
	
	while(*s)
	{
			while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); 
			USART_SendData(USART3 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET){}
	
	USART3_485_RECEIVE();
	
	return 0;
}

//����4���ͺ���
static int usart4_send_pstr(void* str)
{
	Usart_int8 * s;
	
 
	
	s = (Usart_int8 *)str;
	
	while(*s)
	{
			while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET); 
			USART_SendData(UART4 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET){}
	
 
	
	return 0;
}

//����5���ͺ���
static int usart5_send_pstr(void* str)
{
	Usart_int8 * s;
	
	USART5_485_SEND();
	
	s = (Usart_int8 *)str;
	
	while(*s)
	{
			while(USART_GetFlagStatus(UART5, USART_FLAG_TC)==RESET); 
			USART_SendData(UART5 ,*s++);//���͵�ǰ�ַ�
	}
	
	while(USART_GetFlagStatus(UART5, USART_FLAG_TC)==RESET){}
	
	USART5_485_RECEIVE();
	
	return 0;
}


//----------------�������� end----------------

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//>>>>>>>>>>>>>>>>>�жη������>>>>>>>>>>>>>>>>>>
void USART1_IRQHandler(void)                	//����1�жϷ������
{
		Usart_uint8 Res=0;	
			
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		{	
				Res = USART_ReceiveData(USART1);
			
				(g_usart1_irq->buff)[g_usart1_irq->receive_count] = Res;  	  //�����յ����ַ����浽������

				(g_usart1_irq->receive_count)++;

				if((g_usart1_irq->receive_count) >= USART_RECEIVE_MAX)       		//���������,������ָ��ָ�򻺴���׵�ַ
				{
					(g_usart1_irq->receive_count) = 0;
				} 
		}
		
} 

void USART2_IRQHandler(void)                	//����2�жϷ������
{
		Usart_uint8 Res=0;
		
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
		{
			Res = USART_ReceiveData(USART2);
		
			(g_usart2_irq->buff)[g_usart2_irq->receive_count] = Res;  	  //�����յ����ַ����浽������

			(g_usart2_irq->receive_count)++;

			if((g_usart2_irq->receive_count) >= USART_RECEIVE_MAX)       		//���������,������ָ��ָ�򻺴���׵�ַ
			{
				(g_usart2_irq->receive_count) = 0;
			}		
		}
} 

void USART3_IRQHandler(void)                	//����3�жϷ������
{
		Usart_uint8 Res=0;
		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
		{
			Res = USART_ReceiveData(USART3);
		
			(g_usart3_irq->buff)[g_usart3_irq->receive_count] = Res;  	  //�����յ����ַ����浽������

			(g_usart3_irq->receive_count)++;

			if((g_usart3_irq->receive_count) >= USART_RECEIVE_MAX)       		//���������,������ָ��ָ�򻺴���׵�ַ
			{
				(g_usart3_irq->receive_count) = 0;
			}	
		}
} 

void UART4_IRQHandler(void)                	//����4�жϷ������
{
		Usart_uint8 Res=0;
		if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
		{
			Res = USART_ReceiveData(UART4);
		
			(g_usart4_irq->buff)[g_usart4_irq->receive_count] = Res;  	  //�����յ����ַ����浽������

			(g_usart4_irq->receive_count)++;

			if((g_usart4_irq->receive_count) >= USART_RECEIVE_MAX)       		//���������,������ָ��ָ�򻺴���׵�ַ
			{
				(g_usart4_irq->receive_count) = 0;
			}	
		}
} 

void UART5_IRQHandler(void)                	//����5�жϷ������
{
		Usart_uint8 Res=0;
		
		if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
		{
			Res = USART_ReceiveData(UART5);

			(g_usart5_irq->buff)[g_usart5_irq->receive_count] = Res;  	  //�����յ����ַ����浽������

			(g_usart5_irq->receive_count)++;

			if((g_usart5_irq->receive_count) >= USART_RECEIVE_MAX)       		//���������,������ָ��ָ�򻺴���׵�ַ
			{
				g_usart5_irq->receive_count = 0;
			}	
		}
} 

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//
//...�ļ���������...
//
