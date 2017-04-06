/**
* @file		eeprom.c
* @brief	Contains API source code for controlling a i2c eeprom
*     		
* @version	1.0
* @date		6 Abr, 2017
* @author	Hugo Reis
**********************************************************************/
#include <eeprom.h>
#include <i2c.h>

Eeprom eeprom;
I2C_Controller i2cifc;

void EEPROM_Init(void){
	I2C_Init(&i2cifc, I2C_IF0, EEPROM_MAX_CLOCK);
	eeprom.control =  EEPROM_CONTROL_BYTE;
	eeprom.address = 0;
}

int8_t EEPROM_Write(uint16_t address, uint8_t *data, uint32_t size){

	eeprom.address = address;
	eeprom.result = I2C_WriteControl(&i2cifc, (uint8_t*)&eeprom, sizeof(Eeprom));

	if(!eeprom.result)
			eeprom.result = I2C_Write(&i2cifc, data, size);

	return eeprom.result;
}

int8_t EEPROM_Read(uint16_t address, uint8_t *data, uint32_t size){

	eeprom.address = address;
	eeprom.result = I2C_WriteControl(&i2cifc, (uint8_t*)&eeprom, sizeof(Eeprom));

	if(!eeprom.result)
		eeprom.result = I2C_Read(&i2cifc, data, size);

	return eeprom.result;
}

