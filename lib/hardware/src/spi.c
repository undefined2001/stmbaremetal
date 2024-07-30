#include "spi.h"

void SPI_Enable(void)
{
    SPI1->CR1 |= (1 << 6); // SPE=1, Peripheral enabled
}

void SPI_Disable(void)
{
    SPI1->CR1 &= ~(1 << 6); // SPE=0, Peripheral Disabled
}

void SPIConfig(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI1 CLock

    SPI_Disable();

    SPI1->CR1 &= ~(SPI_CR1_CPHA_Msk | SPI_CR1_CPOL_Msk); // CPOL=0, CPHA=0

    SPI1->CR1 |= SPI_CR1_MSTR; // Master Mode

    SPI1->CR1 |= 6 << SPI_CR1_BR_Pos; // BR[2:0] = 011: fPCLK/16, PCLK2 = 90MHz, SPI clk = 5MHz

    SPI1->CR1 |= SPI_CR1_LSBFIRST; // LSBFIRST = 0, MSB first

    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI; // SSM=1, SSi=1 -> Software Slave Management

    SPI1->CR1 &= ~SPI_CR1_RXONLY; // RXONLY = 0, full-duplex

    SPI1->CR1 &= ~SPI_CR1_DFF; // DFF=0, 8 bit data

    SPI1->CR2 = 0;

    SPI_Enable();
}

void SPI_Transmit(uint8_t *data, int size)
{

    /************** STEPS TO FOLLOW *****************
    1. Wait for the TXE bit to set in the Status Register
    2. Write the data to the Data Register
    3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
    4. Clear the Overrun flag by reading DR and SR
    ************************************************/

    int i = 0;
    while (i < size)
    {
        while (!((SPI1->SR) & SPI_SR_TXE))
        {
        }; // wait for TXE bit to set -> This will indicate that the buffer is empty
        SPI1->DR = data[i]; // load the data into the Data Register
        i++;
    }

    /*During discontinuous communications, there is a 2 APB clock period delay between the
    write operation to the SPI_DR register and BSY bit setting. As a consequence it is
    mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
    data.
    */
    while (!((SPI1->SR) & (1 << 1)))
    {
    }; // wait for TXE bit to set -> This will indicate that the buffer is empty
    while (((SPI1->SR) & (1 << 7)))
    {
    }; // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication

    //  Clear the Overrun flag by reading DR and SR
    (void)(SPI1->DR & SPI1->SR);
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

void SPI_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI1 CLock

    SPI1->CR1 &= ~SPI_CR1_SPE;

    SPI1->CR1 &= ~(SPI_CR1_CPHA_Msk | SPI_CR1_CPOL_Msk); // CPOL=0, CPHA=0

    SPI1->CR1 |= SPI_CR1_MSTR; // Master Mode

    SPI1->CR1 |= 6 << SPI_CR1_BR_Pos; // BR[2:0] = 011: fPCLK/16, PCLK2 = 90MHz, SPI clk = 5MHz

    SPI1->CR1 |= SPI_CR1_LSBFIRST; // LSBFIRST = 0, MSB first

    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI; // SSM=1, SSi=1 -> Software Slave Management

    SPI1->CR1 &= ~SPI_CR1_RXONLY; // RXONLY = 0, full-duplex

    SPI1->CR1 &= ~SPI_CR1_DFF; // DFF=0, 8 bit data

    SPI1->CR2 = 0;

    SPI1->CR1 |= SPI_CR1_SPE;
}