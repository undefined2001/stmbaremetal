#ifndef _INC_I2C_H
#define _INC_I2C_H
#include "stm32f446xx.h"

void I2C_GPIO_Init();
void I2C_Init();
void I2C_Start();
void I2C_SendAddress(uint8_t address, uint8_t rw);
void I2C_Stop();
void I2C_WriteByte(uint8_t data);
uint8_t I2C_ReadByte();
void I2C_Brust_Read(uint8_t address, uint8_t reg, uint8_t *buffer, uint8_t len);
void I2C_Write_Buffer(uint8_t address, uint8_t *buffer, uint8_t len);
void I2C_ACK_Enable();
void I2C_ACK_Disable();

#endif