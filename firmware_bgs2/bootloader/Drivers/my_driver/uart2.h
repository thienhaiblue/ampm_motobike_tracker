#ifndef __UART2_H__
#define __UART2_H__

#include "stm32f10x.h"
#include <stdint.h>
#include "ringbuf.h"
#include "hw_config.h"


extern uint8_t USART2_RxBuff[1024];
extern RINGBUF USART2_RxRingBuff;

void USART2_Init(uint32_t pclk1);
uint8_t USART2_PutChar (uint8_t ch);
uint8_t USART2_GetChar (void) ;

#define __USART2_BAUDRATE         9600			
#define __USART2_DATABITS         0x00000000
#define __USART2_STOPBITS         0x00000000
#define __USART2_PARITY           0x00000000
#define __USART2_FLOWCTRL         0x00000000

#endif

