#ifndef _INC_PWM_H
#define _INC_PWM_H
#include "stm32f446xx.h"

void PWM_Init();
void PWM_Three_Phase_init();
void PWM_Write(uint16_t Duty);

#endif