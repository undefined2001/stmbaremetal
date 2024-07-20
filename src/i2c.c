#include "i2c.h"
#include "clock.h"

uint32_t I2C_GetStatusFlag(uint32_t reg, uint32_t pos)
{
    return reg & pos;
}

void I2C_ClockEnable(I2C_TypeDef *pI2C)
{
    if (pI2C == I2C1)
        RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    else if (pI2C == I2C2)
        RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    else if (pI2C == I2C3)
        RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
}

void I2C_AckControl(I2C_TypeDef *pI2C, uint32_t state)
{
    if (state == I2C_ACK_DISABLE)
    {
        pI2C->CR1 &= ~I2C_CR1_ACK;
    }
    else if (state == I2C_ACK_ENABLE)
    {
        pI2C->CR1 |= I2C_CR1_ACK;
    }
}

void I2C_PerpheralControl(I2C_TypeDef *pI2C, uint32_t state)
{
    if (state == ENABLE)
    {
        pI2C->CR1 |= I2C_CR1_PE;
    }
    else if (state == DISABLE)
    {
        pI2C->CR1 &= ~I2C_CR1_PE;
    }
}

void I2C_GPIO_Init()
{
    // Enabling Clock for GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1;
    GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9;
    GPIOB->AFR[1] |= (4U << GPIO_AFRH_AFSEL8_Pos) | (4U << GPIO_AFRH_AFSEL9_Pos);
}

void I2C_Init(I2C_Handle_t *I2C_Handle)
{
    I2C_ClockEnable(I2C_Handle->pI2C);

    I2C_Handle->pI2C->CR1 |= I2C_CR1_SWRST;
    I2C_Handle->pI2C->CR1 &= ~I2C_CR1_SWRST;

    I2C_PerpheralControl(I2C_Handle->pI2C, DISABLE);
    I2C_AckControl(I2C_Handle->pI2C, I2C_Handle->Config.AckControl);

    uint32_t tempreg = 0, ccr_val = 0;
    uint32_t fpclk = GetAPB1Clock();

    I2C_Handle->pI2C->CR2 |= (fpclk / 1000000U) & 0x3F;

    /**
     * @brieg: Here goes the CCR(Clock Control Register) configuration
     */
    if (I2C_Handle->Config.I2CSpeed <= I2C_SPEED_100KHz)
    {
        ccr_val = GetAPB1Clock() / (2 * I2C_Handle->Config.I2CSpeed);
        I2C_Handle->pI2C->CCR |= (ccr_val & 0xFFF);
    }
    else
    {
        /**
         * @brief: setting mode to fast mode and selecting duty cycle
         */
        I2C_Handle->pI2C->CCR |= I2C_CCR_FS;

        /**
         * @brief: setting dutycycle of I2C scl
         */
        tempreg |= I2C_Handle->Config.FMDutyCycle << I2C_CCR_DUTY_Pos;
        I2C_Handle->pI2C->CCR |= tempreg;
        if (I2C_Handle->Config.FMDutyCycle == I2C_FM_DUTY_16_9)
        {
            ccr_val = fpclk / (25 * I2C_Handle->Config.I2CSpeed);
            I2C_Handle->pI2C->CCR |= (ccr_val & 0xFFF);
        }
        else if (I2C_Handle->Config.FMDutyCycle == I2C_FM_DUTY_16_9)
        {
            ccr_val = fpclk / (3 * I2C_Handle->Config.I2CSpeed);
            I2C_Handle->pI2C->CCR |= (ccr_val & 0xFFF);
        }
    }
    I2C_Handle->pI2C->TRISE = (fpclk / 1000000UL) + 1;
    I2C_PerpheralControl(I2C_Handle->pI2C, ENABLE);
}

void I2C_MasterSendData(I2C_TypeDef *pI2C, uint8_t address, uint8_t *buffer, uint8_t len)
{
    pI2C->CR1 |= I2C_CR1_START;
    while (!(pI2C->SR1 & I2C_SR1_SB))
        ;
    pI2C->DR = (address << 1U);
    while (!(pI2C->SR1 & I2C_SR1_ADDR))
        ;
    (void)(pI2C->SR1 & pI2C->SR2);

    while (len--)
    {
        pI2C->DR = *buffer++;
        while (!(pI2C->SR1 & I2C_SR1_TXE))
            ;
        while (!(pI2C->SR1 & I2C_SR1_BTF))
            ;
    }
    pI2C->CR1 |= I2C_CR1_STOP;
}

void I2C_MasterReceiveData(I2C_TypeDef *pI2C, uint8_t address, uint8_t *buffer, uint8_t len)
{
    pI2C->CR1 |= I2C_CR1_START;
    while (!(pI2C->SR1 & I2C_SR1_SB))
        ;
    pI2C->DR = (address << 1U) | 1;
    while (!(pI2C->SR1 & I2C_SR1_ADDR))
        ;
    (void)(pI2C->SR1 & pI2C->SR2);

    while (len)
    {
        if (len == 1)
        {
            pI2C->CR1 &= ~I2C_CR1_ACK;
            pI2C->CR1 = I2C_CR1_STOP;
        }

        while (!(pI2C->SR1 & I2C_SR1_RXNE))
            ;
        *buffer = pI2C->DR;
        buffer++;

        len--;
    }
    pI2C->CR1 |= I2C_CR1_STOP;
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

void I2C_SendAddress(uint8_t address, uint8_t rw) // R:1, W:0
{
    // 0x27  0b0010_0111 << 1 = 0b010_0111_0 | 1
    I2C1->DR = (address << 1) | rw;

    while (!(I2C1->SR1 & I2C_SR1_ADDR))
        ;
    (void)(I2C1->SR1 | I2C1->SR2);
}

void I2C_WriteByte(uint8_t data)
{
    while (!(I2C1->SR1 & I2C_SR1_TXE))
        ;
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF))
        ;
}

void I2C_Read_Buffer(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len)
{
    I2C1->CR1 |= I2C_CR1_ACK;
    I2C_Start();
    I2C_SendAddress(address, 0);
    I2C_WriteByte(reg);
    I2C_Start();
    I2C_SendAddress(address, 1);
    while (len--)
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

void I2C_Write_Buffer(uint8_t address, uint8_t buffer[], uint8_t len)
{
    I2C_Start();
    I2C_SendAddress(address, 0);
    int i = 0;
    while (i < len)
    {
        I2C_WriteByte(buffer[i]); // 0x11 --> 5, 0x12 --> 10
        i++;
    }
    I2C_Stop();
}
