#include <stdio.h>
#include <string.h>
#include <led.h>
#include <lcd.h>
#include <rtc.h>
#include <button.h>
#include <eth.h>
#include <Task_Net.h>
#include <util.h>

#if defined(__LPCXpresso__)
#include <eeprom.h>
#include <spi.h>
#elif defined(__BB__)
#include <eeprom.h>
#else
#include <flash.h>
#include <spi.h>
#endif

#include <misc.h>
#include <FreeRTOS.h>
#include <task.h>
#include <Task_Common.h>

const int defaultRtc[]={0,0,0,0,0,2010,0,0,0,0,0};

SaveData saveddata;

#if TASK_STARTUP_DEBUG
static void LOG(char *msg){
	printf("Startup: %s", msg);
}
#else
#define LOG(x)
#endif


 /**
  * @brief peripheral initialization
  * */
void SYS_Init(void){

	#if defined(__LPCXpresso__)

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

    LCD_Clear(BLACK);
    LCD_Bkl(ON);

    //LCD_Rotation(LCD_REVERSE_PORTRAIT);

	LCD_SetColors(YELLOW, BLACK);
	LCD_WriteString("Hello\n");

	LED_Init(LED,LED_OFF);
	LCD_WriteString("Led Initialized: OFF\n");

	BUTTON_Init(500);
	LCD_WriteString("Buttons Initialized: 500ms\n");

	EEPROM_Init();
	LCD_WriteString("EEPROM on I2C");
	LCD_WriteInt(EEPROM_GetIfNumber(),10);
	LCD_WriteChar('\n');

	RTC_Init(0); //dummy value, rtc value is restored from flash
	LCD_WriteString("RTC Initialized: NOT SET!\n");

	ETH_Init();
	LCD_WriteString("ETH: MAC ");LCD_WriteString(IF_MAC);
	LCD_WriteChar('\n');
	LCD_WriteString("ETH: PHY ID 0x");
	LCD_WriteInt(ETH_GetPHY_ID(),16);LCD_WriteChar('\n');
}

/**
 * @brief Main Function, initialize all hardware and start tasks
 **/
int main(void){
	 SYS_Init();

	    LCD_WriteString("Starting RTOS Tasks\n");

	    if(xTaskCreate(Task_Save, "Save", TASK_SAVE_HEAP, NULL , TASK_SAVE_PRIORITY, NULL ) != pdPASS){
			LOG("Unable to start SaveSend Task\n");
		}

	    if(xTaskCreate(Task_Button, "Button", TASK_BUTTON_HEAP , NULL, TASK_BUTTON_PRIORITY, NULL )  != pdPASS){
			LOG("Unable to start Button Task\n");
		}

	    if(xTaskCreate(Task_Net, "Network", TASK_NET_HEAP, NULL, TASK_NET_PRIORITY, NULL )  != pdPASS){
	    	LOG("Unable to start Micro ip Task\n");
	    }

	    if(xTaskCreate(Task_Main, "Main", TASK_MAIN_HEAP, &saveddata, TASK_MAIN_PRIORITY, NULL )   != pdPASS){
		    LOG("Unable to start main app Task\n");
		}

	    topscore_queue = xQueueCreate(MAX_TOP_SCORES + 3, sizeof(RtopScore));
	    vSemaphoreCreateBinary(topscore_semaphore);

	    vTaskStartScheduler();

	    while(1){

	    }
}


