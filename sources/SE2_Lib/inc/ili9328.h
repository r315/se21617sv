 /** 
 @file ili9328.h
 @brief  ILI9328 Lcd controller driver
 @author Hugo Reis
 @date 06-02-2017
 **/

#ifndef _ili9328_h_
#define _ili9328_h_

#include <LPC17xx.h>
#include <stdint.h>

//-------------------------------------------------------------------
#define LCD_START_OSC			0x00
#define LCD_DRIV_OUT_CTRL		0x01
#define LCD_DRIV_WAV_CTRL		0x02
#define LCD_ENTRY_MOD			0x03
#define LCD_RESIZE_CTRL			0x04
#define LCD_DISP_CTRL1			0x07
#define LCD_DISP_CTRL2			0x08
#define LCD_DISP_CTRL3			0x09
#define LCD_DISP_CTRL4			0x0A
#define LCD_RGB_DISP_IF_CTRL1	0x0C
#define LCD_FRM_MARKER_POS		0x0D
#define LCD_RGB_DISP_IF_CTRL2	0x0F
#define LCD_POW_CTRL1			0x10
#define LCD_POW_CTRL2			0x11
#define LCD_POW_CTRL3			0x12
#define LCD_POW_CTRL4			0x13
#define LCD_GRAM_HOR_AD			0x20
#define LCD_GRAM_VER_AD			0x21
#define LCD_RW_GRAM				0x22
#define LCD_POW_CTRL7			0x29
#define LCD_FRM_RATE_COL_CTRL	0x2B
#define LCD_GAMMA_CTRL1			0x30
#define LCD_GAMMA_CTRL2			0x31
#define LCD_GAMMA_CTRL3			0x32
#define LCD_GAMMA_CTRL4			0x35 
#define LCD_GAMMA_CTRL5			0x36
#define LCD_GAMMA_CTRL6			0x37
#define LCD_GAMMA_CTRL7			0x38
#define LCD_GAMMA_CTRL8			0x39
#define LCD_GAMMA_CTRL9			0x3C
#define LCD_GAMMA_CTRL10		0x3D
#define LCD_HOR_START_AD		0x50
#define LCD_HOR_END_AD			0x51
#define LCD_VER_START_AD		0x52
#define LCD_VER_END_AD			0x53
#define LCD_GATE_SCAN_CTRL1		0x60
#define LCD_GATE_SCAN_CTRL2		0x61
#define LCD_GATE_SCAN_CTRL3		0x6A
#define LCD_PART_IMG1_DISP_POS	0x80
#define LCD_PART_IMG1_START_AD	0x81
#define LCD_PART_IMG1_END_AD	0x82
#define LCD_PART_IMG2_DISP_POS	0x83
#define LCD_PART_IMG2_START_AD	0x84
#define LCD_PART_IMG2_END_AD	0x85
#define LCD_PANEL_IF_CTRL1		0x90
#define LCD_PANEL_IF_CTRL2		0x92
#define LCD_PANEL_IF_CTRL3		0x93
#define LCD_PANEL_IF_CTRL4		0x95
#define LCD_PANEL_IF_CTRL5		0x97
#define LCD_PANEL_IF_CTRL6		0x98
#define LCD_DELAY 0xFF

/*-------------------------------------------------------------------
 
  v-- origin in software
  ___________________ 
 | _______________   |	ORIENTATION90
 ||               |[]|	ENTRY MOD = 0x38
 ||               |  |	SHIFT DIR = 0x0000
 || ------        |[]|	SCAN CTRL1 = 0xA700
 ||      /        |  |
 ||      ----->   |[]|
 ||               |  |
 ||_______________|[]|
 |___________________|
  ^-- LCD's origin (0x0000)
 
  ___________________ 
 | _______________   |	ORIENTATION270
 ||               |[]|	ENTRY MOD = 0x38
 ||               |  |	SHIFT DIR = 0x0100
 || <------       |[]|	SCAN CTRL1 = 0x2700
 ||       /       |  |
 ||      ------   |[]|
 ||	              |  |
 ||_______________|[]|
 |___________________|
                 ^-- origin in software
  ^-- LCD's origin (0x0000)

  ________________
 | ______________ |		
 ||B             ||		default
 ||              ||		ENTRY MOD = 0x30
 || ------       ||		SHIFT DIR = 0x0100
 ||      /       ||		SCAN CTRL1 = 0x2700
 ||      ------> ||		
 ||              ||
 ||              ||
 ||_____________E||
 | [] [] [] [] [] |
 |________________|		


  ________________
 | ______________ |		
 ||             B||		decrement
 ||              ||		ENTRY MOD = 0x20
 ||       ------ ||		SHIFT DIR = 0x0100
 ||      \       ||		SCAN CTRL1 = 0x2700
 || <-----       ||		
 ||              ||
 ||              ||
 ||E_____________||
 | [] [] [] [] [] |
 |________________|	

*/

//-------------------------------------------------------------------	
#if defined(LCD_LANDSCAPE)
	#define VAL_ENTRY_MOD	0x0038 | BGR_BIT
	#define SHIFT_DIR		0x0000 // clr ss
	#define VAL_GATE_SCAN	0xA700 
	#define GRAM_ADX		LCD_GRAM_VER_AD
	#define GRAM_ADY		LCD_GRAM_HOR_AD
	#define START_ADX   	LCD_VER_START_AD
	#define END_ADX   		LCD_VER_END_AD
	#define START_ADY   	LCD_HOR_START_AD
	#define END_ADY   		LCD_HOR_END_AD			
#elif defined(ORIENTATION270)
	#define VAL_ENTRY_MOD	0x0038 | BGR_BIT
	#define SHIFT_DIR		0x0100 // set ss
	#define VAL_GATE_SCAN	0x2700 
	#define GRAM_ADX		LCD_GRAM_VER_AD
	#define GRAM_ADY		LCD_GRAM_HOR_AD
	#define START_ADX   	LCD_VER_START_AD
	#define END_ADX   		LCD_VER_END_AD
	#define START_ADY   	LCD_HOR_START_AD
	#define END_ADY   		LCD_HOR_END_AD
#else
	#define VAL_ENTRY_MOD	0x0030	| BGR_BIT//16bit interface
	#define SHIFT_DIR		0x0100	 //set SS
	#define VAL_GATE_SCAN	0xA700 
	#define GRAM_ADX		LCD_GRAM_HOR_AD
	#define GRAM_ADY		LCD_GRAM_VER_AD
	#define START_ADX   	LCD_HOR_START_AD
	#define END_ADX   		LCD_HOR_END_AD
	#define START_ADY   	LCD_VER_START_AD
	#define END_ADY   		LCD_VER_END_AD	 	
#endif 

#ifdef BGR_MODE
	#define BGR_BIT 0
#else	
	#define BGR_BIT (1<<12)
#endif

//-------------------------------------------------------------------
#define  LCD_CS	 (1<<10) //P1.10
#define  LCD_RS	 (1<<9)	 //P1.9
#define  LCD_WR	 (1<<8)	 //P1.8
#define  LCD_RD	 (1<<4)	 //P1.4
#define  LCD_BKL (1<<1)  //P1.1 // verificar este circuito
#define  LCD_RST (1<<0)	 //P1.0	

#define  CTRLPORT LPC_GPIO1
#define  DATAPORT LPC_GPIO0->FIOPIN0

#define  LCDCS0    CTRLPORT->FIOCLR = LCD_CS;
#define  LCDCS1    CTRLPORT->FIOSET = LCD_CS; 
#define  LCDRS0    CTRLPORT->FIOCLR = LCD_RS;
#define  LCDRS1    CTRLPORT->FIOSET = LCD_RS;
#define  LCDWR0    CTRLPORT->FIOCLR = LCD_WR; //asm("nop");
#define  LCDWR1    CTRLPORT->FIOSET = LCD_WR;  
#define  LCDRD0    CTRLPORT->FIOCLR = LCD_RD;
#define  LCDRD1    CTRLPORT->FIOSET = LCD_RD; 
#define  LCDRST0   CTRLPORT->FIOCLR = LCD_RST;
#define  LCDRST1   CTRLPORT->FIOSET = LCD_RST; 
#define  LCD_BKL_OFF CTRLPORT->FIOCLR = LCD_BKL;
#define  LCD_BKL_ON  CTRLPORT->FIOSET = LCD_BKL;

#define LCD_IO_INIT                                                    \
	LPC_GPIO1->FIODIR |= LCD_CS|LCD_RS|LCD_WR|LCD_RD|LCD_BKL|LCD_RST;  \
	LPC_GPIO0->FIODIR |= 0xFF;

/**
 @brief 
 **/
uint16_t LCD_ReadData(void);
/**
 @brief 
 **/
void LCD_Data(uint16_t data);

/**
 @brief 
 **/
void LCD_Goto(uint16_t x, uint16_t y);

/**
 @brief 
 **/
void LCD_Fill( uint32_t n, uint16_t color);

/**
 @brief 
 **/
void LCD_Cmd(uint8_t ins);

#endif

