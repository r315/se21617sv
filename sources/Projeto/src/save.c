#include <string.h>
#include <stdint.h>
#include <flash.h>
#include "save.h"


#if !defined(__EMU__)
#define SAVE_BLOCK (uint32_t)((FLASH_LAST_SECTOR - 2) * FLASH_SECTOR_SIZE)
#else
static uint8_t SAVE_BLOCK[0x2000];
#endif


int saveData(void *data, int dataSize){    
    FLASH_WriteData((void*)SAVE_BLOCK, data, dataSize);
    return FLASH_VerifyData((void*)SAVE_BLOCK, data, dataSize); 
}

int restoreData(void *data, int dataSize){
    memcpy(data, (void*)SAVE_BLOCK, dataSize);
    return CMD_SUCESS;
}
