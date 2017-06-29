/*
===============================================================================
 Name        : Projeto.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>
#include <string.h>
#include <lcd.h>
#include <led.h>
#include <rtc.h>
#include <util.h>
#include <misc.h>
#include <Task_Config.h>
#include <Task_Space.h>
#include <Task_Common.h>

xTaskHandle *resumeTaskHandle;
xQueueHandle *topscore_queue;
xSemaphoreHandle *topscore_semaphore;

#if TASK_MAIN_DEBUG
static void LOG(char *msg){
	printf("Task Main: %s", msg);
}
#else
#define LOG(x)
#endif

static const char title[]={
    "           MAIN\n\n"
    "C   -  start new Game\n"
    "U   -  Load Game\n"
    "L   -  for config(2s)\n\n"
    "Top Scores:\n"
};



void PRINT_DateTime(struct tm *rtc){
    LCD_WriteInt(rtc->tm_mday, TIME_FORMAT);
    LCD_WriteChar('/');
    LCD_WriteInt(rtc->tm_mon, TIME_FORMAT);
    LCD_WriteChar('/');
    LCD_WriteInt(rtc->tm_year, YEAR_FORMAT);
    LCD_WriteChar(' ');
    LCD_WriteInt(rtc->tm_hour, TIME_FORMAT);
    LCD_WriteChar(':');
    LCD_WriteInt(rtc->tm_min, TIME_FORMAT);
    LCD_WriteChar(':');
    LCD_WriteInt(rtc->tm_sec, TIME_FORMAT);
}

/**
 * @brief
 * */
void MAIN_HighScores(uint32_t x, uint32_t y, uint32_t *scores){
uint8_t n;
    for(n = 0; n < MAX_TOP_SCORES; n++){
        LCD_Goto(x,y);
        LCD_WriteInt(scores[n], SCORES_FORMAT);
        y += LCD_GetFontHeight();
    }
}

void MAIN_AllTimeTopScores(uint16_t x, uint16_t y, xQueueHandle *qh){
RtopScore itm;
	while(xQueueReceive(qh,&itm,0) == pdPASS ){
        LCD_Goto(x,y);
        LCD_WriteString(itm.name);
        y += LCD_GetFontHeight();
    }
}


void MAIN_Init(void *ptr){
    LCD_Clear(BLACK);
    LCD_SetColors(RED,BLACK);
    LCD_Goto(0,0);
    LCD_WriteString((char*)title);

    LCD_SetColors(YELLOW,BLACK);
    MAIN_HighScores(80, 112, ((SaveData *)ptr)->spaceInvaders.topscores);
    LCD_SetColors(GREEN,BLACK);
    BUTTON_SetHoldTime(ENTER_CONFIG_TIME);

    webclient_get(SCORES_SERVER_ADDR,8080,"/emb");
}

void MAIN_Update(void){
	static uint32_t updateTime;
	struct tm rtc;

    if( TIME_Elapsed(updateTime) > 1000){
        RTC_GetValue(&rtc);
        LCD_Goto(0,LCD_H - LCD_GetFontHeight());
        PRINT_DateTime(&rtc);
        updateTime = TIMER0_GetValue();
        if(xSemaphoreTake(topscore_semaphore, 0)){
        	MAIN_AllTimeTopScores(80, 112, topscore_queue);
        }
    }
}

void MAIN_Game(SaveData *ptr){
	resumeTaskHandle = xTaskGetCurrentTaskHandle(); // Salvaguarda o handle da task corrente para poder ser retomada
	if(!xTaskCreate(Task_Space, "Space", TASK_SPACE_HEAP, &ptr->spaceInvaders, TASK_SPACE_PRIORITY, NULL )){
		LOG("unable to start task Space\n");
	}
	vTaskSuspend(NULL);		// suspende a task Main
	ptr->checksum = generateChecksum(&ptr->spaceInvaders, sizeof(GameData));
	SAVE_QueuePut(ptr, sizeof(SaveData));
}


/**
 * @brief Task criada apos startup
 */
void Task_Main(void *ptr){
uint8_t refreshMain = ON;
BTN_Event button;
SaveData *saveddata;
saveddata = (SaveData *)ptr;

SAVE_QueueGet(saveddata, sizeof(SaveData));

LCD_SetColors(GREEN,BLACK);

vTaskDelay(1500);

while(1){

	if(refreshMain == ON){
		MAIN_Init(ptr);
		refreshMain = OFF;
	}

	button = BUTTON_QueueGet();
	MAIN_Update();

	if(button.event != BUTTON_EMPTY){
		if(button.event == BUTTON_PRESSED){
			switch(button.value){

			case BUTTON_C:
				SPACE_NewGame(&saveddata->spaceInvaders);
				MAIN_Game(saveddata);
				refreshMain = ON;
				break;

			case BUTTON_U:
				SAVE_QueueGet(saveddata, sizeof(SaveData));
				if(!checksumData((void*)&(saveddata->spaceInvaders), sizeof(GameData), saveddata->checksum)){
					MAIN_Game(saveddata);
					refreshMain = ON;
				}
				else{
					LOG("Bad Checksum of GameData\n");
				}
				break;

			case BUTTON_R:
				//mesureDisplayDraw();
				break;

			default: break;
			}

		}else if(button.event == BUTTON_HOLD){
			switch(button.value){
				case (BUTTON_L):
					if(!xTaskCreate(Task_Config, "Config", TASK_CONFIG_HEAP, &saveddata->rtc, TASK_CONFIG_PRIORITY, NULL )){
						LOG("unable to start task config\n");
					}
					vTaskSuspend(NULL);		// suspende a task corrente
					refreshMain = ON;
					break;

				case BUTTON_R:  //Erase Top Scores
					memset(saveddata, 0, sizeof(SaveData));
					SAVE_QueuePut(saveddata, sizeof(SaveData));
					//switchTo(IDLE);
					//BUTTON_WaitEvent(BUTTON_RELEASED);
					BUTTON_QueueClear();
					refreshMain = ON;
				default: break;
			}
		}
	}
}
}

