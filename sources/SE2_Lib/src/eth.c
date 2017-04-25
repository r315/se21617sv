#include <eth.h>
#include <time_m3.h>

EMAC_Descriptor *rxdescriptor, *txdescriptor;
EMAC_RxStatus *rxstatus;
EMAC_TxStatus *txstatus;
EMAC_Buffer *rxbuf, *txbuf;

uint8_t ETH_HasFrame(void){
	return LPC_EMAC->RxProduceIndex != LPC_EMAC->RxConsumeIndex;
}

uint32_t ETH_Read(void *packet){
	uint32_t size, Index = LPC_EMAC->RxConsumeIndex;

	if(Index == LPC_EMAC->RxProduceIndex)
		return(0); 										// no new data

	size = (rxstatus[Index].info & 0x7FF) + 1;			//get packet size from status
	if (size > ETH_FRAG_SIZE)size = ETH_FRAG_SIZE;		//clip size if bigger than fragment

	memcpy(packet,(unsigned int *)&rxbuf[Index],size);	//copy packet data

	if(++Index > LPC_EMAC->RxDescriptorNumber)			//point to next descriptor
		Index = 0;

	LPC_EMAC->RxConsumeIndex = Index;

	return size;
}


uint8_t ETH_Send(void *packet, uint32_t size){
	uint32_t Index, IndexNext = LPC_EMAC->TxProduceIndex + 1;

	if(size == 0)
		return 1;										//no data to be sent

	if(IndexNext > LPC_EMAC->TxDescriptorNumber)		//point to descriptor
		IndexNext = 0;

	if(IndexNext == LPC_EMAC->TxConsumeIndex)
		return 0;										//fail buffer is full

	Index = LPC_EMAC->TxProduceIndex;
	if (size > ETH_FRAG_SIZE)							//clip size
		size = ETH_FRAG_SIZE;

	memcpy((unsigned int *)&txbuf[Index],packet,size);  //this can be optimize by changing the descriptor pointer to data
	txdescriptor[Index].control &= ~0x7FF;
	txdescriptor[Index].control |= (size-1)&0x7FF;

	LPC_EMAC->TxProduceIndex = IndexNext;

	return 1;
}


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

uint32_t ETH_GetPHY_ID(void){
uint32_t id;
	id = ETH_ReadPHY(PHY_ID1);
	id <<=16;
	id |= ETH_ReadPHY(PHY_ID2);
	return id;
}

void ETH_InitDescriptors(void){
	uint32_t i;
	//Initialize Descriptors
	rxdescriptor = (EMAC_Descriptor*)RX_DESC_BASE;
	rxstatus     = (EMAC_RxStatus*)RX_STAT_BASE;
	txdescriptor = (EMAC_Descriptor*)TX_DESC_BASE;
	txstatus     = (EMAC_TxStatus*)TX_STAT_BASE;
	rxbuf 		 = (EMAC_Buffer*)RX_BUF_BASE;
	txbuf		 = (EMAC_Buffer*)TX_BUF_BASE;

	for(i = 0; i < NUM_RX_FRAG; i++){
		rxdescriptor[i].packet = &rxbuf[i];
		rxdescriptor[i].control = RCTRL_INT | (ETH_FRAG_SIZE - 1);
		rxstatus[i].info = 0;
		rxstatus[i].crc = 0;
	}

	LPC_EMAC->RxDescriptor = RX_DESC_BASE;
	LPC_EMAC->RxStatus = RX_STAT_BASE;
	LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG - 1;
	LPC_EMAC->RxConsumeIndex = 0;

	for(i = 0; i < NUM_TX_FRAG; i++){
		txdescriptor[i].packet  = &txbuf[i];
		txdescriptor[i].control = 0;
		txstatus[i].info = 0;
	}

	LPC_EMAC->TxDescriptor = TX_DESC_BASE;
	LPC_EMAC->TxStatus = TX_STAT_BASE;
	LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG - 1;
	LPC_EMAC->TxProduceIndex = 0;
 }

void ETH_InitPHY(void){
	uint32_t loop;
	uint16_t phystatus;

	ETH_WritePHY(PHY_CR, PHY_RESET);

	phystatus = loop = 0;
	do{
		phystatus = ETH_ReadPHY(PHY_SR);
		loop++;
	}while((phystatus & PHY_RESET) && loop < 0x100);

	loop = 0;
	ETH_WritePHY(PHY_CR, PHY_AUTO_NEGOTIATE); //set auto negotiate

	while(!(phystatus & (PHY_AN_COMPLETED | PHY_LINK)) && loop < 10000){
		phystatus = ETH_ReadPHY(PHY_SR);
		loop++;
	}

	if(phystatus & PHY_AN_COMPLETED){
		 if ( (phystatus & PHY_100FD) || (phystatus & PHY_10FD) ) {
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
}

void ETH_Init(void){
	LPC_SC->PCONP |= ETH_ON;
	ETH_ConfigPins();

	//Reset everything
	LPC_EMAC->MAC1 =  MAC1_RESET_TX | MAC1_RESET_MCS_TX	| MAC1_RESET_RX | MAC1_RESET_MCS_RX | MAC1_RESET_SIM | MAC1_RESET_SOFT;
	LPC_EMAC->Command = CMD_RESET_TX | CMD_RESET_RX | CMD_RESET_REG | CMD_PASS_RUNT_FRM;

	//
	LPC_EMAC->MAC1 = MAC1_PASS_ALL;
	LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;

	LPC_EMAC->MAXF = ETH_MAX_FLEN;
	LPC_EMAC->CLRT = CLRT_DEF;
	LPC_EMAC->IPGR = IPGR_DEF;

	LPC_EMAC->MCFG = MCFG_RESET_MII;
	TIME_DelayMs(5);
	LPC_EMAC->MCFG = MCFG_CK64;		//MII Clock = CCLK/64

	LPC_EMAC->Command = CMD_RMII | CMD_PASS_RUNT_FRM | CMD_PASS_RX_FILT;

	ETH_InitPHY();

	ETH_InitDescriptors();

	//Configure Ethernet physical address
	LPC_EMAC->SA2 = 0x0506;//IF_MAC >> 32;		 //Config MAC address
	LPC_EMAC->SA1 = 0x0304;//IF_MAC >> 16;
	LPC_EMAC->SA0 = 0x0102;//IF_MAC & 0xFFFF;

	LPC_EMAC->RxFilterCtrl = RFC_BROADCAST | RFC_PERFECT;
	LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;
	LPC_EMAC->IntClear  = 0xFFFF;

	LPC_EMAC->Command |= CMD_RX_EN | CMD_TX_EN;
	LPC_EMAC->MAC1 |= MAC1_RCV_EN;
}