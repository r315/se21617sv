/**
* @file		eth.h
* @brief	Contains the Ethernet API header.
*     		
* @version	1.0
* @date		20 Apr. 2017
* @author	Hugo Reis
**********************************************************************/

#ifndef _ETH_H_
#define _ETH_H_

#include <LPC17xx.h>


/**
 * Pinouts
 * Ficha		LPCXpresso
 *  1			TD+
 *  2			TD-
 *  3			VIO
 *  6			VIO
 *  7			RD+
 *  8			RD-
 *	GS1,GS2 	GND
 */

#define PCENET 30
#define ETH_ON (1<<PCENET)

#define ENET_TXD0	0		//P1.0
#define ENET_TXD1	2		//P1.1
#define ENET_TX_EN	8		//P1.4
#define ENET_CRS	16		//P1.8
#define ENET_RXD0 	18		//P1.9
#define ENET_RXD1 	20		//P1.10
#define ENET_RX_ER 	28		//P1.14
#define ENET_REF_CLK 30		//P1.15
#define ENET_MDC	0		//P1.16
#define ENET_MDIO	2		//P1.17

#define ETH_ConfigPins()                                                                              \
	LPC_PINCON->PINSEL2 &= 	~((3<<ENET_TXD0) | (3<<ENET_TXD1) | (3<<ENET_TX_EN) | (3<<ENET_CRS)	      \
							| (3<<ENET_RXD0) | (3<<ENET_RXD1) | (3<<ENET_RX_ER) | (3<<ENET_REF_CLK)); \
	LPC_PINCON->PINSEL3 &=  ~((3<<ENET_MDC) | (3<<ENET_MDIO));                                        \
	LPC_PINCON->PINSEL2 |= 	((1<<ENET_TXD0) | (1<<ENET_TXD1) | (1<<ENET_TX_EN) | (1<<ENET_CRS)	      \
							| (1<<ENET_RXD0) | (1<<ENET_RXD1) | (1<<ENET_RX_ER) | (1<<ENET_REF_CLK)); \
	LPC_PINCON->PINSEL3 |=  ((1<<ENET_MDC) | (1<<ENET_MDIO));

#define ETH_MAC 0x038652201704
#define ETH_RX_ALL_FRAMES (1<<1)

/* EMAC Memory Buffer configuration for 16K Ethernet RAM. */
#define NUM_RX_FRAG 4			/* Num.of RX Fragments 4*1536= 6.0kB */
#define NUM_TX_FRAG 2			/* Num.of TX Fragments 3*1536= 4.6kB */
#define ETH_FRAG_SIZE 1536		/* Packet Fragment size 1536 Bytes */

#define ETH_MAX_FLEN 1536		/* Max. Ethernet Frame Size */
/* EMAC variables located in AHB SRAM bank 1 */
#define RX_DESC_BASE 	0x2007C000						//RamAHB32
#define RX_STAT_BASE	(RX_DESC_BASE + NUM_RX_FRAG*8)
#define TX_DESC_BASE	(RX_STAT_BASE + NUM_RX_FRAG*8)
#define TX_STAT_BASE	(TX_DESC_BASE + NUM_TX_FRAG*8)
#define RX_BUF_BASE		(TX_STAT_BASE + NUM_TX_FRAG*4)
#define TX_BUF_BASE		(RX_BUF_BASE + NUM_RX_FRAG*ETH_FRAG_SIZE)


/* Serial Management Interface */
#define PHY_DEF_ADR 1
#define MCMD_READ   1
#define MIND_BUSY   1
#define SMI_CR		0	//Control Register
#define SMI_SR		1	//Status Register
#define SMI_ID1		2	//PHY ID1
#define SMI_ID2		3   //PHY ID2

#define SMI_RESET	0x8000



/**
* @brief Faz a iniciação do controlador de Ethernet, incluindo a camada PHY
*
*/
void ETH_Init();

/**
* @brief
*/
void ETH_Send();

uint16_t ETH_GetPHY_ID(void);

#endif
