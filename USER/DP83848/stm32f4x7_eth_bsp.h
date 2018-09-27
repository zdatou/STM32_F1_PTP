#ifndef __STM32F4x7_ETH_BSP_H
#define __STM32F4x7_ETH_BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f10x.h"
#include "lwip/netif.h"
#include "stm32_eth.h"
	
	 
static void ETH_GPIO_Config(void);
static void ETH_NVIC_Config(void);
static void ETH_MACDMA_Config(void);
static void TIM_Configuration(void);
	 
void  ETH_BSP_Config(void);
uint32_t Eth_Link_PHYITConfig(uint16_t PHYAddress);
void Eth_Link_EXTIConfig(void);
void Eth_Link_ITHandler(uint16_t PHYAddress);
void ETH_link_callback(struct netif *netif);

	 
/* Specific defines for EXTI line, used to manage Ethernet link status */
#define ETH_LINK_EXTI_LINE             EXTI_Line4
#define ETH_LINK_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOA
#define ETH_LINK_EXTI_PIN_SOURCE       GPIO_PinSource4
#define ETH_LINK_EXTI_IRQn             EXTI4_IRQn 
/* PB14 */
#define ETH_LINK_PIN                   GPIO_Pin_4
#define ETH_LINK_GPIO_PORT             GPIOA
#define ETH_LINK_GPIO_CLK              RCC_APB2Periph_GPIOA

/* Ethernet Flags for EthStatus variable */   
#define ETH_INIT_FLAG           0x01 /* Ethernet Init Flag */
#define ETH_LINK_FLAG           0x10 /* Ethernet Link Flag */

#ifdef __cplusplus
}
#endif

#endif  /* __STM32F4x7_ETH_BSP_H */



