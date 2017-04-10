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
#include <time_m3.h>

I2C_Controller i2cifc;

void EEPROM_Init(void){
	I2C_Init(&i2cifc, I2C_IF0, EEPROM_MAX_CLOCK, EEPROM_CONTROL_BYTE);
}

int8_t I2C_StateMachine(I2C_Controller *i2cifc);

int8_t EEPROM_Write(uint16_t address, uint8_t *data, uint32_t size){
	i2cifc.address  = address;
	i2cifc.count = size;
	i2cifc.data = data;
	i2cifc.operation = ADDRESS_WRITE;
	i2cifc.status = I2C_START;
	i2cifc.interface->I2CONSET = I2C_STA;

	while(I2C_StateMachine(&i2cifc) != IDLE ){
		while(!(i2cifc.interface->I2CONSET & I2C_SI));
	}

	return 0;
}

int8_t EEPROM_Read(uint16_t address, uint8_t *data, uint32_t size){

	i2cifc.address  = address;
	i2cifc.count = 0;
	i2cifc.data = data;
	i2cifc.operation = ADDRESS_WRITE;
	i2cifc.status = SLA_ADDR;
	i2cifc.interface->I2CONSET = I2C_STA;

	while(I2C_StateMachine(&i2cifc) != IDLE ){
		while(!(i2cifc.interface->I2CONSET & I2C_SI));
	}

	TIME_DelayMs(1);

	i2cifc.count = 4;
	i2cifc.operation = DATA_READ;
	i2cifc.interface->I2CONSET = I2C_STA;

	while(I2C_StateMachine(&i2cifc) != IDLE ){
			while(!(i2cifc.interface->I2CONSET & I2C_SI));
	}

return 0;
}

