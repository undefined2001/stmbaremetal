#include "pwm.h"

#define PWM_RESOLUTION_12BIT 4095

void PWM_Init()
{
    // Enable clock for GPIOA
    RCC->AHB1ENR |= 0x1U << 0;

    // Set PA6 to alternate function mode
    GPIOA->MODER &= ~(0x3U << 0);
    GPIOA->MODER |= 0x2U << 0;

    // GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0;

    // Set PA6 to AF3 (TIM2_CH1)
    GPIOA->AFR[0] &= ~(0xF << 0);
    GPIOA->AFR[0] |= 0x1U << 0;

    // Enable clock for TIM2
    RCC->APB1ENR |= 0x1U << 0;

    TIM2->CR1 &= ~(0x1U << 0);

    // Set resolution to 12 bits (4096 - 1)
    TIM2->ARR = PWM_RESOLUTION_12BIT;

    // Enable auto-reload preload
    TIM2->CR1 |= (0x1U << 7);

    // Set prescaler to 0
    TIM2->PSC = 0;

    // Set PWM mode 1 on channel 1
    TIM2->CCMR1 &= ~(0x7U << 4);
    TIM2->CCMR1 |= 0x6U << 4;
    TIM2->CCMR1 |= 0x1U << 3; // Enable preload for CCR1

    // Enable capture/compare on channel 1
    TIM2->CCER &= ~(1U << 1);
    TIM2->CCER |= 1U << 0;

    // Set duty cycle
    TIM2->CCR1 = PWM_RESOLUTION_12BIT / 3;

    // Enable counter

    // Generate an update event to load the prescaler value immediately
    TIM2->EGR |= 1U << 0;
    TIM2->CR1 |= TIM_CR1_CEN;

    // Wait for the update event flag to be setzI
    while (!(TIM2->SR & (1 << 0)))
        ;
    TIM2->SR &= ~(1 << 0);

    // Reset the counter
    TIM2->CNT = 0;
}

void PWM_Three_Phase_init()
{
    /*Enabling Clock For GPIOA*/


    /*Enabling Clock For TIM1*/
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
}