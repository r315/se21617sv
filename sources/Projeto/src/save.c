#include <string.h>
#include <stdint.h>
#if defined(__LPCX__) || defined(__BB__)
#include <eeprom.h>
#else
#include <flash.h>
#endif

#include "save.h"


#if defined(__LPCX__) || defined(__BB__)

int saveData(void *data, int dataSize){
	return EEPROM_Write(0, data, dataSize);
}

int restoreData(void *data, int dataSize){
	return EEPROM_Read(0, data, dataSize);
}
#else
#if defined(__EMU__)
static uint8_t SAVE_BLOCK[0x2000];
#else
#define SAVE_BLOCK (uint32_t)((FLASH_LAST_SECTOR - 2) * FLASH_SECTOR_SIZE)
#endif
int saveData(void *data, int dataSize){    
    FLASH_WriteData((void*)SAVE_BLOCK, data, dataSize);
    return FLASH_VerifyData((void*)SAVE_BLOCK, data, dataSize); 
}

int restoreData(void *data, int dataSize){
    memcpy(data, (void*)SAVE_BLOCK, dataSize);
    return CMD_SUCESS;
}
#endif


