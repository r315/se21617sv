
#include <display.h>

static Display _display;

void DISPLAY_SetFcolor(uint16_t color){ _display.forecolor = color;}
void DISPLAY_SetBcolor(uint16_t color){ _display.backcolor = color;}
int DISPLAY_GetFontWidth(void){ return _display.font_w; }
int DISPLAY_GetFontHeight(void){ return _display.font_h; }

//----------------------------------------------------------
//stdout calls this function
//----------------------------------------------------------
void DISPLAY_ConsoleChar(char c)
{
	if (c == '\n' || _display.cx == LCD_W){
		_display.cx = 0;
		_display.cy += _display.font_h;
		if(_display.cy == LCD_H){
			_display.cy = 0;
			if(!_display.sc)
				_display.sc = _display.font_h;
		}
		if(_display.sc){			
			LCD_Scroll(_display.sc);			
			_display.sc += _display.font_h;

			_display.cx = LCD_W;
			while(_display.cx){			// limpa a proxima linha
				DISPLAY_Char(_display.cx,_display.cy,' ');
				_display.cx -= _display.font_w;
			}
		}
		if(c== '\n')
			return;	
	}
	if(c == '\r'){
		_display.cx = 0;
		return;
	}	
	_display.cx = DISPLAY_Char(_display.cx, _display.cy, c);
}

//----------------------------------------------------------
//draws a character without any attributes
//TODO: optimize
//----------------------------------------------------------
int drawCharSimple(int x, int y, unsigned char *d_char){
unsigned char w,h;
	for (h=0; h < _display.font_h; h++){ 	
		for(w=0; w < _display.font_h; w++){
			if(*d_char & (0x80 >> w))
				LCD_Data(_display.forecolor);			
			else
				LCD_Data(_display.backcolor);			
		}
		d_char += 1;
 	}
	return x+(_display.font_w);
}

int drawCharInverted(int x, int y, unsigned char *d_char){
unsigned char w,h;
	for (h=0; h < _display.font_h; h++){ 	
		for(w=0; w < _display.font_h; w++){
			if(*d_char & (0x80 >> w))
				LCD_Data(_display.backcolor);
			else
				LCD_Data(_display.forecolor);
		}
		d_char += 1;		
 	}
	return x+(_display.font_w);
}
int drawCharTransparent(int x, int y, unsigned char *d_char)
{
char w,h;
	for (h=0; h < _display.font_h; h++){
		for(w=0;w<_display.font_w; w++){
			if(*d_char & (0x80 >> w))
				DISPLAY_Pixel(x+w,y+h);							
		}	
		d_char += 1;
	}
	return x+(_display.font_w);
}
int drawCharDouble(int x, int y, unsigned char *d_char)
{
unsigned char w,h;
    LCD_Window(x,y,_display.font_w * 2, _display.font_h * 2);
	for (h=0;h<_display.font_h;h++){			    // altura
		for(w=0;w<_display.font_w;w++){			// primeira linha
			if(*d_char & (0x80 >> w))	// se pixel
			{
				LCD_Data(_display.forecolor);			// desenha 2 px w de FC
				LCD_Data(_display.forecolor);
			}
	        else
			{
    	        LCD_Data(_display.backcolor);			// desenha 2 px w de BC
				LCD_Data(_display.backcolor);
			}
		}
		for(w=0;w<_display.font_w;w++)			// segunda linha igual a primeira
		{
			if(*d_char & (0x80 >> w))
			{
				LCD_Data(_display.forecolor);
				LCD_Data(_display.forecolor);
			}
	        else
			{
    	        LCD_Data(_display.backcolor);					
				LCD_Data(_display.backcolor);
			}
		}
		d_char += 1;
 	}	
	return x+(_display.font_w * 2);
}
//----------------------------------------------------------
//
//----------------------------------------------------------
int DISPLAY_Char(int x, int y, unsigned char c)
{
#ifdef UPPER_CASE_FONT
    if(c >= 'a' )
        c=toupper(c);	
#endif

    if(!_display.font)
    	DISPLAY_SetFont(FONT_INIT);

    c -= 0x20;    
    LCD_Window(x,y,_display.font_w,_display.font_h);	
	return _display.drawChar(x, y, (uint8_t*)_display.font+(c*_display.font_h));
}
//----------------------------------------------------------
//
//----------------------------------------------------------
int DISPLAY_Text(int x, int y, const char *s)
{   
    while(*s)
        x = DISPLAY_Char(x,y,*s++);
	return x;
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void *DISPLAY_GetReference(void){
	DISPLAY_Init();
	return &_display;
}

void DISPLAY_Init(void){
	_display.font = (uint8_t*)(FONTDEFAULT+2);
	_display.font_w = FONTDEFAULT[0];
	_display.font_h = FONTDEFAULT[1];
	_display.forecolor = WHITE;
	_display.backcolor = BLACK;
	_display.cx = 0;
	_display.cy = 0;
	_display.sc = 0;
	_display.xputc = DISPLAY_ConsoleChar;
	_display.drawChar = drawCharSimple;
}
//----------------------------------------------------------
//TODO: fix ignore atribute when setAttribute is called before
//any drawChar
//----------------------------------------------------------
void DISPLAY_SetFont(uint8_t fnt)
{
#ifndef MULTIPLE_FONTS
	if(_display.drawChar != drawCharSimple)
		DISPLAY_Init();
#else
	switch(fnt)
	{
		default:
			_display.forecolour = WHITE;
			_display.backcolour = BLACK;
			if(!_display.drawChar)
				_display.drawChar = drawCharSimple;	
		case FONT_DEFAULT:
			_display.font = (uint8_t*)(FONTDEFAULT);			
			break;
		case FONT_BOLD:
			_display.font = (uint8_t*)(FONTBOLD);			
			break;
		case FONT_LCD:
			_display.font = (uint8_t*)(FONTLCD);			
			break;
		case FONT_PIXELDUST:
			_display.font = (uint8_t*)(FONTPIXELDUST);			
			break;
	}	
	_display.font_w = _display.font[0];
	_display.font_h = _display.font[1];
	_display.font += 2;	
#endif			
}
//----------------------------------------------------------
//
//----------------------------------------------------------
void DISPLAY_SetAttribute(uint8_t atr){
	switch(atr){
		default:
		case FONT_NORMAL: _display.drawChar = drawCharSimple;break;
		case FONT_INVERTED: _display.drawChar = drawCharInverted;break;
		case FONT_DOUBLE: _display.drawChar = drawCharDouble;break;
		case FONT_TRANSPARENT: _display.drawChar = drawCharTransparent;break;
	}
}
//----------------------------------------------------------
// setwrap(sx,sy,w,h) tem de ser chamado anteriormente
// a ESTE drawPixel(x,y) pixel com cor Foreground color
//----------------------------------------------------------
void DISPLAY_Pixel(int x0, int y0)
{
    LCD_Goto(x0,y0);
    LCD_Data(_display.forecolor);
}
//----------------------------------------------------------
// 
//----------------------------------------------------------
int DISPLAY_Uint(int x, int y, unsigned int v, unsigned char radix)
{
unsigned char i=0,c,dig[8];
	do{
		c = (unsigned char)(v % radix);		
		v /= radix;
		dig[i++]=c;
	} while(v);
	
	while(i--){
		c = dig[i];
		if (c >= 10)
			c += ('A'-('9'+1));
		x = DISPLAY_Char(x,y, c + '0');		
	}
	return x;
}
