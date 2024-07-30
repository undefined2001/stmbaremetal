#ifndef _INC_SPI_H
#include "stm32f446xx.h"

void SPI_Enable();
void SPI_Disable();
void SPIConfig();
void SPI_Transmit(uint8_t *data, int size);
void SPIGPIOConfig(void);
void CS_Enable();
void CS_Disable();

#define _INC_SPI_H
#endif