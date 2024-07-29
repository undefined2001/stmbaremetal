#include "adc.h"

__STATIC_INLINE void ADC_ClockEn(ADC_TypeDef *pADC)
{
    (pADC == ADC1) ? RCC->APB2ENR |= RCC_APB2ENR_ADC1EN : (pADC == ADC2) ? RCC->APB2ENR |= RCC_APB2ENR_ADC2EN
                                                      : (pADC == ADC3)   ? RCC->APB2ENR |= RCC_APB2ENR_ADC3EN
                                                                         : 0;
}

void ADCInit()
{
    ADC_ClockEn(ADC1); // Enabling ADC1 Clock

    // Setting Resolution to 12-bit (you can change this if needed)
    ADC1->CR1 &= ~(0x3U << ADC_CR1_RES_Pos);

    // Enable scan mode for multiple channel
    ADC1->CR1 |= ADC_CR1_SCAN;

    // Setting Sampling Rate wait cycle for channels 0, 1, and 4 (you can adjust as needed)
    ADC1->SMPR2 |= (0x5U << ADC_SMPR2_SMP0_Pos) | (0x5U << ADC_SMPR2_SMP1_Pos) | (0x5U << ADC_SMPR2_SMP4_Pos);

    // Setting Number of conversions we want to read (3 channels)
    ADC1->SQR1 = 0;
    ADC1->SQR1 |= (2U << ADC_SQR1_L_Pos); // 2 means 3 conversions (L + 1)

    // Setting Sequence for channels 0, 1, and 4
    ADC1->SQR3 = 0;
    ADC1->SQR3 |= (0x0 << ADC_SQR3_SQ1_Pos) | (0x1 << ADC_SQR3_SQ2_Pos) | (0x4 << ADC_SQR3_SQ3_Pos);

    ADC1->CR2 |= ADC_CR2_CONT;

    ADC1->CR2 |= ADC_CR2_EOCS;
    // Enabling ADC
    ADC1->CR2 |= ADC_CR2_ADON;

    ;
}

void ADC_Continious()
{
    ADC1->CR2 |= ADC_CR2_CONT;

    ADC1->CR2 |= ADC_CR2_SWSTART;
}

void ADC_Conti_Stop()
{
    ADC1->CR2 &= ~ADC_CR2_CONT;
    ADC1->CR2 &= ~ADC_CR2_SWSTART;
}

uint16_t ADC_Read()
{

    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_STRT))
        ;

    while (!(ADC1->SR & ADC_SR_EOC))
        ;            // Wait until end of conversion
    return ADC1->DR; // Read the data register
}

