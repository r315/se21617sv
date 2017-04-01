#include <rtc.h>

#if defined(__LPC17XX__)
void RTC_Init(struct tm *dateTime){

}

void RTC_GetValue(struct tm *dateTime){

}

void RTC_SetValue(struct tm *dateTime){

}

void RTC_GetAlarmValue(struct tm *dateTime){

}

void RTC_SetAlarmValue(struct tm *dateTime){

}

uint32_t RTC_CheckAlarm(void){
   return 0;
}

void RTC_ClearAlarm(void){

}

void RTC_ActivateAlarm(uint8_t alarm){

}

void RTC_DeactivateAlarm(uint8_t alarm){

}


#else
#include <clock.h>

void RTC_Init(struct tm *dateTime){

	RTC_PowerUp();
	RTC->CCR = 2;	// stop clock and reset
	RTC->PREINT = (CLOCK_GetPCLK() / RTC_DIVIDER) - 1;
	RTC->PREFRAC = CLOCK_GetPCLK() - ((RTC->PREINT + 1) * RTC_DIVIDER);
	RTC->ILR = 3;
	RTC->CIIR = 0; /* Counter Increment Interrupt Disable */
	RTC_DeactivateAlarm(RTC_AMR_OFF);
	RTC_SetValue(dateTime);
}

void RTC_GetValue(struct tm *dateTime){
volatile uint64_t *consolidate = (uint64_t*) &RTC->CTIME0;
uint64_t rtcval;

	do{
		rtcval = *consolidate;
	}while(rtcval != *consolidate);

	dateTime->tm_sec = rtcval & 0x3F;  		  // 0-59 range
	dateTime->tm_min = (rtcval>>8) & 0x3F;
	dateTime->tm_hour = (rtcval>>16) & 0x3F;
	dateTime->tm_wday = (rtcval>>24) & 0x07;  //0-6 range

	dateTime->tm_mday = (rtcval>>32) & 0x1F;  //1-31 range
	dateTime->tm_mon = (rtcval>>40) & 0x0F;   //1-12 range
	dateTime->tm_year = (rtcval>>48) & 0x0FFF;//0-4095 range

	dateTime->tm_yday = RTC->CTIME2 & 0x07FF; //1-365 range
}

void RTC_SetValue(struct tm *dateTime){
	RTC->CCR = 0;	// stop clock
	
	RTC->SEC = dateTime->tm_sec % 60;
	RTC->MIN = dateTime->tm_min % 60;
	RTC->HOUR = dateTime->tm_hour % 24;
	RTC->DOM = (dateTime->tm_mday > 1 && dateTime->tm_mday < 32)? dateTime->tm_mday : 1;
	RTC->MONTH = (dateTime->tm_mon > 1 && dateTime->tm_mon < 13)? dateTime->tm_mon : 1;
	RTC->DOW = (dateTime->tm_wday > 1 && dateTime->tm_wday < 7)? dateTime->tm_wday : 0;
	RTC->YEAR = (dateTime->tm_year > 0 && dateTime->tm_year < 4096)? dateTime->tm_year : 1900;
	RTC->DOY = (dateTime->tm_yday > 1 && dateTime->tm_yday < 366)? dateTime->tm_yday : 1;
	
	RTC->CCR = RTC_CLKEN;
}

void RTC_GetAlarmValue(struct tm *dateTime){
	dateTime->tm_sec = RTC->ALSEC;
	dateTime->tm_min = RTC->ALMIN;
	dateTime->tm_hour = RTC->ALHOUR;
	dateTime->tm_mday = RTC->ALDOM;
	dateTime->tm_mon = RTC->ALMONTH;
	dateTime->tm_wday = RTC->ALDOW;
	dateTime->tm_year = RTC->ALYEAR;	
	dateTime->tm_yday = RTC->ALDOY;
}

void RTC_SetAlarmValue(struct tm *dateTime){
	RTC->ALSEC = dateTime->tm_sec % 60;
	RTC->ALMIN = dateTime->tm_min % 60;
	RTC->ALHOUR = dateTime->tm_hour % 24;
	RTC->ALDOM = (dateTime->tm_mday > 1 && dateTime->tm_mday < 32)? dateTime->tm_mday : 1;
	RTC->ALMONTH = (dateTime->tm_mon > 1 && dateTime->tm_mon < 13)? dateTime->tm_mon : 1;
	RTC->ALDOW = (dateTime->tm_wday > 1 && dateTime->tm_wday < 7)? dateTime->tm_wday : 0;
	RTC->ALYEAR =  (dateTime->tm_year > 0 && dateTime->tm_year < 4096)? dateTime->tm_year : 1900;	
	RTC->ALDOY = (dateTime->tm_yday > 1 && dateTime->tm_yday < 366)? dateTime->tm_yday : 1;
}

uint32_t RTC_CheckAlarm(void){
   return RTC->ILR & RTC_RTCALF;
}

void RTC_ClearAlarm(void){
	 RTC->ILR |= RTC_RTCALF;
}

void RTC_ActivateAlarm(uint8_t alarm){
	RTC->AMR &= ~alarm;
}

void RTC_DeactivateAlarm(uint8_t alarm){
	RTC->AMR |= alarm;
}

#endif



