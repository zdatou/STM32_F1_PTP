#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"
#include "stm32_eth.h"
#include "timer.h"

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);
struct netif *ethernetif_register(void);
void Set_MAC_Address(unsigned char* macadd);

void ETH_PTPTime_SetTime(struct ptptime_t * timestamp);
void ETH_PTPTime_GetTime(struct ptptime_t * timestamp);

void ETH_PTPTime_UpdateOffset(struct ptptime_t * timeoffset);
void ETH_PTPTime_AdjFreq(int32_t Adj);

  /* Examples of subsecond increment and addend values using SysClk = 72 MHz
   
   Addend * Increment = 2^63 / SysClk

    ptp_tick = Increment * 10^9 / 2^31
  
   +-----------+-----------+------------+
   | ptp tick  | Increment | Addend     |
   +-----------+-----------+------------+
   |  119 ns   |   255     | 0x1DF170C7 |
   |  100 ns   |   215     | 0x238391AA |
   |   50 ns   |   107     | 0x475C1B20 |
   |   20 ns   |    43     | 0xB191D856 |
   |   14 ns   |    30     | 0xFE843E9E |
   +-----------+-----------+------------+
  */

//#define ADJ_FREQ_BASE_ADDEND      0xB191D856
//#define ADJ_FREQ_BASE_INCREMENT   43

  /* Examples of subsecond increment and addend values using SysClk = 150 MHz
   
   Addend * Increment = 2^63 / SysClk

    ptp_tick = Increment * 10^9 / 2^31
  
   +-----------+-----------+------------+
   | ptp tick  | Increment | Addend     |
   +-----------+-----------+------------+
   |   20 ns   |    13     | 0x553BC400 |
   +-----------+-----------+------------+
  */


#define ADJ_FREQ1_BASE_ADDEND      0x553BC400
#define ADJ_FREQ1_BASE_INCREMENT   43

#define ADJ_FREQ_BASE_ADDEND      0xB191D856
#define ADJ_FREQ_BASE_INCREMENT   43


#ifdef SERVER

#define MAC_ADDR0 0x00
#define MAC_ADDR1 0x00
#define MAC_ADDR2 0x00
#define MAC_ADDR3 0x00
#define MAC_ADDR4 0x00
#define MAC_ADDR5 0x01

#else

#define MAC_ADDR0 0x00
#define MAC_ADDR1 0x00
#define MAC_ADDR2 0x00
#define MAC_ADDR3 0x00
#define MAC_ADDR4 0x00
/* #define MAC_ADDR5 0x02 */
#define MAC_ADDR5 0x03
/* #define MAC_ADDR5 0x04 */

#endif

#endif
