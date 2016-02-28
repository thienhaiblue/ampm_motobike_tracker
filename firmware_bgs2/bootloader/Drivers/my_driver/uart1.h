#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f10x.h"
#include <stdint.h>
#include "ringbuf.h"
#include "hw_config.h"
#define USE_TX_RINGBUFF


#define __USART1_BAUDRATE         115200			
#define __USART1_DATABITS         0x00000000
#define __USART1_STOPBITS         0x00000000
#define __USART1_PARITY           0x00000000
#define __USART1_FLOWCTRL         0x00000000


#define COMM_Open		USART1_Init
#define commBuf			USART1_RxRingBuff
#define COMM_Puts		USART1_PutString
#define COMM_Putc		USART1_PutChar

#ifdef USE_TX_RINGBUFF
extern uint8_t USART1_TxBuff[512];
extern RINGBUF USART1_TxRingBuff;
#endif
extern uint8_t USART1_RxBuff[512];
extern RINGBUF USART1_RxRingBuff;
extern RINGBUF USART1_USB_RxRingBuff;

void USART1_Init(uint32_t pclk2);
uint8_t USART1_PutChar (uint8_t ch);
uint8_t USART1_GetChar (void) ;
void USART1_PutString (uint8_t *s);
void USART1_DeInit(void);




#endif

