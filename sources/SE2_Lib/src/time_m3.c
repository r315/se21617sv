#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include <time_m3.h>
#include <clock.h>

#if defined(__USE_SYSTICK)
volatile unsigned int systemticks;

void SysTick_Handler(void){
	systemticks++;
}

void TIME_Init(void){
	SysTick_Config(SystemCoreClock/1000);  //1ms interrupt using CMSIS macros
}

uint32_t TIME_GetValue(void){
	return systemticks;
}
#else

void TIME_Init(void){
	LPC_TIM0->TCR = 0;				// Reset Timer
	TIMER_SetClockDivider(PCLK_1);	// Set clock divider for prescaler
	LPC_TIM0->CTCR = 0;				// Timer Mode
	LPC_TIM0->PR = SystemCoreClock/1000;
	LPC_TIM0->TCR |= TIMER_ENABLE;
}

uint32_t TIME_GetValue(void){
	return LPC_TIM0->TC;
}
void TIMER_SetClockDivider(uint8_t ckdiv){
	LPC_SC->PCLKSEL0 &= ~(3<<PCLK_TIMER0);
    LPC_SC->PCLKSEL0 |= ((ckdiv&3)<<PCLK_TIMER0);
}

uint32_t TIME_Elapsed(uint32_t ticks){
	return LPC_TIM0->TC - ticks;
}

void TIME_DelayMs(uint32_t ms){
uint32_t endtime = TIME_GetValue() + ms;
	while(LPC_TIM0->TC < endtime);
}
#endif

#endif
