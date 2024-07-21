#include "lcd.h"
#define __STATIC_VOID static void


__STATIC_VOID LCD_SendData(char data)
{
    char data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xf0); //1111 0000
    data_l = ((data << 4) & 0xf0); //1111 0000
    data_t[0] = data_u | 0x0D; // en=1, rs=1 
    data_t[1] = data_u | 0x09; // en=0, rs=1
    data_t[2] = data_l | 0x0D; // en=1, rs=1     
    data_t[3] = data_l | 0x09; // en=0, rs=1
    I2C_Write_Buffer(LCD_ADDRESS, data_t, 4);
}

__STATIC_VOID LCD_SendCommand(char cmd)
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

void LCD_Init(void)
{
    DelayMS(50);
    LCD_SendCommand(0x30);
    DelayMS(5); 
    LCD_SendCommand(0x30);
    DelayMS(1); 
    LCD_SendCommand(0x30);
    DelayMS(10);
    LCD_SendCommand(LCD_CMD_4BIT_MODE); 
    DelayMS(10);

 
    LCD_SendCommand(0x28);
    DelayMS(1);
    LCD_SendCommand(0x08); 
    DelayMS(1);
    LCD_SendCommand(LCD_CMD_CLEAR_DISPLAY); 
    DelayMS(2);
    LCD_SendCommand(0x06);
    DelayMS(1);
    LCD_SendCommand(0x0C); 
}

void LCD_Printf(const char *format, ...) //lcd_printf("Temp: %d", temp)
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
        LCD_SendData(*data++);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address = (row) ? 0x40 : 0x00;
    address += col;
    LCD_SendCommand(0x80 | address);
}

void LCD_ClearDisplay()
{
    LCD_SendCommand(LCD_CMD_CLEAR_DISPLAY);
    DelayMS(10);
}
