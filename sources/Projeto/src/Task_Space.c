#include <string.h>
#include <lcd.h>
#include <stdint.h>
#include "space.h"
#include "../sprites/sprites.h"
#include <misc.h>
//#include <spi.h>
#include <Task_Common.h>

#include <util.h>

static const char title[] = { "      # Space Invaders #\n\n" };
GameData *gamedata;

//--------------------- Game functions -------------------
/**
 * @brief internal function that simply adds the offset to the game area to
 *        before creating a window for write on the display
 *
 **/
void LCD_OffsetWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	LCD_Window(SCREEN_SX + x, SCREEN_SY + y, w, h);
}

/**
 * @brief draws the given sprite on is x,y coordenates
 * */
void drawSprite(Sprite *sp) {
	LCD_OffsetWindow(sp->x, sp->y, SPRITE_W, SPRITE_H);
	LCD_IndexedColor((uint16_t*) pal1,
			(uint8_t*) &SPRITES_DATA[SPRITE_SIZE * sp->data], SPRITE_SIZE);

}

/**
 * @brief clears game area for new games
 */
void clearGame(void) {
	LCD_FillRect(SCREEN_SX, SCREEN_SY + SCREEN_TOP, SCREEN_W,
	SCREEN_BOTTOM - SCREEN_TOP, BGCOLOR);
}

/**
 * @brief copy bitmap data of tank from flash to sprite on ram
 * */
void loadTank(Sprite *sp) {
	sp->data = TANK_DATA_IDX;
	sp->x = (SCREEN_W/2) - (SPRITE_W/2);
	sp->y = SCREEN_H - SPRITE_H - 1 - (LCD_GetFontHeight());
}

void moveTank(Sprite *tk, int8_t dir) {
	int newx;
	int8_t d;

	newx = tk->x + dir;

	if (newx > (SCREEN_W - SPRITE_W) || newx < 0)
		return;

	d = (dir < 0) ? -dir : dir;

	// erase only necessary data from old position
	if (newx >= tk->x)
		LCD_OffsetWindow(tk->x, tk->y, d, SPRITE_H);
	else
		LCD_OffsetWindow(newx + SPRITE_W, tk->y, d, SPRITE_H);

	LCD_Fill(d * SPRITE_H, BGCOLOR);

	tk->x = newx;
	drawSprite(tk);
}

void eraseProjectils(Projectile *pj, uint8_t n) {
	while (n--) {
		if (pj->inmotion) {
			LCD_OffsetWindow(pj->x, pj->y, PROJECTILE_W, PROJECTILE_H);
			LCD_Fill(PROJECTILE_W * PROJECTILE_H, BGCOLOR);
		}
		pj += 1;
	}
}

void moveProjectile(Projectile *pj) {
	int newy;

	if (!pj->inmotion)
		return;

	newy = pj->y + pj->velocity;

	//check if hit bottom or top of screen
	if (newy > (SCREEN_BOTTOM - PROJECTILE_H) || newy < SCREEN_TOP) {
		pj->inmotion = 0;
		return;
	}

	pj->y = newy;
	LCD_OffsetWindow(pj->x, pj->y, PROJECTILE_W, PROJECTILE_H);
	LCD_Fill(PROJECTILE_W * PROJECTILE_H, pj->color);

}
/**
 * @brief creates a projectile on the given coordenates
 * */
void newProjectile(Projectile *proj, int x, int y, int8_t velocity,
		uint16_t color) {
	uint8_t i;
	for (i = 0; i < TANK_MAX_PROJECTILES; i++) {
		if (!proj[i].inmotion) {
			proj[i].x = x;
			proj[i].y = y;
			proj[i].velocity = velocity;
			proj[i].color = color;
			proj[i].inmotion = ON;
			return;
		}
	}
}
/**
 * @brief copy bitmap data os aliens from flash to ram
 * */
void loadAliens(Sprite *als, const uint8_t *al, uint16_t y) {
	uint8_t i, j, a = 0;
	uint16_t x = SPRITE_W;
	for (i = 0; i < ALIENS_ROWS; i++) {
		for (j = 0; j < ALIENS_COLS; j++, al += 1) {
			als[a].x = x;
			als[a].y = y;
			als[a].type = (*al) - 1;
			als[a].data = als[j].type;
			als[a].alive = ON;
			x += SPRITE_W;
			a++;
		}
		x = SPRITE_W;
		y += SPRITE_H;
	}
}

int8_t moveAliens(Sprite *als, const uint8_t *alienFrame, int8_t dir) {
	uint8_t i, drop = 0;

	if (((dir < 0) && (als[0].x <= 0))
			|| (dir > 0 && als[ALIENS_COLS - 1].x >= SCREEN_W - SPRITE_W)) {
		dir = -dir;
		drop = ALIENS_DROP_GRAVITY;
		LCD_OffsetWindow(als[0].x, als[0].y, SPRITE_W * ALIENS_COLS,
		ALIENS_DROP_GRAVITY);
		LCD_Fill(SPRITE_SIZE * ALIENS_COLS * ALIENS_DROP_GRAVITY, BGCOLOR);
	}

	if (dir < 0) {
		LCD_OffsetWindow(als[ALIENS_COLS - 1].x + SPRITE_W - ALIENS_MOVE_SPEED,
				als[ALIENS_COLS - 1].y, ALIENS_MOVE_SPEED,
				SPRITE_H * ALIENS_ROWS);
		LCD_Fill(ALIENS_MOVE_SPEED * SPRITE_H * ALIENS_ROWS, BGCOLOR);
	} else {
		LCD_OffsetWindow(als[0].x, als[0].y, ALIENS_MOVE_SPEED,
		SPRITE_H * ALIENS_ROWS);
		LCD_Fill(ALIENS_MOVE_SPEED * SPRITE_H * ALIENS_ROWS, BGCOLOR);
	}

	for (i = 0; i < MAX_ALIENS; i++, alienFrame += 1) {
		als[i].x += dir;
		als[i].y += drop;

		if (als[i].alive) {
			drawSprite(&als[i]);
			als[i].data = (*alienFrame) - 1;
		} else {
			LCD_OffsetWindow(als[i].x, als[i].y, SPRITE_W, SPRITE_H);
			LCD_Fill(SPRITE_SIZE, BGCOLOR);
		}
	}
	return dir;
}

uint8_t checkPlayerHit(Projectile *proj, Sprite *als) {
	uint8_t i;
	uint8_t score;
	if (proj->inmotion == OFF)
		return 0;

	for (i = 0; i < MAX_ALIENS; i++) {
		if (als[i].alive) {
			if (als[i].x <= proj->x && (als[i].x + SPRITE_W) >= proj->x) {
				if (als[i].y <= proj->y && (als[i].y + SPRITE_H) >= proj->y) {
					als[i].alive = OFF;
					eraseProjectils(proj, 1);
					proj->inmotion = OFF;
					score = als[i].type + 1;
					//printf("Score: %u\n",score);
					return score;
				}
			}
		}
	}
	return 0;
}

uint8_t checkAlienHit(Projectile *proj, Sprite *player) {
	if (proj->inmotion == OFF)
		return OFF;

	if (player->x <= proj->x && (player->x + SPRITE_W) >= proj->x) {
		if (player->y <= proj->y + PROJECTILE_H) {
			eraseProjectils(proj, 1);
			proj->inmotion = OFF;
			return ON;
		}
	}
	return OFF;
}

uint8_t checkAliensOnBottom(Sprite *aliens, Sprite *player) {
	uint8_t i;
	for (i = 0; i < MAX_ALIENS; i++) {
		if (aliens[i].alive) {
			if (aliens[i].x <= player->x
					&& (aliens[i].x + SPRITE_W) >= player->x) {
				if (aliens[i].y + SPRITE_H >= player->y)
					return ON;

			}
		}
	}
	return OFF;
}

void updateScore(uint32_t scr) {
	LCD_Goto(SCREEN_SX + LCD_GetFontWidth() * 7, SCREEN_SY + 3);
	LCD_WriteInt(scr, SCORES_FORMAT);
}

void updateLives(uint8_t lives) {
	LCD_Goto(SCREEN_SX + LCD_GetFontWidth() * 7,
			 SCREEN_SY + SCREEN_H - LCD_GetFontHeight());
	LCD_WriteInt(lives, 10);
}




void updatePlayerName(char *name) {
	LCD_Goto(SCREEN_SX + 30 + 12 * 8, SCREEN_SY + 50);
	LCD_WriteString(gamedata->playername);
}

void selectPlayerName(char *pname, uint8_t curchar, int8_t inc) {
	uint8_t ch = *(pname + curchar);
	ch += inc;
	if (ch >= 'A' && ch <= 'Z') {
		*(pname + curchar) = ch;
		updatePlayerName(pname);
	}
}

void showScoreTable(void) {
	LCD_SetColors(GREEN, BLACK);
	LCD_Goto(SCREEN_SX + 30, SCREEN_SY + 50);
	if(gamedata->playername[0] == '\0'){
		gamedata->playername[0] = 'A';
		gamedata->playername[1] = 'A';
		gamedata->playername[2] = 'A';
		gamedata->playername[3] = '\0';
	}
	LCD_WriteString("Enter Name: ");
	LCD_SetColors(SPACE_FONT_COLOR, BLACK);
	updatePlayerName(gamedata->playername);
	/*LCD_WriteString("Full Score:
	 Big
	 little
	 ");*/
}


void SPACE_Init(void *ptr) {

	gamedata = (GameData*) ptr;

	LCD_Clear(UBUNTU);
	LCD_SetColors(WHITE, UBUNTU);

	LCD_OffsetWindow(0, 0, SCREEN_W, SCREEN_H);
	LCD_Fill(SCREEN_W * SCREEN_H, BGCOLOR);

	LCD_Goto(0, 0);
	LCD_WriteString((char*) title);

	LCD_SetColors(SPACE_FONT_COLOR, BLACK);
	LCD_Goto(SCREEN_SX + LCD_GetFontWidth(), SCREEN_SY + 3);
	LCD_WriteString("SCORE:");

#if (SCREEN_W > 200)
	LCD_Goto(SCREEN_SX + 110, SCREEN_SY + 3);
	LCD_WriteString("HI-SCORE:");
	LCD_WriteInt(gamedata->topscore, SCORES_FORMAT);
#endif

	LCD_Goto(SCREEN_SX + 3, SCREEN_SY + SCREEN_H - LCD_GetFontHeight());
	LCD_WriteString("Lives:");

	LCD_FillRect(SCREEN_SX, SCREEN_SY + SCREEN_H - 1 - LCD_GetFontHeight(),
	SCREEN_W, 1, WHITE);

	BUTTON_SetHoldTime(80);

	moveTank(&gamedata->tank, 0);
	moveAliens(gamedata->aliens, Aliens1, 0);
	updateScore(gamedata->score);
	updateLives(gamedata->lives);
}

void SPACE_NewGame(GameData *gd) {
	loadTank(&gd->tank);
	loadAliens(gd->aliens, Aliens0, SCREEN_TOP + SPRITE_H);
	gd->alienscount = MAX_ALIENS;
	gd->lives = MAX_LIVES;
	gd->state = RUNNING;
	gd->score = 0;
}

void Task_Space(void *ptr) {
	uint8_t n, randalien;
	BTN_Event button;
	int8_t dir = ALIENS_MOVE_SPEED;
	uint8_t speed = SPEED;
	uint8_t alienframe;
	uint16_t alienpoints;
	uint32_t frametime = 0;

	SPACE_Init(ptr);

	while(1){
		button = BUTTON_QueueGet();
	if (gamedata->state == ENDED) {
		if (button.event == BUTTON_PRESSED) {
			switch (button.value) {
			case BUTTON_U:
				selectPlayerName(gamedata->playername, alienframe, 1);
				break;
			case BUTTON_D:
				selectPlayerName(gamedata->playername, alienframe, -1);
				break;
			case BUTTON_C:
				if ((++alienframe) == 3) {
					SPACE_NewGame(gamedata);
					clearGame();
				}
				break;
			default:
				break;
			}
		}
	} else {
		if (TIMER0_GetValue() > frametime) {
			switch (button.value) {

			case BUTTON_L:
				moveTank(&gamedata->tank, -1);							//Move nave para esquerda
				break;

			case BUTTON_R:
				moveTank(&gamedata->tank, 1);							//Move nave para direita
				break;

			case BUTTON_U:
				newProjectile(gamedata->tankprojectiles,				//Dispara um projectil
						gamedata->tank.x + SPRITE_W / 2,
						gamedata->tank.y - PROJECTILE_H, -1, PINK);
				break;
			//TODO: optimizar criando uma funçao para mover nave
			case (BUTTON_U | BUTTON_L):									//Move nave para a esquerda e dispara um projectil
				moveTank(&gamedata->tank, -1);
				newProjectile(gamedata->tankprojectiles,
						gamedata->tank.x + SPRITE_W / 2,
						gamedata->tank.y - PROJECTILE_H, -1, PINK);
				break;

			case (BUTTON_U | BUTTON_R):									//Move nave para a direita e dispara um projectil
				moveTank(&gamedata->tank, 1);
				newProjectile(gamedata->tankprojectiles,
						gamedata->tank.x + SPRITE_W / 2,
						gamedata->tank.y - PROJECTILE_H, -1, PINK);
				break;

			case BUTTON_C:
				TASK_EXIT;
				return;

			default:
				break;
			}

			//movimentação dos aliens
			if (!(--speed)) {
				dir = moveAliens(gamedata->aliens,
						(alienframe & 4) ? Aliens0 : Aliens1, dir);
				randalien = rand() % MAX_ALIENS;
				newProjectile(gamedata->alienprojectiles,
						gamedata->aliens[randalien].x,
						gamedata->aliens[randalien].y, 1, RED);
				speed = SPEED;
				alienframe++;
				if (checkAliensOnBottom(gamedata->aliens, &gamedata->tank))
					SPACE_NewGame(gamedata);
			}

			// apaga os projeteis do jogador
			eraseProjectils(gamedata->tankprojectiles,
			TANK_MAX_PROJECTILES);

			// move os projeteis do jogador e verifica se colidiu com algum alien
			for (n = 0; n < TANK_MAX_PROJECTILES; n++) {
				moveProjectile(&gamedata->tankprojectiles[n]);
				alienpoints = checkPlayerHit(&gamedata->tankprojectiles[n],
						gamedata->aliens);
				gamedata->score += alienpoints;
				if (alienpoints) {
					gamedata->alienscount--;
					if (!gamedata->alienscount) {
						gamedata->state = ENDED;
						showScoreTable();
						alienframe = 0; 		//variable reuse
					}
				}
			}

			// apaga os projecteis dos aliens
			eraseProjectils(gamedata->alienprojectiles,
			ALIENS_MAX_PROJECTILES);

			//move os projeteis disparados pelos aliens e testa se algum colidiu com o jogador
			for (n = 0; n < ALIENS_MAX_PROJECTILES; n++) {
				moveProjectile(&gamedata->alienprojectiles[n]);
				if (checkAlienHit(&gamedata->alienprojectiles[n],
						&gamedata->tank)) {
					gamedata->lives--;
					updateLives(gamedata->lives);
					if (!gamedata->lives) {
						gamedata->state = ENDED;
						showScoreTable();
						alienframe = 0; 		//variable reuse
					}
				}
			}

			updateScore(gamedata->score);

			MISC_countFps();
			frametime += SPACE_UPDATE_RATE;
		}
	}
	}
}
