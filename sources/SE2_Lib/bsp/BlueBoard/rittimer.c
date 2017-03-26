#include <LPC17xx.h>
#include <core_cm3.h>
#include <clock.h>
#include <rittimer.h>

void (*_rit_handler)(void);
volatile unsigned int _rit_ticks;

void RIT_CountTicks(void){
	_rit_ticks++;
}

int RIT_GetTicks(void){
	return _rit_ticks;
}
//--------------------------------------------------
//
//--------------------------------------------------
void RIT_Init(unsigned int us){
	_rit_ticks = 0;
	RIT_InitHandler(us, RIT_CountTicks);
}
//--------------------------------------------------
//
//--------------------------------------------------
void RIT_InitHandler(unsigned int us, void *handler){
	LPC_SC->PCONP |=  RIT_PEN; // power up rit
	LPC_RIT->RICOMPVAL = us * (CLOCK_GetCCLK() / RIT_CCLK_USDIV);
	LPC_RIT->RICTRL = 0; 		// stop timer
	LPC_RIT->RICOUNTER = 0;     // reset timer
	_rit_handler = (void(*)(void))handler; 
	NVIC_EnableIRQ(RIT_IRQn);   // enable irq
	LPC_RIT->RICTRL = RITENCLR | RITENBR | RITEN; //Start timer
}
//--------------------------------------------------
//  hw irq call
//--------------------------------------------------
void RIT_IRQHandler(void){
	LPC_RIT->RICTRL |= RITINT;    // flag must be clear at the begin of handler
	_rit_handler();
}

//rit timer must be counting ms TODO: Implement
void RIT_DelayMs(unsigned int ms){
	unsigned int ticks = _rit_ticks + ms;
	while(_rit_ticks < ticks);
}
