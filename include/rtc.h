/**
* @file		rtc.h
* @brief	Contains the RTC API header.
*     		
* @version	1.0
* @date		10 Nov. 2016
* @author	Hugo Reis
**********************************************************************/

#ifndef _rtc_h_
#define _rtc_h_

#include <stdint.h>
#include <time.h>
#if defined(__LPCX__) || defined(__BB__)
#include <LPC17xx.h>
#define RTC_PowerUp() LPC_SC->PCONP |= RTC_ON
#elif defined(__LPC2106__)
#include <lpc2106.h>
#define RTC_PowerUp() SC->PCONP |= RTC_ON
#endif

#define RTC_ON     (1<<9)
#define RTC_CLKEN  (1<<0)
#define RTC_CTCRST (1<<2)
#define RTC_RTCALF (1<<1)


#define RTC_DIVIDER 32768

#define RTC_AMR_OFF 0xFF

#define RTC_AMR_SEC     (1<<0)
#define RTC_AMR_MIN     (1<<1)
#define RTC_AMR_HOUR    (1<<2)
#define RTC_AMR_DOM     (1<<3)
#define RTC_AMR_DOW     (1<<4)
#define RTC_AMR_DOY     (1<<5)
#define RTC_AMR_MON     (1<<6)
#define RTC_AMR_YEAR    (1<<7)

#define TIME_FORMAT (2<<8) | 10 // print decimal with 2 digits  
#define YEAR_FORMAT (4<<8) | 10 // print decimal with 4 digits  

/** 
* @brief Faz a iniciação do sistema para permitir o acesso ao periférico RTC.
*        O RTC é iniciado com os valores do parâmetro.
**/
void RTC_Init(struct tm *dateTime); 	 

/** 
* @brief Devolve em dateTime o valor corrente do RTC.
**/
void RTC_GetValue(struct tm *dateTime);

/** 
* @brief Realiza a atualização do RTC com os valores do parâmetro dateTime.
**/
void RTC_SetValue(struct tm *dateTime);

/** 
* @brief Devolve em dateTime o valor corrente do alarme do RTC.
**/
void RTC_GetAlarmValue(struct tm *dateTime);

/** 
* @brief Realiza a atualização do alarme do RTC com os valores do parâmetro dateTime.
**/
void RTC_SetAlarmValue(struct tm *dateTime);

/** 
* @brief Activa um alarme
**/
void RTC_ActivateAlarm(uint8_t alarm);

/** 
* @brief Desactiva um alarme
**/
void RTC_DeactivateAlarm(uint8_t alarm);

/** 
* @brief Verifica se algum alarme foi gerado.
* @return 1 = true
**/
uint32_t RTC_CheckAlarm(void);

/** 
* @brief clears any activated alarm
**/
void RTC_ClearAlarm(void);

#endif

