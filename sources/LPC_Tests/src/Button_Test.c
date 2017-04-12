#include <button.h>
#include <lcd.h>

void Button_Test(void){

		if(BUTTON_GetEvents()==BUTTON_PRESSED){
			LCD_Goto(0,0);
			switch(BUTTON_GetButton()){
			case BUTTON_F:
				LCD_WriteString("FIRE ");
				break;

			case BUTTON_L:
				LCD_WriteString("LEFT ");
				break;

			case BUTTON_R:
				LCD_WriteString("RIGHT");
				break;
			case BUTTON_U:
				LCD_WriteString("UP   ");
				break;
			case BUTTON_D:
				LCD_WriteString("DOWN ");
				break;
			}
		}


}
