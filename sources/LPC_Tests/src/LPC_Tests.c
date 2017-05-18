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
	LCD_WriteString("LED: OFF\n");

	BUTTON_Init(BUTTON_DEFAULT_HOLD_TIME);
	LCD_WriteString("BUTTONS: Hold 2s\n");

	EEPROM_Init();
	LCD_WriteString("EEPROM: Bus I2C");
	LCD_WriteInt(EEPROM_GetIfNumber(),10);
	LCD_WriteChar('\n');

	ETH_Init();
	LCD_WriteString("ETH: MAC ");
	LCD_WriteInt((uint8_t)(IF_MAC>>5*8), (2<<8) | 16); LCD_WriteChar(':');
	LCD_WriteInt((uint8_t)(IF_MAC>>4*8), (2<<8) | 16); LCD_WriteChar(':');
	LCD_WriteInt((uint8_t)(IF_MAC>>3*8), (2<<8) | 16); LCD_WriteChar(':');
	LCD_WriteInt((uint8_t)(IF_MAC>>2*8), (2<<8) | 16); LCD_WriteChar(':');
	LCD_WriteInt((uint8_t)(IF_MAC>>1*8), (2<<8) | 16); LCD_WriteChar(':');
	LCD_WriteInt((uint8_t)(IF_MAC>>0*8), (2<<8) | 16); LCD_WriteChar('\n');
	LCD_WriteString("ETH: PHY ID 0x");
	LCD_WriteInt(ETH_GetPHY_ID(),16);
	LCD_WriteChar('\n');
}

int main(void) {

	TEST_Init();

	#ifdef __EEPROM__
	EEprom_Test();
	#endif



	while(1) {

	#ifdef __UIP__
		uIP_Test();
	#elif __ETH__
		ETH_Test();
		//BUTTON_Hit();
		//Button_Test();
		//PWM_Test();
		//PWM_TestInit();
	#endif
		TIME_DelayMs(100);
	}

	return 0 ;
}
