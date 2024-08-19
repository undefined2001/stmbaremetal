#include "stm32f446xx.h"
#include "tft.h"
#include "timer.h"
#include "debug.h"
#include "ili9488.h"

int main()
{
    TimerInit();
    ILI9488_Init();
    DelayMS(1000);
    ILI9488_FillScreen(ILI9488_SEAGREEN);
    ILI9488_SetRotation(2);
    for (int i = 0; i < 20; i++)
    {
        ILI9488_DrawCircle(160, 160, 150 - i, ILI9488_BLUE);
    }
    for (int i = 0; i < 20; i++)
    {
        ILI9488_DrawCircle(160, 160, 150 - 20 - i, ILI9488_GREEN);
    }
    for (int i = 0; i < 20; i++)
    {
        ILI9488_DrawCircle(160, 160, 150 - 40 - i, ILI9488_DARKRED);
    }
    for (int i = 0; i < 20; i++)
    {
        ILI9488_DrawCircle(160, 160, 150 - 60 - i, ILI9488_CYAN);
    }
    for (int i = 0; i < 20; i++)
    {
        ILI9488_DrawCircle(160, 160, 150 - 80 - i, ILI9488_RED);
    }
    for (int i = 0; i < 20; i++)
    {
        ILI9488_DrawCircle(160, 160, 150 - 100 - i, ILI9488_YELLOW);
    }
    for (int i = 0; i < 20; i++)
    {
        ILI9488_DrawCircle(160, 160, 150 - 120 - i, ILI9488_PINK);
    }
    for (int i = 0; i < 20; i++)
    {
        ILI9488_DrawCircle(160, 160, 150 - 140 - i, ILI9488_OLIVE);
    }

    // ILI9488_FillRect(10, 10, 200, 300, ILI9488_GREEN);
    while (1)
    {
        /* code */
    }
}
