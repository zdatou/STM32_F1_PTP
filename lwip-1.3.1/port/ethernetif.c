

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"
#include "lwip/err.h"
#include "ethernetif.h"

#include "stm32_eth.h"
#include <string.h>

/* TCP and ARP timeouts */
volatile int tcp_end_time, arp_end_time;

/* Define those to better describe your network interface. */
#define IFNAME0 's'
#define IFNAME1 't'


/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  int unused;
};

/* Forward declarations. */
err_t  ethernetif_input(struct netif *netif);

uint8_t MACaddr[6];
ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];/* Ethernet Rx & Tx DMA Descriptors */
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE], Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE];/* Ethernet buffers */

extern __IO ETH_DMADESCTypeDef  *DMATxDescToSet;
extern __IO ETH_DMADESCTypeDef  *DMARxDescToGet;

typedef struct{
  u32 length;
  u32 buffer;
  __IO ETH_DMADESCTypeDef *descriptor;

#if LWIP_PTP
  __IO ETH_DMADESCTypeDef *PTPdescriptor;
#endif
}FrameTypeDef;

#if ! LWIP_PTP
static void ETH_RxPkt_ChainMode(FrameTypeDef * frame);
static u32 ETH_TxPkt_ChainMode(u16 FrameLength);
static u8 * ETH_TxPkt_PrepareBuffer();
#endif

static void ETH_RxPkt_ChainMode_CleanUp(FrameTypeDef * frame);
static u32 ETH_GetCurrentTxBuffer(void);


#if LWIP_PTP
ETH_DMADESCTypeDef  DMAPTPRxDscrTab[ETH_RXBUFNB], DMAPTPTxDscrTab[ETH_TXBUFNB];/* Ethernet Rx & Tx PTP Helper Descriptors */
extern __IO ETH_DMADESCTypeDef  *DMAPTPTxDescToSet;
extern __IO ETH_DMADESCTypeDef  *DMAPTPRxDescToGet;
static void ETH_PTPStart(uint32_t UpdateMethod);
static void ETH_PTPRxPkt_ChainMode(FrameTypeDef * frame);
static void ETH_PTPRxPkt_ChainMode_CleanUp(FrameTypeDef * frame, struct ptptime_t * timestamp);
static u32 ETH_PTPTxPkt_ChainMode(u16 FrameLength, struct ptptime_t * timestamp);
static u8 * ETH_PTPTxPkt_PrepareBuffer(void);
#endif

u32_t ETH_PTPSubSecond2NanoSecond(u32_t SubSecondValue)
{
  uint64_t val = SubSecondValue * 1000000000ll;
  val >>=31;
  return val;
}

u32_t ETH_PTPNanoSecond2SubSecond(u32_t SubSecondValue)
{
  uint64_t val = SubSecondValue * 0x80000000ll;
  val /= 1000000000;
  return val;
}

void ETH_PTPTime_GetTime(struct ptptime_t * timestamp) {
  timestamp->tv_nsec = ETH_PTPSubSecond2NanoSecond(ETH_GetPTPRegister(ETH_PTPTSLR));
  timestamp->tv_sec = ETH_GetPTPRegister(ETH_PTPTSHR);
}

/**
 * Setting the MAC address.
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void Set_MAC_Address(uint8_t* macadd)
{
  MACaddr[0] = macadd[0];
  MACaddr[1] = macadd[1];
  MACaddr[2] = macadd[2];
  MACaddr[3] = macadd[3];
  MACaddr[4] = macadd[4];
  MACaddr[5] = macadd[5];

  ETH_MACAddressConfig(ETH_MAC_Address0, macadd);
}


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
	uint8_t macaddress[6]={0};	
	u32 sn0;
	sn0=*(vu32*)(0x1FFFF7E8);
	macaddress[0]=2;
	macaddress[1]=0;
	macaddress[2]=0;
	macaddress[3]=(sn0>>16)&0XFF;
	macaddress[4]=(sn0>>8)&0XFFF;
	macaddress[5]=sn0&0XFF; 
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =  MACaddr[0];
  netif->hwaddr[1] =  MACaddr[1];
  netif->hwaddr[2] =  MACaddr[2];
  netif->hwaddr[3] =  MACaddr[3];
  netif->hwaddr[4] =  MACaddr[4];
  netif->hwaddr[5] =  MACaddr[5];

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

#if LWIP_PTP
  /* Initialize Tx Descriptors list: Chain Mode */
   
   ETH_DMAPTPTxDescChainInit(DMATxDscrTab, DMAPTPTxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  /* Initialize Rx Descriptors list: Chain Mode  */
  ETH_DMAPTPRxDescChainInit(DMARxDscrTab, DMAPTPRxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
#else
  /* Initialize Tx Descriptors list: Chain Mode */
  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  /* Initialize Rx Descriptors list: Chain Mode  */
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
#endif

  /* Enable Ethernet Rx interrrupt */
  { int i;
    for(i=0; i<ETH_RXBUFNB; i++)
    {
      ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
    }
  }

#ifdef CHECKSUM_BY_HARDWARE
  /* Enable the checksum insertion for the Tx frames */
  { int i;
    for(i=0; i<ETH_TXBUFNB; i++)
    {
      ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
    }
  }
#endif

#if LWIP_PTP
  /* Enable PTP Timestamping */
  ETH_PTPStart(ETH_PTP_FineUpdate);
  /* ETH_PTPStart(ETH_PTP_CoarseUpdate); */
#endif
  /* Enable MAC and DMA transmission and reception */
  ETH_Start();
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int l = 0;
  u8 *buffer;

#if LWIP_PTP
  struct ptptime_t timestamp;
  buffer = ETH_PTPTxPkt_PrepareBuffer();
#else
  buffer = ETH_TxPkt_PrepareBuffer();
#endif
  
  if(buffer == NULL) {
    return ERR_INPROGRESS; /* ERR_MEM */
  }

  for(q = p; q != NULL; q = q->next)
  {
    memcpy((u8_t*)&buffer[l], q->payload, q->len);
    l = l + q->len;
  }

#if LWIP_PTP
  if( ETH_SUCCESS == ETH_PTPTxPkt_ChainMode(l, &timestamp) ) {
    p->time_sec = timestamp.tv_sec;
    p->time_nsec = timestamp.tv_nsec;
  } else {
    return ERR_IF;
  }
#else
  if( ETH_ERROR == ETH_TxPkt_ChainMode(l) ) {
    return ERR_IF;
  }

#endif
  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len;
  int l =0;
  FrameTypeDef frame;
  u8 *buffer;
#if LWIP_PTP
  struct ptptime_t timestamp;
#endif

  p = NULL;
  frame.descriptor = NULL;
#if LWIP_PTP
  frame.PTPdescriptor = NULL;
  ETH_PTPRxPkt_ChainMode(&frame);
#else
  ETH_RxPkt_ChainMode(&frame);
#endif
  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = frame.length;

  buffer = (u8 *)frame.buffer;

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL)
  {
    for (q = p; q != NULL; q = q->next)
    {
      memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
      l = l + q->len;
    }
  }

#if LWIP_PTP
  ETH_PTPRxPkt_ChainMode_CleanUp(&frame, &timestamp);
  if(p != NULL)
  {
	p->time_sec = timestamp.tv_sec;
	p->time_nsec = timestamp.tv_nsec;
  }
#endif

  ETH_RxPkt_ChainMode_CleanUp(&frame);

  return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
err_t
ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);

  /* no packet could be read, silently ignore this */
  if (p == NULL) return ERR_MEM;

  err = netif->input(p, netif);
  if (err != ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;
  }

  return err;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  ethernetif = (struct ethernetif*)mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_init(netif);

#ifdef LWIP_IGMP
  netif->flags |= NETIF_FLAG_IGMP | NETIF_FLAG_BROADCAST;
#endif

  return ERR_OK;
}

#if ! LWIP_PTP

/*******************************************************************************
* Function Name  : ETH_RxPkt_ChainMode
* Description    : Receives a packet.
* Input          : None
* Output         : frame: farme size and location
* Return         : None
*******************************************************************************/
static void ETH_RxPkt_ChainMode(FrameTypeDef * frame)
{
  u32 framelength = 0;
  frame->length = 0;
  frame->buffer = 0;

  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (u32)RESET)
  {
    frame->length = ETH_ERROR;

    if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)
    {
      /* Clear RBUS ETHERNET DMA flag */
      ETH->DMASR = ETH_DMASR_RBUS;
      /* Resume DMA reception */
      ETH->DMARPDR = 0;
    }

    /* Return error: OWN bit set */
    return;
  }

  if(((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32)RESET) &&
     ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32)RESET) &&
     ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32)RESET))
  {
    /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
    framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;
    /* Get the addrees of the actual buffer */
    frame->buffer = DMARxDescToGet->Buffer1Addr;
  }
  else
  {
    /* Return ERROR */
    framelength = ETH_ERROR;
  }

  frame->length = framelength;


  frame->descriptor = DMARxDescToGet;

  /* Update the ETHERNET DMA global Rx descriptor with next Rx decriptor */
  /* Chained Mode */
  /* Selects the next DMA Rx descriptor list for next buffer to read */
  DMARxDescToGet = (ETH_DMADESCTypeDef*) (DMARxDescToGet->Buffer2NextDescAddr);

  /* Return Frame */
  return;
}

#endif

/*******************************************************************************
* Function Name  : ETH_RxPkt_ChainMode_CleanUp
* Description    : Return memory to DMA.
* Input          : frame: farme size and location
* Output         : None
* Return         : None
*******************************************************************************/
static void ETH_RxPkt_ChainMode_CleanUp(FrameTypeDef * frame)
{
  /* Set Own bit of the Rx descriptor Status: gives the buffer back to ETHERNET DMA */
  frame->descriptor->Status = ETH_DMARxDesc_OWN;

  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)
  {
    /* Clear RBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_RBUS;
    /* Resume DMA reception */
    ETH->DMARPDR = 0;
  }
}

#if ! LWIP_PTP

/*******************************************************************************
* Function Name  : ETH_TxPkt_ChainMode
* Description    : Transmits a packet, from application buffer, pointed by ppkt.
* Input          : - FrameLength: Tx Packet size.
* Output         : None
* Return         : ETH_ERROR: in case of Tx desc owned by DMA
*                  ETH_SUCCESS: for correct transmission
*******************************************************************************/
static u32 ETH_TxPkt_ChainMode(u16 FrameLength)
{
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
  {
    /* Return ERROR: OWN bit set */
    return ETH_ERROR;
  }

  /* Setting the Frame Length: bits[12:0] */
  DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);

  /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
  DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

  /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
  DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)
  {
    /* Clear TBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    ETH->DMATPDR = 0;
  }

  /* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor */
  /* Chained Mode */
  /* Selects the next DMA Tx descriptor list for next buffer to send */
  DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);

  /* Return SUCCESS */
  return ETH_SUCCESS;
}

#endif

/*******************************************************************************
* Function Name  : ETH_GetCurrentTxBuffer
* Description    : Return the address of the buffer pointed by the current descritor.
* Input          : None
* Output         : None
* Return         : Buffer address
*******************************************************************************/
static u32 ETH_GetCurrentTxBuffer(void)
{
  /* Return Buffer address */
  return (DMATxDescToSet->Buffer1Addr);
}

#if ! LWIP_PTP
/*******************************************************************************
* Function Name  : ETH_TxPkt_PrepareBuffer
* Description    : Return correct Tx Buffer according to timestamps usage
* Input          : None
* Output         : None
* Return         : Current Tx Buffer
*******************************************************************************/
static u8 * ETH_TxPkt_PrepareBuffer() {
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
  {
    /* Return ERROR: OWN bit set */
    return NULL;
  }

  return (u8 *) ETH_GetCurrentTxBuffer();
}

#endif

#if LWIP_PTP

/*******************************************************************************
* Function Name  : ETH_PTPTxPkt_PrepareBuffer
* Description    : Return correct Tx Buffer according to timestamps usage
* Input          : None
* Output         : None
* Return         : Current Tx Buffer
*******************************************************************************/
static u8 * ETH_PTPTxPkt_PrepareBuffer() {
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
  {
    /* Return ERROR: OWN bit set */
    return NULL;
  }

  DMATxDescToSet->Buffer1Addr = DMAPTPTxDescToSet->Buffer1Addr;
  DMATxDescToSet->Buffer2NextDescAddr = DMAPTPTxDescToSet->Buffer2NextDescAddr;

  return (u8 *) ETH_GetCurrentTxBuffer();
}

/*******************************************************************************
* Function Name  : ETH_PTPRxPkt_ChainMode
* Description    : Receives a packet.
* Input          : None
* Output         : frame: farme size and location
* Return         : None
*******************************************************************************/
static void ETH_PTPRxPkt_ChainMode(FrameTypeDef * frame)
{
  u32 framelength = 0;
  frame->length = 0;
  frame->buffer = 0;

  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (u32)RESET)
  {
    frame->length = ETH_ERROR;

    if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)
    {
      /* Clear RBUS ETHERNET DMA flag */
      ETH->DMASR = ETH_DMASR_RBUS;
      /* Resume DMA reception */
      ETH->DMARPDR = 0;
    }

    /* Return error: OWN bit set */
    return;
  }

  if(((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32)RESET) &&
     ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32)RESET) &&
     ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32)RESET))
  {
    /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
    framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;

    /* Get the addrees of the actual buffer */
    frame->buffer = DMAPTPRxDescToGet->Buffer1Addr;
  }
  else
  {
    /* Return ERROR */
    framelength = ETH_ERROR;
  }

  frame->length = framelength;


  frame->descriptor = DMARxDescToGet;
  frame->PTPdescriptor = DMAPTPRxDescToGet;

  /* Update the ETHERNET DMA global Rx descriptor with next Rx decriptor */
  /* Chained Mode */
  /* Selects the next DMA Rx descriptor list for next buffer to read */
  DMARxDescToGet = (ETH_DMADESCTypeDef*) (DMAPTPRxDescToGet->Buffer2NextDescAddr);
  if(DMAPTPRxDescToGet->Status != 0)
  {
    DMAPTPRxDescToGet = (ETH_DMADESCTypeDef*) (DMAPTPRxDescToGet->Status);
  }
  else
  {
    DMAPTPRxDescToGet++;
  }

  /* Return Frame */
  return;
}


/*******************************************************************************
* Function Name  : ETH_PTPRxPkt_ChainMode_CleanUp
* Description    : Correct chain mode descriptor addresses
* Input          : frame: farme size and location
* Output         : timestamp: PTP packet timestamp
* Return         : None
*******************************************************************************/
static void ETH_PTPRxPkt_ChainMode_CleanUp(FrameTypeDef * frame, struct ptptime_t * timestamp)
{
  timestamp->tv_nsec = ETH_PTPSubSecond2NanoSecond(frame->descriptor->Buffer1Addr);
  timestamp->tv_sec = frame->descriptor->Buffer2NextDescAddr;

  frame->descriptor->Buffer1Addr = frame->PTPdescriptor->Buffer1Addr;
  frame->descriptor->Buffer2NextDescAddr = frame->PTPdescriptor->Buffer2NextDescAddr;
}




/*******************************************************************************
* Function Name  : ETH_PTPTxPkt_ChainMode
* Description    : Transmits a packet, from application buffer, pointed by ppkt.
* Input          : - FrameLength: Tx Packet size.
* Output         : None
* Return         : ETH_ERROR: in case of Tx desc owned by DMA
*                  ETH_SUCCESS: for correct transmission
*******************************************************************************/
static u32 ETH_PTPTxPkt_ChainMode(u16 FrameLength, struct ptptime_t * timestamp)
{
  unsigned int timeout = 0;

  /* Setting the Frame Length: bits[12:0] */
  DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);

  /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
  DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

  /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
  DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)
  {
    /* Clear TBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    ETH->DMATPDR = 0;
  }

  /* Wait for ETH_DMATxDesc_TTSS flag to be set */
  do
  {
    timeout++;
  } while (!(DMATxDescToSet->Status & ETH_DMATxDesc_TTSS) && (timeout < PHY_READ_TO));
  /* Return ERROR in case of timeout */
  if(timeout == PHY_READ_TO)
  {
    return ETH_ERROR;
  }

  timestamp->tv_nsec = ETH_PTPSubSecond2NanoSecond(DMATxDescToSet->Buffer1Addr);
  timestamp->tv_sec = DMATxDescToSet->Buffer2NextDescAddr;

  /* Clear the DMATxDescToSet status register TTSS flag */
  DMATxDescToSet->Status &= ~ETH_DMATxDesc_TTSS;

  DMATxDescToSet->Buffer1Addr = DMAPTPTxDescToSet->Buffer1Addr;
  DMATxDescToSet->Buffer2NextDescAddr = DMAPTPTxDescToSet->Buffer2NextDescAddr;

  /* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor */
  /* Chained Mode */
  /* Selects the next DMA Tx descriptor list for next buffer to send */
  DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);

  if(DMAPTPTxDescToSet->Status != 0)
  {
    DMAPTPTxDescToSet = (ETH_DMADESCTypeDef*) (DMAPTPTxDescToSet->Status);
  }
  else
  {
    DMAPTPTxDescToSet++;
  }

  /* Return SUCCESS */
  return ETH_SUCCESS;
}

/*******************************************************************************
* Function Name  : ETH_PTPStart
* Description    : Initialize timestamping ability of ETH
* Input          : UpdateMethod:
*                       ETH_PTP_FineUpdate   : Fine Update method
*                       ETH_PTP_CoarseUpdate : Coarse Update method 
* Output         : None
* Return         : None
*******************************************************************************/
static void ETH_PTPStart(uint32_t UpdateMethod) {
  /* Check the parameters */
  assert_param(IS_ETH_PTP_UPDATE(UpdateMethod));

  /* Mask the Time stamp trigger interrupt by setting bit 9 in the MACIMR register. */
  ETH_MACITConfig(ETH_MAC_IT_TST, DISABLE);
  /* Program Time stamp register bit 0 to enable time stamping. */
  ETH_PTPTimeStampCmd(ENABLE);

  /* Program the Subsecond increment register based on the PTP clock frequency. */
  ETH_SetPTPSubSecondIncrement(ADJ_FREQ_BASE_INCREMENT); /* to achieve 20 ns accuracy, the value is ~ 43 */

  if (UpdateMethod == ETH_PTP_FineUpdate) {

    /* If you are using the Fine correction method, program the Time stamp addend register
     * and set Time stamp control register bit 5 (addend register update). */
    ETH_SetPTPTimeStampAddend(ADJ_FREQ_BASE_ADDEND);
    ETH_EnablePTPTimeStampAddend();
    /* Poll the Time stamp control register until bit 5 is cleared. */
    while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSARU) == SET);
  }

  /* To select the Fine correction method (if required),
   * program Time stamp control register  bit 1. */
  ETH_PTPUpdateMethodConfig(UpdateMethod);
  /* Program the Time stamp high update and Time stamp low update registers
   * with the appropriate time value. */
  ETH_SetPTPTimeStampUpdate(ETH_PTP_PositiveTime, 0, 0);
  /* Set Time stamp control register bit 2 (Time stamp init). */
  ETH_InitializePTPTimeStamp();
  /* The Time stamp counter starts operation as soon as it is initialized
   * with the value written in the Time stamp update register. */

  /* Enable the MAC receiver and transmitter for proper time stamping. ETH_Start(); */
}

#endif /* LWIP_PTP */

/*******************************************************************************
* Function Name  : ETH_PTPTimeStampAdjFreq
* Description    : Updates time stamp addend register
* Input          : Correction value in thousandth of ppm (Adj*10^9)
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_PTPTime_AdjFreq(int32_t Adj)
{
    uint32_t addend;

    
    /* calculate the rate by which you want to speed up or slow down the system time
       increments */
   
    /* precise */
    /*
    int64_t addend;
    addend = Adj;
    addend *= ADJ_FREQ_BASE_ADDEND;
    addend /= 1000000000-Adj;
    addend += ADJ_FREQ_BASE_ADDEND;
    */

    /* 32bit estimation
    ADJ_LIMIT = ((1l<<63)/275/ADJ_FREQ_BASE_ADDEND) = 11258181 = 11 258 ppm*/
    if( Adj > 5120000) Adj = 5120000;
    if( Adj < -5120000) Adj = -5120000;

    addend = ((((275LL * Adj)>>8) * (ADJ_FREQ_BASE_ADDEND>>24))>>6) + ADJ_FREQ_BASE_ADDEND;
    
    /* Reprogram the Time stamp addend register with new Rate value and set ETH_TPTSCR */
    ETH_SetPTPTimeStampAddend((uint32_t)addend);
    ETH_EnablePTPTimeStampAddend();
}

/*******************************************************************************
* Function Name  : ETH_PTPTimeStampUpdateOffset
* Description    : Updates time base offset
* Input          : Time offset with sign
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_PTPTime_UpdateOffset(struct ptptime_t * timeoffset)
{
    uint32_t Sign;
    uint32_t SecondValue;
    uint32_t NanoSecondValue;
    uint32_t SubSecondValue;
    uint32_t addend;

    /* determine sign and correct Second and Nanosecond values */
    if(timeoffset->tv_sec < 0 || (timeoffset->tv_sec == 0 && timeoffset->tv_nsec < 0)) {
        Sign = ETH_PTP_NegativeTime;
        SecondValue = -timeoffset->tv_sec;
        NanoSecondValue = -timeoffset->tv_nsec;
    } else {
        Sign = ETH_PTP_PositiveTime;
        SecondValue = timeoffset->tv_sec;
        NanoSecondValue = timeoffset->tv_nsec;
    }

    /* convert nanosecond to subseconds */
    SubSecondValue = ETH_PTPNanoSecond2SubSecond(NanoSecondValue);
 
    /* read old addend register value*/
    addend = ETH_GetPTPRegister(ETH_PTPTSAR);

    while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSSTU) == SET);
    while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSSTI) == SET);

    /* Write the offset (positive or negative) in the Time stamp update high and low registers. */
    ETH_SetPTPTimeStampUpdate(Sign, SecondValue, SubSecondValue);
    /* Set bit 3 (TSSTU) in the Time stamp control register. */
    ETH_EnablePTPTimeStampUpdate();
    /* The value in the Time stamp update registers is added to or subtracted from the system */
    /* time when the TSSTU bit is cleared. */
    while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSSTU) == SET);      

    /* write back old addend register value */
    ETH_SetPTPTimeStampAddend(addend);
    ETH_EnablePTPTimeStampAddend();
}

/*******************************************************************************
* Function Name  : ETH_PTPTimeStampSetTime
* Description    : Initialize time base
* Input          : Time with sign
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_PTPTime_SetTime(struct ptptime_t * timestamp)
{
    uint32_t Sign;
    uint32_t SecondValue;
    uint32_t NanoSecondValue;
    uint32_t SubSecondValue;

    /* determine sign and correct Second and Nanosecond values */
    if(timestamp->tv_sec < 0 || (timestamp->tv_sec == 0 && timestamp->tv_nsec < 0)) {
        Sign = ETH_PTP_NegativeTime;
        SecondValue = -timestamp->tv_sec;
        NanoSecondValue = -timestamp->tv_nsec;
    } else {
        Sign = ETH_PTP_PositiveTime;
        SecondValue = timestamp->tv_sec;
        NanoSecondValue = timestamp->tv_nsec;
    }

    /* convert nanosecond to subseconds */
    SubSecondValue = ETH_PTPNanoSecond2SubSecond(NanoSecondValue);
 
    /* Write the offset (positive or negative) in the Time stamp update high and low registers. */
    ETH_SetPTPTimeStampUpdate(Sign, SecondValue, SubSecondValue);
    /* Set Time stamp control register bit 2 (Time stamp init). */
    ETH_InitializePTPTimeStamp();
    /* The Time stamp counter starts operation as soon as it is initialized
     * with the value written in the Time stamp update register. */
    while(ETH_GetPTPFlagStatus(ETH_PTP_FLAG_TSSTI) == SET);
}

#if SYS_LIGHTWEIGHT_PROT
/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.
*/
sys_prot_t sys_arch_protect(void)
{
    sys_prot_t old_val = ETH->DMAIER;
    ETH->DMAIER = 0x00000000; // disable ethernet interrupts
	return old_val;
}

/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void sys_arch_unprotect(sys_prot_t pval)
{ 
	//( void ) pval;
    ETH->DMAIER = pval;
}
#endif


