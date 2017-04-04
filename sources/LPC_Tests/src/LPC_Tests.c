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

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

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
    LCD_WriteString("Hello");

    TIME_DelayMs(2000);

    LED_Init(LED,LED_OFF);

    BUTTON_Init(BUTTON_DEFAULT_HOLD_TIME);

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
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
        			break;
        	case BUTTON_L:
        	        			LCD_WriteString("LEFT ");
        	        			break;

        	case BUTTON_R:
        	        			LCD_WriteString("RIGHT");
        	        			break;
        	case BUTTON_S:
        	        			LCD_WriteString("SAVE ");
        	        			break;

        	}
        }

    }
    return 0 ;
}
