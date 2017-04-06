/**
* @file		i2c.h
* @brief	Contains the i2c API header.
*     		
* @version	1.0
* @date		6 Apr. 2017
* @author	Hugo Reis
**********************************************************************/

#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include <LPC17xx.h>

#define I2C_IF0 0
#define I2C_IF1 1
#define I2C_IF2 2

#define I2C0_ON (1<<7)  //PCI2C0
#define I2C1_ON (1<<19) //PCI2C1
#define I2C2_ON (1<<26) //PCI2C2

typedef struct _i2cControl{
	LPC_I2C_TypeDef *interface;
	uint8_t status;
}I2C_Controller;

//P0.27 -> SDA
//P0.28 -> SCL
#define I2C0_ConfigPins()            \
	LPC_PINCON->PINSEL1 &= ~(3<<22); \
	LPC_PINCON->PINSEL1 &= ~(3<<24); \
	LPC_PINCON->PINSEL1 |= ~(1<<22); \
	LPC_PINCON->PINSEL1 |= ~(1<<24); \

//P0.0 -> SDA
//P0.1 -> SCL
#define I2C1_ConfigPins()            \
	LPC_PINCON->PINSEL1 |= ~(3<<0);  \
	LPC_PINCON->PINSEL1 |= ~(3<<2);  \

//P0.10 -> SDA
//P0.11 -> SCL
#define I2C2_ConfigPins()            \
	LPC_PINCON->PINSEL1 &= ~(3<<22); \
	LPC_PINCON->PINSEL1 &= ~(3<<24); \
	LPC_PINCON->PINSEL1 |= ~(2<<22); \
	LPC_PINCON->PINSEL1 |= ~(2<<24); \

#define I2C_AA   (1<<2)
#define I2C_SI   (1<<3)
#define I2C_STO  (1<<4)
#define I2C_STA  (1<<5)
#define I2C_I2EN (1<<6)

#define I2C_S_OK 0x08




/**
* @brief Faz a iniciação do interface i2c (0-2) e define o ritmo de clock
*
*/
void I2C_Init(I2C_Controller *i2cifc, uint8_t ifNumber, uint32_t freq);

/**
* @brief Escreve control data, this sends START + CONTROL
*/
int8_t I2C_WriteControl(I2C_Controller *i2cifc, uint8_t *control, uint32_t size);

/**
* @brief Escreve dados de um buffer para o bus
*/
int8_t I2C_Write(I2C_Controller *i2cifc, uint8_t *data, uint32_t size);

/**
* @brief Lê dados do bus i2c para um buffer
**/
int8_t I2C_Read(I2C_Controller *i2cifc, uint8_t *data, uint32_t size);

#endif
