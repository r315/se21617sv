#include <eth.h>

unsigned short ETH_ReadPHY (unsigned char reg){
	unsigned int loop = 3;
	// Set up address to access in MII Mgmt Address Register
	LPC_EMAC->MADR = PHY_DEF_ADR | reg;
	// Trigger a PHY read via MII Mgmt Command Register
	LPC_EMAC->MCMD = MCMD_READ;
	// Loop read from PHY completes

	while(loop){
		if((LPC_EMAC->MIND & MIND_BUSY) == 0)
			break;
		TIME_DelayMs(5);
	}

	LPC_EMAC->MCMD = 0; // Cancel read
	// Returned value is in MII Mgmt Read Data Register
	return (LPC_EMAC->MRDD);
}

void ETH_WritePHY (int reg, int writeval){
	unsigned int loop;
	// Set up address to access in MII Mgmt Address Register
	LPC_EMAC->MADR = PHY_DEF_ADR | reg;
	// Write value into MII Mgmt Write Data Register
	LPC_EMAC->MWTD = writeval;
	// Loop write to PHY completes
	while(loop){
		if((LPC_EMAC->MIND & MIND_BUSY) == 0)
			break;
		TIME_DelayMs(5);
	}
}


void ETH_Init(void){

	LPC_SC->PCONP |= ETH_ON;
	ETH_ConfigPins();

	LPC_EMAC->MAC1 |= ETH_RX_ALL_FRAMES;
	LPC_EMAC->MAC2 |= (1<<5);

	//Configure PHY
	ETH_WritePHY(SMI_CR,SMI_RESET);
	//Configure DMA

	LPC_EMAC->SA2 = ETH_MAC >> 32;
	LPC_EMAC->SA1 = ETH_MAC >> 16;
	LPC_EMAC->SA0 = ETH_MAC;


}

uint16_t ETH_GetPHY_ID(void){
	return ETH_ReadPHY(SMI_ID1) | ETH_ReadPHY(SMI_ID1) << 16;

}
