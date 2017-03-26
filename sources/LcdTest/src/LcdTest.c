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

#include <stdio.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

int main(void) {
	TIME_Init();
    SPI_Init(100000,SPI_8BIT);
    LCD_Init();
    SPI_Init(6000000,SSP_16BIT);

    LCD_Clear(BLUE);

    LCD_Rect(20,20,100,100,RED);

    LCD_SetColors(BLUE,YELLOW);
    LCD_WriteString("Hello");

    TIME_DelayMs(2000);

    LED_Init(LED2,LED_OFF);

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
        LED_SetState(i & 1);
        TIME_DelayMs(100);
        LCD_Clear(i*16);

    }
    return 0 ;
}
