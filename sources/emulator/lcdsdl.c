//Using SDL and standard IO 
#include <SDL2/SDL.h> 
#include <stdio.h> 
#include <lcd.h>
#include "lcdsdl.h"

#define UPDATE_TIME 30 //33fps
#define LCD_FISICAL_W 240
#define LCD_FISICAL_H 320

#define LCD_OFFSETX (LCD_FISICAL_W/2) - (LCD_W/2) //center lcd window
#define LCD_OFFSETY (LCD_FISICAL_H/2) - (LCD_H/2)

struct LCD{
	SDL_Window *window;
	SDL_Surface *surface;
	uint32_t w;
	uint32_t h;
	uint32_t wx;			//wrap x
	uint32_t wy;			//wrap y
	uint32_t ww;			//wrap w
	uint32_t wh;			//wrap h
	uint32_t mx;			//memory current x
	uint32_t my;			//memory current y	
    //SDL_Thread *thread_update;
    uint32_t auto_update;
}lcd;

void LCD_Bkl(uint8_t state){

}

uint32_t LCD_Update(uint32_t interval, void *ptr){    
/*    while(lcd.update){
    	SDL_UpdateWindowSurface(lcd.window);
        SDL_Delay(10);
    }*/
	
	 SDL_UpdateWindowSurface(lcd.window);
    if(!lcd.auto_update)
      return 0;	
    return interval;  //timer is set using return value
}

void LCD_Init(void){

	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) { 
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return; 
	}

	lcd.w = LCD_W;
	lcd.h = LCD_H;	
  
	lcd.window = SDL_CreateWindow( 	windowTitle, 
									SDL_WINDOWPOS_UNDEFINED,
									SDL_WINDOWPOS_UNDEFINED, 
									LCD_FISICAL_W, 
									LCD_FISICAL_H, 
									SDL_WINDOW_SHOWN); 
    if( lcd.window == NULL ) { 
    	printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		return;
   	}
    	
    lcd.surface = SDL_GetWindowSurface( lcd.window );
     
    fprintf(stdout,"Window size: %dx%d %dbpp\n",lcd.surface->w, lcd.surface->h, lcd.surface->format->BitsPerPixel);    
    

	
    if(LCD_FISICAL_W != LCD_W || LCD_FISICAL_H != LCD_H){
        SDL_FillRect(lcd.surface, NULL, SDL_MapRGB(lcd.surface->format, 0x0, 0x0, 0 ) );        
        fprintf(stdout,"Display size: %dx%d\n",(int)LCD_W,(int)LCD_H);
    }else
       SDL_FillRect(lcd.surface, NULL, SDL_MapRGB(lcd.surface->format, 0x30, 0x0A, 0x24 ) );    
    
    lcd.auto_update = 1;
    //lcd.thread_update = SDL_CreateThread(LCD_Update, "Lcd Update Thread", (void *)NULL);

     //if(lcd.thread_update == NULL)
     //   fprintf(stderr, "Error creating update thread\n");
//	 LCD_Update(UPDATE_TIME,(void*)NULL);

	SDL_AddTimer(UPDATE_TIME, LCD_Update, (void*)NULL);
}

void LCD_Close(void){
    lcd.auto_update = 0;
    //SDL_WaitThread(lcd.thread_update, &tr);    
    
	SDL_DestroyWindow(lcd.window); 
	SDL_Quit(); 
}

void LCD_Fill(uint32_t n, uint16_t color){
	while(n--)
		LCD_Data(color);
}

void LCD_IndexedColor(uint16_t *colors, uint8_t *index, uint32_t size){
	while(size--){
        LCD_Data(colors[*index]);
		index += 1;
    }
}

void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
   lcd.wx = x;
   lcd.wy = y;
	lcd.ww = x + (w-1);
   lcd.wh = y + (h-1);	

	lcd.mx = lcd.wx;
   lcd.my = lcd.wy;   
}

//-------------------------------------------------------------
//        RRRRRGGGGGGBBBBB 
//        BBBBBGGGGGGRRRRR
//RRRRRRRRGGGGGGGGBBBBBBBB
//-------------------------------------------------------------
void LCD_Data(uint16_t color)
{
   uint32_t *pixels = (uint32_t *)lcd.surface->pixels;    
   
	if(lcd.mx >= lcd.wx && lcd.mx <= lcd.ww && 
       lcd.my >= lcd.wy && lcd.my <= lcd.wh &&
       lcd.mx < lcd.w && lcd.my < lcd.h)
	{
    	pixels[ ((lcd.my + LCD_OFFSETY) * LCD_FISICAL_W) + lcd.mx + LCD_OFFSETX ] = (uint32_t) ( ((color&0xf800)<<8) | ((color&0x7e0)<<5) | ((color&0x1f)<<3) );   
        if(lcd.mx == lcd.ww){
            lcd.mx = lcd.wx;
            if(lcd.my == lcd.wh) lcd.my = lcd.wy;
            else lcd.my++;
        }
        else lcd.mx++;
    }   
}


