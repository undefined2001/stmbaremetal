#include "dht.h"

/**
 * @version 0.0.1
 * @file dht.c
 * @author Asraful Islam Taj
 * @brief this file contain all the code needed to control the dht Sensor
 * @date 31 May 2024
 */

/**
 ###############################################################################################################
 #                                                                                                             #
 #                                   FROM HERE ALL THE HELPER FUNCTIONS STARTS                                 #
 #                                                                                                             #
 ###############################################################################################################
 */


/**
 * @brief Sets The GPIO Mode into output
 * @param pGPIO pointer of GPIO_TypeDef struct
 * @param pin takes a unsigned integer number valid (0-15)
 */
static inline void SetPinModeOutput(GPIO_TypeDef *pGPIO, uint8_t pin)
{
    pGPIO->MODER &= ~(3u << pin * 2);
    pGPIO->MODER |= 1u << (pin * 2);
    pGPIO->OTYPER &= ~(1 << pin);
}

/**
 * @brief Sets The GPIO Mode into Input Mode with Pullup
 * @param pGPIO pointer of GPIO_TypeDef struct
 * @param pin takes a unsigned integer number valid (0-15)
 */
static inline void SetPinModeInput(GPIO_TypeDef *pGPIO, uint8_t pin)
{
    pGPIO->MODER &= ~(3u << pin * 2);
    pGPIO->PUPDR |= 1 << (pin * 2);
}

/**
 * @brief Write High or Low into a GPIO Pin
 * @param pGPIO pointer of GPIO_TypeDef struct
 * @param pin takes a unsigned integer number valid (0-15)
 * @param state takes 0 or 1 (0 means low 1 means high)
 */
static inline void WriteGPIOPin(GPIO_TypeDef *pGPIO, uint8_t pin, uint8_t state)
{
    if (state == 1)
    {
        pGPIO->BSRR |= (1 << pin);
    }
    else
    {
        pGPIO->BSRR |= (1 << (pin + 16));
    }
}

/**
 * @brief Reads a value from a GPIO Pin
 * @param pGPIO pointer of GPIO_TypeDef struct
 * @param pin takes a unsigned integer number valid (0-15)
 */
uint8_t ReadGPIOPin(GPIO_TypeDef *pGPIO, uint8_t pin)
{
    return ((pGPIO->IDR >> pin) & 0x1U);
}


/**
 * @brief This send a start signal to the DHT sensor
 * @param Config a struct of DHT
 */
static void DHT_SendStart(DHT Config)
{
    SetPinModeOutput(Config.pGPIO, Config.Pin);
    WriteGPIOPin(Config.pGPIO, Config.Pin, 0);
    DelayMS(2);
    WriteGPIOPin(Config.pGPIO, Config.Pin, 1);
    DelayUS(30);
    SetPinModeInput(Config.pGPIO, Config.Pin);
}


/**
 * @brief check and return the response of the DHT22 Sensor
 * @param Config a struct of DHT
 * @return for successful response returns 1 otherwise -1
 */
static uint8_t DHT_CheckResponse(DHT Config)
{
    SetPinModeInput(Config.pGPIO, Config.Pin);
    uint8_t response = 0;
    DelayUS(40);
    if (!ReadGPIOPin(Config.pGPIO, Config.Pin))
    {
        DelayUS(80);
        if (ReadGPIOPin(Config.pGPIO, Config.Pin))
            response = 1;
        else
            response = -1;
    }
    while (ReadGPIOPin(Config.pGPIO, Config.Pin))
        ;
    return response;
}

/**
 * @brief reads and returns a singebyte of data from the DHT22
 * @param Config a struct of DHT
 * @return returns a single byte of data from the DHT22 Sensor
 */
static uint8_t DHT_ReadByte(DHT Config)
{
    int bit_pos;
    int8_t data;
    for (bit_pos = 7; bit_pos >= 0; bit_pos--)
    {
        while (!(ReadGPIOPin(Config.pGPIO, Config.Pin)))
            ;
        DelayUS(40);
        if (!(ReadGPIOPin(Config.pGPIO, Config.Pin)))
        {
            data &= ~(1 << bit_pos);
        }
        else
            data |= (1 << bit_pos);

        while ((ReadGPIOPin(Config.pGPIO, Config.Pin)))
            ;
    }
    return data;
}

/**
 * @brief reads and returns the whole buffer of 5 bytes or 40bits from DHT22
 * @param Config a struct of DHT
 * @param buffer pointer of a uint8_t type array
 * @param len length of the buffer array **For this program it is always 5**
 * @return nothing
 */
static void DHT_ReadBuffer(DHT Config, uint8_t *buffer, uint8_t len)
{
    DHT_SendStart(Config);
    DHT_CheckResponse(Config);
    while (len--)
        *buffer++ = DHT_ReadByte(Config);
    DelayUS(100);
}


/**
 ###############################################################################################################
 #                                                                                                             #
 #                                    HERE THE HELPER FUNCTIONS SECTION ENDS                                   #
 #                                                                                                             #
 ###############################################################################################################
 */



/**
 * @brief This Function initialize the basic things needed for the DHT sensor to works
 *        Enables the GPIO Clock and Initializes the timer so that precise delay can be used
 * @param Config a struct of DHT
 */
void DHT_begin(DHT Config)
{
    // Enabling Clock for the given GPIO Port
    if (Config.pGPIO == GPIOA)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    }
    if (Config.pGPIO == GPIOB)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    }
    TimerInit();
}


/**
 * @brief return the tempareture
 * @param Config a struct of DHT
 * @param isFahrenheit takes true or false for returning the temp in Celcius or Fahrenheit
 * @return tempareture value
 */
float DHT_ReadTemperature(DHT Config, bool isFahrenheit)
{
    float temp;
    uint8_t buffer[5];
    DHT_ReadBuffer(Config, buffer, sizeof(buffer));
    int16_t tempdata = ((buffer[2] & 0x7F) << 8) | buffer[3];
    temp = (float)(tempdata * 0.1f);
    if ((buffer[2] & 0x80)) //Checking the MSB of First byte of tempareture if it is 0 then it is a positive number so don't have to do anything 
    {
        temp *= -1; //if the above condition is true then it is a negative number so have to multiply with -1 to make it negative
    }
    if (isFahrenheit)
    {
        temp = (temp * (9.0f / 5.0f)) + 32;
    }
    return temp;
}

/**
 * @brief return the Humidity
 * @param Config a struct of DHT
 * @return tempareture value
 */

float DHT_ReadHumidity(DHT Config)
{
    float humidity;
    uint8_t buffer[5];
    DHT_ReadBuffer(Config, buffer, sizeof(buffer));
    uint16_t huumdata = (buffer[0] << 8) | buffer[1];
    humidity = (float)(huumdata / 10.0f);
    return humidity;
}