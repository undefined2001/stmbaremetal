#include "stm32f446xx.h"
#include "timer.h"
#include "debug.h"
#include "i2c.h"

I2C_HandleTypeDef i2c;
UART_HandleTypeDef uart2;

uint8_t RxComplete = 0, TxComplete = 0;

void UART_GPIO_Init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1;
    GPIOA->OTYPER |= GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3;
    GPIOA->AFR[0] |= 7 << GPIO_AFRL_AFSEL2_Pos | 7 << GPIO_AFRL_AFSEL3_Pos;
}

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

uint8_t response[5] = {0, 1, 2, 3, 5};

int main()
{
    TimerInit();
    TIM2_Init();
    I2C_GPIO_Init();
    i2c.pI2Cx = I2C1;
    i2c.Config.FMDutyCycle = I2C_FM_DUTY_16_9;
    i2c.Config.SCLSpeed = I2C_SM_SPEED_100KHz;

    uart2.pUARTx = USART2;
    uart2.Config.BaudRate = UART_BR_115200;
    uart2.Config.Parity = UART_PARITY_NONE;
    uart2.Config.WordLen = UART_WORD_LEN_8_BIT;
    uart2.Config.Mode = UART_MODE_TX_ONLY;

    I2C_Init(&i2c);
    UART_GPIO_Init();
    UART_Init(&uart2);
    // NVIC_EnableIRQ(I2C1_EV_IRQn);

    // uint8_t buffer[] = {0xD0, 0x00};

    while (1)
    {

        UartSendBuffer("Testing UART Init Function\n");
        DelayMS(1000);
        // while (I2C_MasterSendDataIT(&i2c, 0x77, buffer, sizeof(buffer), I2C_REPEATED_START) != I2C_READY)
        //     ;
        // while (!TxComplete)
        //     ;
        // while (I2C_MasterReceiveDataIT(&i2c, 0x77, response, sizeof(response), 0) != I2C_READY)
        //     ;
        // while (!RxComplete)
        //     ;
        // LOG("Receive Complete!\n");
        // RxComplete = 0;
        // TxComplete = 0;
    }

    return 0;
}

void I2C1_EV_IRQHandler()
{
    I2C_EV_IRQHandling(&i2c);
}

void I2C_ApplicationEventCallback(I2C_HandleTypeDef *pI2CHandle, uint8_t Event)
{
    if (Event == I2C_RX_COMPLETE)
    {
        RxComplete = 1;
    }
    if (Event == I2C_TX_COMPLETE)
    {
        TxComplete = 1;
    }
}