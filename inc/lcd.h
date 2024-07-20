#ifndef INC_LCD_H
#define INC_LCD_H
#include "i2c.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "timer.h"

#define LCD_ADDRESS 0x27
#define LCD_CMD_CLEAR_DISPLAY 0x01
#define LCD_CMD_4BIT_MODE 0x20

void LCD_Init();
void LCD_Printf(const char *format, ...);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_ClearDisplay();


#endif