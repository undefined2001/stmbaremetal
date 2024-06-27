#ifndef _INC_UART_H
#define _INC_UART_H

#include "stm32f446xx.h"
#include "debug.h"

typedef struct
{
    uint8_t TxBusy;
    uint8_t RxBusy;
    uint8_t TxLen;
    uint8_t RxLen;
    uint8_t *pTxBuffer;
    uint8_t *pRxBuffer;
} UART_Handle_t;

void UartInit();
void UartSendBuffer(char *buffer);
void UartReceiveBuffer(char *buffer, uint8_t len);
void UartClearBuffer(char *buffer, uint8_t len);
void UART_ReceiveIT(uint8_t *pRxBuffer, uint32_t Len);

#endif