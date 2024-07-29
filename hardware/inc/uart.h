#ifndef _INC_UART_H
#define _INC_UART_H

#include "stm32f446xx.h"

#define UART_ENABLE 1U
#define UART_DISABLE 0U

#define UART_MODE_TX_ONLY 0U
#define UART_MODE_RX_ONLY 1U
#define UART_MODE_TX_RX_BOTH 2U

#define UART_PARITY_EVEN 0U
#define UART_PARITY_ODD 1U
#define UART_PARITY_NONE 2U

#define UART_BR_9600 9600U
#define UART_BR_19200 19200U
#define UART_BR_34800 34800U
#define UART_BR_115200 115200U

#define UART_WORD_LEN_8_BIT 0U
#define UART_WORD_LEN_9_BIT 1U

typedef struct
{
    uint32_t BaudRate;
    uint32_t Mode;
    uint32_t Parity;
    uint32_t WordLen;

} UART_ConfigTypeDef;

typedef struct
{
    USART_TypeDef *pUARTx;
    UART_ConfigTypeDef Config;

    uint8_t TxBusy;
    uint8_t RxBusy;
    uint8_t TxLen;
    uint8_t RxLen;
    uint8_t *pTxBuffer;
    uint8_t *pRxBuffer;
} UART_HandleTypeDef;

void UartInit();
void UART_Init(UART_HandleTypeDef *pUARTHandle);
void UartSendBuffer(char *buffer);
void UartReceiveBuffer(char *buffer, uint8_t len);
void UartClearBuffer(char *buffer, uint8_t len);
void UART_ReceiveIT(uint8_t *pRxBuffer, uint32_t Len);

#endif