#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include <time.h>

volatile unsigned int systemticks;

void TIME_Init(uint32_t cclk){
	//SysTick->LOAD = cclk/1000;
	//SysTick->CTRL = (1<<2) | ( 1<<0); //core clock used for SysTick, enable counter
	//NVIC_EnableIRQ(SysTick_IRQn);
	SysTick_Config(cclk/1000);  //1ms interrupt
}

void TIME_DelayMs(uint32_t ms){
uint32_t endtime = TIME_GetSysTicks() + ms;
	while(TIME_GetSysTicks() < endtime);
}

void SysTick_Handler(void){
	systemticks++;
}

#endif
