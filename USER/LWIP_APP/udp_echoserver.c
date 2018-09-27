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

/*NTP 监控设置*/

struct sntp_c SntpMonitorl[MonitorlConstant];

#define UDP_DEMO_RX_BUFSIZE		2048	//定义udp最大接收数据长度 
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP接收数据缓冲区

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
	if(p!=NULL)	//接收到不为空的数据时
	{
		get_sys_timer(&recv_t);
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //数据接收缓冲区清零
		memset(&sntp_msg,0,sizeof(sntp_msg));
		for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
		{
			//判断要拷贝到UDP_DEMO_RX_BUFSIZE中的数据是否大于UDP_DEMO_RX_BUFSIZE的剩余空间，如果大于
			//的话就只拷贝UDP_DEMO_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//拷贝数据
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
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
			else if( (MSG_MODE(sntp_msg.li_vn_md) == MODE_CONTROL) )//控制消息
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
	ptr=pbuf_alloc(PBUF_TRANSPORT,sizeof(sntp_msg),PBUF_POOL); //申请内存
	if(ptr)
	{
	  memset(&sntp_msg, 0, sizeof(sntp_msg));
    /* 接收到客户的数据, 包含了传输时间戳 */
    if (xmt)
    {
        mode = MODE_SERVER;
        sntp_msg.orig = *xmt;     //源时间
				sntp_msg.recv = recv;    //接收时间
        HTONL_FP(&sntp_msg.recv,&sntp_msg.recv);//主机字节转换成网络字节
    }
		sntp_msg.ref_time.intg.ui = recv.intg.ui; //最后一次校准时间
		HTONL_FP(&sntp_msg.ref_time,&sntp_msg.ref_time);
    sntp_msg.li_vn_md = MSG_LI_VN_MODE(sntp_state.sync_state, SNTP_VERSION, mode);
    sntp_msg.stratum = SNTP_STRATUM;
    sntp_msg.ref_id = upcb->local_ip.addr;
    get_sys_timer(&sntp_msg.xmt);//获取系统时间
    HTONL_FP(&sntp_msg.xmt, &sntp_msg.xmt); //转换网络格式
		pbuf_take(ptr,&sntp_msg,sizeof(sntp_msg)); //将tcp_demo_sendbuf中的数据打包进pbuf结构中
		udp_send(upcb,ptr);	//udp发送数据 
		pbuf_free(ptr);//释放内存
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
    if(SntpMonitorl[i].ipaddr ==  upcb->remote_ip.addr) //数组里面有相同IP则更新里面数据
		{
			SntpMonitorl[i].root_disp.intg.si  = disp / 100000000;
			SntpMonitorl[i].root_disp.frag.uf  = disp % 100000000;
			SntpMonitorl[i].Time = ETH->PTPTSHR;
			return  ;
		}
	}
	for( i = 0 ; i < MonitorlConstant ; i ++  )//历遍 一次 数组 寻找空的地址 存数据
	{
	  if(SntpMonitorl[i].ipaddr == NULL)//存在数组里IP地址为空的数组
		{
			SntpMonitorl[i].ipaddr     =  upcb->remote_ip.addr;
			SntpMonitorl[i].root_disp.intg.si  = disp / 100000000;
			SntpMonitorl[i].root_disp.frag.uf  = disp % 100000000;
			SntpMonitorl[i].Time = ETH->PTPTSHR;
			return ;
		}
	}
	//若目标IP不再数组里面，则清除第一个IP，替换新的IP地址。
	  SntpMonitorl[cnt  ].ipaddr		 = upcb->remote_ip.addr;
		SntpMonitorl[cnt  ].Time       = ETH->PTPTSHR;
	  SntpMonitorl[cnt  ].root_disp.intg.si  = disp / 100000000;
		SntpMonitorl[cnt++].root_disp.frag.uf  = disp % 100000000;
	 if( cnt > MonitorlConstant ) { cnt = 0 ;}
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
