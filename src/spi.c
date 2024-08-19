#include "spi.h"

static void SPI_Enable(void)
{
    SPI1->CR1 |= (1 << 6); // SPE=1, Peripheral enabled
}

static void SPI_Disable(void)
{
    SPI1->CR1 &= ~(1 << 6); // SPE=0, Peripheral Disabled
}

void SPI_Init(SPI_HandleTypeDef *pSPIHandle)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    uint32_t tempReg = 0;
    pSPIHandle->pSPIx->CR1 &= ~SPI_CR1_SPE;
    /**
     * @brief: Setting the Mode it canbe Master or Slave
     */
    tempReg |= (pSPIHandle->Config.Mode << SPI_CR1_MSTR_Pos);

    /**
     * @brief: Setting the Bus Configuration
     */
    if (pSPIHandle->Config.BUSConfig == SPI_BUS_FULL_DUPLEX)
    {
        tempReg &= ~SPI_CR1_BIDIMODE; // Enabling Two Unidirectional Line
    }
    else if (pSPIHandle->Config.BUSConfig == SPI_BUS_HALF_DUPLEX)
    {
        tempReg |= SPI_CR1_BIDIMODE; // Enabling Omni Directional Single Bus
    }
    else if (pSPIHandle->Config.BUSConfig == SPI_BUS_SIMPLEX_RX)
    {
        tempReg &= ~SPI_CR1_BIDIMODE; // Enabling Two Unidirectional Line
        tempReg |= SPI_CR1_RXONLY;    // Selecting the MISO Line Only
    }
    /**
     * @brief: Setting the SPI Speed
     */
    tempReg |= pSPIHandle->Config.Speed << SPI_CR1_BR_Pos;

    /**
     * @brief: Setting the Clock Polarity and Clock Phase
     */

    tempReg |= pSPIHandle->Config.ClockPolarity << SPI_CR1_CPOL_Pos; // Setting Clock Polatity

    tempReg |= pSPIHandle->Config.ClockPhase << SPI_CR1_CPHA_Pos; // Setting Clock Phase

    tempReg |= pSPIHandle->Config.LSBFirst << SPI_CR1_LSBFIRST_Pos; // Setting Lsb first or Msbfirst

    tempReg |= pSPIHandle->Config.DataFrame << SPI_CR1_DFF_Pos; // Setting Data Frame Format 8 or 16 bits

    /**
     * @brief: Setting the Slave Mangament
     */
    if (pSPIHandle->Config.SlaveManagement == SPI_SSM_HARDWARE)
    {
        tempReg &= ~SPI_CR1_SSM; // Disabling Software Slave Management
    }
    else
    {
        tempReg |= SPI_CR1_SSM; // Enabling Software Slave Management
        tempReg |= SPI_CR1_SSI; // Setting the SSI High so that SPI Works Fine
    }

    tempReg |= SPI_CR1_SPE; // Enabling Spi
    pSPIHandle->pSPIx->CR1 = tempReg;
}

void SPIConfig(void)
{

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI1 CLock

    SPI1->CR1 &= ~SPI_CR1_SPE;

    SPI1->CR1 &= ~(SPI_CR1_CPHA_Msk | SPI_CR1_CPOL_Msk); // CPOL=0, CPHA=0

    SPI1->CR1 |= SPI_CR1_MSTR; // Master Mode

    SPI1->CR1 |= 5U << SPI_CR1_BR_Pos; // BR[2:0] = 011: fPCLK/16, PCLK2 = 90MHz, SPI clk = 5MHz

    SPI1->CR1 |= SPI_CR1_LSBFIRST; // LSBFIRST = 0, MSB first

    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI; // SSM=1, SSi=1 -> Software Slave Management

    SPI1->CR1 &= ~SPI_CR1_RXONLY; // RXONLY = 0, full-duplex

    SPI1->CR1 &= ~SPI_CR1_DFF; // DFF=0, 8 bit data

    SPI1->CR2 = 0;

    SPI1->CR1 |= SPI_CR1_SPE;
}

void SPI_MasterTransmit(SPI_HandleTypeDef *pSPIHandle, uint8_t *pBuffer, int32_t len)
{
    while (len > 0)
    {
        while (!(pSPIHandle->pSPIx->SR & SPI_SR_TXE))
            ;
        if (pSPIHandle->Config.DataFrame == SPI_DFF_8BITS)
        {
            pSPIHandle->pSPIx->DR = *pBuffer;
            pBuffer++;
            len--;
        }
        else
        {
            pSPIHandle->pSPIx->DR = *((uint16_t *)pBuffer);
            (uint16_t *)pBuffer++;
            len -= 2;
        }
    }

    while (((SPI1->SR) & SPI_SR_BSY))
        ;

    // //  Clear the Overrun flag by reading DR and SR
    // (void)(SPI1->DR & SPI1->SR);
}

void SPIGPIOConfig(void)
{
    RCC->AHB1ENR |= (1 << 0); // Enable GPIO Clock

    GPIOA->MODER |= (2 << 10) | (2 << 12) | (2 << 14) | (1 << 18); // Alternate functions for PA5, PA6, PA7 and Output for PA9

    GPIOA->OSPEEDR |= (3 << 10) | (3 << 12) | (3 << 14) | (3 << 18); // HIGH Speed for PA5, PA6, PA7, PA9

    GPIOA->AFR[0] |= (5 << 20) | (5 << 24) | (5 << 28); // AF5(SPI1) for PA5, PA6, PA7
}

void CS_Enable(void)
{
    GPIOA->BSRR |= (1 << 9) << 16;
}

void CS_Disable(void)
{
    GPIOA->BSRR |= (1 << 9);
}

typedef struct main
{
    uint8_t TxState;
    uint8_t RxState;
    uint8_t *pTxBuffer;
    uint8_t *pRxBuffer;
    uint8_t TxLen;
    uint8_t RxLen;
} SPI_Handle;
