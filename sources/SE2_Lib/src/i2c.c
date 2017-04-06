/**
* @file		i2c.c
* @brief	Contains API source code for controlling a i2c bus
*     		
* @version	1.0
* @date		6 Abr, 2017
* @author	Hugo Reis
**********************************************************************/

#include <i2c.h>


void I2C_Init(I2C_Controller *i2cifc, uint8_t ifNumber, uint32_t freq){

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

	i2cifc->interface->I2CONSET = I2C_I2EN | I2C_SI ;
	i2cifc->interface->I2CONCLR = I2C_STA | I2C_STO;
	i2cifc->status = I2C_IDLE;

}

int8_t I2C_WriteControl(I2C_Controller *i2cifc, uint8_t *control, uint32_t size){
	//if(i2cifc->interface->I2STAT != I2C_S_OK ) return i2cifc->interface->I2STAT; // must be on correct state
	i2cifc->interface->I2CONSET = I2C_STA;
	i2cifc->interface->I2CONCLR = I2C_SI;
	while(!(i2cifc->interface->I2STAT & I2C_SI));
	if(i2cifc->interface->I2STAT != I2C_S_OK ) return i2cifc->interface->I2STAT;
	i2cifc->interface->I2DA = *control;      // send control0
	i2cifc->interface->I2CONCLR = I2C_STA;
	i2cifc->interface->I2CONCLR = I2C_SI;
	while(!(i2cifc->interface->I2STAT & I2C_SI));
	if(i2cifc->interface->I2STAT != I2C_SLA_W ) return i2cifc->interface->I2STAT;

}

int8_t I2C_Write(I2C_Controller *i2cifc, uint8_t *data, uint32_t size){

}

int8_t I2C_Read(I2C_Controller *i2cifc, uint8_t *data, uint32_t size){

}
