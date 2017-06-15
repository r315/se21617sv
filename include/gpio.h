/**
* @file		gpio.h
* @brief	common functions for io port access
* @version	2.0
* @date		30 Out. 2016
* @modified 25 Mar. 2017
* @author	Hugo Reis
**********************************************************************/

#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdint.h>


#if defined(__ARM_ARCH_7M__)
#define P0_0

#define ON 1
#define OFF 0
#define GPIO_OUTPUT 1
#define GPIO_HIGH   1
#define GPIO_LOW    0


#if defined(__USE_CMSIS)
#include <LPC17xx.h>

/**
* @brief Initialyze pin and set his state
**/
void GPIO_Init(uint32_t pin, uint8_t dir, uint8_t state);

/**
* @brief Set state of pin given the number of GPIO0
**/
void GPIO_SetState(uint32_t pin, uint8_t state);

/**
* @brief Set pin of GPIO0 to high level
**/
#define GPIO_Set(pin) LPC_GPIO0->FIOSET = (1<<pin)

/**
* @brief Set pin of GPIO0 to low level
**/
#define GPIO_Clr(pin) LPC_GPIO0->FIOCLR = (1<<pin)

/**
* @brief Set pin of GPIO0 as output
**/
#define GPIO_SetOutput(pin) LPC_GPIO0->FIODIR |= (1<<pin)

/**
* @brief Set pin of GPIO0 as input
**/
#define GPIO_SetInput(pin) LPC_GPIO0->FIODIR &= ~(1<<pin)

#else
#include <lpc1768.h>

/**
* @brief Initialyze pin and set his state
**/
void GPIO_Init(uint32_t pin, uint8_t dir, uint8_t state);

/**
* @brief Set state of pin given the number of GPIO0
**/
void GPIO_SetState(uint32_t pin, uint8_t state);

/**
* @brief Set pin of GPIO0 to high level
**/
#define GPIO_Set(pin) GPIO0->FIOSET = (1<<pin)

/**
* @brief Set pin of GPIO0 to low level
**/
#define GPIO_Clr(pin) GPIO0->FIOCLR = (1<<pin)

/**
* @brief Set pin of GPIO0 as output
**/
#define GPIO_SetOutput(pin) GPIO0->FIODIR |= (1<<pin)

/**
* @brief Set pin of GPIO0 as input
**/
#define GPIO_SetInput(pin) GPIO0->FIODIR &= ~(1<<pin)
#endif  /* __USE_CMSIS  */

#elif defined(__LPC2106__)
#include <lpc2106.h>
/*
#define GPIO_Set(n) GPIO0->SET = (1 << n)  // change the state of only one pin
#define GPIO_Clr(n) GPIO0->CLR = (1 << n)  // by giving his number

#define GPIO_SetN(x) GPIO0->SET = x        // change selected pins
#define GPIO_ClrN(x) GPIO0->CLR = x        // by giving the pretended bts

#define GPIO_Write(x) GPIO0->PIN = x		//change all pins
#define GPIO_Read()   GPIO0->PIN

#define GPIO_SetOutput(n) GPIO0->DIR |= (1 << n) //affect single pin
#define GPIO_SetInput(n)  GPIO0->DIR &= ~(1 << n)

#define GPIO_SetOutputN(n) GPIO0->DIR |= n //affect multiple pin

#define GPIO_SetDir(n)  GPIO0->DIR = x //affect all pins
*/

#define GPIO_Set(n) FIO0->SET = (1 << n)        // change the state of only
#define GPIO_Clr(n) FIO0->CLR = (1 << n)        // one pin
#define GPIO_SetOutput(n) FIO0->DIR |= (1 << n) //affect single pin
#define GPIO_SetInput(n)  FIO0->DIR &= ~(1 << n)

#define GPIO_SetN(x) FIO0->SET = x              //change selected pins
#define GPIO_ClrN(x) FIO0->CLR = x
#define GPIO_Write(x) FIO0->PIN = x             //change all pins
#define GPIO_Read() FIO0->PIN
#define GPIO_SetDir(n)  FIO0->DIR = x           //affect all pins
#define GPIO_SetOutputN(n) FIO0->DIR |= n       //affect multiple pin
#define GPIO_SetInputN(n)  FIO0->DIR &= ~n      //affect multiple pin

#endif /* __ARM_ARCH_7M__ */

#endif

