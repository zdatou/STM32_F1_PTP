#ifndef  _TCP_CLIENT_H_
#define  _TCP_CLIENT_H_

#include "common.h"



/***************������ip��MAC����*************************/
#define IMT407G_IP  			  192,168,31,25   //������ip 
#define IMT407G_NETMASK   	255,255,255,0   //��������������
#define IMT407G_WG		   	  192,168,31,1   	//������������
#define IMT407G_MAC_ADDR    0,0,0,0,0,1			//������MAC��ַ

#define TCP_LOCAL_PORT     	2040 
#define TCP_Server_PORT    	2041
#define TCP_Server_IP   		172,16,40,21    //(���Զ�)������IP



extern struct tcp_pcb *tcp_client_pcb;

void TCP_Client_Init(u16_t local_port,u16_t remote_port,unsigned char a,unsigned char b,unsigned char c,unsigned char d);
err_t TCP_Client_Send_Data(struct tcp_pcb *cpcb,unsigned char *buff,unsigned int length);
struct tcp_pcb *Check_TCP_Connect(void);

#endif

