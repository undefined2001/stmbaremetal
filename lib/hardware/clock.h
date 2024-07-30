#ifndef _INC_CLOCK_H
#define _INC_CLOCK_H
#include "stm32f446xx.h"

#define RCC_FLAG_MSK 0x1FU
#define _GET_RCC_REGISTER(__FLAG__) (((__FLAG__ >> 5u) == 1) ? RCC->CR : \
                                     ((__FLAG__ >> 5u) == 2) ? RCC->PLLCFGR : \
                                     ((__FLAG__ >> 5u) == 3) ? RCC->CFGR:0)

#define _GET_RCC_FLAG_POS(__FLAG__) ((__FLAG__) & RCC_FLAG_MSK)

#define _GET_RCC_FLAG(__FLAG__) ((_GET_RCC_REGISTER(__FLAG__) & (1U << _GET_RCC_FLAG_POS(__FLAG__))) ? 1 : 0)
#define _GET_RCC_2BIT_FLAG(__FLAG__) ((_GET_RCC_REGISTER(__FLAG__) & (3U << _GET_RCC_FLAG_POS(__FLAG__))) >> _GET_RCC_FLAG_POS(__FLAG__))

#define RCC_CFGR_SWS_FLAG 0x62u
#define RCC_PLLCFGR_PLLSRC_FLAG 0X56u

/**
 * @brief: These Macro's is to get all the bus prescaler for example AHBx, APBx
 */
#define _GET_AHB1_PRESCALER() ((RCC->CFGR & RCC_CFGR_HPRE_Msk) >> RCC_CFGR_HPRE_Pos)
#define _GET_APB1_PRESCALER() ((RCC->CFGR & RCC_CFGR_PPRE1_Msk) >> RCC_CFGR_PPRE1_Pos)
#define _GET_APB2_PRESCALER() ((RCC->CFGR & RCC_CFGR_PPRE2_Msk) >> RCC_CFGR_PPRE2_Pos)

/**
 * @brief: The Value of Internal Oscillator frequency
 */
#ifndef HSI_VALUE
#define HSI_VALUE 16000000UL
#endif

/**
 * @brief: The Value of External Oscillator frequency
 */
#ifndef HSE_VALUE
#define HSE_VALUE 8000000UL
#endif

typedef struct
{
    uint8_t HSI;
    uint8_t HSE;

} ClockSrc_t;

typedef struct
{
    ClockSrc_t src;
    uint32_t pllm;
    uint32_t plln;
    uint32_t pllp;

} SysClockConf_t;

void SetSystemClock(uint8_t pllm, uint32_t plln, uint8_t pllp);

uint32_t GetAHB1Clock();
uint32_t GetAPB1Clock();
uint32_t GetAPB2Clock();

void SetSystemClock160MHz();
void SetSystemClock180MHz();

#endif
