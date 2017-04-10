/**
 * @file		i2c.c
 * @brief	Contains API source code for controlling a i2c bus
 *
 * @version	1.0
 * @date		6 Abr, 2017
 * @author	Hugo Reis
 **********************************************************************/

#include <i2c.h>

void I2C_Init(I2C_Controller *i2cifc, uint8_t ifNumber, uint32_t freq, uint8_t dev){

	switch(ifNumber){
	default :
	case I2C_IF0:
		i2cifc->interface = LPC_I2C0;
		LPC_SC->PCONP |= I2C0_ON;
		LPC_SC->PCLKSEL0 |= (3<<14); //CCLK / 8
		I2C0_ConfigPins();
		break;
	case I2C_IF1:
		i2cifc->interface = LPC_I2C1;
		LPC_SC->PCONP |= I2C1_ON;
		LPC_SC->PCLKSEL1 |= (3<<6); //CCLK / 8
		I2C1_ConfigPins();
		break;
	case I2C_IF2:
		i2cifc->interface = LPC_I2C2;
		LPC_SC->PCONP |= I2C2_ON;
		LPC_SC->PCLKSEL1 |= (3<<20); //CCLK / 8
		I2C2_ConfigPins();
		break;
	}

	i2cifc->device = dev;

	i2cifc->interface->I2SCLH = 128;
	i2cifc->interface->I2SCLL = 128;

	i2cifc->interface->I2CONSET = I2C_I2EN ;
	i2cifc->interface->I2CONCLR = I2C_STA | I2C_STO | I2C_SI;
	i2cifc->status = IDLE;
}

/**
 * @brief this state machine only process write and read sequences
 * Write  |S|dev+W| data | data+n |P|
 * Read   |S|dev+R| data | data+n |P|
 **/
int8_t I2C_StateMachine(I2C_Controller *i2cifc){

	switch(i2cifc->interface->I2STAT)
	{
	case I2C_START:
	case I2C_REPEATEAD_START:
		if(i2cifc->operation == DATA_READ){
			i2cifc->interface->I2DAT = i2cifc->device | 1;
		}
		else{
			i2cifc->interface->I2DAT = i2cifc->device;
		}
		i2cifc->status = SLA_ADDR;
		i2cifc->interface->I2CONCLR = I2C_SI | I2C_STA;
		break;

	//SLA+W was transmitted, ACK from slave was received
	//if address write operation, send address high, else send first data
	case I2C_SLA_W_ACK:
		if(i2cifc->operation == ADDRESS_WRITE){
			i2cifc->status = ADDRESS_HIGH;
			i2cifc->interface->I2DAT = i2cifc->address>>8;
			i2cifc->interface->I2CONCLR = I2C_SI | I2C_STA | I2C_STO;
			break;
		}
		i2cifc->operation = DATA_WRITE; // set status to data write
		i2cifc->status = DATA_WRITE;

	//Data was transmitted and ACK received,
	//if address write, send address low else send remaining data
	case I2C_DTA_W_ACK:
		switch(i2cifc->status){
			case ADDRESS_HIGH:
				i2cifc->status = ADDRESS_LOW;
				i2cifc->interface->I2DAT = i2cifc->address;
				i2cifc->interface->I2CONCLR = I2C_SI;
				break;

			case ADDRESS_LOW:
				i2cifc->status = DATA_WRITE;
			case DATA_WRITE:
				if(i2cifc->count){
					i2cifc->interface->I2DAT = *(i2cifc->data++);
					i2cifc->interface->I2CONSET = I2C_AA;
					i2cifc->count--;
				}else{
					i2cifc->status = IDLE;
					i2cifc->interface->I2CONSET = I2C_STO;
				}
				i2cifc->interface->I2CONCLR = I2C_SI;
				break;
			default:
				break;
		}
		break;

		case I2C_SLA_R_ACK:
		case I2C_DTA_R_ACK:
			*(i2cifc->data++) = i2cifc->interface->I2DAT;
			i2cifc->count--;
			if(i2cifc->count){
				i2cifc->interface->I2CONSET = I2C_AA;
			}else{
				i2cifc->interface->I2CONCLR = I2C_AA;
			}

			i2cifc->interface->I2CONCLR = I2C_SI;
			break;

		case I2C_SLA_W_NACK:
			i2cifc->status = IDLE;
			i2cifc->interface->I2CONCLR = I2C_STA | I2C_SI;
			i2cifc->interface->I2CONSET = I2C_STO;
			break;

		case I2C_DTA_W_NACK:
			i2cifc->status = IDLE;
			i2cifc->interface->I2CONCLR = I2C_SI;
			i2cifc->interface->I2CONSET = I2C_STO;
			break;

		case I2C_SLA_R_NACK:
			i2cifc->status = IDLE;
			i2cifc->interface->I2CONCLR = I2C_STA | I2C_SI;
			i2cifc->interface->I2CONSET = I2C_STO;
			break;

		case I2C_DTA_R_NACK:
			i2cifc->status = IDLE;
			i2cifc->interface->I2CONCLR = I2C_SI;
			i2cifc->interface->I2CONSET = I2C_STO;
			break;

		case I2C_SLA_LOST:
			i2cifc->status = IDLE;
			i2cifc->interface->I2CONCLR = I2C_SI | I2C_STA | I2C_STO;
			break;

		case I2C_NO_INFO:
			i2cifc->interface->I2CONCLR = I2C_I2EN;
			i2cifc->interface->I2CONSET = I2C_I2EN;
			break;
	}

	return i2cifc->status;
}


