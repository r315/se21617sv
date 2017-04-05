#ifndef _space_h_
#define _space_h_

#define SCREEN_W 240
#define SCREEN_H 224

#define SCREEN_SX (LCD_W/2) - (SCREEN_W/2)
#define SCREEN_SY (LCD_H/2) - (SCREEN_H/2)

#define SPRITE_H 8
#define SPRITE_W 16
#define SPRITE_SIZE (SPRITE_W * SPRITE_H)

#define BGCOLOR BLACK	
#define SPACE_FONT_COLOR RGB(12,32,20)

#define SPEED 1         //frames for alien update 

#define ALIENS_COLS 6
#define ALIENS_ROWS 6
#define ALIENS_DROP_GRAVITY 8  //number of pixels in each drop
#define ALIENS_MOVE_SPEED 16    //number of pixels for side movement
 
#define TANK_MAX_PROJECTILES 1
#define ALIENS_MAX_PROJECTILES 3
#define MAX_ALIENS ALIENS_COLS * ALIENS_ROWS
#define SPRITES_DATA spcinv
#define TANK_DATA SPRITES_DATA + (SPRITE_SIZE * 6)

#define PROJECTILE_W 2
#define PROJECTILE_H 6

#define MAX_LIVES 3

enum gStates{
    RUNNING,
    END,
    SELECT
};

typedef struct _sprite{
    int x;
    int y;
    uint8_t type;    
    uint8_t alive;
	uint8_t *data;
}Sprite;

typedef struct projectile{
    int x;
    int y;
    uint16_t color;
    uint8_t inmotion;
	int8_t velocity;
}Projectile;

typedef struct _gameData{
    uint32_t score;
    uint8_t state;
	uint32_t topscore;
	uint8_t lives;
    uint8_t alienscount;
    uint8_t end;
    uint8_t playername[3];
    Sprite tank;
	Sprite bonusalien;
    Sprite aliens[MAX_ALIENS];
    Projectile tankprojectiles[TANK_MAX_PROJECTILES];
	Projectile alienprojectiles[ALIENS_MAX_PROJECTILES];
  //  Projectile alienprojectiles[MAX_PROJECTILES];
}GameData;

void popSpace(void *ptr);
void space(int b);
void newGame(GameData *gd);

#endif
