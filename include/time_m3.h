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

extern volatile unsigned int systemticks;

/**
* @brief Faz a iniciação do SysTick de modo a que este incremente um contador de milisegundos
*/
void TIME_Init(void);

/**
* @brief Devolve o valor corrente do Systick em unidades de contagem (ticks).
**/
static inline uint32_t TIME_GetValue(void){
	return systemticks;
}
/**
* @brief Devolve o valor, em unidades de contagem (ticks), desde ticks até ao valor actual do systick
**/
uint32_t TIME_Elapsed(uint32_t ticks);

/**
* @brief Active wait in miliseconds defined by ms
**/
void TIME_DelayMs(uint32_t ms);

/**
* @brief convertion of ticks to miliseconds, using systemtick one tick is one milisecond
**/
#define TIME_TicksToMs(ticks) ticks




#endif
