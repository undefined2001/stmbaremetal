
#include "tft.h"
#include "timer.h"
#include "font5x7.h"

static SPI_HandleTypeDef tft_spi;

// ST7735 Datasheet
// https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf
// Delays

// COLMOD Parameter
#define ST7735_COLMOD_16_BPP 0x05 // 101 - 16-bit/pixel

// 5x7 Font
#define FONT_WIDTH 5  // Font width
#define FONT_HEIGHT 7 // Font height

static uint16_t _cursor_x = 0;
static uint16_t _cursor_y = 0;                   // Cursor position (x, y)
static uint16_t _color = WHITE;                  // Color
static uint16_t _bg_color = BLACK;               // Background color
static uint8_t _buffer[ST7735_WIDTH << 1] = {0}; // DMA buffer, long enough to fill a row.

/// \brief Send 8-Bit Command
/// \param cmd 8-bit command
static void write_command_8(uint8_t cmd)
{
    COMMAND_MODE();
    SPI_MasterTransmit(&tft_spi, &cmd, 1);
}

/// \brief Send 8-Bit Data
/// \param cmd 8-bit data
static void write_data_8(uint8_t data)
{
    DATA_MODE();
    SPI_MasterTransmit(&tft_spi, &data, 1);
}

/// \brief Send 16-Bit Data
/// \param cmd 16-bit data
static void write_data_16(uint16_t data)
{
    DATA_MODE();
    uint8_t buffer[2];
    buffer[0] = data >> 8;
    buffer[1] = data & 0xFF;
    SPI_MasterTransmit(&tft_spi, buffer, 2);
}

static void tft_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    write_command_8(ST7735_CASET);
    write_data_16(x0);
    write_data_16(x1);
    write_command_8(ST7735_RASET);
    write_data_16(y0);
    write_data_16(y1);
    write_command_8(ST7735_RAMWR);
}

/**
 * Clears the entire screen with a solid color.
 * @param color The color to fill the screen with.
 */
static void tft_clear_screen(uint16_t color)
{
    tft_set_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);

    uint8_t color_high = color >> 8;
    uint8_t color_low = color & 0xFF;

    START_WRITE();

    // Send color data for every pixel on the screen
    for (uint32_t i = 0; i < ST7735_WIDTH * ST7735_HEIGHT; i++)
    {
        write_data_8(color_high);
        write_data_8(color_low);
    }

    END_WRITE();
}

void tft_init()
{
    /* Setting SPI Peripheral */
    tft_spi.pSPIx = SPI1;
    tft_spi.Config.Mode = SPI_MODE_MASTER;
    tft_spi.Config.BUSConfig = SPI_BUS_FULL_DUPLEX;
    tft_spi.Config.ClockPhase = SPI_CPHA_LOW;
    tft_spi.Config.ClockPolarity = SPI_CPOL_LOW;
    tft_spi.Config.Speed = SPI_SCLK_DIV128;
    tft_spi.Config.SlaveManagement = SPI_SSM_SOFTWARE;
    tft_spi.Config.DataFrame = SPI_DFF_8BITS;
    tft_spi.Config.LSBFirst = SPI_DATABITS_MSBFIRST;

    SPI_Init(&tft_spi);

    /* Enable GPIO Clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    /* Configure GPIO Pins */
    GPIOA->MODER |= 1 << (PIN_DC * 2);
    GPIOC->MODER |= 1 << (PIN_RESET * 2);
    GPIOB->MODER |= 1 << (PIN_CS * 2);

    /* Reset the display */
    RESET_LOW();
    DelayMS(50);
    RESET_HIGH();
    DelayMS(50);

    START_WRITE();

    /* Exit sleep mode */
    write_command_8(ST7735_SLPOUT);
    DelayMS(ST7735_SLPOUT_DELAY);

    /* Set rotation */
    write_command_8(ST7735_MADCTL);
    write_data_8(ST7735_MADCTL_BGR);

    // write_data_8(ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_BGR);
    // write_data_8(ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR);

    /* Set Interface Pixel Format - 16-bit/pixel */
    write_command_8(ST7735_COLMOD);
    write_data_8(ST7735_COLMOD_16_BPP);

    /* Gamma Adjustments (pos. polarity) */
    uint8_t gamma_p[] = {
        0x09, 0x16, 0x09, 0x20, 0x21, 0x1B, 0x13, 0x19,
        0x17, 0x15, 0x1E, 0x2B, 0x04, 0x05, 0x02, 0x0E};
    write_command_8(ST7735_GMCTRP1);
    DATA_MODE();
    SPI_MasterTransmit(&tft_spi, gamma_p, 16);

    /* Gamma Adjustments (neg. polarity) */
    uint8_t gamma_n[] = {
        0x0B, 0x14, 0x08, 0x1E, 0x22, 0x1D, 0x18, 0x1E,
        0x1B, 0x1A, 0x24, 0x2B, 0x06, 0x06, 0x02, 0x0F};
    write_command_8(ST7735_GMCTRN1);
    DATA_MODE();
    SPI_MasterTransmit(&tft_spi, gamma_n, 16);

    DelayMS(10);

    /* Invert display */
    write_command_8(ST7735_INVON);

    /* Normal display on */
    write_command_8(ST7735_NORON);
    DelayMS(10);

    /* Turn on the display */
    write_command_8(ST7735_DISPON);
    DelayMS(10);

    /* Clear the screen to a solid color (e.g., red) */
    tft_clear_screen(GREEN);

    END_WRITE();
}

void tft_set_cursor(uint16_t x, uint16_t y)
{
    _cursor_x = x + ST7735_X_OFFSET;
    _cursor_y = y + ST7735_Y_OFFSET;
}

void tft_set_color(uint16_t color)
{
    _color = color;
}

/// \brief Set Text Background Color
/// \param color Text background color
/// \details Set to `_bg_color` variable
void tft_set_background_color(uint16_t color)
{
    _bg_color = color;
}

void tft_print_char(char c)
{
    const unsigned char *start = &font[(c - 32) * FONT_WIDTH]; // Assuming font starts at ASCII 32 (space)

    uint16_t sz = 0;
    for (uint8_t i = 0; i < FONT_WIDTH; i++)
    {
        uint8_t line = start[i];
        for (uint8_t j = 0; j < FONT_HEIGHT; j++)
        {
            if (line & 0x01)
            {
                _buffer[sz++] = _color >> 8;
                _buffer[sz++] = _color;
            }
            else
            {
                _buffer[sz++] = _bg_color >> 8;
                _buffer[sz++] = _bg_color;
            }
            line >>= 1;
        }
    }

    START_WRITE();
    tft_set_window(_cursor_x, _cursor_y, _cursor_x + FONT_WIDTH - 1, _cursor_y + FONT_HEIGHT - 1);
    DATA_MODE();
    SPI_MasterTransmit(&tft_spi, _buffer, sz); // Transmit the correct number of bytes

    END_WRITE();
}

void tft_print(const char *str)
{
    while (*str)
    {
        tft_print_char(*str++);
        _cursor_x += FONT_WIDTH + 1;
    }
}