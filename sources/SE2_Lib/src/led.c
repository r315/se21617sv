/**
* @file		led.c
* @brief	Contains API source code for controlling a single led on any pin.
*     		
* @version	1.0
* @date		3 Nov. 2016
* @author	Hugo Reis
**********************************************************************/
#include <led.h>
#include <gpio.h>

int __pin;
int __state;

void LED_Init(int pinId, int state){
    __pin = pinId;
    GPIO_SetOutput(__pin);
    LED_SetState(state);
}

int LED_GetState(void){
    return __state;
}

void LED_SetState(int state){
    __state = state & 1;
    
#if defined(__LPC17XX__)
    if(__state)				// led is connected in common Anode configuration
        GPIO_Set(__pin);
    else
        GPIO_Clr(__pin);
#else
    if(!__state)				// led is connected in common Anode configuration
        GPIO_Set(__pin);
    else
        GPIO_Clr(__pin);
#endif
}

void LED_Toggle(void){
	LED_SetState(~__state);
}

