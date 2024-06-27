#include "clock.h"

void SetSystemClock(uint8_t pllm, uint32_t plln, uint8_t pllp)
{
    // Enabling HSE
    RCC->CR |= RCC_CR_HSEON;

    // Wait untill HSE is stabled
    while (!(RCC->CR & RCC_CR_HSERDY))
        ;

    // Enabling PWR Clock
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    // Setting Power Scaling to 3
    PWR->CR |= PWR_CR_VOS;

    // Setting Flash Latency, Enabling Data and Instruction Cache and prefetching
    FLASH->ACR |= FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS;

    // Turning PLL OFF
    RCC->CR &= ~RCC_CR_PLLON;

    // Selecting HSE as PLL Source
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC;

    // Setting Prescaler of AHB1, APB1 and APB2
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;

    // Setting Value of PLLM, PLLN, and PLLP Prescalers
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLP); // Clearing Bits for those positions
    RCC->PLLCFGR |= (pllm << RCC_PLLCFGR_PLLM_Pos) | (plln << RCC_PLLCFGR_PLLN_Pos) | (pllp << RCC_PLLCFGR_PLLP_Pos);

    // Turning PLL on
    RCC->CR |= RCC_CR_PLLON;

    // Wait until PLL is Ready
    while (!(RCC->CR & RCC_CR_PLLRDY))
        ;

    // Settin PLL As System Clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // Wait untill the clock source switched perfectly
    while (((RCC->CFGR & RCC_CFGR_SWS) >> 2) != 2)
        ;
}


void SetSystemClock180MHz()
{
    SetSystemClock(4, 180, 0);
}

void SetSystemClock160MHz()
{
    SetSystemClock(4, 160, 0);
}


uint32_t GetAHB1Clock()
{
    uint8_t SysClockSrc = _GET_RCC_2BIT_FLAG(RCC_CFGR_SWS_FLAG);
    uint32_t SysClockFreq = 0;
    if (SysClockSrc == 0)
    {
        SysClockFreq = HSI_VALUE;
    }
    else if (SysClockSrc == 1)
    {
        SysClockFreq = HSE_VALUE;
    }
    else if (SysClockSrc == 2)
    {
        uint8_t PllSrc = _GET_RCC_FLAG(RCC_PLLCFGR_PLLSRC_FLAG);
        uint32_t pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk);
        uint32_t plln = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos;
        uint32_t pllp = (RCC->PLLCFGR & RCC_PLLCFGR_PLLP_Msk) >> RCC_PLLCFGR_PLLP_Pos;
        if (PllSrc)
        {
            SysClockFreq = ((HSE_VALUE / pllm) * plln) / ((pllp + 1) * 2);
        }
        else
        {
            SysClockFreq = ((HSI_VALUE / pllm) * plln) / ((pllp + 1) * 2);
        }
    }

    return (SysClockFreq >>= AHBPrescTable[_GET_AHB1_PRESCALER()]);
}

uint32_t GetAPB1Clock()
{
    return (GetAHB1Clock() >> APBPrescTable[_GET_APB1_PRESCALER()]);
}

uint32_t GetAPB2Clock()
{
    return (GetAHB1Clock() >> APBPrescTable[_GET_APB2_PRESCALER()]);
}