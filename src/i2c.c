#include "i2c.h"

void I2C_GPIO_Init()
{
    // Enabling Clock for GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1;
    GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0;
    GPIOB->AFR[1] |= (4U << GPIO_AFRH_AFSEL8_Pos) | (4U << GPIO_AFRH_AFSEL9_Pos);
}

void I2C_Init()
{
    // Enabling Clock for I2C1
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Disabling I2c
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    I2C1->CR2 |= 45 << I2C_CR2_FREQ_Pos;

    I2C1->CCR = 225;

    I2C1->TRISE = 45 + 1;

    I2C1->CR1 |= I2C_CR1_PE;
}

void I2C_Start()
{
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB))
        ;
}

void I2C_Stop()
{
    // while (!(I2C1->SR1 & I2C_SR1_BTF))
    //     ;
    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C_SendAddress(uint8_t address, uint8_t rw)
{
    if (rw == 0)
    {
        I2C1->DR = address << 1;
    }
    else
    {
        I2C1->DR = (address << 1) | 1;
    }
    while (!(I2C1->SR1 & I2C_SR1_ADDR))
        ;
    (void)(I2C1->SR1 & I2C1->SR2);
}

void I2C_WriteByte(uint8_t data)
{
    while (!(I2C1->SR1 & I2C_SR1_TXE))
        ;
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF))
        ;
}

void I2C_Brust_Read(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len)
{
    I2C1->CR1 |= I2C_CR1_ACK;
    I2C_Start();
    I2C_SendAddress(address, 0);
    I2C_WriteByte(reg);
    I2C_Start();
    I2C_SendAddress(address, 1);
    while (len-- > 0)
    {

        while (!(I2C1->SR1 & I2C_SR1_RXNE))
            ;
        *buffer++ = I2C1->DR;

        if (len == 1)
        {
            I2C1->CR1 &= ~I2C_CR1_ACK;
            I2C_Stop();
        }
    }
}

void I2C_ACK_Enable()
{
    I2C1->CR1 |= I2C_CR1_ACK;
}

void I2C_ACK_Disable()
{
    I2C1->CR1 &= ~I2C_CR1_ACK;
}

uint8_t I2C_ReadByte()
{
    while (!(I2C1->SR1 & I2C_SR1_RXNE))
        ;
    return I2C1->DR;
}

void I2C_Write_Buffer(uint8_t address, uint8_t *buffer, uint8_t len)
{
    I2C_Start();
    I2C_SendAddress(address, 0);
    while (len-- > 0)
    {
        I2C_WriteByte(*buffer++);
    }
    I2C_Stop();
}
