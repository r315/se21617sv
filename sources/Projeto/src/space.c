    #include <string.h>
    #include <lcd.h>
    #include <stdint.h>
    #include <button.h>
    #include "proj.h"
    #include "space.h"
    #include "../sprites/sprites.h"

    #include <gpio.h>
    #include <spi.h>

    #include <util.h>

    static const char title[]={
        "      # Space Invaders #\n\n"    
    };

    GameData *gamedata;
    static uint32_t frametime;

     /**
     * @brief internal function that simply adds the offset to the game area to
     *        before creating a window for write on the display
     *
     **/
    void LCD_OffsetWindow(uint32_t x, uint32_t y, uint32_t w, uint32_t h){
        LCD_Window(SCREEN_SX + x, SCREEN_SY + y, w, h);
    }
     /**
      * @brief draws the given sprite on is x,y coordenates
      * */
    void drawSprite(Sprite *sp){               
        LCD_OffsetWindow(sp->x, sp->y,SPRITE_W,SPRITE_H);   
        LCD_IndexedColor((uint16_t*)pal1, sp->data, SPRITE_SIZE);
    }
     /**
      * @brief copy bitmap data of tank from flash to sprite on ram
      * */
    void loadTank(Sprite *sp){
        sp->data = (uint8_t*)TANK_DATA;
        sp->x = (SCREEN_W/2)-8;
        sp->y = SCREEN_H-16;
    }

    void moveShip(Sprite *tk, int8_t dir){
    int newx;
    int8_t d;

        newx = tk->x + dir;

        if(newx > (SCREEN_W-SPRITE_W) || newx < 0)
            return;

        d = (dir < 0)? -dir : dir;

        // erase only necessary data from old position
        if(newx >= tk->x)
            LCD_OffsetWindow(tk->x, tk->y, d, SPRITE_H);
        else
            LCD_OffsetWindow(newx+SPRITE_W, tk->y, d, SPRITE_H);

        LCD_Fill(d * SPRITE_H, BGCOLOR);

        tk->x = newx;
        drawSprite(tk);
    }

    void removeProjectile(Projectile *pj){
        LCD_OffsetWindow(pj->x, pj->y, PROJECTILE_W, PROJECTILE_H);
        LCD_Fill(PROJECTILE_W * PROJECTILE_H, BGCOLOR);
    }

    void moveProjectile(Projectile *pj){
    int newy;

        if(!pj->inmotion)
            return;

	    removeProjectile(pj);

	    newy = pj->y + pj->velocity;

        if(newy > (SCREEN_H - SPRITE_H) || newy < (2 * SPRITE_H)){
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
    void newProjectile(Projectile *proj, int x, int y, int8_t velocity, uint16_t color){
    uint8_t i;
        for(i = 0; i < MAX_PROJECTILES; i++){
            if(!proj[i].inmotion){
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
    void loadAliens(Sprite *als, const uint8_t *al, uint16_t y){
    uint8_t i,j,a = 0;
    uint16_t x = SPRITE_W;
        for(i=0; i < ALIENS_ROWS; i++){        
            for(j=0; j < ALIENS_COLS; j++, al+=1){
                als[a].x = x;
                als[a].y = y;
                als[a].type = (*al) - 1;
                als[a].data = (uint8_t*)(SPRITES_DATA + (als[j].type * SPRITE_SIZE));
                als[a].alive = ON;
                x += SPRITE_W;
                a++;
            }
            x = SPRITE_W;
            y += SPRITE_H;
        }    
    }

    int8_t moveAliens( Sprite *als, const uint8_t *alienFrame, int8_t dir, int8_t height){
    uint8_t i;

        if(((dir < 0) && (als[0].x <= 0)) || (dir > 0 && als[ALIENS_COLS - 1].x >= SCREEN_W-SPRITE_W))
            dir  = -dir;

        for(i = 0; i< MAX_ALIENS; i++, alienFrame +=  1){
            als[i].x += dir;
            als[i].y += height;
            
            if(als[i].alive){
                drawSprite(&als[i]);
                als[i].data = (uint8_t*)(SPRITES_DATA + (((*alienFrame) - 1) * SPRITE_SIZE));
            }
            else
            {
                LCD_OffsetWindow(als[i].x, als[i].y, SPRITE_W, SPRITE_H);   
                LCD_Fill(SPRITE_SIZE, BGCOLOR);
            }
        }
        return dir;
    }

    uint8_t checkColision(Projectile *proj, Sprite *als){
    uint8_t i;
    uint8_t score;
        if(proj->inmotion == OFF)
            return 0;   
        
        for(i = 0; i< MAX_ALIENS; i++){
            if(als[i].alive){
                if(als[i].x <= proj->x && (als[i].x + SPRITE_W) >= proj->x){
                    if(als[i].y <= proj->y && (als[i].y + SPRITE_H) >= proj->y){
                        als[i].alive = OFF;
                        removeProjectile(proj);
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

    void updateScore(uint32_t scr){
        LCD_Goto(SCREEN_SX + LCD_GetFontWidth() * 8, SCREEN_SY + 3);
        LCD_WriteInt(scr, SCORES_FORMAT);    
    }

    void updateLives(uint8_t lives){
	    LCD_Goto(SCREEN_SX + LCD_GetFontWidth() * 7, SCREEN_SY + SCREEN_H - LCD_GetFontHeight());
	    LCD_WriteInt(lives, 10);    
    }

    void newGame(GameData *gd){
        memset(gd, 0, sizeof(GameData));
        loadTank(&gd->tank);    
        loadAliens(gd->aliens, Aliens0, SPRITE_H * 3);
        gd->score = 0;    
    }

    void popSpace(void *ptr){

        LCD_Clear(UBUNTU);
        LCD_SetColors(WHITE,UBUNTU);

        LCD_OffsetWindow(0, 0, SCREEN_W, SCREEN_H);
        LCD_Fill(SCREEN_W * SCREEN_H, BGCOLOR);

        LCD_Goto(0,0);
        LCD_WriteString((char*)title);
        
        LCD_SetColors(SPACE_FONT_COLOR,BLACK);
	    LCD_Goto(SCREEN_SX + 3, SCREEN_SY + 3);
	    LCD_WriteString("SCORE:");

	    LCD_Goto(SCREEN_SX + 128, SCREEN_SY + 3);
	    LCD_WriteString("HI-SCORE:");

	    LCD_Goto(SCREEN_SX + 3, SCREEN_SY + SCREEN_H - LCD_GetFontHeight());
	    LCD_WriteString("Lives:");

	    BUTTON_SetHoldTime(80);

        gamedata = (GameData*)ptr; 
        moveShip(&gamedata->tank,0);   
        moveAliens(gamedata->aliens, Aliens1, 0,0);
        updateScore(gamedata->score);
	    updateLives(gamedata->lives);
    }


    void space(int b){
    uint8_t n;
    static int8_t dir = -1;
    static uint8_t speed = SPEED;
    static uint8_t f;
        
        if(TIMER0_GetValue() > frametime){
            
            switch(b){
                case BUTTON_L:  moveShip(&gamedata->tank,-1); break;
                case BUTTON_R:  moveShip(&gamedata->tank,1); break;
                case BUTTON_F:  newProjectile(gamedata->tankprojectiles, gamedata->tank.x + SPRITE_W/2, gamedata->tank.y -  PROJECTILE_H, -1, PINK); break;            
                
                case (BUTTON_F | BUTTON_L): 
                    moveShip(&gamedata->tank,-1);
                    newProjectile(gamedata->tankprojectiles, gamedata->tank.x + SPRITE_W/2, gamedata->tank.y - PROJECTILE_H, -1, PINK);
                    break;
                
                case (BUTTON_F | BUTTON_R):
                    moveShip(&gamedata->tank,1);
                    newProjectile(gamedata->tankprojectiles, gamedata->tank.x + SPRITE_W/2, gamedata->tank.y - PROJECTILE_H, -1, PINK);
                    break;
                
                default: break;
            }
            
            for(n=0; n < MAX_PROJECTILES; n++){
                moveProjectile(&gamedata->tankprojectiles[n]);
                gamedata->score += checkColision(&gamedata->tankprojectiles[n], gamedata->aliens);            
            }
            
            updateScore(gamedata->score);
            
            if(!(--speed)){
                dir = moveAliens(gamedata->aliens, (f & 4)? Aliens0 : Aliens1, dir, 0); //move aliens, no descend
                speed = SPEED;
                f++;
            }  
            //TODO: implement aliens down and shot
                        
            frametime = TIMER0_GetValue() + 20;  //50fps
        }
    }
