
#include <string.h>
#include "packet_queue.h"

/**
* enqueue a binary packet, insert nescessary headers and escape characters
*/
int16_t PQUEUE_Put(RINGBUF *r, uint8_t *packet)
{
	uint16_t len = *(uint16_t*)packet;
	uint8_t c;
	uint16_t i = 2;
	
	RINGBUF_Put(r, 0x7E);
	
	while(len--){
		c = *packet++;
		
		switch(c){
			case 0x7D:
			case 0x7E:
			case 0x7F:
				RINGBUF_Put(r, 0x7D);
				c ^= 0x20;
				i++;
			default:
				RINGBUF_Put(r, c);
				i++;
		}
	}
	
	RINGBUF_Put(r, 0x7F);
	
	return i;
}
/**
* dequeue a binary packet
* \return packet size (may larger than bufSize)
*/
int16_t PQUEUE_Get(RINGBUF *r, uint8_t *buf, int16_t bufSize)
{
	uint8_t c;
	int16_t i = 1;
	
	do{
		if(RINGBUF_Get(r, &c)) return -1;
	}while(c != 0x7E);
	
	if(bufSize > 0 && buf != NULL)
		*buf++ = 0x7E;
	
	do{		
		if(RINGBUF_Get(r, &c)) return -1;
		
		if(bufSize && buf != NULL){
			*buf++ = c;
			bufSize--;
		}
		i++;
	}while(c != 0x7F);
	
	return i;
}
/**
* enqueue an ASCII packet
*/
int16_t APQUEUE_Put(RINGBUF *r, uint8_t *packet)
{
	uint16_t len = strlen((char *)packet);
	uint16_t i = len;
	
	while(i--)
		RINGBUF_Put(r, *packet++);
	RINGBUF_Put(r, 0);//terminate
	return len;
}
/**
* dequeue an ASCII packet
* \return packet size (may larger than bufSize)
*/
int16_t APQUEUE_Get(RINGBUF *r, uint8_t *buf, int16_t bufSize)
{
	uint8_t c;
	int16_t i = 0;
	
	while(1)
	{
		if(RINGBUF_Get(r, &c)) return -1;
		if(c == '$')
		{
			*buf ++ = c;
			break;
		}
	}
	
	while(1)
	{		
		if(RINGBUF_Get(r, &c)) return -1;	
		if(c == 0)
		{
			i++;
			return i;
		}
		else if(bufSize > 0 && buf != NULL)
		{
			*buf ++ = c;
			bufSize --;
		}
		i ++;
	}
}
