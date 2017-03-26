
#include <LPC17xx.h>
#include <uart.h>

static Uart _uart;

void *UART_GetReference(void){
	_uart.xputc = UART_SendChar;
	_uart.xgetchar = UART_GetChar;
	return &_uart;
}

//TODO: Fix bug, add end condition for pointer on cycle do while
unsigned char frdivLookup(struct FdrPair *frdiv, unsigned int baudrate, uint64_t pclk){
unsigned int DLest,FRest;
struct FdrPair *pfrdvtab = (struct FdrPair *)_frdivTab;

	do{		
		DLest = (pclk * 1000)/ (16 * baudrate * (pfrdvtab->fr));
		FRest = (pclk * 1000)/ (16 * baudrate * DLest);
		pfrdvtab += 1;
	}while(FRest < 1000 || FRest > 1900);
	
	frdiv->fr = pfrdvtab->fr;
	frdiv->FdrVal = pfrdvtab->FdrVal;
return DLest & 255;
}

void UART_Init(unsigned char number, unsigned int baudrate, uint64_t pclk){
//struct FdrPair frdiv;
//unsigned char DLest;
	
	// Turn on power to UART0
	LPC_SC->PCONP |=  PCUART0_POWERON;
		
	// Turn on UART0 peripheral clock
	LPC_SC->PCLKSEL0 &= ~(PCLK_UART0_MASK); 		// PCLK_periph = CCLK/4
	//SC->PCLKSEL0 |=  (0 << PCLK_UART0);	
	
	// Set PINSEL0 so that P0.2 = TXD0, P0.3 = RXD0
	LPC_PINCON->PINSEL0 &= ~0xf0;
	LPC_PINCON->PINSEL0 |= ((1 << 4) | (1 << 6));
	
	LPC_UART0->LCR = 0x83;		// 8 bits, no Parity, 1 Stop bit, DLAB=1
    
	#ifndef DEBUG
	DLest = frdivLookup(&frdiv, baudrate, pclk);	
    UART0->DLM = DLest >> 8;
    UART0->DLL = DLest & 0xFF;
	UART0->FDR = frdiv.FdrVal;
	#else	
	LPC_UART0->DLM = 0; //config for CCLK=4Mhz, 9600bps
	LPC_UART0->DLL = 6;
	LPC_UART0->FDR = 0xC1;
	#endif
	
	LPC_UART0->LCR = 0x03;		// 8 bits, no Parity, 1 Stop bit DLAB = 0
	LPC_UART0->FCR = 0x07;		// Enable and reset TX and RX FIFO
}

char UART_GetChar(void){
char c;
	while( (LPC_UART0->LSR & LSR_RDR) == 0 );  // Nothing received so just block
	c = LPC_UART0->RBR; // Read Receiver buffer register
	return c;
}

void UART_SendChar(char c){	
	while( (LPC_UART0->LSR & LSR_THRE) == 0 );	// Block until tx empty
	LPC_UART0->THR = c;
}
