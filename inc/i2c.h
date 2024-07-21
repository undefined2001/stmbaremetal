#ifndef _INC_I2C_H
#define _INC_I2C_H
#include "stm32f446xx.h"

#define I2C_GET_FLAG(__FLAG, __FLAG_POS) (__FLAG & __FLAG_POS)

#define DISABLE 0U
#define ENABLE 1U

#define I2C_NO_REPEATED_START 0U
#define I2C_REPEATED_START 1U

#define I2C_MODE_MASTER 0U
#define I2C_MODE_SLAVE 1U

#define I2C_ACK_DISABLE 0U
#define I2C_ACK_ENABLE 1U

#define I2C_SPEED_100KHz 100000UL
#define I2C_SPEED_200KHz 200000UL
#define I2C_SPEED_300KHz 300000UL
#define I2C_SPEED_350KHz 350000UL
#define I2C_SPEED_400KHz 400000UL

#define I2C_FM_DUTY_2 0U
#define I2C_FM_DUTY_16_9 1U

typedef enum
{
    I2C_BUSY_TX = 1,
    I2C_BUSY_RX = 2,
    I2C_READY = 0,
    I2C_ACK_ERR = -1,
    I2C_ERR = -3
} I2C_Status;

typedef enum
{
    I2C_TX_COMPLETE = 0,
    I2C_RX_COMPLETE = 1,
} App_Event;

typedef struct
{
    uint32_t Mode;
    uint32_t I2CSpeed;
    uint32_t FMDutyCycle;
    uint32_t AckControl;

} I2C_Config_t;

typedef struct
{
    I2C_TypeDef *pI2C;
    I2C_Config_t Config;
    uint32_t RxLen;
    uint32_t TxLen;
    uint8_t *RxBuffer;
    uint8_t *TxBuffer;
    uint32_t TxRxState;
    uint8_t DevAddr;
    uint8_t Sr;
} I2C_Handle_t;

void I2C_GPIO_Init();
void I2C_ClockEnable(I2C_TypeDef *pI2C);
void I2C_AckControl(I2C_TypeDef *pI2C, uint32_t state);
void I2C_PeripheralControl(I2C_TypeDef *pI2C, uint32_t state);
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_Start(I2C_TypeDef *pI2C);
void I2C_Stop(I2C_TypeDef *pI2C);
void I2C_SendAddress(I2C_TypeDef *pI2C, uint8_t address, uint8_t rw);
void I2C_ClearAddressFlag(I2C_TypeDef *pI2C);

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t address, uint8_t *buffer, uint8_t len, uint8_t Sr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t address, uint8_t *buffer, uint8_t len, uint8_t Sr);

/**
 * @brief: API for using I2C with Interrupt
 */
I2C_Status I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t address, uint8_t *buffer, uint8_t len, uint8_t Sr);
I2C_Status I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t address, uint8_t *buffer, uint8_t len, uint8_t Sr);

I2C_Status I2C1_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
I2C_Status I2C1_ERR_IRQHandling(I2C_Handle_t *pI2CHandle);

void I2C_CloseTransmission(I2C_Handle_t *pI2CHandle);
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, App_Event event);

#endif