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

#include <string.h>
#include <button.h>
#include <lcd.h>
#include <led.h>
#include <spi.h>
#include <rtc.h>
#include <util.h>
#include "proj.h"
#include "system.h"
#include "save.h"
#include "config.h"
#include "space.h"
#include "idle.h"
#include <FreeRTOS.h>
#include <task.h>
#include <micro_ip.h>

uint16_t rand(void);
//---------------------------------------------------------------
// Constants and macros
//---------------------------------------------------------------


//---------------------------------------------------------------
// global variables
//---------------------------------------------------------------

SaveData saveddata;

//---------------------------------------------------------------
// Functions
//---------------------------------------------------------------
 /**
  * @brief calls the screen configurator for the given state
  * @param newstate: the state that main loop is going to enter
  * */
State switchTo(State newState){
    switch(newState){
        case IDLE:   popIdle(saveddata.topscores); break;
        case CONFIG: popConfig(&saveddata.rtc); break;
        case GAME:   popSpace(&saveddata.spaceInvaders);break;
        case SAVE:   break;
    }
    return newState;
}

uint8_t generateChecksum(void *data, uint32_t size){
uint8_t sum = 0;
    while(size--){
        //printf("data %u\n",*((uint8_t*)data));
        sum += *((uint8_t*)data++);
    }
    sum = 0xFF - sum;
    //printf("Generated Checksum %u\n", sum);
    return sum;
}

uint8_t checksumData(void *data, uint32_t size, uint8_t checksum){
uint8_t sum = 0;
    while(size--){
        sum += *((uint8_t*)data++);
    }
    //printf("Calculated Checksum %u\n", sum);
    return (uint8_t)(sum + (checksum + 1));
}

 /**
  * @brief Returns the number of pixels that a string ocupies on the display
  * @param str: String to check
  * @return size in pixels
  * */
uint32_t strlenInPixels(char *str){
uint8_t count = 0;
    while(*str++)
        count++;
    return count * LCD_GetFontWidth();
}

 /**
  * @brief for debug purpose
  * */
void mesureDisplayDraw(void){
uint32_t time;
    LED_SetState(LED_ON);
    time = TIME_GetValue();
    LCD_Clear(RED);
    time = TIME_TicksToMs(TIME_Elapsed(time));
    LED_SetState(LED_OFF);
    LCD_Goto(0,0);
    LCD_WriteInt(time,10);
}

void MAIN_App_Task(void){

State state;
uint32_t button,res;

    restoreData(&saveddata,sizeof(SaveData));
    
    //RTC_SetValue(&saveddata.rtc);
    
    LCD_SetColors(GREEN,BLACK);

    state = switchTo(IDLE);
    
    //printf("PRJ: SaveData Size: %u Bytes\n",sizeof(SaveData));
	//printf("PRJ: Random number %u\n",rand());

    while(loop){

        //Check Events
        button = BUTTON_Hit();

        #if 0
        if(BUTTON_GetEvents() != BUTTON_EMPTY){
           printf("Button %u State %u\n",button, BUTTON_GetEvents());
        }
        #endif

        #if 0
        if(button){
            LCD_Goto(0,0);
            LCD_WriteInt(button,(4<<8)|16);
            LCD_WriteChar(':');
            LCD_WriteInt(BUTTON_GetEvents(),16);
        }
        #endif

        //main state machine
        switch(state){
            case IDLE:
                idle();

                if(BUTTON_GetEvents() == BUTTON_PRESSED){
                    switch(button){
                        case BUTTON_F:
                            newGame(&saveddata.spaceInvaders);
                            state = switchTo(GAME);
                            break;
                        case BUTTON_S:
                            restoreData(&saveddata,sizeof(SaveData));
                            if(!checksumData(&saveddata.spaceInvaders, sizeof(GameData), saveddata.checksum))
                                state = switchTo(GAME);
                            break;
                        case BUTTON_R:
                            //mesureDisplayDraw();
                            break;
                        default: break;
                    }
                    break;
                }

                if(BUTTON_GetEvents() == BUTTON_HOLD){
                    switch(button){
                        //case BUTTON_L:
                        case (BUTTON_D):
                            state = switchTo(CONFIG);
                            break;

                        case BUTTON_R:  //Erase Top Scores
                            memset(&saveddata, 0, sizeof(SaveData));
                            saveData(&saveddata, sizeof(SaveData));
                            switchTo(IDLE);
                            BUTTON_WaitEvent(BUTTON_RELEASED);
                        default: break;
                    }
                }
                break;

            case CONFIG:
                if(!config(button)){
                    state = switchTo(IDLE);
                    RTC_SetValue(&saveddata.rtc); // RTC Is saved on flash when a game is saved
                }
                break;

            case GAME:
                if(button == BUTTON_S){
                    if(BUTTON_GetEvents() == BUTTON_PRESSED){
                        state = switchTo(SAVE);
                        break;
                    }
                }
                space(button);
                break;

            case SAVE:
            	displaySaveResult(0x30);
                LED_SetState(LED_ON);
                //printf("Score: %u\n",saveddata.spaceInvaders.score);
                // check if last score is top and insert it on table
                saveTopScore(saveddata.spaceInvaders.score, saveddata.topscores);
                // update gamedata with top score
                saveddata.spaceInvaders.topscore = saveddata.topscores[0];

                saveddata.checksum = generateChecksum(&saveddata.spaceInvaders, sizeof(GameData));

                res = saveData(&saveddata, sizeof(SaveData));

                LED_SetState(LED_OFF);

                displaySaveResult(res);

                state = switchTo(IDLE);
                break;
        }
    }
return 0;
}

/**
 * @brief Main Function, initialize all hardware and start tasks
 **/
int main(void){
portBASE_TYPE xres;
	 SYS_Init();

	    LCD_WriteString("Starting RTOS Tasks\n");

	    xres = xTaskCreate( MICRO_IP_Task, "Micro ip task", 0x200, NULL, 30, NULL );
	    if(!xres){
	    	LCD_WriteString("Unable to start Micro ip Task: ");
	    	LCD_WriteInt(xres,10);
	    }

	    xres = xTaskCreate( MAIN_App_Task, "Main app task", 0x200, NULL, 30, NULL );
	    	    if(!xres){
	    	    	LCD_WriteString("Unable to start main app Task: ");
	    	    	LCD_WriteInt(xres,10);
	    	    }
	    vTaskStartScheduler();
}
