/**
 * \addtogroup Tasks
 * @{
 */
#ifndef __task_config_h__
#define __task_config_h__

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

/**
 * @brief Funcao chamada pela task, para execução da funcionalidade de configuração
 */
void Task_Config(void *ptr);

#endif
