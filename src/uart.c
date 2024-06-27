#include "uart.h"
#include "clock.h"

void UartInit()
{
    // Enabling Clock for GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Configuring RX and TX Pin
    GPIOA->MODER |= GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1;
    GPIOA->OTYPER |= GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3;
    GPIOA->AFR[0] |= 7 << GPIO_AFRL_AFSEL2_Pos | 7 << GPIO_AFRL_AFSEL3_Pos;

    // Enabling Clock for UART
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Disabling UART for Configuring
    USART2->CR1 &= ~USART_CR1_UE;

    // Enabling TX
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

    // Configuring Baud Rate
    uint32_t uart_div = /*GetAPB1Clock()*/ 45000000UL / 115200U;

    USART2->BRR = ((uart_div / 16) << USART_BRR_DIV_Mantissa_Pos) | ((uart_div % 16) << USART_BRR_DIV_Fraction_Pos);

    // Enabling UART
    USART2->CR1 |= USART_CR1_UE;
    NVIC_EnableIRQ(USART2_IRQn);
}

void UartSendBuffer(char *buffer)
{
    while (*buffer)
    {
        while (!(USART2->SR & USART_SR_TXE))
            ;
        USART2->DR = *buffer++;
    }
}

void UartReceiveBuffer(char *buffer, uint8_t len)
{
    while (len--)
    {
        while (!(USART2->SR & USART_SR_RXNE))
            ;
        *buffer++ = USART2->DR;
        if ((*(buffer - 1) == '\r') || (*(buffer - 1) == '\n'))
            break;
    }
}

void UartClearBuffer(char *buffer, uint8_t len)
{
    for (int i = 0; i < len; i++)
    {
        buffer[i] = 0;
    }
}

static UART_Handle_t handle;

void UART_ReceiveIT(uint8_t *pRxBuffer, uint32_t Len)
{
    if (!handle.RxBusy)
    {
        // Store the buffer and length
        handle.pRxBuffer = pRxBuffer;
        handle.RxLen = Len;
        handle.RxBusy = 1;

        // Enable RXNE interrupt
        USART2->CR1 |= USART_CR1_RXNEIE;
    }
}
static void USART_Handle_RxNIE()
{
    if (handle.RxLen > 0)
    {
        // Read the received data
        *handle.pRxBuffer = USART2->DR;
        handle.pRxBuffer++;
        handle.RxLen--;

        // If reception is complete, disable RXNE interrupt and mark RX as not busy
        if (handle.RxLen == 0)
        {
            USART2->CR1 &= ~USART_CR1_RXNEIE;
            handle.RxBusy = 0;
            USART2->SR &= ~USART_SR_RXNE;

        }
    }
}

// Interrupt service routine for USART2
void USART2_IRQHandler(void)
{
    if ((USART2->SR & USART_SR_RXNE)) // Check if RXNE flag is set
    {
        USART_Handle_RxNIE();
    }
}
