#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include <gpio.h>


void GPIO_SetState(uint32_t pin, uint8_t state){
	if(state == GPIO_HIGH)
			LPC_GPIO0->FIOSET = (1<<pin);
		else
			LPC_GPIO0->FIOCLR = (1<<pin);
}

void GPIO_Init(uint32_t pin, uint8_t dir, uint8_t state){
	if(dir == GPIO_OUTPUT)
		LPC_GPIO0->FIODIR |= (1<<pin);
	else
		LPC_GPIO0->FIODIR &= ~(1<<pin);

	GPIO_SetState(pin, state);
}

#endif
