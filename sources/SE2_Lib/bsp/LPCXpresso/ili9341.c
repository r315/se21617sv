#ifdef __USE_CMSIS
#include <time_m3.h>
#else
#include <clock.h>
#endif
#include <spi.h>

#include <ili9341.h>

static uint16_t _width, _height;

void LCD_Command(uint16_t data){
    LCD_RS0;
    LCD_CS0;
    SPI_Send(data);
    LCD_CS1;
}

void LCD_Data(uint16_t data){
    LCD_RS1;
    LCD_CS0;
    SPI_Send(data);
    LCD_CS1;
}

uint32_t LCD_GetWidth(void){
	return _width;
}

uint32_t LCD_GetHeight(void){
	return _height;
}

uint32_t LCD_GetSize(void){
	return _height*_width;
}

void LCD_Scroll(uint16_t sc){
	LCD_Command(VSCRSADD);
	LCD_Data(sc);
}

void LCD_Bkl(uint8_t state){
    if(state) LCD_BKL1;
    else LCD_BKL0;
}


void LCD_Fill(uint32_t n, uint16_t data){
    LCD_RS1;
    LCD_CS0;
    while(n--)        
        SPI_Send(data);
    LCD_CS1;
}

void LCD_IndexedColor(uint16_t *colors, uint8_t *index, uint32_t size){
    LCD_RS1;
    LCD_CS0;
    while(size--){
        SPI_Send(colors[*index]);
        index += 1;
    }
    LCD_CS1;
}

void LCD_Window(uint16_t x, uint16_t y, uint16_t w, uint16_t h){
    LCD_Command(CASET);  
    //LCD_Data((x<<8) | (x + w -1));   
    LCD_Data(x);
    LCD_Data(x + w -1);
  
    LCD_Command(PASET);
    LCD_Data(y);
    LCD_Data(y + h - 1);

    LCD_Command(RAMWR);
}

void LCD_Init(void){

        LCD_PIN_INIT;

        LCD_BKL0;

        LCD_RST0;
        DelayMs(10);
        LCD_RST1;
        
        LCD_Command(SWRST); 
        DelayMs(5);
        LCD_Command(DISPOFF); 
        
        LCD_Command(PCONB);  
        LCD_Data(0x00); 
        LCD_Data(0XC1); 
        LCD_Data(0X30); 
        
        LCD_Command(PSCON);  
        LCD_Data(0x64); 
        LCD_Data(0x03); 
        LCD_Data(0X12); 
        LCD_Data(0X81); 
        
        LCD_Command(DTCONA);  
        LCD_Data(0x85); 
        LCD_Data(0x00); 
        LCD_Data(0x78);        
  
        LCD_Command(PCONA);  
        LCD_Data(0x39); 
        LCD_Data(0x2C); 
        LCD_Data(0x00); 
        LCD_Data(0x34); 
        LCD_Data(0x02); 

        LCD_Command(PRCON);  
        LCD_Data(0x20);         

        LCD_Command(DTCONB);  
        LCD_Data(0x00); 
        LCD_Data(0x00);         
        /* Power Control */
        LCD_Command(PCON1);    
        LCD_Data(0x23);      //VRH[5:0] 
 
        LCD_Command(PCON2);
        LCD_Data(0x10);      //SAP[2:0];BT[3:0] 
        /* VCOM Control */
        LCD_Command(VCOM1);
        LCD_Data(0x3e);      //Contrast
        LCD_Data(0x28); 
 
        LCD_Command(VCOM2);
        LCD_Data(0x86);  
        /* Memory Access Control */
        LCD_Command(MADCTL);  // Memory Access Control
        LCD_Data(0x48);       //C8  //48 68??//28 E8 ??

        LCD_Command(COLMOD);    
        LCD_Data(0x55);       //16bit/pixel
        /* frame rate */
        LCD_Command(FRCONN);    
        LCD_Data(0x00);  
        LCD_Data(0x18); 
        /* Gamma */
        
        /* ddram */
        
        /*  display  */
        LCD_Command(DFCTL);    // Display Function Control 
        LCD_Data(0x08); 
        LCD_Data(0x82);
        LCD_Data(0x27);
        LCD_Data(0x00);    

        LCD_Command(SLPOUT);    //Exit Sleep 
        DelayMs(120);
                
        LCD_Command(DISPON);    //Display on 
        DelayMs(120);
        LCD_Command(RAMWR);

        _width  = TFT_W;
        _height = TFT_H;
        // user must enable backlight after LCD_Init
}

void LCD_Rotation(uint8_t m) {

    switch (m) {
        case 0:
            m = (MADCTL_MX | MADCTL_BGR);
            _width  = TFT_W;
            _height = TFT_H;
            break;
        case 1:
            m = (MADCTL_MV | MADCTL_BGR);
            _width  = TFT_H;
            _height = TFT_W;
            break;
        case 2:
            m = (MADCTL_MY | MADCTL_BGR);
            _width  = TFT_W;
            _height = TFT_H;
            break;
        case 3:
            m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
            _width  = TFT_H;
            _height = TFT_W;
            break;

        default:
        	return;
    }
    LCD_Command(MADCTL);
    LCD_Data(m<<8);
}


