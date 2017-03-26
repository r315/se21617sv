/*
===============================================================================
 Name        : Blink.c
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

#include <gpio.h>
#include <time_m3.h>

#define LED2 22

int main(void) {
volatile static int i = 0 ;

   GPIO_Init(LED2,GPIO_OUTPUT,GPIO_HIGH);

   TIME_Init();

    while(1) {
     i++;
     GPIO_SetState(LED2, i & 1);
     TIME_DelayMs(500);
    }
    return 0 ;
}
