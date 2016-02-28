#ifndef __UART3_H__
#define __UART3_H__

#include "stm32f10x.h"
#include <stdint.h>
#include "lib/ringbuf.h"
#include "hw_config.h"



void USART3_Init(uint32_t pclk1,uint32_t baudrate);
uint8_t USART3_PutChar (uint8_t ch);
void USART3_PutString (char *s);
uint8_t USART3_GetChar (void) ;

#define __USART3_BAUDRATE         9600			
#define __USART3_DATABITS         0x00000000
#define __USART3_STOPBITS         0x00000000
#define __USART3_PARITY           0x00000000
#define __USART3_FLOWCTRL         0x00000000

#endif

