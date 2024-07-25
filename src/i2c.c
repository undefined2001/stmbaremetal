
#include "i2c.h"

static I2C_Status I2C_WaitFlagTimeout(I2C_TypeDef *pI2C, uint32_t reg, uint32_t MaxTimeOut)
{
    uint32_t prevTime = GetTick();
    I2C_Status status = I2C_OK;
    while (!(pI2C->SR1 & reg))
    {
        if ((GetTick() - prevTime) > MaxTimeOut)
        {
            break;
            status = I2C_TIMEOUT;
        }
    };
    return status;
}

__STATIC_INLINE bool I2C_CheckValidation(I2C_TypeDef *pI2Cx)
{
    bool res = false;
    if (pI2Cx == I2C1 || pI2Cx == I2C2 || pI2Cx == I2C3)
    {
        res = true;
    }
    return res;
}

__STATIC_INLINE void I2C_PeripheralControl(I2C_TypeDef *pI2Cx, uint8_t State)
{
    if (State == I2C_ENABLE)
    {
        pI2Cx->CR1 |= I2C_CR1_PE;
    }
    else
    {
        pI2Cx->CR1 &= ~I2C_CR1_PE;
    }
}

__STATIC_INLINE void I2C_EnableClock(I2C_TypeDef *pI2Cx)
{
    if (pI2Cx == I2C1)
    {
        RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    }
    else if (pI2Cx == I2C2)
    {
        RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    }
    else if (pI2Cx == I2C3)
    {
        RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
    }
}

I2C_Status I2C_GenStartCondition(I2C_TypeDef *pI2Cx)
{
    I2C_Status status = I2C_OK;
    pI2Cx->CR1 |= I2C_CR1_START;
    if (I2C_WaitFlagTimeout(pI2Cx, I2C_SR1_SB, I2C_MAX_TIMEOUT) != I2C_OK)
    {
        status = I2C_TIMEOUT;
    };
    return status;
}

/**
 * @brief This function initializes the I2C peripheral.
 * @param pI2CHandle Pointer to a structure of type I2C_HandleTypeDef.
 * @return I2C_Status Returns I2C_OK if successful, otherwise returns I2C_ERR.
 */
I2C_Status I2C_Init(I2C_HandleTypeDef *pI2CHandle)
{
    I2C_Status status = I2C_ERR;
    if (!I2C_CheckValidation(pI2CHandle->pI2Cx))
    {
        return status;
    }
    uint32_t CCRValue, PCLK, FreqValue;
    /*Enabling Clock For the I2C Peripheral*/
    I2C_EnableClock(pI2CHandle->pI2Cx);

    /*Disabling the Peripheral So that we can configure it*/
    I2C_PeripheralControl(pI2CHandle->pI2Cx, I2C_DISABLE);

    PCLK = GetAPB1Clock();
    FreqValue = PCLK / 1000000UL;

    /*Reseting the Peripheral*/
    pI2CHandle->pI2Cx->CR1 |= I2C_CR1_SWRST;
    pI2CHandle->pI2Cx->CR1 &= ~I2C_CR1_SWRST;
    /*Configuring the Peripheral*/

    /*Setting the Freq field in CR2*/
    pI2CHandle->pI2Cx->CR2 |= (FreqValue & 0x3F);

    /*Calculating the CCR Value*/
    if (pI2CHandle->Config.SCLSpeed <= I2C_SM_SPEED_100KHz)
    {
        CCRValue = (PCLK / (2U * pI2CHandle->Config.SCLSpeed));
    }
    else
    {
        if (pI2CHandle->Config.FMDutyCycle == I2C_FM_DUTY_2)
        {
            CCRValue = (PCLK / (3U * pI2CHandle->Config.SCLSpeed));
        }
        else if (pI2CHandle->Config.FMDutyCycle == I2C_FM_DUTY_16_9)
        {
            CCRValue = (PCLK / (25U * pI2CHandle->Config.SCLSpeed));
        }
    }

    /* Setting CCR value */
    pI2CHandle->pI2Cx->CCR |= CCRValue & 0xFFF;

    /*Configuring the Trise Value*/
    pI2CHandle->pI2Cx->TRISE = FreqValue + 1;

    I2C_PeripheralControl(pI2CHandle->pI2Cx, I2C_ENABLE);
    status = I2C_OK;
    return status;
}

/**
 * @brief This function Sends Data to the Slave
 * @param pI2CHandle Pointer to a structure of type I2C_HandleTypeDef.
 * @param address Takes Slave Address
 * @param pBuffer Pointer to the Buffer
 * @param len Length of the Buffer we want to send
 * @param RepStart used to make a repeated start after the end of transmission
 * @return I2C_Status Returns I2C_OK if successful, otherwise returns I2C_ERR.
 */
I2C_Status I2C_MasterSendData(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint16_t *pBuffer, uint8_t len, uint32_t RepStart);