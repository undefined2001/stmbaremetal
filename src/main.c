#include "stm32f446xx.h"
#include "uart.h"
#include "timer.h"
#include "debug.h"
#include "dht.h"
#include "lcd.h"

int main()
{
    TimerInit();
    I2C_GPIO_Init();
    I2C_Init();
    UartInit();
    lcd_init();

    lcd_set_cursor(0, 0);
    
    int i = 0;

    while (1)
    {
        lcd_clear_display();
        lcd_printf("Number is: %d", i++);
        DelayMS(1000);
    }
    return 0;
}
