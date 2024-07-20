#include "stm32f446xx.h"
#include "timer.h"
#include "debug.h"

uint8_t buffer[] = "This is New Buffer Sending Via UART DMA\n";

void uart_init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER |= GPIO_MODER_MODE2_1 | GPIO_MODER_MODE2_1;
    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos) | (7 << GPIO_AFRL_AFSEL3_Pos);
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDR_OSPEED3;

    RCC->APB2ENR |= RCC_APB1ENR_USART2EN;

    USART2->CR1 &= ~USART_CR1_UE;

    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

    // USART2->CR3 |= USART_CR3_DMAR;

    USART2->CR3 |= USART_CR3_DMAT;

    uint32_t uartdiv = 45000000 / 115200;

    USART2->BRR |= (uartdiv / 16) << USART_BRR_DIV_Mantissa_Pos | (uartdiv % 16);

    USART2->CR1 |= USART_CR1_UE;
}

void UART_DMAInit()
{
    // Enabling Clock For DMA1 Peripheral
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    DMA1_Stream6->CR &= ~DMA_SxCR_EN;
    while ((DMA1_Stream6->CR & DMA_SxCR_EN))
        ;
    DMA1->HIFCR |= DMA_HIFCR_CTCIF6;

    DMA1_Stream6->CR |= (4U << DMA_SxCR_CHSEL_Pos);

    DMA1_Stream6->CR |= DMA_SxCR_MINC;
    DMA1_Stream6->CR |= (1U << DMA_SxCR_DIR_Pos);

    DMA1_Stream6->PAR = (uint32_t)(&USART2->DR);
    DMA1_Stream6->M0AR = (uint32_t)(buffer);

    DMA1_Stream6->NDTR = sizeof(buffer);

    DMA1_Stream6->FCR &= ~DMA_SxFCR_DMDIS;

    DMA1_Stream6->CR |= DMA_SxCR_TCIE;

    NVIC_EnableIRQ(DMA1_Stream6_IRQn);

    DMA1_Stream6->CR |= DMA_SxCR_EN;
}


// void DMA1_Stream6_IRQHandler()
// {
//     if (DMA1->HISR & DMA_HISR_TCIF6)
//     {
//         DMA1->HIFCR |= DMA_HIFCR_CTCIF6; // Clear transfer complete flag

//         DMA1_Stream6->CR &= ~DMA_SxCR_EN;
//         while (DMA1_Stream6->CR & DMA_SxCR_EN)
//             ;

//         DMA1_Stream6->NDTR = sizeof(buffer); // Reload number of data to transfer
//         DMA1_Stream6->CR |= DMA_SxCR_EN;     // Enable the DMA stream again
//     }
// }