#include "timer.h"

static uint32_t tick = 0;

/**
 * @brief Initializes Timer 6 as a basic timer with a 1 MHz frequency
 */
void TimerInit(void)
{
    /* Enable Clock for Timer6 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    /* Reset Timer6 configuration */
    TIM6->CR1 = 0;
    TIM6->PSC = 90 - 1; // Prescaler to get 1 MHz frequency
    TIM6->ARR = 0xFFFF; // Auto-reload register max value

    TIM6->EGR |= TIM_EGR_UG;

    /* Enable Timer6 */
    TIM6->CR1 |= TIM_CR1_CEN;

    while (!(TIM6->SR & TIM_SR_UIF))
        ;
}

/**
 * @brief Delays for a specified number of microseconds
 * @param delay: number of microseconds to delay
 */
void DelayUS(uint16_t delay)
{
    TIM6->CNT = 0; // Reset the counter
    while (TIM6->CNT < delay)
        ; // Wait until the counter reaches the delay value
}

/**
 * @brief Delays for a specified number of milliseconds
 * @param delay: number of milliseconds to delay
 */
void DelayMS(uint16_t delay)
{
    while (delay--)
    {
        DelayUS(1000); // Delay for 1000 microseconds (1 millisecond)
    }
}

void TIM2_Init()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->PSC = 90 - 1;
    TIM2->ARR = 1000 - 1;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;

    while (!(TIM2->SR & TIM_SR_UIF))
        ;
    TIM2->SR &= ~TIM_SR_UIF;
    TIM2->CNT = 0;
    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler()
{
    // Check if the update interrupt flag is set
    if (TIM2->SR & TIM_SR_UIF)
    {
        // Clear the update interrupt flag
        TIM2->SR &= ~TIM_SR_UIF;
        
        // Increment the tick counter
        tick++;
    }
}

uint32_t GetTick()
{
    uint32_t currTick;
    __disable_irq();
    currTick = tick;
    __enable_irq();
    return currTick;
}