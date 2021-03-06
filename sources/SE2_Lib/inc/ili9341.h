/**
* @file     ili9341.h
* @brief    Contains the ILI9341 lcd controller API headers and symbols.
*           
* @version  1.0
* @date     5 Nov. 2016
* @author   Hugo Reis
**********************************************************************/

#ifndef _ili9341_h_
#define _ili9341_h_

#include <gpio.h>

#define LCD_W LCD_GetWidth()   //240
#define LCD_H LCD_GetHeight()  //320
#define LCD_SIZE LCD_GetSize() //LCD_W * LCD_H

#define TFT_W 240
#define TFT_H 320

#include <stdint.h>
#if defined(__LPCXpresso__)
/**
* @brief Lcd Pin configuration:
*       P0.16  CS
*       P0.23  RST
*       P0.24  C'/D
*       P0.18  SI
*       P0.15  SCK
*       P0.25  Backlight enable
*       P0.17  SO
**/

#define LCD_CS   16
#define LCD_RS   24
#define LCD_RST  23
#define LCD_BKL  25

#else
#include <lpc2106.h>
/**
* @brief Lcd Pin configuration:
*       P0.3  CS
*       P0.4  SCK
*       P0.5  SO
*       P0.6  SI      
*       P0.8  Backlight enable
*       P0.9  C'/D
*       p0.10 RST
**/
 
#define LCD_CS   3
#define LCD_RS   9
#define LCD_RST 10
#define LCD_BKL  8

#endif

#define LCD_CS0  GPIO_Clr(LCD_CS)
#define LCD_CS1  GPIO_Set(LCD_CS)
#define LCD_RS0  GPIO_Clr(LCD_RS)
#define LCD_RS1  GPIO_Set(LCD_RS)
#define LCD_RST0 GPIO_Clr(LCD_RST)
#define LCD_RST1 GPIO_Set(LCD_RST)
#define LCD_BKL0 GPIO_Clr(LCD_BKL)
#define LCD_BKL1 GPIO_Set(LCD_BKL)

#define LCD_PIN_INIT                    \
            GPIO_SetOutput(LCD_CS);     \
            GPIO_SetOutput(LCD_RS);     \
            GPIO_SetOutput(LCD_RST);    \
            GPIO_SetOutput(LCD_BKL);    \


#define SWRST    0x01
#define RDDID    0x04
#define SLPIN    0x10
#define SLPOUT   0x11
#define DISPOFF  0x28
#define DISPON   0x29
#define CASET    0x2A
#define PASET    0x2B
#define RAMWR    0x2C
#define MADCTL   0x36   // Memory Access Control
#define VSCRSADD 0x37
#define COLMOD   0x3A   // Pixel Format set
#define FRCONN   0xB1   // Frame rate control Normal mode
#define FRCONI   0xB2   // Frame rate control Idle mode
#define FRCONP   0xB3   // Frame rate control Partial mode
#define DFCTL    0xB6   // Display Function Control
#define PCON1    0xC0   // power control 1
#define PCON2    0xC1   // power control 2
#define VCOM1    0xC5   // VCOM control 1
#define VCOM2    0xC7   // VCOM control 2
#define PCONA    0xCB   // Power Control A
#define PCONB    0xCF   // Power Control B
#define DTCONA   0xE8   // Driver Timming Control A
#define DTCONB   0xEA   // Driver Timming Control B
#define PSCON    0xED   // Power on Sequence control
#define PRCON    0xF7   // Pump ratio control

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

#define LCD_PORTRAIT  0
#define LCD_LANDSCAPE 1
#define LCD_REVERSE_PORTRAIT  2
#define LCD_REVERSE_LANDSCAPE 3


/**
* @brief Fills n pixels with the same color
*         Note: LCD_OpenFrame must be called 
*               setting a frame os n pixels
**/
void LCD_Fill(uint32_t n, uint16_t color);


/**
 * @brief
 */
void LCD_Scroll(uint16_t sc);

/**
 * @brief
 */
uint32_t LCD_GetWidth(void);

/**
 * @brief
 */
uint32_t LCD_GetHeight(void);

/**
 * @brief
 */
uint32_t LCD_GetSize(void);

/*
 * @brief set display rotation
 */
void LCD_Rotation(uint8_t m);

#endif

