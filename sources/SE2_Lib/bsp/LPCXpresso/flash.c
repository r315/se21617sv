#include <flash.h>

#if defined(__LPC17XX__)
#define CLOCK_GetCCLK() SystemCoreClock
#else
#include <clock.h>
#endif


uint32_t __command[5], __result[3];

/**
* @brief call's IAP function in thumb mode.
* @param pointer to command an parameter table
* @param pointer to result table
**/
__attribute__ ((naked)) void iap_entry(uint32_t *command, uint32_t *result){    
    asm("ldr r2,[pc]");
    asm("bx  r2");
    asm(".word 0x7FFFFFF1");
}

/**
* @brief prepares sector(s) for errase or write
* @param start sector
* @param end sector
* @return CMD_SUCCESS | BUSY | INVALID_SECTOR
**/
uint32_t prepare_sector(uint32_t start, uint32_t end){
    __command[0] = FLASH_CMD_PREPARE_SECTOR;
    __command[1] = start;
    __command[2] = end;
    __command[3] = CLOCK_GetCCLK() / 1000;  //clock specified in Khz
    iap_entry(__command,__result);
    return __result[0];
}

/**
 * @return CMD_SUCCESS | BUSY | SECTOR_NOT_BLANK | INVALID_SECTOR
 **/
 uint32_t blank_check(uint32_t start, uint32_t end){
    __command[0] = FLASH_CMD_BLANK_CHECK;
    __command[1] = start;
    __command[2] = end;    
    iap_entry(__command,__result);
    return __result[0];
 }


unsigned int FLASH_EraseSectors(unsigned int startSector, unsigned int endSector){
    if( prepare_sector(startSector,endSector) == CMD_SUCESS){
        __command[0] = FLASH_CMD_ERASE_SECTOR;
        __command[1] = startSector;
        __command[2] = endSector;
        __command[3] = CLOCK_GetCCLK() / 1000;
        iap_entry(__command,__result);
    }
    return __result[0];
}

/**
 * @return CMD_SUCCESS | SRC_ADDR_ERROR | DST_ADDR_ERROR | SRC_ADDR_NOT_MAPPED |
 *         DST_ADDR_NOT_MAPPED | COUNT_ERROR | SECTOR_NOT_PREPARED_FOR_WRITE_OPERATIONm | BUSY.
 * 
 * */
unsigned int FLASH_WriteBlock( void *dstAddr, void *srcAddr, unsigned int size){
uint32_t startSector, endSector;
   
    startSector = (uint32_t)dstAddr / FLASH_SECTOR_SIZE;
    endSector = ((uint32_t)dstAddr + size) / FLASH_SECTOR_SIZE;    
        
    if(prepare_sector(startSector,endSector) == CMD_SUCESS){
       __command[0] = FLASH_CMD_RAM_TO_FLASH;
       __command[1] = (uint32_t)dstAddr;
       __command[2] = (uint32_t)srcAddr;
       __command[3] = size;
       __command[4] = CLOCK_GetCCLK() / 1000;
       iap_entry(__command,__result);
    }
    
    return __result[0];
}


unsigned int FLASH_WriteData(void *dstAddr, void *srcAddr, unsigned int size){
uint32_t startSector, endSector;   
   
    startSector = (uint32_t)dstAddr / FLASH_SECTOR_SIZE;
    endSector = ((uint32_t)dstAddr + size) / FLASH_SECTOR_SIZE;
    
    if( blank_check(startSector, endSector) != CMD_SUCESS ){
       if( FLASH_EraseSectors(startSector, endSector) != CMD_SUCESS )
         return __result[0];
    }

    do{
       if(FLASH_WriteBlock(dstAddr,srcAddr,FLASH_MIN_DATA_SIZE) != CMD_SUCESS)
         break;

       dstAddr += FLASH_MIN_DATA_SIZE;
       srcAddr += FLASH_MIN_DATA_SIZE;

       if(size > FLASH_MIN_DATA_SIZE)
          size -= FLASH_MIN_DATA_SIZE;
       else
          size = 0;
    }while(size);
          
    return __result[0];
}

/**
 * @return CMD_SUCCESS | COMPARE_ERROR | COUNT_ERROR | ADDR_ERROR |ADDR_NOT_MAPPED
 **/
unsigned int FLASH_VerifyData(void *dstAddr, void *srcAddr, unsigned int size){
    __command[0] = FLASH_CMD_VERIFY;
    __command[1] = (uint32_t)dstAddr & 0xFFFFFFFC;
    __command[2] = (uint32_t)srcAddr & 0xFFFFFFFC;
    __command[3] = (size >> 2) & 0xFFFFFFFC;           //word bondary
    iap_entry(__command,__result);
    return __result[0];    
}

