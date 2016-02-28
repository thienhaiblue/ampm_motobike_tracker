#ifndef __UART2_H__
#define __UART2_H__

#include "stm32f10x.h"
#include <stdint.h>
#include "lib/ringbuf.h"


extern uint8_t USART2_RxBuff[128];
extern RINGBUF USART2_RxRingBuff;

void USART2_Init(uint32_t pclk1,uint32_t baudrate);
void USART2_DeInit(void);
uint8_t USART2_PutChar (uint8_t ch);
uint8_t USART2_GetChar (void) ;

#define __USART2_BAUDRATE         9600			
#define __USART2_DATABITS         0x00000000
#define __USART2_STOPBITS         0x00000000
#define __USART2_PARITY           0x00000000
#define __USART2_FLOWCTRL         0x00000000

#endif

