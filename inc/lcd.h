#ifndef INC_LCD_H
#define INC_LCD_H
#include "i2c.h"
#include "timer.h"
#include "string.h"
#include <stdio.h>
#include <stdarg.h>

#define LCD_ADDRESS 0x27
#define LCD_CMD_CLEAR_DISPLAY 0x01
#define LCD_CMD_4BIT_MODE 0x20
void lcd_init();
void lcd_printf(const char *format, ...);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_clear_display();

#endif