/**
  ******************************************************************************
  * @file    udp_echoserver.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   UDP echo server
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Allheader.h"


/* Private typedef -----------------------------------------------------------*/
extern __IO state sntp_state;
/* Private define ------------------------------------------------------------*/
#define UDP_SERVER_PORT    123   /* define the UDP local connection port */
#define UDP_CLIENT_PORT    123   /* define the UDP remote connection port */

/*NTP �������*/

struct sntp_c SntpMonitorl[MonitorlConstant];

#define UDP_DEMO_RX_BUFSIZE		2048	//����udp���������ݳ��� 
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ�����

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void sntp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
void sntp_packet_senddata(struct udp_pcb *upcb,time_fp *xmt, time_fp recv);
void sntp_unpack_control(struct udp_pcb *upcb,struct sntp_s *msg);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void sntp_server_init(void)//NTP 
{
   struct udp_pcb *upcb;
   err_t err;
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, sntp_server_receive_callback, NULL);
      }
      else
      {
        udp_remove(upcb);
      }
   }
	 
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void sntp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
  
 struct sntp_s sntp_msg;
 time_fp recv_t;
 u32 data_len = 0;
 struct pbuf *q;
	if(p!=NULL)	//���յ���Ϊ�յ�����ʱ
	{
		get_sys_timer(&recv_t);
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
		memset(&sntp_msg,0,sizeof(sntp_msg));
		for(q=p;q!=NULL;q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		 udp_connect(upcb, addr, port);
		 memcpy((char*)&sntp_msg,udp_demo_recvbuf,sizeof(sntp_msg));
		  if ((data_len < sizeof(sntp_msg)) || (MSG_MODE(sntp_msg.li_vn_md) != MODE_CLIENT))
			{
						udp_disconnect(upcb);
      }
			else if( (MSG_MODE(sntp_msg.li_vn_md) == MODE_CLIENT) )
			{
				memcpy((char*)&sntp_msg,udp_demo_recvbuf,sizeof(sntp_msg));
				/* Tell the client that we have accepted it */
				sntp_packet_senddata(upcb, &sntp_msg.xmt, recv_t);
				/* free the UDP connection, so we can accept new clients */
				udp_disconnect(upcb);
			}
			else if( (MSG_MODE(sntp_msg.li_vn_md) == MODE_CONTROL) )//������Ϣ
			{
			  	memcpy((char*)&sntp_msg,udp_demo_recvbuf,sizeof(sntp_msg));
			  	sntp_unpack_control(upcb,&sntp_msg);
				 	udp_disconnect(upcb);
			}
		
	}
	
  /* Free the p buffer */
    pbuf_free(p);
}

void sntp_packet_senddata(struct udp_pcb *upcb,time_fp *xmt, time_fp recv)
{
	struct pbuf *ptr;
	unsigned char mode = MODE_SERVER;
  struct sntp_s sntp_msg;
	ptr=pbuf_alloc(PBUF_TRANSPORT,sizeof(sntp_msg),PBUF_POOL); //�����ڴ�
	if(ptr)
	{
	  memset(&sntp_msg, 0, sizeof(sntp_msg));
    /* ���յ��ͻ�������, �����˴���ʱ��� */
    if (xmt)
    {
        mode = MODE_SERVER;
        sntp_msg.orig = *xmt;     //Դʱ��
				sntp_msg.recv = recv;    //����ʱ��
        HTONL_FP(&sntp_msg.recv,&sntp_msg.recv);//�����ֽ�ת���������ֽ�
    }
		sntp_msg.ref_time.intg.ui = recv.intg.ui; //���һ��У׼ʱ��
		HTONL_FP(&sntp_msg.ref_time,&sntp_msg.ref_time);
    sntp_msg.li_vn_md = MSG_LI_VN_MODE(sntp_state.sync_state, SNTP_VERSION, mode);
    sntp_msg.stratum = SNTP_STRATUM;
    sntp_msg.ref_id = upcb->local_ip.addr;
    get_sys_timer(&sntp_msg.xmt);//��ȡϵͳʱ��
    HTONL_FP(&sntp_msg.xmt, &sntp_msg.xmt); //ת�������ʽ
		pbuf_take(ptr,&sntp_msg,sizeof(sntp_msg)); //��tcp_demo_sendbuf�е����ݴ����pbuf�ṹ��
		udp_send(upcb,ptr);	//udp�������� 
		pbuf_free(ptr);//�ͷ��ڴ�
	} 
} 



void sntp_unpack_control(struct udp_pcb *upcb,struct sntp_s *msg)
{
	 static u8 cnt = 0;
	 uint8_t i = 0;
   uint64_t disp = 0;
	 double nanosec = 0;
	 time_fp offset , delay;
	
    NTOHL_FP(&msg->recv, &offset);
    NTOHL_FP(&msg->xmt,  &delay);   
	  TS_FPTOD(&offset,nanosec);
	  disp = nanosec*100000000;
	 for( i = 0 ; i < MonitorlConstant ; i ++ )
	{
    if(SntpMonitorl[i].ipaddr ==  upcb->remote_ip.addr) //������������ͬIP�������������
		{
			SntpMonitorl[i].root_disp.intg.si  = disp / 100000000;
			SntpMonitorl[i].root_disp.frag.uf  = disp % 100000000;
			SntpMonitorl[i].Time = ETH->PTPTSHR;
			return  ;
		}
	}
	for( i = 0 ; i < MonitorlConstant ; i ++  )//���� һ�� ���� Ѱ�ҿյĵ�ַ ������
	{
	  if(SntpMonitorl[i].ipaddr == NULL)//����������IP��ַΪ�յ�����
		{
			SntpMonitorl[i].ipaddr     =  upcb->remote_ip.addr;
			SntpMonitorl[i].root_disp.intg.si  = disp / 100000000;
			SntpMonitorl[i].root_disp.frag.uf  = disp % 100000000;
			SntpMonitorl[i].Time = ETH->PTPTSHR;
			return ;
		}
	}
	//��Ŀ��IP�����������棬�������һ��IP���滻�µ�IP��ַ��
	  SntpMonitorl[cnt  ].ipaddr		 = upcb->remote_ip.addr;
		SntpMonitorl[cnt  ].Time       = ETH->PTPTSHR;
	  SntpMonitorl[cnt  ].root_disp.intg.si  = disp / 100000000;
		SntpMonitorl[cnt++].root_disp.frag.uf  = disp % 100000000;
	 if( cnt > MonitorlConstant ) { cnt = 0 ;}
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
