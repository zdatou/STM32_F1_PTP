#ifndef _CMD_HANDLE_
#define _CMD_HANDLE_

#define CONNECT   0
#define UNCONNECT 1
#define SYSTEM    2
#define SEND      3
#define UDP_CONNET 4
#define IPSHOW 5
#define IP_ERROR 6
#define SET_FREQUENCY 7
#define UBOLX_MSG 8
#define EDGE_MSG 8
#define REMOTE_IPSHOW 9
#define NET_BACK 10
#define PTP_SLAVE_BACK 11
#define PTP_MASTER_BACK 12
 

extern u8 remote_ip_addr1;
extern u8 remote_ip_addr2;
extern u8 remote_ip_addr3;
extern u8 remote_ip_addr4;

extern uint8 netif_ip_addr1;
extern uint8 netif_ip_addr2;
extern uint8 netif_ip_addr3;
extern uint8 netif_ip_addr4;

extern uint8 netif_ip_gw1;
extern uint8 netif_ip_gw2;
extern uint8 netif_ip_gw3;
extern uint8 netif_ip_gw4;

extern uint8 netif_ip_netmask1;
extern uint8 netif_ip_netmask2;
extern uint8 netif_ip_netmask3;
extern uint8 netif_ip_netmask4;





uint8 cmd_handle_usart1(void *str, uint16 len);
uint8 cmd_handle_usart5(void *str, uint16 len);
uint8 cmd_handle_usart2(void *str, uint16 len);

#endif

