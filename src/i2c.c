#include "i2c.h"
#include "clock.h"
#include "debug.h"

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

void I2C_PeripheralControl(I2C_TypeDef *pI2C, uint32_t state)
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

void I2C_Init(I2C_Handle_t *pI2CHandle)
{
    I2C_ClockEnable(pI2CHandle->pI2C);

    pI2CHandle->pI2C->CR1 |= I2C_CR1_SWRST;
    pI2CHandle->pI2C->CR1 &= ~I2C_CR1_SWRST;

    I2C_PeripheralControl(pI2CHandle->pI2C, DISABLE);
    I2C_AckControl(pI2CHandle->pI2C, pI2CHandle->Config.AckControl);

    uint32_t tempreg = 0, ccr_val = 0;
    uint32_t fpclk = GetAPB1Clock();

    pI2CHandle->pI2C->CR2 |= (fpclk / 1000000U) & 0x3F;

    /**
     * @brieg: Here goes the CCR(Clock Control Register) configuration
     */
    if (pI2CHandle->Config.I2CSpeed <= I2C_SPEED_100KHz)
    {
        ccr_val = GetAPB1Clock() / (2 * pI2CHandle->Config.I2CSpeed);
        pI2CHandle->pI2C->CCR |= (ccr_val & 0xFFF);
    }
    else
    {
        /**
         * @brief: setting mode to fast mode and selecting duty cycle
         */
        pI2CHandle->pI2C->CCR |= I2C_CCR_FS;

        /**
         * @brief: setting dutycycle of I2C scl
         */
        tempreg |= pI2CHandle->Config.FMDutyCycle << I2C_CCR_DUTY_Pos;
        pI2CHandle->pI2C->CCR |= tempreg;
        if (pI2CHandle->Config.FMDutyCycle == I2C_FM_DUTY_16_9)
        {
            ccr_val = fpclk / (25 * pI2CHandle->Config.I2CSpeed);
            pI2CHandle->pI2C->CCR |= (ccr_val & 0xFFF);
        }
        else if (pI2CHandle->Config.FMDutyCycle == I2C_FM_DUTY_2)
        {
            ccr_val = fpclk / (3 * pI2CHandle->Config.I2CSpeed);
            pI2CHandle->pI2C->CCR |= (ccr_val & 0xFFF);
        }
    }
    pI2CHandle->pI2C->TRISE = (fpclk / 1000000UL) + 1;
    I2C_PeripheralControl(pI2CHandle->pI2C, ENABLE);
}

void I2C_Start(I2C_TypeDef *pI2C)
{
    pI2C->CR1 |= I2C_CR1_START;
    while (!I2C_GET_FLAG(pI2C->SR1, I2C_SR1_SB))
        ;
}

void I2C_SendAddress(I2C_TypeDef *pI2C, uint8_t address, uint8_t rw)
{
    pI2C->DR = (address << 1) | rw;
    while (!I2C_GET_FLAG(pI2C->SR1, I2C_SR1_ADDR))
        ;
}

void I2C_ClearAddressFlag(I2C_TypeDef *pI2C)
{
    (void)(pI2C->SR1 & pI2C->SR2);
}

void I2C_Stop(I2C_TypeDef *pI2C)
{
    pI2C->CR1 |= I2C_CR1_STOP;
}

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t address, uint8_t *buffer, uint8_t len, uint8_t Sr)
{
    I2C_Start(pI2CHandle->pI2C);

    I2C_SendAddress(pI2CHandle->pI2C, address, 0);
    I2C_ClearAddressFlag(pI2CHandle->pI2C);

    while (len--)
    {
        pI2CHandle->pI2C->DR = *buffer++;
        while (!I2C_GET_FLAG(pI2CHandle->pI2C->SR1, I2C_SR1_TXE))
            ;
        while (!I2C_GET_FLAG(pI2CHandle->pI2C->SR1, I2C_SR1_BTF))
            ;
    }
    if (!Sr)
        I2C_Stop(pI2CHandle->pI2C);
}

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t address, uint8_t *buffer, uint8_t len, uint8_t Sr)
{
    I2C_Start(pI2CHandle->pI2C);

    I2C_SendAddress(pI2CHandle->pI2C, address, 1);
    I2C_ClearAddressFlag(pI2CHandle->pI2C);

    while (len)
    {
        if (len == 1)
        {
            I2C_AckControl(pI2CHandle->pI2C, I2C_ACK_DISABLE);
            I2C_Stop(pI2CHandle->pI2C);
        }

        while (!I2C_GET_FLAG(pI2CHandle->pI2C->SR1, I2C_SR1_RXNE))
            ;
        *buffer = pI2CHandle->pI2C->DR;
        buffer++;

        len--;
    }
    I2C_AckControl(pI2CHandle->pI2C, I2C_ACK_ENABLE);
}

I2C_Status I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t address, uint8_t *buffer, uint8_t len, uint8_t Sr)
{

    I2C_Status status = pI2CHandle->TxRxState;
    if (status == I2C_READY)
    {
        pI2CHandle->TxBuffer = buffer;
        pI2CHandle->TxLen = len;
        pI2CHandle->DevAddr = address;
        pI2CHandle->TxRxState = I2C_BUSY_TX;
        pI2CHandle->Sr = Sr;

        I2C_Start(pI2CHandle->pI2C);
        pI2CHandle->pI2C->CR2 |= I2C_CR2_ITBUFEN;
        pI2CHandle->pI2C->CR2 |= I2C_CR2_ITEVTEN;
        pI2CHandle->pI2C->CR2 |= I2C_CR2_ITERREN;
    }

    return status;
}

I2C_Status I2C_MasterReceivDataIT(I2C_Handle_t *pI2CHandle, uint8_t address, uint8_t *buffer, uint8_t len, uint8_t Sr)
{

    I2C_Status status = pI2CHandle->TxRxState;
    if (status == I2C_READY)
    {
        pI2CHandle->RxBuffer = buffer;
        pI2CHandle->RxLen = len;
        pI2CHandle->DevAddr = address;
        pI2CHandle->TxRxState = I2C_BUSY_RX;
        pI2CHandle->Sr = Sr;

        I2C_Start(pI2CHandle->pI2C);

        pI2CHandle->pI2C->CR2 |= I2C_CR2_ITBUFEN;
        pI2CHandle->pI2C->CR2 |= I2C_CR2_ITEVTEN;
        pI2CHandle->pI2C->CR2 |= I2C_CR2_ITERREN;
    }

    return pI2CHandle->TxRxState;
}

I2C_Status I2C1_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
    uint32_t temp1 = (pI2CHandle->pI2C->CR2 & I2C_CR2_ITEVTEN);
    uint32_t temp2 = (pI2CHandle->pI2C->CR2 & I2C_CR2_ITBUFEN);
    uint32_t temp3 = I2C_GET_FLAG(pI2CHandle->pI2C->SR1, I2C_SR1_SB);

    /**
     * @brief: If Start is Generated Successfully then Execute Address Phase
     */
    if (temp1 && temp3)
    {
        if (pI2CHandle->TxRxState == I2C_BUSY_TX)
        {
            I2C_SendAddress(pI2CHandle->pI2C, pI2CHandle->DevAddr, 0);
        }
        else
        {
            I2C_SendAddress(pI2CHandle->pI2C, pI2CHandle->DevAddr, 1);
        }
    }
    temp3 = I2C_GET_FLAG(pI2CHandle->pI2C->SR1, I2C_SR1_ADDR);

    if (temp1 && temp3)
    {
        I2C_ClearAddressFlag(pI2CHandle->pI2C);
    }

    temp3 = I2C_GET_FLAG(pI2CHandle->pI2C->SR1, I2C_SR1_BTF);
    if (temp1 && temp3)
    {

        if (pI2CHandle->TxRxState == I2C_BUSY_TX)
        {
            if (pI2CHandle->TxLen == 0 && I2C_GET_FLAG(pI2CHandle->pI2C->SR1, I2C_SR1_TXE))
            {
                I2C_Stop(pI2CHandle->pI2C);
                I2C_CloseTransmission(pI2CHandle);
                // LOG("Control Reached to BTF!\n");
                I2C_ApplicationEventCallback(pI2CHandle, I2C_TX_COMPLETE);
            }
        }
    }
    temp3 = I2C_GET_FLAG(pI2CHandle->pI2C->SR1, I2C_SR1_TXE);
    if (temp1 && temp2 && temp3)
    {
        if (pI2CHandle->TxLen > 0)
        {
            pI2CHandle->pI2C->DR = *pI2CHandle->TxBuffer;
            pI2CHandle->TxBuffer++;
            pI2CHandle->TxLen--;
        }
    }
    return I2C_READY;
}

void I2C_CloseTransmission(I2C_Handle_t *pI2CHandle)
{
    pI2CHandle->pI2C->CR2 &= ~I2C_CR2_ITBUFEN;
    pI2CHandle->pI2C->CR2 &= ~I2C_CR2_ITEVTEN;
    pI2CHandle->pI2C->CR2 &= ~I2C_CR2_ITERREN;

    pI2CHandle->TxBuffer = NULL;
    pI2CHandle->TxLen = 0;
    pI2CHandle->TxRxState = I2C_READY;
}

__attribute__((weak)) void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, App_Event event)
{
}
