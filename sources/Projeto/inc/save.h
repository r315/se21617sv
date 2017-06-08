/**
* @file     save.h
* @brief    Contains API header save and restore data from flash memory
*
* @version  1.0
* @date     12 dec. 2016
* @author   Hugo Reis
**/
#ifndef _save_h_
#define _save_h_

int saveData(void *data, int dataSize);
int restoreData(void *data, int dataSize);
void saveTopScore(uint32_t score, uint32_t *scorestab);
void displaySaveResult(uint32_t res);

#endif
