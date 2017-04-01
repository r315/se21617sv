#include <lcd.h>
#include <time.h>
#include <rtc.h>
#include <button.h>
#include "idle.h"
#include "proj.h"
#include <util.h>

//#include "config.h"
//#include "space.h"

static const char title[]={
    "           IDLE\n\n"
    "F   -  start new Game\n"
    "S   -  Load Game\n"
    "L&R -  for config\n\n"
    "Top Scores:\n"
};

static uint32_t updateTime;

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
void PRINT_HighScores(uint32_t x, uint32_t y, uint32_t *scores){   
uint8_t n;    
    for(n = 0; n < MAX_TOP_SCORES; n++){
        LCD_Goto(x,y);
        LCD_WriteInt(scores[n], SCORES_FORMAT);
        y += LCD_GetFontHeight();        
    }
}

void popIdle(void *ptr){      
    updateTime = TIME_GetValue() - 1000;  //force update
    LCD_Clear(BLACK);
    LCD_SetColors(RED,BLACK);
    LCD_Goto(0,0);    
    LCD_WriteString((char*)title);    
    
    LCD_SetColors(YELLOW,BLACK);
    PRINT_HighScores(96,112,(uint32_t *)ptr);
    LCD_SetColors(GREEN,BLACK);   
    BUTTON_SetHoldTime(ENTER_CONFIG_TIME);
}

void idle(void){
struct tm rtc;    
    if( TIME_Elapsed(updateTime) > 1000){
        RTC_GetValue(&rtc);   
        LCD_Goto(0,LCD_H-16);        
        PRINT_DateTime(&rtc);                
        updateTime = TIMER0_GetValue();
    }
}
