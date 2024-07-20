#ifndef _INC_I2C_H
#define _INC_I2C_H
#include "stm32f446xx.h"

#define DISABLE 0U
#define ENABLE 1U

#define I2C_MODE_MASTER 0U
#define I2C_MODE_SLAVE 1U

#define I2C_ACK_DISABLE 0U
#define I2C_ACK_ENABLE 1U

#define I2C_SPEED_100KHz 100000UL
#define I2C_SPEED_200KHz 200000UL
#define I2C_SPEED_400KHz 400000UL

#define I2C_FM_DUTY_2 0U
#define I2C_FM_DUTY_16_9 1U

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
} I2C_Handle_t;

void I2C_GPIO_Init();
void I2C_ClockEnable(I2C_TypeDef *pI2C);
void I2C_AckControl(I2C_TypeDef *pI2C, uint32_t state);
void I2C_PerpheralControl(I2C_TypeDef *pI2C, uint32_t state);
void I2C_Init(I2C_Handle_t *I2C_Handle);
void I2C_Start();
void I2C_SendAddress(uint8_t address, uint8_t rw);
void I2C_Stop();
void I2C_WriteByte(uint8_t data);
uint8_t I2C_ReadByte();
void I2C_Read_Buffer(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len);
void I2C_Write_Buffer(uint8_t address, uint8_t *buffer, uint8_t len);
void I2C_ACK_Enable();
void I2C_ACK_Disable();

#endif