#ifndef _INC_ST7735_H_
#define _INC_ST7735_H_

#include "spi.h"
#include <stdint.h>

#define SPI_SCLK 5 // PC5
#define SPI_MOSI 6 // PC6
#define PIN_DC 9
#define PIN_RESET 7
#define PIN_CS 6

// Define screen resolution and offset
#define ST7735_WIDTH 128
#define ST7735_HEIGHT 160
#define ST7735_X_OFFSET 1
#define ST7735_Y_OFFSET 1

// Note: To not use CS, uncomment the following line and pull CS to ground.
//  #define ST7735_NO_CS

#define DATA_MODE() (GPIOA->BSRR |= (1 << PIN_DC))           // DC High
#define COMMAND_MODE() (GPIOA->BSRR |= (1 << (PIN_DC + 16))) // DC Low
#define RESET_HIGH() (GPIOC->BSRR |= (1 << PIN_RESET))
#define RESET_LOW() (GPIOC->BSRR |= (1 << (PIN_RESET + 16)))

#define START_WRITE() (GPIOB->BSRR |= (1 << (PIN_CS + 16))) // CS Low
#define END_WRITE() (GPIOB->BSRR |= (1 << PIN_CS))          // CS High

#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))
#define BGR565(r, g, b) ((((b) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((r) >> 3))
#define RGB RGB565

#define BLACK RGB(0, 0, 0)
#define NAVY RGB(0, 0, 123)
#define DARKGREEN RGB(0, 125, 0)
#define DARKCYAN RGB(0, 125, 123)
#define MAROON RGB(123, 0, 0)
#define PURPLE RGB(123, 0, 123)
#define OLIVE RGB(123, 125, 0)
#define LIGHTGREY RGB(198, 195, 198)
#define DARKGREY RGB(123, 125, 123)
#define RED RGB(0, 0, 255)
#define GREEN RGB(0, 255, 0)
#define CYAN RGB(0, 255, 255)
#define BLUE RGB(255, 0, 0)
#define MAGENTA RGB(255, 0, 255)
#define YELLOW RGB(255, 255, 0)
#define WHITE RGB(255, 255, 255)
#define ORANGE RGB(255, 165, 0)
#define GREENYELLOW RGB(173, 255, 41)
#define PINK RGB(255, 130, 198)

#define ST7735_RST_DELAY 50     // delay ms wait for reset finish
#define ST7735_SLPOUT_DELAY 120 // delay ms wait for sleep out finish

// System Function Command List - Write Commands Only
#define ST7735_SLPIN 0x10   // Sleep IN
#define ST7735_SLPOUT 0x11  // Sleep Out
#define ST7735_PTLON 0x12   // Partial Display Mode On
#define ST7735_NORON 0x13   // Normal Display Mode On
#define ST7735_INVOFF 0x20  // Display Inversion Off
#define ST7735_INVON 0x21   // Display Inversion On
#define ST7735_GAMSET 0x26  // Gamma Set
#define ST7735_DISPOFF 0x28 // Display Off
#define ST7735_DISPON 0x29  // Display On
#define ST7735_CASET 0x2A   // Column Address Set
#define ST7735_RASET 0x2B   // Row Address Set
#define ST7735_RAMWR 0x2C   // Memory Write
#define ST7735_PLTAR 0x30   // Partial Area
#define ST7735_TEOFF 0x34   // Tearing Effect Line Off
#define ST7735_TEON 0x35    // Tearing Effect Line On
#define ST7735_MADCTL 0x36  // Memory Data Access Control
#define ST7735_IDMOFF 0x38  // Idle Mode Off
#define ST7735_IDMON 0x39   // Idle Mode On
#define ST7735_COLMOD 0x3A  // Interface Pixel Format

// Panel Function Command List - Only Used
#define ST7735_GMCTRP1 0xE0 // Gamma '+' polarity Correction Characteristics Setting
#define ST7735_GMCTRN1 0xE1 // Gamma '-' polarity Correction Characteristics Setting

// MADCTL Parameters
#define ST7735_MADCTL_MH 0x04  // Bit 2 - Refresh Left to Right
#define ST7735_MADCTL_RGB 0x00 // Bit 3 - RGB Order
#define ST7735_MADCTL_BGR 0x08 // Bit 3 - BGR Order
#define ST7735_MADCTL_ML 0x10  // Bit 4 - Scan Address Increase
#define ST7735_MADCTL_MV 0x20  // Bit 5 - X-Y Exchange
#define ST7735_MADCTL_MX 0x40  // Bit 6 - X-Mirror
#define ST7735_MADCTL_MY 0x80  // Bit 7 - Y-Mirror

void tft_init(void);
void tft_set_cursor(uint16_t x, uint16_t y);
void tft_set_color(uint16_t color);
void tft_set_background_color(uint16_t color);
void tft_print_char(char c);
void tft_print(const char *str);
#endif