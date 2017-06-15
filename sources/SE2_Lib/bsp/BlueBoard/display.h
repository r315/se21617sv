#ifndef _display_h_
#define _display_h_

#include <stdint.h>
#include "fonts.h"
#include <lcd.h>

#ifndef __ctype_h
	#define toupper(c) ( (c)-'a'+'A' )  // so funciona para tipo unsigned char
#endif 


typedef struct _Display{
	void (*xputc)(char);		//for use on xstdout
	char (*xgetchar)(void);
	uint16_t forecolor;
	uint16_t backcolor;
	uint8_t *font;
	uint8_t	font_w;
	uint8_t font_h;
	uint16_t cx;
	uint16_t cy;
	uint16_t sc;
	int (*drawChar)(int  x, int y, unsigned char *d_char);
}Display;

void DISPLAY_Init(void);
void DISPLAY_SetFont(uint8_t fnt);
void DISPLAY_SetAttribute(uint8_t atr);
int  DISPLAY_Char(int  x, int y, unsigned char c); 
int  DISPLAY_Text(int x, int y, const char *s);
void DISPLAY_Pixel(int x0, int y0);
int  DISPLAY_Uint(int x, int y, unsigned int v, unsigned char radix);
void *DISPLAY_GetReference(void);
void DISPLAY_SetFcolor(uint16_t color);
void DISPLAY_SetBcolor(uint16_t color);
int DISPLAY_GetFontWidth(void);
int DISPLAY_GetFontHeight(void);

#endif
