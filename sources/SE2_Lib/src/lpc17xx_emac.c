#if 0
/*
 *  CodeRed - minor modifications for port to RDB1768 development board
 */
#include <string.h>
#include "lpc17xx_emac.h"

//#include "uip-conf.h"
//#include "uipopt.h"

/* The following macro definitions may be used to select the speed
   of the physical link:

  _10MBIT_   - connect at 10 MBit only
  _100MBIT_  - connect at 100 MBit only

  By default an autonegotiation of the link speed is used. This may take
  longer to connect, but it works for 10MBit and 100MBit physical links.     */

/* Local Function Prototypes */
static void   rx_descr_init(void);
static void   tx_descr_init(void);
static void write_PHY (UNS_32 PhyReg, UNS_32 Value);
static UNS_16 read_PHY (UNS_8 PhyReg) ;



/*************************************************
 * CodeRed - PHY definitions for RDB1768 rev 2
 * which uses SMSC LAN8720 PHY instead of DP83848C
 *************************************************/
#define LAN8720_ID          0x0007C0F0  /* PHY Identifier                    */



/*--------------------------- EMAC_Init ---------------------------------*/

BOOL_32 ETH_InitPHY(void){
	 /* Put the PHY in reset mode */
	  write_PHY (PHY_REG_BMCR, 0x8000);
		for (tout = 1000; tout; tout--);

	  /* Wait for hardware reset to end. */
	  for (tout = 0; tout < 0x100000; tout++) {
	  	regv = read_PHY (PHY_REG_BMCR);
	#if	defined (KEIL_BOARD_MCB17XX) || defined (CODERED_BOARD_RDB1768)
	    if (!(regv & 0x8000))
	#else
		#error "No board!"
	#endif
		{
	      /* Reset complete */
	      break;
	    }
	  }
	  if (tout >= 0x100000)
	  	return FALSE; /* reset failed */

	  /* Check if this is a DP83848C PHY. */
	  id1 = read_PHY (PHY_REG_IDR1);
	  id2 = read_PHY (PHY_REG_IDR2);
	  phy_id = ((id1 << 16) | (id2 & 0xFFF0));

	#if	defined (KEIL_BOARD_MCB17XX) || defined (CODERED_BOARD_RDB1768)
		  if ((phy_id != DP83848C_ID) && (phy_id != LAN8720_ID))
	#else
		#error "No board"
	#endif
	  	return FALSE;

		/* Configure the PHY device */
	      /* Configure the PHY device */
	#if defined (_10MBIT_)
	      /* Connect at 10MBit */
	      write_PHY (PHY_REG_BMCR, PHY_FULLD_10M);
	#elif defined (_100MBIT_)
	      /* Connect at 100MBit */
	      write_PHY (PHY_REG_BMCR, PHY_FULLD_100M);
	#else
	      /* Use autonegotiation about the link speed. */
	      write_PHY (PHY_REG_BMCR, PHY_AUTO_NEG);
	      /* Wait to complete Auto_Negotiation. */
	      for (tout = 0; tout < 0x100000; tout++) {
	         regv = read_PHY (PHY_REG_BMSR);
	         if (regv & 0x0020) {
	            /* Autonegotiation Complete. */
	            break;
	         }
	      }

		if (tout >= 0x100000)
			return FALSE; // auto_neg failed
	#endif

		  phy_linkstatus_reg = PHY_REG_STS;		// Default to DP83848C
		  phy_linkstatus_mask = 0x0001;

		  if (phy_id == LAN8720_ID) {
			  phy_linkstatus_reg = PHY_REG_BMSR;
			  phy_linkstatus_mask = 0x0004;
		  }

	  /* Check the link status. */
	  for (tout = 0; tout < 0x100000; tout++) {
	#if	defined (KEIL_BOARD_MCB17XX) || defined (CODERED_BOARD_RDB1768)
	    regv = read_PHY (phy_linkstatus_reg);
	    if (regv & phy_linkstatus_mask)
	#else
		#error "No board"
	#endif
		{
	      /* Link is on. */
	      break;
	    }
	  }

	  if (tout >= 0x100000)
	  	return FALSE;

	  /* Configure Full/Half Duplex mode. */
	#if	defined (KEIL_BOARD_MCB17XX) || defined (CODERED_BOARD_RDB1768)
	  if (regv & 0x0004)
	#else
		#error "No board"
	#endif
		{
	    /* Full duplex is enabled. */
		  LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
		  LPC_EMAC->Command |= CR_FULL_DUP;
		  LPC_EMAC->IPGT     = IPGT_FULL_DUP;
	  }
	  else {
	    /* Half duplex mode. */
		  LPC_EMAC->IPGT = IPGT_HALF_DUP;
	  }

	  /* Configure 100MBit/10MBit mode. */
	#if	defined (KEIL_BOARD_MCB17XX) || defined (CODERED_BOARD_RDB1768)
	  if (regv & 0x0002) {
	#else
		#error "No baord"
	#endif
	    /* 10MBit mode. */
		  LPC_EMAC->SUPP = 0;
	  }
	  else {
	    /* 100MBit mode. */
		  LPC_EMAC->SUPP = SUPP_SPEED;
	  }
	return TRUE;
}

BOOL_32 ETH_Init(void)
{
   /* Initialize the EMAC ethernet controller. */
  UNS_32 regv, id1,id2;
  volatile unsigned int tout;
  unsigned phy_id;
  unsigned phy_linkstatus_reg;
  unsigned phy_linkstatus_mask;

   /* Power Up the EMAC controller. */
   LPC_SC->PCONP |= 0x40000000;

   /* Enable P1 Ethernet Pins. */
   LPC_PINCON->PINSEL2 = 0x50150105;
   LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~0x0000000F) | 0x00000005;

  /* Reset all EMAC internal modules. */
   LPC_EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | MAC1_RES_MCS_RX |
             MAC1_SIM_RES | MAC1_SOFT_RES;
   LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES | CR_PASS_RUNT_FRM;

  /* A short delay after reset. */
  for (tout = 100; tout; tout--);

  /* Initialize MAC control registers. */
  LPC_EMAC->MAC1 = MAC1_PASS_ALL;
  LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;
  LPC_EMAC->MAXF = ETH_MAX_FLEN;
  LPC_EMAC->CLRT = CLRT_DEF;
  LPC_EMAC->IPGR = IPGR_DEF;


     /* Enable Reduced MII interface. */
    LPC_EMAC->MCFG = MCFG_CLK_DIV64 | MCFG_RES_MII;
     for (tout = 100; tout; tout--);
     LPC_EMAC->MCFG = MCFG_CLK_DIV64;

  /* Enable Reduced MII interface. */
   LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM | CR_PASS_RX_FILT;

  /* Reset Reduced MII Logic. */
   LPC_EMAC->SUPP = SUPP_RES_RMII | SUPP_SPEED;
   for (tout = 100; tout; tout--);
   LPC_EMAC->SUPP = SUPP_SPEED;

   if(ETH_InitPHY())
	   return FALSE;

  /* Set the Ethernet MAC Address registers */
//  MAC_SA0 = (MYMAC_6 << 8) | MYMAC_5;
//  MAC_SA1 = (MYMAC_4 << 8) | MYMAC_3;
//  MAC_SA2 = (MYMAC_2 << 8) | MYMAC_1;
  LPC_EMAC->SA0 = (6<<8) | 5;
  LPC_EMAC->SA1 = (4<<8) | 3;
  LPC_EMAC->SA2 = (2<<8) | 1;

  /* Initialize Tx and Rx DMA Descriptors */
  rx_descr_init ();
  tx_descr_init ();

  /* Receive Broadcast and Perfect Match Packets */
  LPC_EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;

  /* Enable EMAC interrupts. */
  LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;

  /* Reset all interrupts */
  LPC_EMAC->IntClear  = 0xFFFF;

  /* Enable receive and transmit mode of MAC Ethernet core */
  LPC_EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
  LPC_EMAC->MAC1     |= MAC1_REC_EN;

   /* Configure VIC for EMAC interrupt. */
   //VICVectAddrxx = (UNS_32)xx;

  return TRUE;
}

/*--------------------------- write_PHY -------------------------------------*/

static void write_PHY (UNS_32 PhyReg, UNS_32 Value)
{
  unsigned int tout;

  LPC_EMAC->MADR = DP83848C_DEF_ADR | PhyReg;
  LPC_EMAC->MWTD = Value;

  /* Wait utill operation completed */
  tout = 0;
  for (tout = 0; tout < MII_WR_TOUT; tout++) {
    if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
      break;
    }
  }
}

/*--------------------------- read_PHY -------------------------------------*/

static UNS_16 read_PHY (UNS_8 PhyReg)
{
  UNS_32 tout;

  LPC_EMAC->MADR = DP83848C_DEF_ADR | PhyReg;
  LPC_EMAC->MCMD = MCMD_READ;

  /* Wait until operation completed */
  tout = 0;
  for (tout = 0; tout < MII_RD_TOUT; tout++) {
    if ((LPC_EMAC->MIND & MIND_BUSY) == 0) {
      break;
    }
  }
  LPC_EMAC->MCMD = 0;
  return (LPC_EMAC->MRDD);
}

/*--------------------------- EMAC_ReadPacket ---------------------------------*/

UNS_32 ETH_Read(void * pPacket)
{
	UNS_32 Index = LPC_EMAC->RxConsumeIndex;
	UNS_32 size;

  if(Index == LPC_EMAC->RxProduceIndex)
  {
    return(0);
  }

  size = (RX_STAT_INFO(Index) & 0x7ff)+1;
  if (size > ETH_FRAG_SIZE)
  	size = ETH_FRAG_SIZE;

  memcpy(pPacket,(unsigned int *)RX_BUF(Index),size);

  if(++Index > LPC_EMAC->RxDescriptorNumber)
  {
    Index = 0;
  }
  LPC_EMAC->RxConsumeIndex = Index;

  return(size);
}

/*--------------------------- EMAC_SendPacket ---------------------------------*/

BOOL_32 ETH_Send(void *pPacket, UNS_32 size)
{
	UNS_32 	Index;
	UNS_32	IndexNext = LPC_EMAC->TxProduceIndex + 1;

  if(size == 0)
  {
    return(TRUE);
  }
  if(IndexNext > LPC_EMAC->TxDescriptorNumber)
  {
    IndexNext = 0;
  }

  if(IndexNext == LPC_EMAC->TxConsumeIndex)
  {
    return(FALSE);
  }
  Index = LPC_EMAC->TxProduceIndex;
  if (size > ETH_FRAG_SIZE)
  	size = ETH_FRAG_SIZE;

  memcpy((unsigned int *)TX_BUF(Index),pPacket,size);
  TX_DESC_CTRL(Index) &= ~0x7ff;
  TX_DESC_CTRL(Index) |= (size - 1) & 0x7ff;

  LPC_EMAC->TxProduceIndex = IndexNext;

  return(TRUE);
}

/*--------------------------- rx_descr_init ---------------------------------*/

static void rx_descr_init (void)
{
  UNS_32 i;

  for (i = 0; i < NUM_RX_FRAG; i++) {
    RX_DESC_PACKET(i)  = RX_BUF(i);
    RX_DESC_CTRL(i)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
    RX_STAT_INFO(i)    = 0;
    RX_STAT_HASHCRC(i) = 0;
  }

  /* Set EMAC Receive Descriptor Registers. */
  LPC_EMAC->RxDescriptor    = RX_DESC_BASE;
  LPC_EMAC->RxStatus        = RX_STAT_BASE;
  LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

  /* Rx Descriptors Point to 0 */
  LPC_EMAC->RxConsumeIndex  = 0;
}


/*--------------------------- tx_descr_init ---------------------------------*/

static void tx_descr_init (void) {
  UNS_32 i;

  for (i = 0; i < NUM_TX_FRAG; i++) {
    TX_DESC_PACKET(i) = TX_BUF(i);
    TX_DESC_CTRL(i)   = (1<<31) | (1<<30) | (1<<29) | (1<<28) | (1<<26) | (ETH_FRAG_SIZE-1);
    TX_STAT_INFO(i)   = 0;
  }

  /* Set EMAC Transmit Descriptor Registers. */
  LPC_EMAC->TxDescriptor    = TX_DESC_BASE;
  LPC_EMAC->TxStatus        = TX_STAT_BASE;
  LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

  /* Tx Descriptors Point to 0 */
  LPC_EMAC->TxProduceIndex  = 0;
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
#else
/*
 *  CodeRed - minor modifications for port to RDB1768 development board
 */
#include <string.h>
#include "lpc17xx_emac.h"
#include <eth.h>
#include <util.h>

/*************************************************
 * CodeRed - PHY definitions for RDB1768 rev 2
 * which uses SMSC LAN8720 PHY instead of DP83848C
 *************************************************/
#define LAN8720_ID          0x0007C0F0  /* PHY Identifier */

/*--------------------------- EMAC_ReadPacket ---------------------------------*/

uint32_t ETH_Read(void * pPacket)
{
	UNS_32 Index = LPC_EMAC->RxConsumeIndex;
	UNS_32 size;

  if(Index == LPC_EMAC->RxProduceIndex)
  {
    return(0);
  }

  size = (RX_STAT_INFO(Index) & 0x7ff)+1; 
  if (size > ETH_FRAG_SIZE)
  	size = ETH_FRAG_SIZE;
  	
  memcpy(pPacket,(unsigned int *)RX_BUF(Index),size);

  if(++Index > LPC_EMAC->RxDescriptorNumber)
  {
    Index = 0;
  }
  LPC_EMAC->RxConsumeIndex = Index;
  
  return(size);
}

/*--------------------------- EMAC_SendPacket ---------------------------------*/

uint8_t ETH_Send(void *pPacket, UNS_32 size)
{
	UNS_32 	Index;
	UNS_32	IndexNext = LPC_EMAC->TxProduceIndex + 1;
	
  if(size == 0)
  {
    return(TRUE);
  }
  if(IndexNext > LPC_EMAC->TxDescriptorNumber)
  {
    IndexNext = 0;
  }
 
  if(IndexNext == LPC_EMAC->TxConsumeIndex)
  {
    return(FALSE);
  }
  Index = LPC_EMAC->TxProduceIndex;
  if (size > ETH_FRAG_SIZE)
  	size = ETH_FRAG_SIZE;

  memcpy((unsigned int *)TX_BUF(Index),pPacket,size);  
  TX_DESC_CTRL(Index) &= ~0x7ff;
  TX_DESC_CTRL(Index) |= (size - 1) & 0x7ff; 

  LPC_EMAC->TxProduceIndex = IndexNext;

  return(TRUE);
}

/*--------------------------- PHY -------------------------------------*/
uint16_t ETH_ReadPHY (unsigned char reg){
uint16_t tout = PHY_REG_TOUT;
	// Set up address to access in MII Mgmt Address Register
	LPC_EMAC->MADR = PHY_ADR | reg;
	// Trigger a PHY read via MII Mgmt Command Register
	LPC_EMAC->MCMD = MCMD_READ;
	// Loop read from PHY completes

	while(tout--){
		if((LPC_EMAC->MIND & MIND_BUSY) == 0)
			break;
		TIME_DelayMs(5);
	}

	LPC_EMAC->MCMD = 0; // Cancel read
	// Returned value is in MII Mgmt Read Data Register
	return (LPC_EMAC->MRDD);
}

void ETH_WritePHY (int reg, int writeval){
	unsigned int tout = PHY_REG_TOUT;
	// Set up address to access in MII Mgmt Address Register
	LPC_EMAC->MADR = PHY_ADR | reg;
	// Write value into MII Mgmt Write Data Register
	LPC_EMAC->MWTD = writeval;
	// Loop write to PHY completes
	while(tout--){
		if((LPC_EMAC->MIND & MIND_BUSY) == 0)
			break;
		TIME_DelayMs(5);
	}
}
uint32_t ETH_GetPHY_ID(){
	  uint16_t id1,id2;
	  id1 = ETH_ReadPHY (PHY_REG_IDR1);
	  id2 = ETH_ReadPHY (PHY_REG_IDR2);
	  return ((id1 << 16) | (id2 & 0xFFF0));
  }

/*--------------------------- rx_descr_init ---------------------------------*/

static void ETH_InitDescriptors(void)
{
  UNS_32 i;

  for (i = 0; i < NUM_RX_FRAG; i++) {
    RX_DESC_PACKET(i)  = RX_BUF(i);
    RX_DESC_CTRL(i)    = RCTRL_INT | (ETH_FRAG_SIZE-1);
    RX_STAT_INFO(i)    = 0;
    RX_STAT_HASHCRC(i) = 0;
  }

  /* Set EMAC Receive Descriptor Registers. */
  LPC_EMAC->RxDescriptor    = RX_DESC_BASE;
  LPC_EMAC->RxStatus        = RX_STAT_BASE;
  LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG-1;

  /* Rx Descriptors Point to 0 */
  LPC_EMAC->RxConsumeIndex  = 0;

  for (i = 0; i < NUM_TX_FRAG; i++) {
      TX_DESC_PACKET(i) = TX_BUF(i);
      TX_DESC_CTRL(i)   = (1<<31) | (1<<30) | (1<<29) | (1<<28) | (1<<26) | (ETH_FRAG_SIZE-1);
      TX_STAT_INFO(i)   = 0;
    }

    /* Set EMAC Transmit Descriptor Registers. */
    LPC_EMAC->TxDescriptor    = TX_DESC_BASE;
    LPC_EMAC->TxStatus        = TX_STAT_BASE;
    LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG-1;

    /* Tx Descriptors Point to 0 */
    LPC_EMAC->TxProduceIndex  = 0;
}

void ETH_InitPHY(void){
#if 0
	uint32_t loop;
	uint16_t phystatus;

	ETH_WritePHY(PHY_CR, PHY_RESET);
	TIME_DelayMs(5);

	phystatus = loop = 0;
	do{
		phystatus = ETH_ReadPHY(PHY_SR);
		loop++;
	}while((phystatus & PHY_RESET) && loop < 0x100);

	loop = 0;
	ETH_WritePHY(PHY_CR, PHY_AUTO_NEGOTIATE); //set auto negotiate

	while(!(phystatus & (PHY_AN_COMPLETED | PHY_LINK)) && loop < 50){
		phystatus = ETH_ReadPHY(PHY_SR);
		loop++;
	}

	if(phystatus & PHY_AN_COMPLETED){
		 if ( phystatus & (PHY_100FD | PHY_10FD) ) {
			  //full duplex mode
			  LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
			  LPC_EMAC->Command |= CMD_FULL_DUP;
			  LPC_EMAC->IPGT     = IPGT_FULL_DUP;
		  }
		  else {
			  //half duplex mode
			  LPC_EMAC->IPGT = IPGT_HALF_DUP;
		  }
		 // select 100/10Mbit
		 LPC_EMAC->SUPP = (phystatus & PHY_100FD) ? SUPP_SPEED : 0;
	}
#else
	uint32_t loop;
	uint16_t phystatus = 0;

	/* Reset PHY */
	ETH_WritePHY(PHY_CR, PHY_RESET);

	loop = 100;
	do{
		phystatus = ETH_ReadPHY(PHY_SR);
		loop--;
	 }while((phystatus & PHY_RESET) && loop);

	if(!loop) printf("PHY Reset Fail: %x", phystatus);

	/* Setup Auto negotiation */
	ETH_WritePHY(PHY_CR, PHY_AUTO_NEGOTIATE);

	loop = 1000;
	do{
		phystatus = ETH_ReadPHY(PHY_SR);
		loop--;
	}while(!(phystatus & PHY_AN_COMPLETED) && loop != 0);

	if(!loop) printf("Auto negotiation Fail: %x", phystatus);

	/* Check the link status. */
	loop = 1000;
	do{
		phystatus = ETH_ReadPHY(PHY_SR);
		loop--;
	}while(!(phystatus & PHY_LINK) && loop != 0);

	if(!loop) printf("No link: %x", phystatus);

	/* Configure connection mode */
	if(phystatus & PHY_LINK){
		//full duplex mode
	  	LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
	  	LPC_EMAC->Command |= CMD_FULL_DUP;
	  	LPC_EMAC->IPGT     = IPGT_FULL_DUP;
	}else{
		//No link detected configure half duplex mode
	  	LPC_EMAC->IPGT = IPGT_HALF_DUP;
	 }

	// Configure 10/100Mbit based on status
	LPC_EMAC->SUPP = (phystatus & PHY_100FD) ? SUPP_SPEED : 0;
#endif
}

/*--------------------------- EMAC_Init ---------------------------------*/
void ETH_Init(void){
	LPC_SC->PCONP |= ETH_ON;
	ETH_ConfigPins();

	//Reset everything
	LPC_EMAC->MAC1 =  MAC1_RESET_TX | MAC1_RESET_MCS_TX	| MAC1_RESET_RX | MAC1_RESET_MCS_RX | MAC1_RESET_SIM | MAC1_RESET_SOFT;
	LPC_EMAC->Command = CMD_RESET_TX | CMD_RESET_RX | CMD_RESET_REG | CMD_PASS_RUNT_FRM;

	//
	LPC_EMAC->MAC1 = MAC1_PASS_ALL;				//accept all frames on interface
	LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN; //generate CRC and padding

	LPC_EMAC->MAXF = ETH_MAX_FLEN;				//Set frame size
	LPC_EMAC->CLRT = CLRT_DEF;
	LPC_EMAC->IPGR = IPGR_DEF;

	LPC_EMAC->MCFG = MCFG_CK64 | MCFG_RESET_MII;
	TIME_DelayMs(5);
	LPC_EMAC->MCFG = MCFG_CK64;					//MII Clock, select divider for 2,5Mhx to 12Mhz

	LPC_EMAC->Command = CMD_RMII | CMD_PASS_RUNT_FRM | CMD_PASS_RX_FILT;

	ETH_InitPHY();

	ETH_InitDescriptors();

	//Configure Ethernet physical address
	LPC_EMAC->SA2 = IF_MAC >> 32;
	LPC_EMAC->SA1 = IF_MAC >> 16;
	LPC_EMAC->SA0 = IF_MAC & 0xFFFF;

	LPC_EMAC->RxFilterCtrl = RFC_BROADCAST | RFC_PERFECT; 	//Filter only good packets ans broadcast
	LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;	  	//Enable interrupts for dedicated DMA
	LPC_EMAC->IntClear  = 0xFFFF;

	LPC_EMAC->Command |= (CMD_RX_EN | CMD_TX_EN);			//enable send and receive
	LPC_EMAC->MAC1 |= MAC1_RCV_EN;							//enable frame receive
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
#endif
