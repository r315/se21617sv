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
	I2C_Init((I2C_Controller*)&eeprom, EEPROM_INTERFACE, I2C_DEFAULT_CLOCK , EEPROM_CONTROL_BYTE);
}

uint8_t EEPROM_GetIfNumber(void){
	return EEPROM_INTERFACE;
}

/**
 * @brief this state machine dedicated to process eeprom write and read sequences
 * Write  |S| dev+W | MSB | LSB | data | data+n |P|
 * Read   |S| dev+W | MSB | LSB |S| dev+R | data | data+n |P|
 **/
int8_t I2C_EEpromStateMachine(Eeprom *eeprom){

	switch(eeprom->i2cif->I2STAT)
	{
	case I2C_START:
		eeprom->i2cif->I2DAT = eeprom->device;
		eeprom->i2cif->I2CONCLR = I2C_SI | I2C_STA;
		eeprom->status = ADDRESS_HIGH;  //next state
		break;

	case I2C_REPEATEAD_START:
		eeprom->i2cif->I2DAT = eeprom->device | 1;  // SET READ BIT
		eeprom->i2cif->I2CONCLR = I2C_SI | I2C_STA;
		eeprom->status = DATA_READ;
		break;

	//SLA+W was transmitted, ACK from slave was received
	//address high must be sent
	case I2C_SLA_W_ACK:
		eeprom->i2cif->I2DAT = eeprom->address >> 8; 	// send address high
		eeprom->i2cif->I2CONCLR = I2C_SI | I2C_STA | I2C_STO;
		eeprom->status = ADDRESS_LOW;
		break;

	//Data was transmitted and ACK received,
	//if address write, send address low else send remaining data
	case I2C_DTA_W_ACK:
		switch(eeprom->status){
			case ADDRESS_LOW:
				eeprom->i2cif->I2DAT = eeprom->address; // send address low
				eeprom->i2cif->I2CONCLR = I2C_SI;
				eeprom->status = (eeprom->operation == DATA_WRITE) ? DATA_WRITE : REPEATED_START;
				break;

			case REPEATED_START:
				eeprom->i2cif->I2CONSET = I2C_STA;  // send Repeated start
				eeprom->i2cif->I2CONCLR = I2C_SI;
				eeprom->status = DATA_READ;
				break;

			case DATA_WRITE:
				if(eeprom->count){
					eeprom->i2cif->I2DAT = *(eeprom->data++);
					eeprom->i2cif->I2CONSET = I2C_AA;
					eeprom->count--;
				}else{
					eeprom->i2cif->I2CONSET = I2C_STO;
					eeprom->status = I2C_IDLE;
					eeprom->operation = I2C_IDLE;
				}
				eeprom->i2cif->I2CONCLR = I2C_SI;
				break;
			default:
				break;
		}
		break;

		case I2C_SLA_R_ACK:
			eeprom->i2cif->I2CONSET = I2C_AA;     //Send ACK for first received data
			eeprom->i2cif->I2CONCLR = I2C_SI | I2C_STA;
			eeprom->count--;
			break;
		case I2C_DTA_R_ACK:
			*(eeprom->data++) = eeprom->i2cif->I2DAT;
			eeprom->count--;
			if(eeprom->count){
				eeprom->i2cif->I2CONSET = I2C_AA; //Send ACK for next received data
			}else{
				eeprom->i2cif->I2CONCLR = I2C_AA; //Send NACK for next received data
			}
			eeprom->i2cif->I2CONCLR = I2C_SI;
			break;

		case I2C_SLA_W_NACK:
			eeprom->i2cif->I2CONCLR = I2C_STA | I2C_SI;
			eeprom->i2cif->I2CONSET = I2C_STO;
			eeprom->status = ERROR_SLA_W_NACK;
			eeprom->operation = I2C_IDLE;
			break;

		case I2C_DTA_W_NACK:
			//eeprom->i2cif->I2CONCLR = I2C_SI;
			eeprom->i2cif->I2CONSET = I2C_STO;
			eeprom->status = ERROR_DTA_W_NACK;
			eeprom->operation = I2C_IDLE;
			break;

		case I2C_SLA_R_NACK:
			eeprom->i2cif->I2CONCLR = I2C_STA | I2C_SI;
			eeprom->i2cif->I2CONSET = I2C_STO;
			eeprom->status = ERROR_SLA_R_NACK;
			eeprom->operation = I2C_IDLE;
			break;

		case I2C_DTA_R_NACK:
			*(eeprom->data) = eeprom->i2cif->I2DAT;  //save last received byte
			eeprom->i2cif->I2CONCLR = I2C_SI;
			eeprom->i2cif->I2CONSET = I2C_STO;
			eeprom->operation = I2C_IDLE;
			break;

		case I2C_SLA_LOST:
			eeprom->i2cif->I2CONCLR = I2C_SI | I2C_STA | I2C_STO;
			eeprom->operation = I2C_IDLE;
			break;

		case I2C_NO_INFO:
			eeprom->i2cif->I2CONCLR = I2C_I2EN;
			eeprom->i2cif->I2CONSET = I2C_I2EN;
			break;
	}

	return eeprom->operation;
}

int8_t EEPROM_Start(uint16_t address, uint8_t *data, uint32_t size){
	eeprom.address  = address;
	eeprom.count = size;
	eeprom.data = data;
	eeprom.i2cif->I2CONSET = I2C_STA;

	while(I2C_EEpromStateMachine(&eeprom) != I2C_IDLE ){
		while(!(eeprom.i2cif->I2CONSET & I2C_SI));
	}
	return eeprom.status;
}

int8_t EEPROM_Write(uint16_t address, uint8_t *data, uint32_t size){
int8_t status;

	while(size){
		eeprom.operation = DATA_WRITE;
		if(size > EEPROM_PAGE_SIZE){
			status =  EEPROM_Start(address, data, EEPROM_PAGE_SIZE);
			size -= EEPROM_PAGE_SIZE;
			address += EEPROM_PAGE_SIZE;
			data += EEPROM_PAGE_SIZE;
		}else{
			status =  EEPROM_Start(address, data, size);
			size -= size;
		}
		TIME_DelayMs(5);
		if(status)
			break;
	}
	return status;
}

int8_t EEPROM_Read(uint16_t address, uint8_t *data, uint32_t size){
	eeprom.operation = DATA_READ;
	return EEPROM_Start(address, data, size);
}

