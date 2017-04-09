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

#include <spi.h>
#include <lcd.h>
#include <time_m3.h>
#include <gpio.h>
#include <led.h>
#include <button.h>

#include <stdio.h>

#include <eeprom.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {
	uint8_t i;
	TIME_Init();
	uint8_t buf[1024];
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
	LCD_WriteString("Hello");

	TIME_DelayMs(2000);

	LED_Init(LED,LED_OFF);

	BUTTON_Init(BUTTON_DEFAULT_HOLD_TIME);

	EEPROM_Init();



	// Force the counter to be placed into memory

	// Enter an infinite loop, just incrementing a counter
	while(1) {
		//LED_Toggle();
		TIME_DelayMs(100);
		//LCD_Clear(i*16);

		BUTTON_Hit();
		if(BUTTON_GetEvents()==BUTTON_PRESSED){
			LCD_Goto(0,0);
			switch(BUTTON_GetButton()){
			case BUTTON_F:
				LCD_WriteString("FIRE ");

				EEPROM_Read(0x0,buf,256);

				for(i=0; i< 10; i++)
					printf("%x ",buf[i]);
				break;

			case BUTTON_L:
				LCD_WriteString("LEFT ");
				EEPROM_Write(0x0,(uint8_t*)"0123456789",10);
				break;

			case BUTTON_R:
				LCD_WriteString("RIGHT");
				break;
			case BUTTON_U:
				LCD_WriteString("UP   ");
				break;
			case BUTTON_D:
				LCD_WriteString("DOWN ");
				break;
			}
		}

	}
	return 0 ;
}
