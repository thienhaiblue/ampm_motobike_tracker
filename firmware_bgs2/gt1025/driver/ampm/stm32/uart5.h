#ifndef __UART5_H__
#define __UART5_H__

#include "stm32f10x.h"
#include <stdint.h>
#include "lib/ringbuf.h"
#include "hw_config.h"

#define USE_TX_RINGBUFF

extern uint8_t UART5_TxBuff[256];
extern RINGBUF UART5_TxRingBuff;
extern uint8_t UART5_RxBuff[256];
extern RINGBUF UART5_RxRingBuff;

void UART5_Init(uint32_t pclk1,uint32_t baudrate);
uint8_t UART5_PutChar (uint8_t ch);
void UART5_PutString (char *s);
uint8_t UART5_GetChar (void) ;

#define __UART5_BAUDRATE         115200			
#define __UART5_DATABITS         0x00000000
#define __UART5_STOPBITS         0x00000000
#define __UART5_PARITY           0x00000000
#define __UART5_FLOWCTRL         0x00000000

#endif

