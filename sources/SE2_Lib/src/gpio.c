
#include <gpio.h>

#if defined(__LPCXpresso__)
#include <LPC17xx.h>

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

#else
#include <lpc2106.h>
void GPIO_Init(uint8_t bit, uint8_t dir, uint8_t state){
	GPIO0->DIR &= ~(1<< bit);
	GPIO0->DIR |= (dir<< bit);
	GPIO_SetPin(bit, state);
}

void GPIO_SetPin(uint8_t bit, uint8_t state){
	if(state)
		GPIO0->SET = (1<<bit);
	else
		GPIO0->CLR = (1<<bit);
}

uint8_t GPIO_GetPin(uint8_t bit){
	return (GPIO0->PIN & (1<<bit)) == 1? 1 : 0;
}
#endif


