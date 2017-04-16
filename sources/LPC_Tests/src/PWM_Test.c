#include <lcd.h>
#include <button.h>
#include <time_m3.h>
#include <pwm.h>

typedef struct _field{
	uint16_t x,y;
	uint32_t value;
	char *postfix;
	uint32_t min;
	uint32_t max;
	uint8_t base;
}Field;
Field duty;

#define PWM_CHANNEL PWM_2
#define PWM_P 100

void GEN_PrintField(Field *fld){
	LCD_Goto(fld->x,fld->y);
	LCD_WriteInt(fld->value, fld->base);
	LCD_WriteString(fld->postfix);
}

void GEN_ChangeVar(Field *fld){
	if(BUTTON_GetEvents() == BUTTON_PRESSED || BUTTON_GetEvents() == BUTTON_HOLD){
		switch(BUTTON_GetButton()){
			case BUTTON_U:
				if(fld->value < fld->max)
					fld->value++;
				break;
			case BUTTON_D:
			if(fld->value > 0)
					fld->value--;
				break;
			default:
				break;
		}
		GEN_PrintField(fld);
		PWM_Set(PWM_CHANNEL,fld->value);
	}
}

void PWM_TestInit(void){
	BUTTON_SetHoldTime(200);
	LCD_Clear(BLACK);
	duty.x = 0;
	duty.y = LCD_GetFontHeight();
	duty.postfix = "%  ";
	duty.value = 50;
	duty.min = 0;
	duty.max = 100;
	duty.base = 10;
	LCD_Goto(0,0);
	LCD_WriteString("PWM1.2");
	GEN_PrintField(&duty);
	PWM_Init(PWM_P);
}

void PWM_Test(void){
static uint32_t uitime = 0;
	if(uitime < TIME_GetValue()){
		BUTTON_Hit();
		GEN_ChangeVar(&duty);
		uitime = TIME_GetValue() + 30;
	}
}
