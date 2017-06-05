#include <eth.h>
#include <string.h>
#include <time_m3.h>

static uint8_t _ifmac[6];
static EMAC_Memory *emac_memory;

uint32_t ETH_Read(void *packet){
	uint32_t size, Index = LPC_EMAC->RxConsumeIndex;

	if(Index == LPC_EMAC->RxProduceIndex)
		return(0); 										// no new data

	size = (emac_memory->rxstatus[Index].info & 0x7FF) + 1;			//get packet size from status
	if (size > ETH_FRAG_SIZE)size = ETH_FRAG_SIZE;		//clip size if bigger than fragment

	memcpy(packet,(uint8_t *)&emac_memory->rxbuffer[Index],size);	//copy packet data

	if(++Index > LPC_EMAC->RxDescriptorNumber)			//point to next descriptor
		Index = 0;

	LPC_EMAC->RxConsumeIndex = Index;

	return size;
}

uint32_t ETH_Send(void *packet, uint32_t size){
	uint32_t Index, IndexNext = LPC_EMAC->TxProduceIndex + 1;

	if(size == 0)
		return 0;										//no data to be sent

	if(IndexNext > LPC_EMAC->TxDescriptorNumber)		//point to descriptor
		IndexNext = 0;

	if(IndexNext == LPC_EMAC->TxConsumeIndex){
		return -1;										//fail buffer is full
	}

	Index = LPC_EMAC->TxProduceIndex;
	if (size > ETH_FRAG_SIZE)							//clip size
		size = ETH_FRAG_SIZE;

	memcpy((uint8_t *)&emac_memory->txbuffer[Index],packet,size);  //this can be optimize by changing the descriptor pointer to data
	emac_memory->txdesc[Index].control &= ~0x7FF;
	emac_memory->txdesc[Index].control |= (size-1)&0x7FF;

	LPC_EMAC->TxProduceIndex = IndexNext;

	return size;
}


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

uint32_t ETH_GetPHY_ID(void){
	return (ETH_ReadPHY(PHY_ID1)<<16) | ETH_ReadPHY(PHY_ID2);
}

void ETH_InitDescriptors(void){
	uint32_t i;

	/* Set EMAC Base Address for descriptors, status and buffers */
	emac_memory = (EMAC_Memory*)EMAC_MEM_BASE;

	/* Initialize receive descriptor packet with pointer to buffer set
	 * Receive interrupt for internal DMA and set packet size on control
	 */
	for(i = 0; i < NUM_RX_FRAG; i++){
		emac_memory->rxdesc[i].packet = &emac_memory->rxbuffer[i];
		emac_memory->rxdesc[i].control = RCTRL_INT | (ETH_FRAG_SIZE - 1);
		emac_memory->rxstatus[i].info = 0;
		emac_memory->rxstatus[i].crc = 0;
	}

	LPC_EMAC->RxDescriptor = (uint32_t)&emac_memory->rxdesc[0];
	LPC_EMAC->RxStatus = (uint32_t)&emac_memory->rxstatus[0];
	LPC_EMAC->RxDescriptorNumber = NUM_RX_FRAG - 1;
	LPC_EMAC->RxConsumeIndex = 0;

	/* Initialize Transmit descriptor packet with pointer to buffer set
	 * Transmit interrupt for internal DMA, misc flags and set packet size on control
	 */
	for(i = 0; i < NUM_TX_FRAG; i++){
		emac_memory->txdesc[i].packet  = &emac_memory->txbuffer[i];
		emac_memory->txdesc[i].control = (0x3D<<26) | (ETH_FRAG_SIZE-1); //INT|LAST|CRC|PAD|OVERRIDE
		emac_memory->txstatus[i].info = 0;
	}

	LPC_EMAC->TxDescriptor = (uint32_t)&emac_memory->txdesc[0];
	LPC_EMAC->TxStatus = (uint32_t)&emac_memory->txstatus[0];
	LPC_EMAC->TxDescriptorNumber = NUM_TX_FRAG - 1;
	LPC_EMAC->TxProduceIndex = 0;
 }

void ETH_InitPHY(void){
	uint16_t phystatus = 0;
	uint32_t loop;

	/* Reset PHY */
	ETH_WritePHY(PHY_CR, PHY_RESET);

	loop = 0x10000;
	do{
		phystatus = ETH_ReadPHY(PHY_SR);
		loop--;
	 }while((phystatus & PHY_RESET) && loop);

	//if(!loop) printf("PHY Reset Fail: %x", phystatus);

	/* Setup Auto negotiation */
	ETH_WritePHY(PHY_CR, PHY_AUTO_NEGOTIATE);

	loop = 10;
	do{
		phystatus = ETH_ReadPHY(PHY_SR);
		loop--;
	}while(!(phystatus & PHY_AN_COMPLETED) && loop != 0);

	//if(!loop) printf("Auto negotiation Fail: %x", phystatus);

	/* Check the link status. */
	loop = 10;
	do{
		phystatus = ETH_ReadPHY(PHY_SR);
		loop--;
	}while(!(phystatus & PHY_LINK) && loop != 0);

	//if(!loop) printf("No link: %x", phystatus);

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
}

uint8_t parseHex(uint8_t *h){
uint8_t hex;
	hex = ((*h) - '0') & 0x1F;
	if(hex > 0x0F)
		hex -= 7;
	return hex;
}

void ETH_ParseIF_MAC(uint8_t *dst, uint8_t *ift){
char hex = 0;
	while(*ift){
		if(*ift != ':'){
			hex <<=4;
			hex |= parseHex(ift);
		}else{
			*dst++ = hex;
			hex = 0;
		}
		ift +=1;
	}
	*dst = hex;
}

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

	ETH_ParseIF_MAC(_ifmac, IF_MAC);

	//Configure Ethernet physical address
	LPC_EMAC->SA2 = (_ifmac[5]<<8) | _ifmac[4];
	LPC_EMAC->SA1 = (_ifmac[3]<<8) | _ifmac[2];
	LPC_EMAC->SA0 = (_ifmac[1]<<8) | _ifmac[0];

	LPC_EMAC->RxFilterCtrl = RFC_BROADCAST | RFC_PERFECT; 	//Filter only good packets ans broadcast
	LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;	  	//Enable interrupts for dedicated DMA
	LPC_EMAC->IntClear  = 0xFFFF;

	LPC_EMAC->Command |= (CMD_RX_EN | CMD_TX_EN);			//enable send and receive
	LPC_EMAC->MAC1 |= MAC1_RCV_EN;							//enable frame receive
}

uint8_t *ETH_GetIF_MAC(void){
	return _ifmac;
}
