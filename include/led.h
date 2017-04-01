/**
* @file		led.h
* @brief	Contains the LED API header.
*     		
* @version	1.0
* @date		3 Nov. 2016
* @author	Hugo Reis
**********************************************************************/

#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>

#if defined(__LPC17XX__)
#define LED 22
#elif defined(__LPC2106__)
#include <lpc2106.h>
#define LED 16
#else
#define LED 0
#endif

#define LED_ON  1
#define LED_OFF 0

/** 
* @brief Faz a iniciação do sistema para permitir a manipulação do estado LED ligado
*        bit pinId (bit) do GPIO. Deixa o LED apagado quando state toma o valor 0 ou aceso no
*        caso contrário. 
**/
void LED_Init(int pinId, int state);

/**
* @brief Devolve 0 se o LED está apagado e um valor diferente de zero no caso contrário. 
**/
int LED_GetState(void);

/**
* @brief Apaga o LED se state toma o valor 0 ou acende o LED no caso contrário.
**/
void LED_SetState(int state);

/**
* @brief Altera o estado corrente para o oposto
**/
void LED_Toggle(void);


#endif

