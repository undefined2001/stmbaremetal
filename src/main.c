#include "stm32f446xx.h"
#include "timer.h"
#include "debug.h"
#include "i2c.h"

uint32_t TxComplete = 0;

I2C_Handle_t i2c;

void I2C_begin()
{
    i2c.pI2C = I2C1;
    i2c.Config.AckControl = I2C_ACK_ENABLE;
    i2c.Config.FMDutyCycle = I2C_FM_DUTY_16_9;
    i2c.Config.Mode = I2C_MODE_MASTER;
    i2c.Config.I2CSpeed = I2C_SPEED_400KHz;
    I2C_GPIO_Init();
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    I2C_Init(&i2c);
}

int main()
{
    UartInit();
    TimerInit();
    I2C_begin();

    uint8_t buffer[] = {'T', 'A', 'J'};

    while (1)
    {
        while(I2C_MasterSendDataIT(&i2c, 0x77, buffer, sizeof(buffer), I2C_REPEATED_START) != I2C_READY);
        while (!TxComplete)
            ;

        LOG("Transmission Finished Successfully: %s\n", buffer);
        TxComplete = 0;
    }

    return 0;
}

void I2C1_EV_IRQHandler()
{
    I2C1_EV_IRQHandling(&i2c);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, App_Event event)
{
    if (event == I2C_TX_COMPLETE)
    {
        TxComplete = 1;
    }
    // Do Nothing;
}