#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include <time_m3.h>

volatile unsigned int systemticks;

void SysTick_Handler(void){
	systemticks++;
}

void TIME_Init(void){
	//SysTick->LOAD = cclk/1000;
	//SysTick->CTRL = (1<<2) | ( 1<<0); //core clock used for SysTick, enable counter
	//NVIC_EnableIRQ(SysTick_IRQn);
	SysTick_Config(SystemCoreClock/1000);  //1ms interrupt
}

uint32_t TIME_Elapsed(uint32_t ticks){
	return TIME_GetValue() - ticks;
}

void TIME_DelayMs(uint32_t ms){
uint32_t endtime = TIME_GetValue() + ms;
	while(TIME_GetValue() < endtime);
}



#endif
