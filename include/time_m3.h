/**
* @file		time_m3.h
* @brief	Contains API header for coretex time related functions.
*     		
* @version	1.0
* @date		25 Mar. 2017
* @author	Hugo Reis
**********************************************************************/
#ifndef _time_m3_h_
#define _time_m3_h_

#include <time.h>
#include <stdint.h>

#define TIMER_ENABLE 	(1<<0)
#define TIMER_RESET 	(1<<1)


#define DelayMs(x) TIME_DelayMs(x)

/**
* @brief Faz a iniciação do SysTick de modo a que este incremente um contador de milisegundos
*/
void TIME_Init(void);

/**
* @brief Devolve o valor corrente do Systick em unidades de contagem (ticks).
**/
uint32_t TIME_GetValue(void);

/**
* @brief Devolve o valor, em unidades de contagem (ticks), desde ticks até ao valor actual do systick
**/
uint32_t TIME_Elapsed(uint32_t ticks);

/**
* @brief Espera Activa em milisegundos
**/
void TIME_DelayMs(uint32_t ms);

/**
* @brief Conversão de ticks para milisegundos
**/
#define TIME_TicksToMs(ticks) ticks

/**
* @brief Configuração de clock para o prescaler dor timer
**/
void TIMER_SetClockDivider(uint8_t ckdiv);



#endif
