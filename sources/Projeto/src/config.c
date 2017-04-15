#include <time.h>
#include <lcd.h>
#include <rtc.h>
#include <button.h>
#include <util.h>
#include "config.h"
#include "proj.h"

//TODO: fix field indicator and save to flash


static struct tm alarm, *cur_time;
static uint8_t field;
const char wdays[7][4]={"mon","tue","wed","thu","fri","sat","sun"};
static const char title[]={
    "           CONFIG\n\n"    
};

void PRINT_Field(uint32_t field, uint32_t format, uint8_t select){
    if(select)
        LCD_SetColors(CFG_BC, CFG_FC); // invert colors

    LCD_WriteInt(field, format);
    LCD_SetColors(CFG_FC, CFG_BC);
}

void PRINT_FullDate(struct tm *rtc, uint8_t select){
    POSITION_DATE;    
    
    PRINT_Field(rtc->tm_hour, TIME_FORMAT, (select == HOUR)? ON : OFF);
    LCD_WriteChar(':');
    PRINT_Field(rtc->tm_min, TIME_FORMAT, (select == MINUTES)? ON : OFF);
    LCD_WriteChar(' ');
    
    if(select == WEEKDAY)
        LCD_SetColors(CFG_BC, CFG_FC);
    LCD_WriteString((char*)&wdays[rtc->tm_wday][0]);
    LCD_SetColors(CFG_FC, CFG_BC);    
    LCD_WriteChar(' ');
    
    PRINT_Field(rtc->tm_mday, TIME_FORMAT, (select == MDAY)? ON : OFF);
    LCD_WriteChar('-');
    PRINT_Field(rtc->tm_mon, TIME_FORMAT, (select == MONTH)? ON : OFF);
    LCD_WriteChar('-');
    PRINT_Field(rtc->tm_year, YEAR_FORMAT, (select == YEAR)? ON : OFF);
}

//TODO FIX for year when overflow go from max to 0
void advanceField(uint32_t *fld, uint16_t max, signed char step){    
    *fld = (*fld + step);    
    if(step > 0)
        *fld = *fld % max;
    else if(*fld > max)
        *fld = max-1;
}

void setTime(struct tm *rtc, uint8_t fld, signed char step){  
    switch(fld){
        case HOUR:      advanceField((uint32_t *)&rtc->tm_hour, 24, step); break;
        case MINUTES:   advanceField((uint32_t *)&rtc->tm_min,  60, step); break;
        case WEEKDAY:   advanceField((uint32_t *)&rtc->tm_wday, 7,  step); break;        
        case MDAY:      advanceField((uint32_t *)&rtc->tm_mday, 32, step); break;        
        case MONTH:     advanceField((uint32_t *)&rtc->tm_mon,  13, step); break;        
        case YEAR:      advanceField((uint32_t *)&rtc->tm_year, 2060, step); break;
   }   
}

void popConfig(void *ptr){  
    field = HOUR;
    cur_time = (struct tm*)ptr;
    
    RTC_GetValue(cur_time);
    RTC_GetAlarmValue(&alarm);
 
    LCD_Clear(BLACK);
    LCD_SetColors(RED,BLACK);
    LCD_Goto(0,0);    
    LCD_WriteString((char*)title);
    LCD_SetColors(GREEN,BLACK);
    PRINT_FullDate(cur_time, field);
    BUTTON_SetHoldTime(500);
}

int config(int b){
signed char step;


    switch(BUTTON_GetEvents()){
        
        case BUTTON_PRESSED:
            if(b == BUTTON_R){
                if((++field) == (YEAR + 1))
                    field = HOUR;
                step = 0;
                break;               
            }            
            if(b == BUTTON_F) return 0;
            
        case BUTTON_HOLD:
            switch(b){
                case BUTTON_D: step = -1; break;
                case BUTTON_U: step =  1; break;
                default: return 1;
            }
            break;
    }

    setTime(cur_time, field, step);

    PRINT_FullDate(cur_time, field);    

    return 1;
}
