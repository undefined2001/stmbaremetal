#ifndef _INC_DHT_H
#define _INC_DHT_H
#include "stm32f446xx.h"
#include "timer.h"
#include <stdbool.h>


/**
 * @brief A Struct Which take the Port and Pin where the DHT22 sensor is connected
*/
typedef struct
{
    GPIO_TypeDef *pGPIO;
    uint8_t Pin;
} DHT;

void DHT_begin(DHT Config);
float DHT_ReadTemperature(DHT Config, bool isFahrenheit);
float DHT_ReadHumidity(DHT Config);

#endif