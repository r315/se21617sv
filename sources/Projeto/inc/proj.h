#ifndef _proj_h_
#define _proj_h_

#include <time.h>
#include "space.h"

#define ON 1
#define OFF 0

#define MAX_TOP_SCORES 3
#define SCORES_FORMAT (4<<8) | 10
#define ENTER_CONFIG_TIME 2000 //2 seconds

typedef enum Mstates{
  IDLE,
  CONFIG,  
  GAME,
  SAVE    
}State;

typedef struct _SaveData{
    uint32_t topscores[MAX_TOP_SCORES];    
    uint8_t checksum;
    GameData spaceInvaders;
    struct tm rtc;
}SaveData;


#endif
