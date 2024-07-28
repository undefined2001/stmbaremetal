#include "stm32f446xx.h"
#include "timer.h"
#include "debug.h"
#include "i2c.h"

void I2C_GPIO_Init()
{
    // Enabling Clock for GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1;
    GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9;
    GPIOB->AFR[1] |= (4U << GPIO_AFRH_AFSEL8_Pos) | (4U << GPIO_AFRH_AFSEL9_Pos);
}

int main()
{
    TIM2_Init();
    I2C_GPIO_Init();
    I2C_HandleTypeDef i2c;
    i2c.pI2Cx = I2C1;
    i2c.Config.FMDutyCycle = I2C_FM_DUTY_16_9;
    i2c.Config.SCLSpeed = I2C_SM_SPEED_100KHz;

    I2C_Init(&i2c);

    uint8_t buffer[] = {0xD0, 0x00};
    uint8_t response[5] = {0};

    I2C_MasterSendData(&i2c, 0x77, buffer, sizeof(buffer), 0);
    I2C_MasterReceiveData(&i2c, 0x77, response, sizeof(response), 0);

    response[sizeof(response) - 1] = 0;

    while (1)
    {
    }

    return 0;
}
