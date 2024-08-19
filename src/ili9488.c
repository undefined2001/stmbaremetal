#include "ili9488.h"
#include "spi.h"

SPI_HandleTypeDef ili9488_spi;

static uint16_t height;
static uint16_t width;

void SPI_GPIO_Init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= (2U << 10U) | (2U << 12U) | (2U << 14U);
    GPIOA->OSPEEDR |= (3U << 10U) | (3U << 12U) | (3U << 14U);
    GPIOA->AFR[0] |= (5U << 20U) | (5U << 24U) | (5U << 28U);
}

static void ILI9488_GPIO_Init()
{
    /*enabling All the necessary Pins*/
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    ILI9488_CS_PORT->MODER |= (1U << (ILI9488_CS_PIN * 2));
    ILI9488_DC_PORT->MODER |= (1U << (ILI9488_DC_PIN * 2));
    ILI9488_RST_PORT->MODER |= (1U << (ILI9488_RST_PIN * 2));

    ili9488_spi.pSPIx = SPI1;
    ili9488_spi.Config.Mode = SPI_MODE_MASTER;
    ili9488_spi.Config.BUSConfig = SPI_BUS_FULL_DUPLEX;
    ili9488_spi.Config.ClockPolarity = SPI_CPOL_LOW;
    ili9488_spi.Config.ClockPhase = SPI_CPHA_LOW;
    ili9488_spi.Config.DataFrame = SPI_DFF_8BITS;
    ili9488_spi.Config.LSBFirst = SPI_DATABITS_MSBFIRST;
    ili9488_spi.Config.SlaveManagement = SPI_SSM_SOFTWARE;
    ili9488_spi.Config.Speed = SPI_SCLK_DIV2;
    SPI_GPIO_Init();
    SPI_Init(&ili9488_spi);
}

static void ILI9488_SendCommand(uint8_t command)
{
    uint8_t tempCommmand = command;
    ILI9488_DC_COMMAND();
    ILI9488_CS_ENABLE();
    SPI_MasterTransmit(&ili9488_spi, &tempCommmand, 1);
    ILI9488_CS_DISABLE();
}
static void ILI9488_SendData(uint8_t data)
{
    uint8_t tempData = data;
    ILI9488_DC_DATA();
    ILI9488_CS_ENABLE();
    SPI_MasterTransmit(&ili9488_spi, &tempData, 1);
    ILI9488_CS_DISABLE();
}

void ILI9488_SendData_Multi(uint8_t *buff, uint16_t buff_size)
{
    ILI9488_DC_DATA();

    ILI9488_CS_ENABLE();
    while (buff_size > 0)
    {
        uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
        SPI_MasterTransmit(&ili9488_spi, buff, chunk_size);
        buff += chunk_size;
        buff_size -= chunk_size;
    }
    ILI9488_CS_DISABLE();
}

void ILI9488_Init()
{
    ILI9488_GPIO_Init();

    ILI9488_CS_DISABLE();
    ILI9488_RST_ENABLE();
    DelayMS(10);
    ILI9488_RST_DISABLE();
    width = ILI9488_WIDTH;
    height = ILI9488_HEIGHT;
    // ILI9488_CS_EN();
    ILI9488_SendCommand(0xE0);
    ILI9488_SendData(0x00);
    ILI9488_SendData(0x03);
    ILI9488_SendData(0x09);
    ILI9488_SendData(0x08);
    ILI9488_SendData(0x16);
    ILI9488_SendData(0x0A);
    ILI9488_SendData(0x3F);
    ILI9488_SendData(0x78);
    ILI9488_SendData(0x4C);
    ILI9488_SendData(0x09);
    ILI9488_SendData(0x0A);
    ILI9488_SendData(0x08);
    ILI9488_SendData(0x16);
    ILI9488_SendData(0x1A);
    ILI9488_SendData(0x0F);

    ILI9488_SendCommand(0XE1);
    ILI9488_SendData(0x00);
    ILI9488_SendData(0x16);
    ILI9488_SendData(0x19);
    ILI9488_SendData(0x03);
    ILI9488_SendData(0x0F);
    ILI9488_SendData(0x05);
    ILI9488_SendData(0x32);
    ILI9488_SendData(0x45);
    ILI9488_SendData(0x46);
    ILI9488_SendData(0x04);
    ILI9488_SendData(0x0E);
    ILI9488_SendData(0x0D);
    ILI9488_SendData(0x35);
    ILI9488_SendData(0x37);
    ILI9488_SendData(0x0F);

    ILI9488_SendCommand(0XC0); // Power Control 1
    ILI9488_SendData(0x17);    // Vreg1out
    ILI9488_SendData(0x15);    // Verg2out

    ILI9488_SendCommand(0xC1); // Power Control 2
    ILI9488_SendData(0x41);    // VGH,VGL

    ILI9488_SendCommand(0xC5); // Power Control 3
    ILI9488_SendData(0x00);
    ILI9488_SendData(0x12); // Vcom
    ILI9488_SendData(0x80);

    ILI9488_SendCommand(0x36); // Memory Access
    ILI9488_SendData(0x48);

    ILI9488_SendCommand(0x3A); // Interface Pixel Format
    ILI9488_SendData(0x66);    // 18 bit

    ILI9488_SendCommand(0XB0); // Interface Mode Control
    ILI9488_SendData(0x80);    // SDO NOT USE

    ILI9488_SendCommand(0xB1); // Frame rate
    ILI9488_SendData(0xA0);    // 60Hz

    ILI9488_SendCommand(0xB4); // Display Inversion Control
    ILI9488_SendData(0x02);    // 2-dot

    ILI9488_SendCommand(0XB6); // Display Function Control  RGB/MCU Interface Control

    ILI9488_SendData(0x02); // MCU
    ILI9488_SendData(0x02); // Source,Gate scan dieection

    ILI9488_SendCommand(0XE9); // Set Image Functio
    ILI9488_SendData(0x00);    // Disable 24 bit data

    ILI9488_SendCommand(0xF7); // Adjust Control
    ILI9488_SendData(0xA9);
    ILI9488_SendData(0x51);
    ILI9488_SendData(0x2C);
    ILI9488_SendData(0x82); // D7 stream, loose

    ILI9488_SendCommand(ILI9488_CMD_SLPOUT); // Exit Sleep

    DelayMS(120);

    ILI9488_SendCommand(ILI9488_CMD_DISPON); // Display on
}

static void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{

    ILI9488_SendCommand(ILI9488_CMD_CASET); // Column addr set
    {
        uint8_t data[] = {(x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF};
        ILI9488_SendData_Multi(data, sizeof(data));
    }
    ILI9488_SendCommand(ILI9488_CMD_PASET);
    {
        uint8_t data[] = {(y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF};
        ILI9488_SendData_Multi(data, sizeof(data));
    }
    ILI9488_SendCommand(ILI9488_CMD_MEMWR); // write to RAM*/
}

void ILI9488_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{

    uint32_t i, n, cnt, buf_size;
    if ((x >= width) || (y >= height))
        return;
    if ((x + w - 1) >= width)
        w = width - x;
    if ((y + h - 1) >= height)
        h = height - y;
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    uint8_t r = (color & 0xF800) >> 11;
    uint8_t g = (color & 0x07E0) >> 5;
    uint8_t b = color & 0x001F;

    r = (r * 255) / 31;
    g = (g * 255) / 63;
    b = (b * 255) / 31;

    n = w * h * 3;
    if (n <= 65535)
    {
        cnt = 1;
        buf_size = n;
    }
    else
    {
        cnt = n / 3;
        buf_size = 3;
        uint8_t min_cnt = n / 65535 + 1;
        for (i = min_cnt; i < n / 3; i++)
        {
            if (n % i == 0)
            {
                cnt = i;
                buf_size = n / i;
                break;
            }
        }
    }
    uint8_t frm_buf[buf_size];
    for (i = 0; i < buf_size / 3; i++)
    {
        frm_buf[i * 3] = r;
        frm_buf[i * 3 + 1] = g;
        frm_buf[i * 3 + 2] = b;
    }

    ILI9488_DC_DATA();
    ILI9488_CS_ENABLE();
    while (cnt > 0)
    {
        SPI_MasterTransmit(&ili9488_spi, frm_buf, buf_size);

        cnt -= 1;
    }
    ILI9488_CS_DISABLE();
}

void ILI9488_SetRotation(uint8_t r)
{

    ILI9488_SendCommand(ILI9488_CMD_MADCTL);
    uint8_t rotation = r % 4; // can't be higher than 3
    switch (rotation)
    {
    case 0:
        ILI9488_SendData(MADCTL_MX | MADCTL_BGR);
        width = ILI9488_WIDTH;
        height = ILI9488_HEIGHT;
        break;
    case 1:
        ILI9488_SendData(MADCTL_MV | MADCTL_BGR);
        width = ILI9488_HEIGHT;
        height = ILI9488_WIDTH;
        break;
    case 2:
        ILI9488_SendData(MADCTL_MY | MADCTL_BGR);
        width = ILI9488_WIDTH;
        height = ILI9488_HEIGHT;
        break;
    case 3:
        ILI9488_SendData(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        width = ILI9488_HEIGHT;
        height = ILI9488_WIDTH;
        break;
    }
}

void ILI9488_FillScreen(uint16_t color)
{
    ILI9488_FillRect(0, 0, width, height, color);
}

void ILI9488_DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x >= width) || (y >= height))
        return;

    setAddrWindow(x, y, x, y); // Set the address window to the single pixel

    uint8_t r = (color & 0xF800) >> 11;
    uint8_t g = (color & 0x07E0) >> 5;
    uint8_t b = color & 0x001F;

    r = (r * 255) / 31;
    g = (g * 255) / 63;
    b = (b * 255) / 31;

    uint8_t frm_buf[3] = {r, g, b};

    ILI9488_DC_DATA();
    ILI9488_CS_ENABLE();
    SPI_MasterTransmit(&ili9488_spi, frm_buf, sizeof(frm_buf));
    ILI9488_CS_DISABLE();
}

void ILI9488_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    if (x1 >= width || y1 >= height || x2 >= width || y2 >= height)
        return;

    int16_t dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int16_t dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int16_t err = dx + dy, e2;

    uint8_t r = (color & 0xF800) >> 11;
    uint8_t g = (color & 0x07E0) >> 5;
    uint8_t b = color & 0x001F;

    r = (r * 255) / 31;
    g = (g * 255) / 63;
    b = (b * 255) / 31;

    while (1)
    {
        setAddrWindow(x1, y1, x1, y1); // Set the address window to the single pixel
        uint8_t frm_buf[3] = {r, g, b};

        ILI9488_DC_DATA();
        ILI9488_CS_ENABLE();
        SPI_MasterTransmit(&ili9488_spi, frm_buf, sizeof(frm_buf));
        ILI9488_CS_DISABLE();

        if (x1 == x2 && y1 == y2)
            break;
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void ILI9488_TestColors()
{
    uint16_t ili9488_colors[] = {
        ILI9488_BLACK,
        ILI9488_NAVY,
        ILI9488_DARKGREEN,
        ILI9488_DARKCYAN,
        ILI9488_MAROON,
        ILI9488_PURPLE,
        ILI9488_OLIVE,
        ILI9488_LIGHTGREY,
        ILI9488_DARKGREY,
        ILI9488_BLUE,
        ILI9488_GREEN,
        ILI9488_CYAN,
        ILI9488_RED,
        ILI9488_MAGENTA,
        ILI9488_YELLOW,
        ILI9488_WHITE,
        ILI9488_ORANGE,
        ILI9488_GREENYELLOW,
        ILI9488_PINK};
    uint8_t size = sizeof(ili9488_colors) / sizeof(ili9488_colors[0]);
    for (int i = 0; i < size; i++)
    {
        ILI9488_FillScreen(ili9488_colors[i]);
        DelayMS(100);
    }
}

void ILI9488_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t x = r;
    int16_t y = 0;
    int16_t err = 0;

    while (x >= y)
    {
        ILI9488_DrawPixel(x0 + x, y0 + y, color);
        ILI9488_DrawPixel(x0 + y, y0 + x, color);
        ILI9488_DrawPixel(x0 - y, y0 + x, color);
        ILI9488_DrawPixel(x0 - x, y0 + y, color);
        ILI9488_DrawPixel(x0 - x, y0 - y, color);
        ILI9488_DrawPixel(x0 - y, y0 - x, color);
        ILI9488_DrawPixel(x0 + y, y0 - x, color);
        ILI9488_DrawPixel(x0 + x, y0 - y, color);

        y += 1;
        err += 1 + 2 * y;
        if (2 * (err - x) + 1 > 0)
        {
            x -= 1;
            err += 1 - 2 * x;
        }
    }
}