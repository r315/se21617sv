#include <string.h>
#include <stdint.h>
#include <flash.h>
#include <lcd.h>
#include "proj.h"


#if defined(__LPCXpresso__)
#include <eeprom.h>
#endif

#include "save.h"


#if defined(__LPCXpresso__)

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

/**
 * @brief if score is a top score insert it on scores table.
 * @param score: score of last game
 * @param scorestab: ponter to array containing the 3 best scores
 * */
void saveTopScore(uint32_t score, uint32_t *scorestab){
uint8_t n;
uint32_t *p1,*p2;
   for(n = 0; n < MAX_TOP_SCORES; n++){
       if(score > scorestab[n]){
           p1 = &scorestab[MAX_TOP_SCORES-2];
           p2 = &scorestab[MAX_TOP_SCORES-1];
           while( (p2) != &scorestab[n]){
               *p2 = *p1;
               p1 -= 1;
               p2 -= 1;
           }
           scorestab[n] = score;
           return;
       }
   }
}

void displaySaveResult(uint32_t res){
char *msg;
uint32_t dl = 1500;

    switch(res){
        case CMD_SUCESS:
            msg = " Save ok "; break;

        case SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION :
            LCD_WriteString("Sector Not Ready");break;

        case INVALID_SECTOR:
            msg = "Invalid Sector";break;

        case SRC_ADDR_ERROR:
            msg = "Invalid Start Address";break;

        case DST_ADDR_ERROR:
            msg = "Invalid Destination Address";break;

        case COUNT_ERROR:
            msg = "Count error";break;

        case BUSY:
            msg = "Busy";break;

        case COMPARE_ERROR:
            msg = "Compare Error";break;

        case 0x30: //TODO: fix enums
            msg = " Saving.."; dl = 0; break;


        default:
                LCD_Goto(0,0);
                LCD_WriteString("ERROR: 0x");
                LCD_WriteInt(res,(2<<8) | 16);
                LCD_WriteChar(' ');
                TIME_DelayMs(1500);
                return;
    }

    LCD_Goto( (LCD_W/2) - (strlenInPixels(msg)/2), LCD_H /2);
    LCD_WriteString(msg);
    TIME_DelayMs(dl);
}


