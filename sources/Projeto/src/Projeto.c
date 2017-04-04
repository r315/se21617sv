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
#include <flash.h>
#include <rtc.h>
#include <util.h>
#include "proj.h"
#include "system.h"
#include "save.h"
#include "config.h"
#include "space.h"
#include "idle.h"

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
 /**
  * @brief if score is a top score insert it on scores table.
  * @param score: score of last game
  * @param scorestab: ponter to array contining the 3 best scores
  * */
void saveTopScore(uint32_t score, uint32_t *scorestab){
uint8_t n;
uint32_t *p1,*p2;
    for(n = 0; n < MAX_TOP_SCORES; n++){
        if(score > scorestab[n]){
            p1 = &scorestab[MAX_TOP_SCORES-2];
            p2 = &scorestab[MAX_TOP_SCORES-1];
            while( (p2) != &scorestab[n]){
                *p2 = *p1;
                p1 -= 1;
                p2 -= 1;
            }
            scorestab[n] = score;
            return;
        }
    }
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

void displaySaveResult(uint32_t res){
char *msg;

    switch(res){
        case CMD_SUCESS:
            msg = "Save ok"; break;

        case SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION :
            LCD_WriteString("Sector Not Ready");break;

        case INVALID_SECTOR:
            msg = "Invalid Sector";break;

        case SRC_ADDR_ERROR:
            msg = "Invalid Start Address";break;

        case DST_ADDR_ERROR:
            msg = "Invalid Destination Address";break;

        case COUNT_ERROR:
            msg = "Count error";break;

        case BUSY:
            msg = "Busy";break;

        case COMPARE_ERROR:
            msg = "Compare Error";break;

        default:
                LCD_Goto(0,0);
                LCD_WriteString("ERROR: 0x");
                LCD_WriteInt(res,(2<<8) | 16);
                LCD_WriteChar(' ');
                TIME_DelayMs(1500);
                return;
    }

    LCD_Goto( (LCD_W/2) - (strlenInPixels(msg)/2), LCD_H /2);
    LCD_WriteString(msg);
    TIME_DelayMs(1500);
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

 /**	
 * @brief main function
 **/
int main(void){
State state;
uint32_t button,res;

    SYS_Init();
    
    restoreData(&saveddata,sizeof(SaveData));
    
    RTC_SetValue(&saveddata.rtc);
    
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
                        case (BUTTON_L| BUTTON_R):
                            state = switchTo(CONFIG);
                            break;

                        case BUTTON_R:  //Erase Top Scores
                            memset(&saveddata, 0, sizeof(SaveData));
                            //saveData(&saveddata, sizeof(SaveData));
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
                LED_SetState(LED_ON);
                //printf("Score: %u\n",saveddata.spaceInvaders.score);
                saveTopScore(saveddata.spaceInvaders.score, saveddata.topscores);

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
