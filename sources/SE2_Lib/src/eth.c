#include <eth.h>
#include <time_m3.h>

EMAC_Descriptor *rxdescriptor, *txdescriptor;
EMAC_RxStatus *rxstatus;
EMAC_TxStatus *txstatus;
EMAC_Buffer *rxbuf, *txbuf;


unsigned short ETH_ReadPHY (unsigned char reg){
	unsigned int tout = PHY_REG_TOUT;
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


void ETH_Init(void){
uint8_t i;

	LPC_SC->PCONP |= ETH_ON;
	ETH_ConfigPins();

	//Reset everything
	LPC_EMAC->MAC1 =  MAC1_RESET_TX | MAC1_RESET_MCS_TX	| MAC1_RESET_RX | MAC1_RESET_MCS_RX | MAC1_RESET_SIM | MAC1_RESET_SOFT;
	LPC_EMAC->Command = CMD_RESET_TX | CMD_RESET_RX | CMD_RESET_REG | CMD_PASSRUNFRAME;
	LPC_EMAC->MCFG = MCFG_RESET_MII;
	TIME_DelayMs(5);

	LPC_EMAC->MAC1 = MAC1_PASS_ALL;
	LPC_EMAC->MCFG = MCFG_CK64;		//MII Clock = CCLK/64

	LPC_EMAC->Command = CMD_RMII;   //Enable MII

	//Configure PHY
	ETH_WritePHY(PHY_CR,PHY_RESET);
	TIME_DelayMs(10);

	ETH_WritePHY(PHY_CR, PHY_10Mbit); //configure PHY to 10Mbit full duplex

	//	ETH_WritePHY(PHY_CR, PHY_10Mbit); //auto negotiate

	LPC_EMAC->MAC2 |= MAC2_FULLDUPLEX;
	LPC_EMAC->Command |= CMD_FULLDUPLEX;
	LPC_EMAC->SUPP = 0;                  //Config 10Mbit

	LPC_EMAC->SA2 = ETH_MAC >> 32;		 //Config MAC address
	LPC_EMAC->SA1 = ETH_MAC >> 16;
	LPC_EMAC->SA0 = ETH_MAC & 0xFFFF;

	rxdescriptor = (EMAC_Descriptor*)RX_DESC_BASE;
	rxstatus     = (EMAC_RxStatus*)RX_STAT_BASE;
	txdescriptor = (EMAC_Descriptor*)TX_DESC_BASE;
	txstatus     = (EMAC_TxStatus*)TX_STAT_BASE;
	rxbuf 		 = (EMAC_Buffer*)RX_BUF_BASE;
	txbuf		 = (EMAC_Buffer*)TX_BUF_BASE;

	for(i = 0; i < NUM_RX_FRAG; i++){
		rxdescriptor[i].packet = &rxbuf[i];
		rxdescriptor[i].control = 0;
		rxstatus[i].info = 0;
		rxstatus[i].crc = 0;
	}

	for(i = 0; i < NUM_TX_FRAG; i++){
		txdescriptor[i].packet  = &txbuf[i];
		txdescriptor[i].control = (1<<31) | (1<<30) | (1<<29) | (1<<28) | (1<<26) | (ETH_FRAG_SIZE - 1);
		txstatus[i].info = 0;
	}

	LPC_EMAC->RxDescriptor = RX_DESC_BASE;
	LPC_EMAC->RxStatus = RX_STAT_BASE;
	LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG - 1;
	LPC_EMAC->RxConsumeIndex = 0;

	LPC_EMAC->TxDescriptor = TX_DESC_BASE;
	LPC_EMAC->TxStatus = TX_STAT_BASE;
	LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG - 1;
	LPC_EMAC->TxProduceIndex = 0;

	LPC_EMAC->RxFilterCtrl = RFC_BROADCAST | RFC_PERFECT;

	//LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;
	LPC_EMAC->IntClear  = 0xFFFF;

	LPC_EMAC->Command |= CMD_RX_EN | CMD_RX_EN;
	LPC_EMAC->MAC1 |= MAC1_RCV_EN;

}

uint32_t ETH_GetPHY_ID(void){
uint32_t id;
	id = ETH_ReadPHY(PHY_ID1);
	id <<=16;
	id |= ETH_ReadPHY(PHY_ID2);
	return id;
}

uint32_t EMAC_Read(void *packet)
{
	uint32_t Index = LPC_EMAC->RxConsumeIndex;
	uint32_t size;

  if(Index == LPC_EMAC->RxProduceIndex) return(0);

  size = (RX_STAT_INFO(Index) & 0x7ff)+1;
  if (size > ETH_FRAG_SIZE)
  	size = ETH_FRAG_SIZE;

  memcpy(packet,(unsigned int *)RX_BUF(Index),size);

  if(++Index > LPC_EMAC->RxDescriptorNumber)
  {
    Index = 0;
  }
  LPC_EMAC->RxConsumeIndex = Index;

  return size;
}


uint8_t ETH_Send(void *packet, uint32_t size)
{
	uint32_t Index;
	uint32_t IndexNext = LPC_EMAC->TxProduceIndex + 1;

  if(size == 0)return 1;

  if(IndexNext > LPC_EMAC->TxDescriptorNumber) {
    IndexNext = 0;
  }

  if(IndexNext == LPC_EMAC->TxConsumeIndex){
    return 0;
  }

  Index = LPC_EMAC->TxProduceIndex;
  if (size > ETH_FRAG_SIZE)
  	size = ETH_FRAG_SIZE;

  //memcpy((unsigned int *)TX_BUF(Index),packet,size);
  //TX_DESC_CTRL(Index) &= ~0x7ff;
  //TX_DESC_CTRL(Index) |= (size - 1) & 0x7ff;

  LPC_EMAC->TxProduceIndex = IndexNext;

  return 1;
}
