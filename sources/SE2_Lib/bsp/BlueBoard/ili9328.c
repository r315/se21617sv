//#include <clock.h>
#include <ili9328.h>
#include <time_m3.h>

//--------------------------------------------------------
//
//--------------------------------------------------------				 
uint16_t LCD_ReadData(void)
{
uint16_t dta;
	LCDRS1; 
	DATAPORT &= ~0xFF;
	LCDRD0;LCDRD0;LCDRD0; dta = DATAPORT << 8; LCDRD1;
	LCDRD0;LCDRD0;LCDRD0; dta |=DATAPORT; LCDRD1;
	DATAPORT |= 0xFF;
return dta;
}
//--------------------------------------------------------
//
//--------------------------------------------------------				 
#if 0
uint16_t LCD_ReadReg(void)
{
uint16_t reg;
	LCDRS0; DATAPORT &= ~0xFF;
	LCDRD0;LCDRD0;LCDRD0; reg = DATAPORT << 8; LCDRD1;
	LCDRD0;LCDRD0;LCDRD0; reg |=DATAPORT; LCDRD1;
	DATAPORT |= 0xFF;
return reg;
}
#endif
//--------------------------------------------------------
//write data to LCD
//TODO: ver se � possivel alterar 2 sinals simultaneamente
//--------------------------------------------------------
void LCD_Data(uint16_t data)
{	
	LCDRS1;	
	DATAPORT = data >> 8; // MSB
	LCDWR0;LCDWR0;LCDWR1;
	DATAPORT = data;      // LSB
	LCDWR0;LCDWR0;LCDWR1;	
}
//--------------------------------------------------------
//write instruction to LCD
//--------------------------------------------------------
void LCD_Cmd(uint8_t ins) 
{
	LCDRS0;	
	DATAPORT = 0;
	LCDWR0;LCDWR0;LCDWR1;
	DATAPORT = ins;
	LCDWR0;LCDWR0;LCDWR1;
}
//--------------------------------------------------------
//
//--------------------------------------------------------
void LCD_Fill(uint32_t count, uint16_t color){
	LCDRS1;	
	while(count--){
		DATAPORT = color>>8;
		LCDWR0;LCDWR0;LCDWR1;
		DATAPORT = color;	
		LCDWR0;LCDWR0;LCDWR1;
	}
}
//-----------------------------------------------------*/
// Set Gram addres inside a created window
//------------------------------------------------------
void LCD_GotoMem(uint16_t x, uint16_t y){
	LCD_Cmd(GRAM_ADX);
	LCD_Data(x);
   	LCD_Cmd(GRAM_ADY);
	LCD_Data(y);
    LCD_Cmd(LCD_RW_GRAM);	
}
//-------------------------------------------------------------------
//	 Creates GRAM Access Range	 
//-------------------------------------------------------------------
void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{    
    LCD_Cmd(START_ADX);
	LCD_Data(x);
	LCD_Cmd(END_ADX);
	LCD_Data(x + w-1);
	LCD_Cmd(START_ADY);
	LCD_Data(y);
	LCD_Cmd(END_ADY);
	LCD_Data(y + h-1);
	
	LCD_GotoMem(x,y);
}
//-----------------------------------------------------------------*/
//	 set VLE bit on GATE_SCAN_CTRL2(61h) first
//-------------------------------------------------------------------
void LCD_Scroll(uint16_t y)
{
	while (y < 0)
 		y += 320;
	while (y >= 320)
 		y -= 320;
	LCD_Cmd(LCD_GATE_SCAN_CTRL3);
 	LCD_Data(y);
}
//--------------------------------------------------------
// initialize lcd
//--------------------------------------------------------
void LCD_Init(void){
	
	LCD_IO_INIT;
	
	LCDCS1;LCDWR1;LCDRD1;
	LCDRST0;		 
	TIME_DelayMs(100);
	LCDRST1;
	LCDCS0;

	LCD_Cmd(0xE5);
	LCD_Data(0x8000); 					//set the internal vcore voltage
	LCD_Cmd(LCD_START_OSC);
	LCD_Data(0x0001); 					//start oscillator
	TIME_DelayMs(50);

	LCD_Cmd(LCD_DRIV_OUT_CTRL);
	LCD_Data(SHIFT_DIR);
	LCD_Cmd(LCD_DRIV_WAV_CTRL);
	LCD_Data(0x0700); 					//set 1 line inversion
	
	LCD_Cmd(LCD_ENTRY_MOD);
	LCD_Data(VAL_ENTRY_MOD);			//set GRAM write direction, BGR=0

	LCD_Cmd(LCD_RESIZE_CTRL);
	LCD_Data(0x0000); 					//no resizing

	LCD_Cmd(LCD_DISP_CTRL2);
	LCD_Data(0x0202); 					//front & back porch periods = 2
	LCD_Cmd(LCD_DISP_CTRL3);
	LCD_Data(0x0000); 					
	LCD_Cmd(LCD_DISP_CTRL4);
	LCD_Data(0x0000); 					
	LCD_Cmd(LCD_RGB_DISP_IF_CTRL1);
	LCD_Data(0x0000); 					//select system interface				
	LCD_Cmd(LCD_FRM_MARKER_POS);
	LCD_Data(0x0000); 					
	LCD_Cmd(LCD_RGB_DISP_IF_CTRL2);
	LCD_Data(0x0000);					
	
	LCD_Cmd(LCD_POW_CTRL1);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_POW_CTRL2);
	LCD_Data(0x0000); 					
	LCD_Cmd(LCD_POW_CTRL3);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_POW_CTRL4);
	LCD_Data(0x0000); 					
	TIME_DelayMs(200);

	LCD_Cmd(LCD_POW_CTRL1);
	LCD_Data(0x17B0);
	LCD_Cmd(LCD_POW_CTRL2);
	LCD_Data(0x0137); 					
	TIME_DelayMs(50);

	LCD_Cmd(LCD_POW_CTRL3);
	LCD_Data(0x013C);
	TIME_DelayMs(50);

	LCD_Cmd(LCD_POW_CTRL4);
	LCD_Data(0x1400);
	LCD_Cmd(LCD_POW_CTRL7);
	LCD_Data(0x0007);
	TIME_DelayMs(50);

	LCD_Cmd(LCD_GAMMA_CTRL1);
	LCD_Data(0x0007);
	LCD_Cmd(LCD_GAMMA_CTRL2);
	LCD_Data(0x0504);
	LCD_Cmd(LCD_GAMMA_CTRL3);
	LCD_Data(0x0703);
	LCD_Cmd(LCD_GAMMA_CTRL4);
	LCD_Data(0x0002);
	LCD_Cmd(LCD_GAMMA_CTRL5);
	LCD_Data(0x0707);
	LCD_Cmd(LCD_GAMMA_CTRL6);
	LCD_Data(0x0406);
	LCD_Cmd(LCD_GAMMA_CTRL7);
	LCD_Data(0x0006);
	LCD_Cmd(LCD_GAMMA_CTRL8);
	LCD_Data(0x0404);
	LCD_Cmd(LCD_GAMMA_CTRL9);
	LCD_Data(0x0700);
	LCD_Cmd(LCD_GAMMA_CTRL10);
	LCD_Data(0x0A08);

	LCD_Cmd(LCD_GRAM_HOR_AD);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_GRAM_VER_AD);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_HOR_START_AD);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_HOR_END_AD);
	LCD_Data(0x00EF);
	LCD_Cmd(LCD_VER_START_AD);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_VER_END_AD);
	LCD_Data(0x013F);
	LCD_Cmd(LCD_GATE_SCAN_CTRL1);
	LCD_Data(VAL_GATE_SCAN);
	LCD_Cmd(LCD_GATE_SCAN_CTRL2);
	LCD_Data(0x0003);		//
	LCD_Cmd(LCD_GATE_SCAN_CTRL3);
	LCD_Data(0x0000);

	LCD_Cmd(LCD_PART_IMG1_DISP_POS);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_PART_IMG1_START_AD);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_PART_IMG1_END_AD);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_PART_IMG2_DISP_POS);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_PART_IMG2_START_AD);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_PART_IMG2_END_AD);
	LCD_Data(0x0000);

	LCD_Cmd(LCD_PANEL_IF_CTRL1);
	LCD_Data(0x0010);
	LCD_Cmd(LCD_PANEL_IF_CTRL2);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_PANEL_IF_CTRL3);
	LCD_Data(0x0003);
	LCD_Cmd(LCD_PANEL_IF_CTRL4);
	LCD_Data(0x0110);
	LCD_Cmd(LCD_PANEL_IF_CTRL5);
	LCD_Data(0x0000);
	LCD_Cmd(LCD_PANEL_IF_CTRL6);
	LCD_Data(0x0000);

	LCD_Cmd(LCD_DISP_CTRL1);
	LCD_Data(0x0173);
	TIME_DelayMs(500);
	
	LCD_BKL_ON;
}

