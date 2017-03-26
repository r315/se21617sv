#include <spi.h>
#include <time_m3.h>
//#include <lcd.h>
#include "ili9341.h"


void LCD_Bkl(uint32_t state){
    if(state) LCD_BKL1;
    else LCD_BKL0;
}

void LCD_Data(uint16_t data){
    LCD_RS1;
    LCD_CS0;
    SPI_Send(data);
    LCD_CS1;
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

void LCD_Command(uint16_t data){
    LCD_RS0;
    LCD_CS0;
    SPI_Send(data);
    LCD_CS1;
}

void LCD_Window(uint32_t x, uint32_t y, uint32_t w, uint32_t h){
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
        TIME_DelayMs(10);
        LCD_RST1;
        
        LCD_Command(SWRST); 
        TIME_DelayMs(5);
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
        LCD_Command(MAC);      // Memory Access Control 
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
        TIME_DelayMs(120);
                
        LCD_Command(DISPON);    //Display on 
        TIME_DelayMs(120);
        LCD_Command(RAMWR);  

        LCD_BKL1; 
}

