#include "stm32f446xx.h"
#include "timer.h"
#include "debug.h"
#include "i2c.h"

void I2C_begin()
{
    I2C_Handle_t i2c;
    i2c.pI2C = I2C1;
    i2c.Config.AckControl = I2C_ACK_ENABLE;
    i2c.Config.FMDutyCycle = I2C_FM_DUTY_16_9;
    i2c.Config.Mode = I2C_MODE_MASTER;
    i2c.Config.I2CSpeed = I2C_SPEED_400KHz;
    I2C_GPIO_Init();
    I2C_Init(&i2c);
}

int main()
{
    UartInit();
    TimerInit();
    I2C_begin();

    uint8_t buffer[] = {0xD0, 0x00};

    uint8_t recv = 0;

    I2C_MasterSendData(I2C1, 0x77, buffer, sizeof(buffer), I2C_REPEATED_START);
    I2C_MasterReceiveData(I2C1, 0x77, &recv, sizeof(recv), I2C_NO_REPEATED_START);

    if (recv == 0x61)
    {
        LOG("Got A Valid BME680 Sensor!\n");
    }

    while (1)
    {
    }

    return 0;
}
