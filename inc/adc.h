#ifndef _INC_ADC_H
#include "stm32f446xx.h"

#define _GET_ADC_CHANNEL_COUNT(pADC) ((pADC->SQR1 >> 20) & 0xF)

enum Resolution
{
    ADC_RESOLUTION_12BIT = 0x0U,
    ADC_RESOLUTION_10BIT,
    ADC_RESOLUTION_8BIT,
    ADC_RESOLUTION_6BIT,
};

typedef struct
{
    uint8_t Resolution; // Resolution of the ADC
    uint8_t nSample;    // Number of Channel we need to Sample
    uint8_t WaitCycle;  // Wait Cycle nedded according to the Resolution
} ADC_Config;

void ADCInit();
uint16_t ADC_Read();
void ADC_Continious();
void ADC_Conti_Stop();
#define _INC_ADC_H
#endif