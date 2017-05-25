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

#if defined(__ETH__) || defined(__UIP__)
	LCD_Rotation(2);
#endif

	LCD_Rect(90,20,100,100,RED);
	LCD_SetColors(YELLOW, BLACK);
	LCD_WriteString("Hello\n");
	LCD_Bkl(ON);

	LCD_WriteString("Clock: ");	LCD_WriteInt(SystemCoreClock,10); LCD_WriteChar('\n');

	LED_Init(LED,LED_OFF);	LCD_WriteString("LED: OFF\n");

	BUTTON_Init(BUTTON_DEFAULT_HOLD_TIME); 	LCD_WriteString("BUTTONS: Hold 2s\n");

	EEPROM_Init();
	LCD_WriteString("EEPROM: Bus I2C");
	LCD_WriteInt(EEPROM_GetIfNumber(),10);
	LCD_WriteChar('\n');
#if defined(__ETH__) || defined(__UIP__)
	ETH_Init();
	LCD_WriteString("ETH: MAC ");LCD_WriteString(IF_MAC);
	LCD_WriteChar('\n');
	LCD_WriteString("ETH: PHY ID 0x");
	LCD_WriteInt(ETH_GetPHY_ID(),16);
	LCD_WriteChar('\n');
#endif
}

int main(void) {


#ifdef __FREE_RTOS__

	FreeRTOS_Test();
#else

	TEST_Init();

	#ifdef __EEPROM__
	EEprom_Test();
	#endif


	while(1) {

	#ifdef __UIP__
		uIP_Test();
	#elif defined(__ETH__)
		ETH_Test();
	#elif defined(__ROT__)
		BUTTON_Hit();

		if(BUTTON_GetEvents() == BUTTON_PRESSED){
			switch(BUTTON_GetValue()){
			case BUTTON_UP:
						LCD_Rotation(0);
						LCD_Goto(0,0);
						LCD_WriteString("Rotaton 0");
						break;
			case BUTTON_DOWN:
						LCD_Rotation(1);
						LCD_Goto(0,0);
						LCD_WriteString("Rotaton 1");
						break;
			case BUTTON_LEFT:
						LCD_Rotation(2);
						LCD_Goto(0,0);
						LCD_WriteString("Rotaton 2");
						break;
			case BUTTON_RIGHT:
						LCD_Rotation(3);
						LCD_Goto(0,0);
						LCD_WriteString("Rotaton 3");
						break;
			}
		}
		//Button_Test();
		//PWM_Test();
		//PWM_TestInit();
	#endif
		TIME_DelayMs(100);
	}
#endif
	return 0 ;
}
