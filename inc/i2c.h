#ifndef __INC_I2C_H__
#define __INC_I2C_H__

#include "stm32f446xx.h"
#include "clock.h"
#include "stdbool.h"
#include "timer.h"

#define I2C_GET_FLAG(__I2C_Reg__, __Flag__) (__I2C_Reg__ & __Flag__)

#define I2C_MAX_TIMEOUT 2000U
#define I2C_MODE_MASTER 1U
#define I2C_MODE_SLAVE 0U

#define I2C_ACK_ENABLE 1U
#define I2C_ACK_DISABLE 0

#define I2C_ENABLE 1U
#define I2C_DISABLE 0U

#define I2C_FM_DUTY_2 0
#define I2C_FM_DUTY_16_9 1U

#define I2C_SM_SPEED_100KHz 100000U
#define I2C_FM_SPEED_200KHz 200000U
#define I2C_FM_SPEED_250KHz 250000U
#define I2C_FM_SPEED_300KHz 300000U
#define I2C_FM_SPEED_350KHz 350000U
#define I2C_FM_SPEED_400KHz 400000U

typedef enum
{
    I2C_ACK_ERR = 3,
    I2C_BUSY_RX = 2,
    I2C_BUSY_TX = 1,
    I2C_OK = 0,
    I2C_READY = 0,
    I2C_ERR = -1,
    I2C_TIMEOUT = -2
} I2C_Status;

typedef struct
{
    uint32_t SCLSpeed;
    uint32_t FMDutyCycle;
    uint32_t OWNAddress;
    uint32_t AddressingMode;
    uint32_t StretchConfig;

} I2C_ConfigTypeDef;

typedef struct
{
    I2C_TypeDef *pI2Cx;
    I2C_ConfigTypeDef Config;
    uint8_t *pTxBuffer;
    uint8_t *pRxBuffer;
    uint8_t TxLen;
    uint8_t RxLen;
    uint8_t RxSize;
    uint8_t TxRxState;
    uint8_t DevAddr;
} I2C_HandleTypeDef;

I2C_Status I2C_ACKControl(I2C_TypeDef *pI2Cx, uint8_t State);
I2C_Status I2C_GenStartCondition(I2C_TypeDef *pI2Cx);
I2C_Status I2C_MasterSendAddress(I2C_TypeDef *pI2Cx, uint8_t SlaveAddress, uint8_t rw);
I2C_Status I2C_GenStopCondition(I2C_TypeDef *pI2Cx);

I2C_Status I2C_Init(I2C_HandleTypeDef *pI2CHandle);
I2C_Status I2C_MasterSendData(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint8_t *pBuffer, uint8_t len, uint32_t RepStart);
I2C_Status I2C_MasterReceiveData(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint8_t *pBuffer, uint8_t len, uint32_t RepStart);
I2C_Status I2C_MasterSendDataIT(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint8_t *pBuffer, uint8_t len, uint32_t RepStart);
I2C_Status I2C_MasterReceiveDataIT(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint8_t *pBuffer, uint8_t len, uint32_t RepStart);
I2C_Status I2C_EV_IRQHandling(I2C_HandleTypeDef *pI2CHandle);
void I2C_CloseTransmission(I2C_HandleTypeDef *pI2CHandle);

#endif