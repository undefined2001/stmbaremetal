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

static uint8_t UART_WaitStatusFlag(USART_TypeDef *pUARTx, uint32_t flag, uint32_t timeout)
{
}

__STATIC_INLINE void UART_EnableClock(USART_TypeDef *pUARTx)
{
    if (pUARTx == USART1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    }
    else if (pUARTx == USART2)
    {
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    }
    else if (pUARTx == USART3)
    {
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    }
}

__STATIC_INLINE void UART_PeripheralControl(USART_TypeDef *pUARTx, uint8_t State)
{
    if (State == UART_ENABLE)
    {
        pUARTx->CR1 |= USART_CR1_UE;
    }
    else if (State == UART_DISABLE)
    {
        pUARTx->CR1 &= ~USART_CR1_UE;
    }
}

void UART_Init(UART_HandleTypeDef *pUARTHandle)
{
    /*Enabling the Clock For UART*/
    UART_EnableClock(pUARTHandle->pUARTx);

    /*Disabling UART for Configuring*/
    UART_PeripheralControl(pUARTHandle->pUARTx, UART_DISABLE);

    uint32_t TempReg = 0;
    uint32_t UartDiv = 0;

    // Configuring the Word Len 0:8bit, 1:9bit
    TempReg |= pUARTHandle->Config.WordLen << USART_CR1_M_Pos;

    if (pUARTHandle->Config.Parity != UART_PARITY_NONE)
    {
        TempReg |= USART_CR1_PCE;
        TempReg |= pUARTHandle->Config.Parity << USART_CR1_PS_Pos;
    }
    if (pUARTHandle->Config.Mode == UART_MODE_RX_ONLY)
    {
        TempReg |= USART_CR1_RE;
        TempReg &= ~USART_CR1_TE;
    }
    else if (pUARTHandle->Config.Mode == UART_MODE_TX_ONLY)
    {
        TempReg |= USART_CR1_TE;
        TempReg &= ~USART_CR1_RE;
    }
    else
    {
        TempReg |= USART_CR1_RE | USART_CR1_TE;
    }
    pUARTHandle->pUARTx->CR1 = TempReg;

    /*Calculating Baud Rate*/
    UartDiv = 45000000U / pUARTHandle->Config.BaudRate;

    USART2->BRR = ((UartDiv / 16) << USART_BRR_DIV_Mantissa_Pos) | ((UartDiv % 16) << USART_BRR_DIV_Fraction_Pos);

    UART_PeripheralControl(pUARTHandle->pUARTx, UART_ENABLE);
}

void UART_SendBuffer(UART_HandleTypeDef *pUARTHandle, uint8_t *buffer)
{
    while (*buffer)
    {
        while (!(pUARTHandle->pUARTx->SR & USART_SR_TXE))
            ;
        pUARTHandle->pUARTx->DR = *buffer++;
    }
}

void UART_ReceiveBuffer(UART_HandleTypeDef *pUARTHandle, uint8_t *buffer)
{
    int len = UART_BUFFER_MAX_LEN;
    while (len--)
    {
        while (!(pUARTHandle->pUARTx->SR & USART_SR_RXNE))
            ;
        *buffer = pUARTHandle->pUARTx->DR;
        if (*buffer == '\n' || *buffer == '\r')
        {
            *buffer++ = '\0';
            break;
        }

        buffer++;
    }
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

static UART_HandleTypeDef handle;

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
