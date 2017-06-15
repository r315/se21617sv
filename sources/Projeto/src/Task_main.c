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

xTaskHandle *taskMainHandle;

#if TASK_MAIN_DEBUG
static void LOG(char *msg){
	printf("Task Main: %s", msg);
}
#else
#define LOG(x)
#endif

static const char title[]={
    "           MAIN\n\n"
    "F   -  start new Game\n"
    "S   -  Load Game\n"
    "D   -  for config(2s)\n\n"
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

void MAIN_Init(void *ptr){
    LCD_Clear(BLACK);
    LCD_SetColors(RED,BLACK);
    LCD_Goto(0,0);
    LCD_WriteString((char*)title);

    LCD_SetColors(YELLOW,BLACK);
    MAIN_HighScores(96,112,(uint32_t *)ptr);
    LCD_SetColors(GREEN,BLACK);
    BUTTON_SetHoldTime(ENTER_CONFIG_TIME);
}

void MAIN_Update(void){
	static uint32_t updateTime;
	struct tm rtc;

    if( TIME_Elapsed(updateTime) > 1000){
        RTC_GetValue(&rtc);
        LCD_Goto(0,LCD_H - LCD_GetFontHeight());
        PRINT_DateTime(&rtc);
        updateTime = TIMER0_GetValue();
    }
}



/**
 * @brief Task criada apos startup
 */
void Task_Main(void *ptr){
uint8_t taskFinished = ON;
//uint32_t res;
BTN_Event button;
SaveData *saveddata;
saveddata = (SaveData *)ptr;

restoreData(saveddata,sizeof(SaveData));

LCD_SetColors(GREEN,BLACK);

vTaskDelay(1000);

taskMainHandle = xTaskGetCurrentTaskHandle(); // Salvaguarda o handle da task main para poder ser retomada

while(1){

	if(taskFinished == ON){
		MAIN_Init(saveddata->topscores);
		taskFinished = OFF;
	}

	button = BUTTON_QueueGet();
	MAIN_Update();

	if(button.event != BUTTON_EMPTY){
		if(button.event == BUTTON_PRESSED){
			switch(button.value){
			case BUTTON_C:
				//newGame(&saveddata.spaceInvaders);
				break;
			case BUTTON_U:
				restoreData(saveddata, sizeof(SaveData));
				if(!checksumData((void*)&(saveddata->spaceInvaders), sizeof(GameData), saveddata->checksum)){
					if(!xTaskCreate(Task_Space, "Space", TASK_SPACE_HEAP, &saveddata->spaceInvaders, TASK_SPACE_PRIORITY, NULL )){
						LOG("unable to start task Space\n");
					}
					vTaskSuspend(NULL);		// suspende a task Main
					taskFinished = ON;
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
					taskFinished = ON;
					break;

				case BUTTON_R:  //Erase Top Scores
					memset(saveddata, 0, sizeof(SaveData));
					saveData(saveddata, sizeof(SaveData));
					//switchTo(IDLE);
					//BUTTON_WaitEvent(BUTTON_RELEASED);
				default: break;
			}
		}
	}


#if 0


        //main state machine
        switch(state){
            case IDLE:





                }
                break;

            case CONFIG:
               /* if(!config()){
                    state = switchTo(IDLE);
                    RTC_SetValue(&saveddata->rtc); // RTC Is saved on flash when a game is saved
                }*/
                break;

            case GAME:
                if(BUTTON_GetValue() == BUTTON_S){
                    if(button.event == BUTTON_PRESSED){
                        state = switchTo(SAVE);
                        break;
                    }
                }
                //space(BUTTON_GetValue());
                break;

            case SAVE:
            	displaySaveResult(0x30);
                LED_SetState(LED_ON);
                //printf("Score: %u\n",saveddata.spaceInvaders.score);
                // check if last score is top and insert it on table
                saveTopScore(saveddata->spaceInvaders.score, saveddata->topscores);
                // update gamedata with top score
                saveddata->spaceInvaders.topscore = saveddata->topscores[0];

                saveddata->checksum = generateChecksum(&saveddata->spaceInvaders, sizeof(GameData));

                res = saveData(saveddata, sizeof(SaveData));

                LED_SetState(LED_OFF);

                displaySaveResult(res);

                state = switchTo(IDLE);
                break;
        }
    	#endif
    }

}

