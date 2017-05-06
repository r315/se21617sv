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
void ETH_Test(void);

void TEST_Init(void){
	TIME_Init();

#ifdef __BB__
	LCD_Init();
#else
	SPI_Init(100000,SPI_8BIT);

	LCD_Init();

	SPI_Init(6000000,SSP_16BIT);
#endif

	LCD_Clear(BLACK);
	LCD_Rect(90,20,100,100,RED);
	LCD_SetColors(YELLOW, BLACK);
	LCD_WriteString("Hello\n");
	LCD_Bkl(ON);

	LED_Init(LED,LED_OFF);
	LCD_WriteString("Led Initialized: OFF\n");

	BUTTON_Init(BUTTON_DEFAULT_HOLD_TIME);
	LCD_WriteString("Buttons Initialized: 2s\n");

	EEPROM_Init();
	LCD_WriteString("EEPROM on I2C");
	LCD_WriteInt(EEPROM_GetIfNumber(),10);
	LCD_WriteChar('\n');

	ETH_Init();
	LCD_WriteString("ETH: MAC 06:05:04:03:02:01\n");
	LCD_WriteString("ETH: PHY ID 0x");
	LCD_WriteInt(ETH_GetPHY_ID(),16);
	LCD_WriteChar('\n');
}

int main(void) {

	TEST_Init();

	//EEprom_Test();

	//PWM_TestInit();

	while(1) {
		//uIP_Test();
		BUTTON_Hit();
		//Button_Test();
		//PWM_Test();
		ETH_Test();
		TIME_DelayMs(100);
	}

	return 0 ;
}
