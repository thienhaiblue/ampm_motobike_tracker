#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f10x.h"
#include <stdint.h>
#include "uart1.h"
#include "lib/ringbuf.h"

#define USE_TX_RINGBUFF
#define UART1_USE_CTS
#define UART1_USE_RTS

#define __USART1_BAUDRATE         115200			
#define __USART1_DATABITS         0x00000000
#define __USART1_STOPBITS         0x00000000
#define __USART1_PARITY           0x00000000
#define __USART1_FLOWCTRL         0x00000000


#ifdef USE_TX_RINGBUFF
extern uint8_t USART1_TxBuff[1024];
extern RINGBUF USART1_TxRingBuff;
#endif
extern uint8_t USART1_RxBuff[1024];
extern RINGBUF USART1_RxRingBuff;
extern RINGBUF USART1_USB_RxRingBuff;

void USART1_Init(uint32_t pclk2,uint32_t baudrate);
uint8_t USART1_PutChar (uint8_t ch);
uint8_t USART1_GetChar (void) ;
void USART1_PutString (uint8_t *s);
void USART1_DeInit(void);




#endif

