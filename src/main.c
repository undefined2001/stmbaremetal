#include "stm32f446xx.h"
#include "timer.h"
#include "debug.h"
#include "i2c.h"

void I2C_begin()
{
    I2C_Handle_t i2c;
    i2c.pI2C = I2C1;
    i2c.Config.AckControl = I2C_ACK_DISABLE;
    i2c.Config.FMDutyCycle = I2C_FM_DUTY_16_9;
    i2c.Config.Mode = I2C_MODE_MASTER;
    i2c.Config.I2CSpeed = I2C_SPEED_100KHz;
    I2C_GPIO_Init();
    I2C_Init(&i2c);
}

int main()
{
    UartInit();
    TimerInit();
    I2C_begin();

    I2C_Start();
    I2C_SendAddress(0x77, 0);
    I2C_Stop();

    LOG("Data Sent Successfully!\n");

    while (1)
    {
    }

    return 0;
}
