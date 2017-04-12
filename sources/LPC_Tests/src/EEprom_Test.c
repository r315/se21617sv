#include <button.h>
#include <eeprom.h>
#include <lcd.h>
#include <string.h>

#define W_SIZ 1024
uint8_t data1[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ#$%&#="};
//I2C_Controller i2cbus;
extern uint8_t rdata[];
uint8_t buf[1024];
uint8_t *dst, *src;

int main(void);

int8_t verify(void *src, void *dst, uint32_t siz){
	while(siz){
		if(*((uint8_t*)src) != *((uint8_t*)dst))
			return 1;
		siz -= 1;
		src += 1;
		dst += 1;
	}
	return 0;
}


void EEprom_Test(void){

	EEPROM_Init();

	src = rdata;
	dst = buf;

	memcpy(dst,src,W_SIZ);
	//src = data1;
	EEPROM_Write(0,src, W_SIZ);

	memset(dst,0,W_SIZ);
	EEPROM_Read(0,dst,W_SIZ);

	if(verify(src,dst,W_SIZ))
	//if(memcmp(src,dst,W_SIZ/sizeof(uint32_t)))
		LCD_WriteString("\n ##EEprom Write Fail##");
	else
		LCD_WriteString("\n EEprom Write Pass!");
	//I2C_Init(&i2cbus,I2C_IF0,EEPROM_MAX_CLOCK, EEPROM_CONTROL_BYTE);


}

