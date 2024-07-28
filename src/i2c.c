#include "i2c.h"

/**
 * @brief Waits for a specific condition on an I2C register within a given maximum time.
 * @param pI2Cx Pointer to the I2C peripheral (I2C_TypeDef).
 * @param reg Register mask to check.
 * @param MaxTimeOut Maximum time to wait in milliseconds.
 * @return I2C_Status Returns I2C_OK if the condition is met, otherwise returns I2C_TIMEOUT.
 */
static I2C_Status I2C_WaitFlagTimeout(I2C_TypeDef *pI2Cx, uint32_t reg, uint32_t MaxTimeOut)
{
    uint32_t prevTime = GetTick();
    while (!(pI2Cx->SR1 & reg))
    {
        if ((GetTick() - prevTime) > MaxTimeOut)
        {
            return I2C_TIMEOUT;
        }
    }
    return I2C_OK;
}

/**
 * @brief Checks if the provided I2C peripheral is valid.
 * @param pI2Cx Pointer to the I2C peripheral (I2C_TypeDef).
 * @return true if valid, false otherwise.
 */
__STATIC_INLINE bool I2C_CheckValidation(I2C_TypeDef *pI2Cx)
{
    return (pI2Cx == I2C1 || pI2Cx == I2C2 || pI2Cx == I2C3);
}

/**
 * @brief Controls the state of the I2C peripheral.
 * @param pI2Cx Pointer to the I2C peripheral (I2C_TypeDef).
 * @param State State to set (I2C_ENABLE or I2C_DISABLE).
 */
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

/**
 * @brief Enables the clock for the specified I2C peripheral.
 * @param pI2Cx Pointer to the I2C peripheral (I2C_TypeDef).
 */
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

/**
 * @brief Controls the ACK bit of the I2C peripheral.
 * @param pI2Cx Pointer to the I2C peripheral (I2C_TypeDef).
 * @param state State to set (I2C_ACK_ENABLE or I2C_ACK_DISABLE).
 * @return I2C_Status Returns I2C_OK if successful, otherwise returns I2C_ERR.
 */
I2C_Status I2C_ACKControl(I2C_TypeDef *pI2Cx, uint8_t state)
{
    if (state == I2C_ACK_ENABLE)
    {
        pI2Cx->CR1 |= I2C_CR1_ACK;
    }
    else if (state == I2C_ACK_DISABLE)
    {
        pI2Cx->CR1 &= ~I2C_CR1_ACK;
    }
    return I2C_OK;
}

/**
 * @brief Generates a START condition on the I2C bus.
 * @param pI2Cx Pointer to the I2C peripheral (I2C_TypeDef).
 * @return I2C_Status Returns I2C_OK if successful, otherwise returns I2C_TIMEOUT.
 */
I2C_Status I2C_GenStartCondition(I2C_TypeDef *pI2Cx)
{
    pI2Cx->CR1 |= I2C_CR1_START;
    return I2C_WaitFlagTimeout(pI2Cx, I2C_SR1_SB, I2C_MAX_TIMEOUT);
}

/**
 * @brief Sends the address of the slave with the read/write bit.
 * @param pI2Cx Pointer to the I2C peripheral (I2C_TypeDef).
 * @param SlaveAddress Slave address to send.
 * @param rw Read/write bit (0 for write, 1 for read).
 * @return I2C_Status Returns I2C_OK if successful, otherwise returns I2C_ERR.
 */
I2C_Status I2C_MasterSendAddress(I2C_TypeDef *pI2Cx, uint8_t SlaveAddress, uint8_t rw)
{
    pI2Cx->DR = (SlaveAddress << 1) | rw;
    return I2C_WaitFlagTimeout(pI2Cx, I2C_SR1_ADDR, I2C_MAX_TIMEOUT);
}

/**
 * @brief Generates a STOP condition on the I2C bus.
 * @param pI2Cx Pointer to the I2C peripheral (I2C_TypeDef).
 * @return I2C_Status Returns I2C_OK if successful, otherwise returns I2C_ERR.
 */
I2C_Status I2C_GenStopCondition(I2C_TypeDef *pI2Cx)
{
    pI2Cx->CR1 |= I2C_CR1_STOP;
    return I2C_OK;
}

/**
 * @brief Initializes the I2C peripheral.
 * @param pI2CHandle Pointer to a structure of type I2C_HandleTypeDef.
 * @return I2C_Status Returns I2C_OK if successful, otherwise returns I2C_ERR.
 */
I2C_Status I2C_Init(I2C_HandleTypeDef *pI2CHandle)
{
    if (!I2C_CheckValidation(pI2CHandle->pI2Cx))
    {
        return I2C_ERR;
    }

    uint32_t CCRValue, PCLK, FreqValue;

    // Enable clock for the I2C peripheral
    I2C_EnableClock(pI2CHandle->pI2Cx);

    // Disable the peripheral for configuration
    I2C_PeripheralControl(pI2CHandle->pI2Cx, I2C_DISABLE);

    PCLK = GetAPB1Clock();
    FreqValue = PCLK / 1000000UL;

    // Reset the peripheral
    pI2CHandle->pI2Cx->CR1 |= I2C_CR1_SWRST;
    pI2CHandle->pI2Cx->CR1 &= ~I2C_CR1_SWRST;

    // Configure the peripheral
    pI2CHandle->pI2Cx->CR2 |= (FreqValue & 0x3F);

    // Calculate and set the CCR value
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

    pI2CHandle->pI2Cx->CCR = CCRValue & 0xFFF;

    // Configure the TRISE value
    pI2CHandle->pI2Cx->TRISE = FreqValue + 1;

    // Enable the peripheral
    I2C_PeripheralControl(pI2CHandle->pI2Cx, I2C_ENABLE);
    I2C_ACKControl(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);

    return I2C_OK;
}

/**
 * @brief Sends data to the slave in master mode.
 * @param pI2CHandle Pointer to a structure of type I2C_HandleTypeDef.
 * @param address Slave address.
 * @param pBuffer Pointer to the data buffer.
 * @param len Length of the data buffer.
 * @param RepStart Indicates whether to generate a repeated start condition.
 * @return I2C_Status Returns I2C_OK if successful, otherwise returns I2C_ERR.
 */
I2C_Status I2C_MasterSendData(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint8_t *pBuffer, uint8_t len, uint32_t RepStart)
{
    if (I2C_GenStartCondition(pI2CHandle->pI2Cx) != I2C_OK)
    {
        return I2C_ERR;
    }
    if (I2C_MasterSendAddress(pI2CHandle->pI2Cx, address, 0) != I2C_OK)
    {
        return I2C_ERR;
    }

    // Clear ADDR flag
    (void)pI2CHandle->pI2Cx->SR1;
    (void)pI2CHandle->pI2Cx->SR2;

    while (len--)
    {
        pI2CHandle->pI2Cx->DR = *pBuffer++;
        if (I2C_WaitFlagTimeout(pI2CHandle->pI2Cx, I2C_SR1_TXE, I2C_MAX_TIMEOUT) != I2C_OK)
        {
            return I2C_ERR;
        }
    }

    if (RepStart != I2C_DISABLE)
    {
        if (I2C_GenStartCondition(pI2CHandle->pI2Cx) != I2C_OK)
        {
            return I2C_ERR;
        }
    }
    else
    {
        I2C_GenStopCondition(pI2CHandle->pI2Cx);
    }

    return I2C_OK;
}

I2C_Status I2C_MasterReceiveData(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint8_t *pBuffer, uint8_t len, uint32_t RepStart)
{
    if (I2C_GenStartCondition(pI2CHandle->pI2Cx) != I2C_OK)
    {
        return I2C_ERR;
    }

    if (I2C_MasterSendAddress(pI2CHandle->pI2Cx, address, 1) != I2C_OK)
    {
        return I2C_ERR;
    }

    // Clear ADDR flag
    (void)pI2CHandle->pI2Cx->SR1;
    (void)pI2CHandle->pI2Cx->SR2;

    while (len)
    {
        if (len == 1)
        {
            I2C_ACKControl(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);
        }
        I2C_WaitFlagTimeout(pI2CHandle->pI2Cx, I2C_SR1_RXNE, I2C_MAX_TIMEOUT);
        *pBuffer++ = pI2CHandle->pI2Cx->DR;

        len--;
    }
    I2C_GenStopCondition(pI2CHandle->pI2Cx);
    return I2C_OK;
}

I2C_Status I2C_MasterReceiveDataIT(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint8_t *pBuffer, uint8_t len, uint32_t RepStart)
{
    uint8_t state = pI2CHandle->TxRxState;
    if (state == I2C_READY)
    {
        pI2CHandle->DevAddr = address;
        pI2CHandle->pRxBuffer = pBuffer;
        pI2CHandle->RxLen = len;
        pI2CHandle->RxSize = len;
        pI2CHandle->TxRxState = I2C_BUSY_RX;

        if (I2C_GenStartCondition(pI2CHandle->pI2Cx) != I2C_OK)
        {
            return I2C_ERR;
        }
        pI2CHandle->pI2Cx->CR2 |= I2C_CR2_ITBUFEN | I2C_CR2_ITERREN | I2C_CR2_ITEVTEN;
    }
    return state;
}

void I2C_CloseReception(I2C_HandleTypeDef *pI2CHandle)
{
    pI2CHandle->pI2Cx->CR2 &= ~I2C_CR2_ITBUFEN;
    pI2CHandle->pI2Cx->CR2 &= ~I2C_CR2_ITEVTEN;

    pI2CHandle->pRxBuffer = NULL;
    pI2CHandle->RxLen = 0;
    pI2CHandle->RxSize = 0;
    pI2CHandle->TxRxState = I2C_READY;

    // Enabling ACK
    I2C_ACKControl(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
}

I2C_Status I2C_EV_IRQHandling(I2C_HandleTypeDef *pI2CHandle)
{
    uint32_t temp1, temp2, temp3;

    // Check if event interrupt is enabled
    temp1 = I2C_GET_FLAG(pI2CHandle->pI2Cx->CR2, I2C_CR2_ITEVTEN);
    // Check if buffer interrupt is enabled
    temp2 = I2C_GET_FLAG(pI2CHandle->pI2Cx->CR2, I2C_CR2_ITBUFEN);

    // Checking for Start Condition
    temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_SB;
    if (temp1 && temp3)
    {
        // Start Condition Generated Successfully, Need to Execute Address Phase
        if (pI2CHandle->TxRxState == I2C_BUSY_TX)
        {
            I2C_MasterSendAddress(pI2CHandle->pI2Cx, pI2CHandle->DevAddr, 0);
        }
        else if (pI2CHandle->TxRxState == I2C_BUSY_RX)
        {
            I2C_MasterSendAddress(pI2CHandle->pI2Cx, pI2CHandle->DevAddr, 1);
        }
    }

    // Checking for Address Flag
    temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_ADDR;
    if (temp1 && temp3)
    {
        // We have to Clear the address flag
        if (pI2CHandle->RxSize == 1)
        {
            I2C_ACKControl(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);
        }
        (void)pI2CHandle->pI2Cx->SR1;
        (void)pI2CHandle->pI2Cx->SR2;
    }

    // Checking if the data transfer is finished
    temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_BTF;
    if (temp1 && temp3)
    {
        if (pI2CHandle->TxLen == 0 && pI2CHandle->TxRxState == I2C_BUSY_TX)
        {
            // Stop the communication
            I2C_GenStopCondition(pI2CHandle->pI2Cx);

            // Close the I2C communication
            // I2C_CloseTransmission(pI2CHandle);

            // Call the Application Callback Function
            I2C_ApplicationEventCallback(pI2CHandle, I2C_TX_COMPLETE);
        }
    }

    // Checking if TXE flag is set
    temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_TXE;
    if (temp1 && temp2 && temp3)
    {
        if (pI2CHandle->TxRxState == I2C_BUSY_TX && pI2CHandle->TxLen > 0)
        {
            pI2CHandle->pI2Cx->DR = *pI2CHandle->pTxBuffer++;
            pI2CHandle->TxLen--;
        }
    }

    // Checking if RXNE flag is set
    temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_RXNE;
    if (temp1 && temp2 && temp3)
    {
        if (pI2CHandle->TxRxState == I2C_BUSY_RX)
        {
            if (pI2CHandle->RxSize > 1)
            {
                if (pI2CHandle->RxLen == 2)
                {
                    // clear the ack bit
                    I2C_ACKControl(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);
                }

                // read DR
                *pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
                pI2CHandle->pRxBuffer++;
                pI2CHandle->RxLen--;
            }

            if (pI2CHandle->RxLen == 0)
            {
                // Stop the communication
                I2C_GenStopCondition(pI2CHandle->pI2Cx);

                // Close the I2C reception
                I2C_CloseReception(pI2CHandle);

                // Call the Application Callback Function
                I2C_ApplicationEventCallback(pI2CHandle, I2C_RX_COMPLETE);
            }
        }
    }

    return I2C_OK;
}

__attribute__((weak)) void I2C_ApplicationEventCallback(I2C_HandleTypeDef *pI2CHandle, uint8_t Event)
{
}