/**
* @file		eeprom.c
* @brief	Contains API source code for controlling a i2c eeprom
*     		
* @version	1.0
* @date		6 Abr, 2017
* @author	Hugo Reis
**********************************************************************/
#include <eeprom.h>
#include <time_m3.h>

Eeprom eeprom;

void EEPROM_Init(void){
	I2C_Init((I2C_Controller*)&eeprom, I2C_IF0, EEPROM_MAX_CLOCK, EEPROM_CONTROL_BYTE);
}

//int8_t I2C_StateMachine(I2C_Controller *i2cifc);

int8_t I2C_EEpromStateMachine(Eeprom *eeprom){

	switch(eeprom->i2cif->I2STAT)
	{
	case I2C_START:
		eeprom->i2cif->I2DAT = eeprom->device;
		eeprom->i2cif->I2CONCLR = I2C_SI | I2C_STA;
		break;

	case I2C_REPEATEAD_START:
		eeprom->i2cif->I2DAT = eeprom->device | 1;  // SET READ BIT
		eeprom->i2cif->I2CONCLR = I2C_SI | I2C_STA;
		break;

	//SLA+W was transmitted, ACK from slave was received
	//address high must be sent
	case I2C_SLA_W_ACK:
		eeprom->status = ADDRESS_HIGH;
		eeprom->i2cif->I2DAT = eeprom->address >> 8;
		eeprom->i2cif->I2CONCLR = I2C_SI | I2C_STA | I2C_STO;
		break;

	//Data was transmitted and ACK received,
	//if address write, send address low else send remaining data
	case I2C_DTA_W_ACK:
		switch(eeprom->status){
			case ADDRESS_HIGH:
				eeprom->status = DATA_WRITE;            // next byte is data
				eeprom->i2cif->I2DAT = eeprom->address; // for now send address low
				eeprom->i2cif->I2CONCLR = I2C_SI;
				break;

			case DATA_WRITE:
				if(eeprom->count){
					eeprom->i2cif->I2DAT = *(eeprom->data++);
					eeprom->i2cif->I2CONSET = I2C_AA;
					eeprom->count--;
				}else{
					eeprom->status = IDLE;
					eeprom->i2cif->I2CONSET = I2C_STO;
				}
				eeprom->i2cif->I2CONCLR = I2C_SI;
				break;
			default:
				break;
		}
		break;

		case I2C_SLA_R_ACK:
		case I2C_DTA_R_ACK:
			*(eeprom->data++) = eeprom->i2cif->I2DAT;
			eeprom->count--;
			if(eeprom->count){
				eeprom->i2cif->I2CONSET = I2C_AA;
			}else{
				eeprom->i2cif->I2CONCLR = I2C_AA;
			}

			eeprom->i2cif->I2CONCLR = I2C_SI;
			break;

		case I2C_SLA_W_NACK:
			eeprom->status = IDLE;
			eeprom->i2cif->I2CONCLR = I2C_STA | I2C_SI;
			eeprom->i2cif->I2CONSET = I2C_STO;
			break;

		case I2C_DTA_W_NACK:
			eeprom->status = IDLE;
			eeprom->i2cif->I2CONCLR = I2C_SI;
			eeprom->i2cif->I2CONSET = I2C_STO;
			break;

		case I2C_SLA_R_NACK:
			eeprom->status = IDLE;
			eeprom->i2cif->I2CONCLR = I2C_STA | I2C_SI;
			eeprom->i2cif->I2CONSET = I2C_STO;
			break;

		case I2C_DTA_R_NACK:
			eeprom->status = IDLE;
			eeprom->i2cif->I2CONCLR = I2C_SI;
			eeprom->i2cif->I2CONSET = I2C_STO;
			break;

		case I2C_SLA_LOST:
			eeprom->status = IDLE;
			eeprom->i2cif->I2CONCLR = I2C_SI | I2C_STA | I2C_STO;
			break;

		case I2C_NO_INFO:
			eeprom->i2cif->I2CONCLR = I2C_I2EN;
			eeprom->i2cif->I2CONSET = I2C_I2EN;
			break;
	}

	return eeprom->status;
}

int8_t EEPROM_Write(uint16_t address, uint8_t *data, uint32_t size){
	eeprom.address  = address;
	eeprom.count = size;
	eeprom.data = data;
	eeprom.operation = ADDRESS_WRITE;
	eeprom.i2cif->I2CONSET = I2C_STA;

	while(I2C_EEpromStateMachine(&eeprom) != IDLE ){
		while(!(eeprom.i2cif->I2CONSET & I2C_SI));
	}

	return 0;
}

int8_t EEPROM_Read(uint16_t address, uint8_t *data, uint32_t size){

	eeprom.address  = address;
	eeprom.count = 0;
	eeprom.data = data;
	eeprom.operation = ADDRESS_WRITE;


return 0;
}

