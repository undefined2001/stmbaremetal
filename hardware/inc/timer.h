#ifndef _INC_TIMER_H
#define _INC_TIMER_H

#include "stm32f446xx.h"

void TimerInit();
void DelayUS(uint16_t delay);
void DelayMS(uint16_t delay);
void TIM2_Init();
uint32_t GetTick();

#endif