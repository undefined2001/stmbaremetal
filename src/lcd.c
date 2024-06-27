#include "lcd.h"
#define __STATIC_VOID static void

__STATIC_VOID lcd_send_data(char data)
{
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);
    data_t[0] = data_u | 0x0D; // en=1, rs=1
    data_t[1] = data_u | 0x09; // en=0, rs=1
    data_t[2] = data_l | 0x0D; // en=1, rs=1
    data_t[3] = data_l | 0x09; // en=0, rs=1
    I2C_Write_Buffer(LCD_ADDRESS, data_t, 4);
}

__STATIC_VOID lcd_send_command(char cmd)
{
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C; // en=1, rs=0
    data_t[1] = data_u | 0x08; // en=0, rs=0
    data_t[2] = data_l | 0x0C; // en=1, rs=0
    data_t[3] = data_l | 0x08; // en=0, rs=0
    I2C_Write_Buffer(LCD_ADDRESS, data_t, 4);
}

void lcd_init(void)
{
    // 4 bit initialisation
    DelayMS(50); // wait for >40ms
    lcd_send_command(0x30);
    DelayMS(5); // wait for >4.1ms
    lcd_send_command(0x30);
    DelayMS(1); // wait for >100us
    lcd_send_command(0x30);
    DelayMS(10);
    lcd_send_command(LCD_CMD_4BIT_MODE); // 4bit mode
    DelayMS(10);

    // dislay initialisation
    lcd_send_command(0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
    DelayMS(1);
    lcd_send_command(0x08); // Display on/off control --> D=0,C=0, B=0  ---> display off
    DelayMS(1);
    lcd_send_command(LCD_CMD_CLEAR_DISPLAY); // clear display
    DelayMS(2);
    lcd_send_command(0x06); // Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
    DelayMS(1);
    lcd_send_command(0x0C); // Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_printf(const char *format, ...)
{
    char buffer[20] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    char *data;
    if (strlen(buffer) > 16)
    {
        data = "Len Ovf: Len>16";
    }
    else
    {
        data = buffer;
    }
    while (*data)
        lcd_send_data(*data++);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t address = (row) ? 0x40 : 0x00;
    address += col;
    lcd_send_command(0x80 | address);
}

void lcd_clear_display()
{
    lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
    DelayMS(10);
}