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
//MAC1 Register bits
#define MAC1_RCV_EN     	(1<<1)
#define MAC1_PASS_ALL		(1<<1)
#define MAC1_RESET_TX 		(1<<8)
#define MAC1_RESET_MCS_TX	(1<<9)
#define MAC1_RESET_RX 		(1<<10)
#define MAC1_RESET_MCS_RX	(1<<11)
#define MAC1_RESET_SIM		(1<<14)
#define MAC1_RESET_SOFT		(1<<15)

#define MAC2_FULLDUPLEX		(1<<0)
#define MAC2_CRC_EN         (1<<4)
#define MAC2_PAD_EN         (1<<5)

//Command Register bits
#define CMD_RX_EN 			(1<<0)
#define CMD_TX_EN			(1<<1)
#define CMD_RESET_REG		(1<<3)
#define CMD_RESET_TX		(1<<4)
#define CMD_RESET_RX		(1<<5)
#define CMD_PASS_RUNT_FRM   (1<<6)
#define CMD_PASS_RX_FILT    (1<<7)
#define CMD_RMII			(1<<9)
#define CMD_FULLDUPLEX		(1<<10)

//MCFG Register bits
#define MCFG_SCAN			(1<<0)
#define MCFG_RESET_MII		(1<<15)
#define MCFG_CK64			(0xF<<2)

//Receive Filter Control bits
#define RFC_BROADCAST 	(1<<1)
#define RFC_PERFECT     (1<<5)

//Interrupt Eanble Register
#define INT_RX_DONE (1<<3)
#define INT_TX_DONE (1<<7)

/* Collision Window/Retry Register */
#define CLRT_DEF            0x0000370F  /* Default value                     */
/* Non Back-to-Back Inter-Packet-Gap Register */
#define IPGR_DEF            0x00000012  /* Recommended value                 */
/* PHY Support Register */
#define SUPP_SPEED          0x00000100  /* Reduced MII Logic Current Speed   */
/* Back-to-Back Inter-Packet-Gap Register */
#define IPGT_FULL_DUP       0x00000015  /* Recommended value for Full Duplex */
#define IPGT_HALF_DUP       0x00000012  /* Recommended value for Half Duplex */
#define RCTRL_SIZE          0x000007FF  /* Buffer size mask                  */
#define RCTRL_INT           0x80000000  /* Generate RxDone Interrupt         */

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

typedef struct _Desc{
	void *packet;
	uint32_t control;
}EMAC_Descriptor;

typedef struct _RxStatus{
	uint32_t info;
	uint32_t crc;
}EMAC_RxStatus;

typedef struct _TxStatus{
	uint32_t info;
}EMAC_TxStatus;

typedef struct _ETHBuf{
	uint8_t data[ETH_FRAG_SIZE];
}EMAC_Buffer;

/* Serial Management Interface Registers*/
#define LAN8720_ID  0x0007C0F0
#define PHY_ADR     0x100
#define MCMD_READ   1
#define MIND_BUSY   1

#define PHY_CR		0	//Control Register
#define PHY_SR		1	//Status Register
#define PHY_ID1		2	//PHY ID1 Register
#define PHY_ID2		3   //PHY ID2 Register
#define PHY_10Mbit   //10Mbit Full duplex
#define PHY_AUTO_NEGOTIATE 0x3000
//Status register bits
#define PHY_LINK         (1<<2)
#define PHY_AN_COMPLETED (1<<5)
#define PHY_10FD		 (1<<12)
#define PHY_100FD		 (1<<14)

/* Serial Management Interface bits*/
#define PHY_RESET	0x8000

#define PHY_REG_TOUT 100

/**
* @brief Faz a iniciação do controlador de Ethernet, incluindo a camada PHY
*
*/
void ETH_Init();

/**
* @brief send packet through ethernet
* @return send result
*/
uint8_t ETH_Send(void *packet, uint32_t size);

/**
* @brief get packet through ethernet
* @return size of packet
*/
uint32_t ETH_Read(void *packet);

/**
* @brief get PHY ID
* @return id
*/
uint32_t ETH_GetPHY_ID(void);

#endif
