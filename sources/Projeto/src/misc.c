#include <stdint.h>
#include <led.h>
#include <lcd.h>
#include <misc.h>

/**
  * @brief Contagem de frames por segundo
  * */
void MISC_countFps(void) {
	static int secondsCount = 0;
	static uint8_t fps = 0;
	if (TIME_GetValue() > secondsCount) {
		LCD_Goto(0, 0);
		LCD_WriteInt(fps, 10);
		LCD_WriteString("FPS  ");
		secondsCount = TIME_GetValue() + 1000;
		fps = 0;
	}
	fps++;
}


/**
 * @brief gera o checksum
 */
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

/**
 * @brief calcula o checksum
 */
uint8_t checksumData(void *data, uint32_t size, uint8_t checksum){
uint8_t sum = 0;
    while(size--){
        sum += *((uint8_t*)data++);
    }
    //printf("Calculated Checksum %u\n", sum);
    return (uint8_t)(sum + (checksum + 1));
}

 /**
  * @brief Calcula o numero de pixels que uma string ocupa no display
  * @param str: String
  * @return Tamanho em pixels da string
  * */
uint32_t strlenInPixels(char *str){
uint8_t count = 0;
    while(*str++)
        count++;
    return count * LCD_GetFontWidth();
}

 /**
  * @brief Apenas para testes de performance
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
