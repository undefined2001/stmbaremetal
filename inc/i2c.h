#ifndef __INC_I2C_H__
#define __INC_I2C_H__

#include "stm32f446xx.h"
#include "clock.h"
#include "stdbool.h"
#include "timer.h"

#define I2C_MAX_TIMEOUT 2000U
#define I2C_MODE_MASTER 1U
#define I2C_MODE_SLAVE 0U

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
    I2C_OK = 0,
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
} I2C_HandleTypeDef;

I2C_Status I2C_ACKControl(I2C_TypeDef *pI2Cx, uint8_t State);
I2C_Status I2C_GenStartCondition(I2C_TypeDef *pI2Cx);
I2C_Status I2C_GenStopCondition(I2C_TypeDef *pI2Cx);

I2C_Status I2C_Init(I2C_HandleTypeDef *pI2CHandle);
I2C_Status I2C_MasterSendData(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint16_t *pBuffer, uint8_t len, uint32_t RepStart);
I2C_Status I2C_MasterReceiveData(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint16_t *pBuffer, uint8_t len, uint32_t RepStart);
I2C_Status I2C_MasterSendDataIT(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint16_t *pBuffer, uint8_t len, uint32_t RepStart);
I2C_Status I2C_MasterReceiveDataIT(I2C_HandleTypeDef *pI2CHandle, uint8_t address, uint16_t *pBuffer, uint8_t len, uint32_t RepStart);

#endif