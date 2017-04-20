/*
===============================================================================
 Name        : LcdTest.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdio.h>

#include <spi.h>
#include <lcd.h>
#include <time_m3.h>
#include <gpio.h>
#include <led.h>
#include <button.h>
#include <eeprom.h>
#include <eth.h>


void Button_Test(void);
void EEprom_Test(void);

int main(void) {
	TIME_Init();
#ifdef __BB__
	LCD_Init();
#else
	SPI_Init(100000,SPI_8BIT);
	LCD_Init();
	SPI_Init(6000000,SSP_16BIT);
#endif

	LCD_Clear(BLUE);

	LCD_Rect(20,20,100,100,RED);

	LCD_SetColors(BLUE,YELLOW);
	LCD_WriteString("Hello\n");

	TIME_DelayMs(2000);

	LED_Init(LED,LED_OFF);

	BUTTON_Init(BUTTON_DEFAULT_HOLD_TIME);

	//EEprom_Test();

	//PWM_TestInit();

	ETH_Init();

	LCD_WriteInt(ETH_GetPHY_ID(),16);

	// Force the counter to be placed into memory

	// Enter an infinite loop, just incrementing a counter
	while(1) {
		BUTTON_Hit();
		//Button_Test();
		//PWM_Test();
		TIME_DelayMs(100);
	}

	return 0 ;
}
