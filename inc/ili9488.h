#ifndef _INC_ILI9488_H_
#define _INC_ILI9488_H_
#include "stm32f446xx.h"
#include "timer.h"
#include <math.h>
#include <stdlib.h>

#define ILI9488_CMD_NOP 0U            // No Operation
#define ILI9488_CMD_SR 0x1U           // SOft Reset
#define ILI9488_CMD_RDII 0x4U         // Read Display Identification Information
#define ILI9488_CMD_RNED 0x5U         // Read Number of the Errors on DSI
#define ILI9488_CMD_RDS 0x9U          // Read Display Status
#define ILI9488_CMD_RDPM 0xAU         // Read Display Power Mode
#define ILI9488_CMD_RDMADCTL 0xBU     // Read Display MADCTL
#define ILI9488_CMD_RDPF 0xCU         // Read Display Pixel Format
#define ILI9488_CMD_RDIM 0xDU         // Read Display Image Mode
#define ILI9488_CMD_RDSM 0xEU         // Read Display Signal Mode
#define ILI9488_CMD_RDSDR 0xFU        // Read Display Self-Diagonstic Result
#define ILI9488_CMD_SLPIN 0x10U       // Sleep II
#define ILI9488_CMD_SLPOUT 0x11U      // Sleep OUT
#define ILI9488_CMD_PAMON 0x12U       // Partial Mode ON
#define ILI9488_CMD_NORMDMON 0x13U    // Normal Display Mode ON
#define ILI9488_CMD_DISPINVOFF 0x20U  // Display Inversion OFF
#define ILI9488_CMD_DISPINVON 0x21U   // Display Inversion ON
#define ILI9488_CMD_ALLPIXOFF 0x22U   // All Pixel OFF
#define ILI9488_CMD_ALLPIXON 0x23U    // All Pixel ON
#define ILI9488_CMD_DISPOFF 0x28U     // Display OFF
#define ILI9488_CMD_DISPON 0x29U      // Display ON
#define ILI9488_CMD_CASET 0x2AU       // Column Addrss Set
#define ILI9488_CMD_PASET 0x2BU       // Page Address Set
#define ILI9488_CMD_MEMWR 0x2CU       // Memory Write
#define ILI9488_CMD_MEMRD 0x2EU       // Memory Read
#define ILI9488_CMD_PAREA 0x30U       // Partial Area
#define ILI9488_CMD_VSCROLDEF 0x33U   // Vartical Scrolling Definition
#define ILI9488_CMD_TELOFF 0x34U      // Tearing Effect Line OFF
#define ILI9488_CMD_TELON 0x35U       // Tearing Effect Line ON
#define ILI9488_CMD_MADCTL 0x36U      // Memory Access Control
#define ILI9488_CMD_VSCROLSRADD 0x37U // Vertical Scrolling Start Address
#define ILI9488_CMD_IDLMOFF 0x38U     // Idle Mode OFF
#define ILI9488_CMD_IDLMON 0x39U      // Idle Mode ON
#define ILI9488_CMD_IPIXFMT 0x3AU     // Interface Pixel Format
#define ILI9488_CMD_MEMWRC 0x3CU      // Memory Write Continue
#define ILI9488_CMD_MEMRDC 0x3EU      // Memroy Read Continue
#define ILI9488_CMD_WRTSL 0x44U       // Write Tear Scan Line
#define ILI9488_CMD_RDTSL 0x45U       // Read Tear Scan Line
#define ILI9488_CMD_WRDBV 0x51U       // Write Display Brightness Value
#define ILI9488_CMD_RDDBV 0x52U       // Read Display Brightness Value
#define ILI9488_CMD_WRCTRLDV 0x53U    // Write CTRL Display Value
#define ILI9488_CMD_RDCTRLDV 0x54U    // Read CTRL Display Value
#define ILI9488_CMD_WRCABCV 0x55U     // Write Content Adapticve Brightness Control Value
#define ILI9488_CMD_RDCABCV 0x56U     // Read Content Adapticve Brightness Control Value
#define ILI9488_CMD_WRCABCMB 0x5EU    // Write CABC Minimum Brightness
#define ILI9488_CMD_WRCABCMB 0x5FU    // Read CABC Minimum Brightness
#define ILI9488_CMD_RDAB 0x68U        // Read Automatic Brightness
#define ILI9488_CMD_RDID1 0xDAU       // Read ID1
#define ILI9488_CMD_RDID2 0xDBU       // Read ID2
#define ILI9488_CMD_RDID3 0xDCU       // Read ID3
#define ILI9488_CMD_PGAMCTRL 0xE0U    // Positive Gamma Control
#define ILI9488_CMD_NGAMCTRL 0xE1U    // Negative Gamma Control

/*Color Section*/
#define ILI9488_BLACK 0x0000       /*   0,   0,   0 */
#define ILI9488_NAVY 0x000F        /*   0,   0, 128 */
#define ILI9488_DARKGREEN 0x03E0   /*   0, 128,   0 */
#define ILI9488_DARKCYAN 0x03EF    /*   0, 128, 128 */
#define ILI9488_MAROON 0x7800      /* 128,   0,   0 */
#define ILI9488_PURPLE 0x780F      /* 128,   0, 128 */
#define ILI9488_OLIVE 0x7BE0       /* 128, 128,   0 */
#define ILI9488_LIGHTGREY 0xC618   /* 192, 192, 192 */
#define ILI9488_DARKGREY 0x7BEF    /* 128, 128, 128 */
#define ILI9488_BLUE 0x001F        /*   0,   0, 255 */
#define ILI9488_GREEN 0x07E0       /*   0, 255,   0 */
#define ILI9488_CYAN 0x07FF        /*   0, 255, 255 */
#define ILI9488_RED 0xF800         /* 255,   0,   0 */
#define ILI9488_MAGENTA 0xF81F     /* 255,   0, 255 */
#define ILI9488_YELLOW 0xFFE0      /* 255, 255,   0 */
#define ILI9488_WHITE 0xFFFF       /* 255, 255, 255 */
#define ILI9488_ORANGE 0xFD20      /* 255, 165,   0 */
#define ILI9488_GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define ILI9488_PINK 0xF81F        /*31, 0, 31*/
#define ILI9488_DARKRED 0x7000     /*14, 0, 0*/
#define ILI9488_SEAGREEN 0x036b    /*0, 27, 11*/

/*MADCTL*/
#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH 0x04

#define ILI9488_WIDTH 320U
#define ILI9488_HEIGHT 480U

#define ILI9488_RESET_DELAY 50

#define ILI9488_CS_PIN 6
#define ILI9488_CS_PORT GPIOB
#define ILI9488_RST_PIN 7
#define ILI9488_RST_PORT GPIOC
#define ILI9488_DC_PIN 9
#define ILI9488_DC_PORT GPIOA

#define ILI9488_DC_DATA() (ILI9488_DC_PORT->BSRR |= (1U << ILI9488_DC_PIN));
#define ILI9488_DC_COMMAND() (ILI9488_DC_PORT->BSRR |= (1U << (ILI9488_DC_PIN + 16)));

#define ILI9488_CS_DISABLE() (ILI9488_CS_PORT->BSRR |= (1U << ILI9488_CS_PIN));
#define ILI9488_CS_ENABLE() (ILI9488_CS_PORT->BSRR |= (1U << (ILI9488_CS_PIN + 16)));

#define ILI9488_RST_DISABLE() (ILI9488_RST_PORT->BSRR |= (1U << ILI9488_RST_PIN));
#define ILI9488_RST_ENABLE() (ILI9488_RST_PORT->BSRR |= (1U << (ILI9488_RST_PIN + 16)));

void ILI9488_Init();
void ILI9488_SetRotation(uint8_t r);
void ILI9488_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void ILI9488_FillScreen(uint16_t color);
void ILI9488_TestColors();
void ILI9488_DrawPixel(int16_t x, int16_t y, uint16_t color);
void ILI9488_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ILI9488_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

#endif