#ifndef _INC_SPI_H
#include "stm32f446xx.h"

/*SPI Modes Master or SLAVE*/
#define SPI_MODE_MASTER 1U
#define SPI_MODE_SLAVE 0U

/*SPI Bus Modes*/
#define SPI_BUS_FULL_DUPLEX 0U
#define SPI_BUS_HALF_DUPLEX 1U
#define SPI_BUS_SIMPLEX_RX 2U

/*SPI Data Frame */
#define SPI_DFF_8BITS 0U
#define SPI_DFF_16BITS 1U

/*Clock Polarity*/
#define SPI_CPOL_LOW 0U
#define SPI_CPOL_HIGH 1U

/*Clock Phase */
#define SPI_CPHA_LOW 0U
#define SPI_CPHA_HIGH 1U

/*SPI Data bits arrangement*/
#define SPI_DATABITS_MSBFIRST 0U
#define SPI_DATABITS_LSBFIRST 1U

/*SPI Slave Management*/
#define SPI_SSM_SOFTWARE 0U
#define SPI_SSM_HARDWARE 1U

/*SPI Clock Speed*/
#define SPI_SCLK_DIV2 0U
#define SPI_SCLK_DIV4 1U
#define SPI_SCLK_DIV8 2U
#define SPI_SCLK_DIV16 3U
#define SPI_SCLK_DIV32 4U
#define SPI_SCLK_DIV64 5U
#define SPI_SCLK_DIV128 6U
#define SPI_SCLK_DIV256 7U

typedef struct
{
    uint32_t Mode;
    uint32_t BUSConfig;
    uint32_t Speed;
    uint32_t ClockPolarity;
    uint32_t ClockPhase;
    uint32_t LSBFirst;
    uint32_t DataFrame;
    uint32_t SlaveManagement;

} SPI_ConfigTypeDef;

typedef struct
{
    SPI_TypeDef *pSPIx;
    SPI_ConfigTypeDef Config;
} SPI_HandleTypeDef;

void SPI_Init(SPI_HandleTypeDef *pSPIHandle);
void SPI_MasterTransmit(SPI_HandleTypeDef *pSPIHandle, uint8_t *pBuffer, int32_t len);

void SPIConfig();
void SPI_Transmit(uint8_t *data, int size);
void SPIGPIOConfig(void);
void CS_Enable();
void CS_Disable();
#define _INC_SPI_H
#endif