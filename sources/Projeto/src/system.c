#include <stdio.h>
#include <string.h>
#include <led.h>
#include <lcd.h>
#include <rtc.h>
#include <button.h>
#if defined(__LPCX__)
#include <eeprom.h>
#include <spi.h>
#elif defined(__BB__)
#include <eeprom.h>
#else
#include <flash.h>
#include <spi.h>
#endif

#include <util.h>

const int defaultRtc[]={0,0,0,0,0,2010,0,0,0,0,0};



 /**
  * @brief peripheral initialization
  * */
void SYS_Init(void){

	#if defined(__LPCX__)

	TIME_Init();

	SPI_Init(100000, SPI_8BIT);    // lcd must be initialyze with low speed and 8bit transfers

	LCD_Init();

	SPI_Init(6000000, SSP_16BIT); //after init use full speed

	EEPROM_Init();

	#elif defined(__LPC2106__)
    
    CLOCK_SetAPBDIV(APBDIV_2);   // PCLK = CCLK / 2
      
    CLOCK_PllInit(MSEL, PSEL);   // if used must be the first peripheral to initialize
    
    TIMER0_Init(MS_IN_1S);    

    SPI_Init(1000, SPI_8BIT);    // lcd must be initialyze with low speed and 8bit transfers

    LCD_Init();

    SPI_Init(SPI_MAX_FREQ, SPI_16BIT); //after init use full speed

	#elif defined(__EMU__)

    LCD_Init();

	#endif

	LED_Init(LED, LED_OFF);

    BUTTON_Init(BUTTON_DEFAULT_HOLD_TIME - 1500);

    LCD_SetColors(GREEN,BLACK);
        
    RTC_Init(0); //dummy value, rtc value is restored from flash
}
