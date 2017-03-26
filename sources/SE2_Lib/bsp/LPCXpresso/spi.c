
#include <spi.h>
#include <gpio.h>

#if defined(__LPC17XX__)
#include <LPC17xx.h>

void SSP_SetPCLK(uint32_t *pksel, uint8_t ck){
	*pksel &= ~(3);  //set lowest clock

	switch(ck){
		case 1: *pksel |= 1; break; //SystemCoreClock
		case 2: *pksel |= 2; break; //SystemCoreClock / 2
		case 4: *pksel |= 0; break; //SystemCoreClock / 4
		default:
		case 8: *pksel |= 3; break; //SystemCoreClock / 8
	}
}

void SSP_Init(LPC_SSP_TypeDef *sspx, uint32_t speed, uint8_t dss){
uint32_t *pksel, cpsr;
uint8_t ck;

	sspx->CR0 = 0;
	sspx->CR1 = 0;

	if((uint32_t)sspx & (1<<15)) // test for ssp0 or ssp1
		pksel = (uint32_t*)&LPC_SC->PCLKSEL1;
	else
		pksel = (uint32_t*)&LPC_SC->PCLKSEL0;

	for(ck = 8; ck > 0; ck >>= 1){          //calculate ssp prescaler
		cpsr = (SystemCoreClock/ck)/speed;  // CPSR = cclk/PCLKSEL/spi speed
		if((cpsr < SSP_MIN_CLK) && (cpsr > SSP_MAX_CLK)){
			SSP_SetPCLK(pksel, ck);
			break;
		}
	}

	if(!ck){
		SSP_SetPCLK(pksel, ck);
		cpsr = SSP_MIN_CLK;
	}

	sspx->CPSR = cpsr & 0xFE;	// must be an even number
	sspx->CR0 = SSP_CPOL | SSP_CPHA | (dss-1); //idle on high level, data valid on rising edge
	sspx->CR1 = SSP_SSE;        // Enable ssp
    sspx->DR = 0xFF;            // read status reg and clear flags
}

void SPI_Init(int frequency, int bitData){
	SSP0_PowerUp();
	SSP0_ConfigPins();
	SSP_Init(LPC_SSP0, frequency, bitData);
}

uint16_t SPI_Send(uint16_t data){
	LPC_SSP0->DR = data;
		while((LPC_SSP0->SR & SSP_BSY));
	return LPC_SSP0->DR;
}

#else
#include <lpc2106.h>
#include <clock.h>

/**
*	default init 8-bit transfer
*   master, mode 3, clk = PCLK/16
*   Note default pclk = cclk/4
**/
void SPI_Init(int frequency, int bitData){
int spccr;	

	SPI_PowerUp();
	PINCON->PINSEL0 = SPI0_PINS;
	SPI0->SPCR = SPI0_MSTR | SPI0_CPOL | SPI0_CPHA | SPI0_EN_NBITS | (bitData << 8);
	
	if(frequency){
		spccr = CLOCK_GetPCLK() / frequency;
		if(spccr < SPI_MAX_CLK)
			spccr = SPI_MAX_CLK;
	}
	else
		spccr = SPI_MAX_CLK;
		
	SPI0->SPCCR = spccr & 0xFE;	// must be an even number
    SPI_Send(0xFF);             // read status reg and clear flags
}

uint16_t SPI_Send(uint16_t data){
	SPI0->SPDR = data;
	while(!(SPI0->SPSR & SPI0_SPIF));
return SPI0->SPDR;
}



#endif

void SPI_Transfer(unsigned short *txBuffer, unsigned short *rxBuffer, int lenght){
	while(lenght--){
		*rxBuffer++ = SPI_Send(*txBuffer++);
	}
}

void SPI_BeginTransfer(int csBitId){
	GPIO_Clr(csBitId);
}

void SPI_EndTransfer(int csBitId){
	GPIO_Set(csBitId);
}
