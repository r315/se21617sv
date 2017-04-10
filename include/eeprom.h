/**
* @file		eeprom.h
* @brief	Contains the eeprom API header.
*     		
* @version	1.0
* @date		6 Apr. 2017
* @author	Hugo Reis
**********************************************************************/

#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdint.h>

typedef struct _eeprom{
	uint16_t address;
	int8_t result;
}Eeprom;

#define EEPROM_MAX_CLOCK     400000 //400khz for 24AA256 (4A256)
#define EEPROM_CONTROL_BYTE  0xA0
#define EEPROM_CONTROL_SIZE  3
#define EEPROM_ADDRESS_SIZE  2

/**
* @brief Inicializa a eeprom, indicando o interface a que se encontra ligado
*/
void EEPROM_Init(void);

/**
* @brief Escreve dados de um buffer para a memoria eerprom
* @return 0 no errors
*/
int8_t EEPROM_Write(uint16_t address, uint8_t *data, uint32_t size);

/**
* @brief Lê dados da eeprom para um buffer
* @return 0 no errors
**/
int8_t EEPROM_Read(uint16_t address, uint8_t *data, uint32_t size);

#endif
