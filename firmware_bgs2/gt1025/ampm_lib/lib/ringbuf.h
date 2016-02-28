#ifndef _RING_BUF_H_
#define _RING_BUF_H_

#include <stdint.h>
#include <stdlib.h>

 typedef struct
{
    volatile uint32_t head;                
    volatile uint32_t tail;                
    volatile uint32_t size;                
    volatile uint8_t *pt;  					
} RINGBUF;

extern RINGBUF UART2_RxRingBuff;

int32_t RINGBUF_Init(RINGBUF *r, uint8_t* buf, uint32_t size);
int32_t RINGBUF_Put(RINGBUF *r, uint8_t c);
int32_t RINGBUF_Get(RINGBUF *r, uint8_t* c);
int32_t RINGBUF_GetFill(RINGBUF *r);
#endif
