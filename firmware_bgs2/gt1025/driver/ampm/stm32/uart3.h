#ifndef __UART3_H__
#define __UART3_H__

#include "stm32f10x.h"
#include <stdint.h>
#include "lib/ringbuf.h"
#include "hw_config.h"

#define UART3_USE_TX_RINGBUFF

void USART3_Init(uint32_t pclk1,uint32_t baudrate);
void USART3_DeInit(void);
uint8_t USART3_PutChar (uint8_t ch);
void USART3_PutString (char *s);
uint8_t USART3_GetChar (void) ;

extern   RINGBUF USART3_RxRingBuff;
#ifdef UART3_USE_TX_RINGBUFF
extern RINGBUF USART3_TxRingBuff;
#endif

#define __USART3_BAUDRATE         115200			
#define __USART3_DATABITS         0x00000000
#define __USART3_STOPBITS         0x00000000
#define __USART3_PARITY           0x00000000
#define __USART3_FLOWCTRL         0x00000000

#endif

