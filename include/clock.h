/**
* @file		clock.h
* @brief	Contains API header for returning the current CCLK and PCLK, also alows initializations of PLL.
*     		
* @version	1.0
* @date		3 Nov. 2016
* @author	Hugo Reis
**********************************************************************/
#ifndef _clock_h_
#define _clock_h_

#if defined(__ARM_ARCH_7M__)
#include <LPC17xx.h>

#else
#include <lpc2106.h>

//For understand this values see file "PLL_Calc.ods"
//or LPC2106 user manual
#define XTAL    14745600
#define MSEL 4  
#define PSEL 1

#define APBDIV_1 1
#define APBDIV_2 2
#define APBDIV_4 0

#ifdef _EMU_
#define CLOCK_SetAPBDIV(x)
#else
#define CLOCK_SetAPBDIV(x) SC->APBDIV = x
#endif

#endif  /* __ARM_ARCH_7M__ */

#define PCLK_TIMER0	2
#define PCLK_TIMER1	4
#define PCLK_TIMER2	12
#define PCLK_TIMER3	14
#define PCLK_1 		1
#define PCLK_2 		2
#define PCLK_4 		0
#define PCLK_8 		3

uint32_t CLOCK_GetPCLK(void);
uint32_t CLOCK_GetCCLK(void);
void CLOCK_PllInit(uint8_t msel, uint8_t psel);

#endif
