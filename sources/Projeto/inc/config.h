#ifndef _config_h_
#define _config_h_

#define CFG_FC GREEN
#define CFG_BC BLACK
#define POSITION_DATE LCD_Goto(16,64);

enum TimeFields{
   HOUR,
   MINUTES,   
   WEEKDAY,   
   MDAY,
   MONTH,
   YEAR   
};

enum RtcFormat{
   RTC_DATETIME,
   RTC_TIME_HHMM,
   RTC_TIME_HHMMSS,
   RTC_DATE, 
   RTC_WDAY  
};

void popConfig(void *ptr);
int config(void);
#endif
