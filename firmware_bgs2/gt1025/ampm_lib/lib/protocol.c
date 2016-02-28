
#include <stdint.h>
#include "fcs.h"
#include "protocol.h"

#define PROTO_BUFFER_SIZE				128

#define PROTO_Info(...)		//PrintfDebug(__VA_ARGS__)


uint8_t PROTO_InitParser(PROTO_PARSER *parser, PROTO_PARSE_CALLBACK *completeCallback, uint8_t *buf, uint16_t bufSize)
{
	parser->buf = buf;
	parser->bufSize = bufSize;
	parser->dataLen = 0;
	parser->callback = completeCallback;
	parser->isEsc = 0;
	return 0;
}
uint8_t PROTO_ParseByte(PROTO_PARSER *parser, uint8_t value)
{
	uint16_t fcs;
	uint16_t rxFcs;
	switch(value){
		case 0x7D:
			parser->isEsc = 1;
			break;
			
		case 0x7E:
			parser->dataLen = 0;
			parser->isEsc = 0;
			break;
			
		case 0x7F:
			if(parser->dataLen >= 4)
			{
				fcs = FCS_Get(parser->buf, parser->dataLen - 2);
				rxFcs = ((uint16_t)parser->buf[parser->dataLen-1]<<8) | parser->buf[parser->dataLen-2];
				parser->dataLen -= 2;	
				if(fcs == rxFcs)
				{
					if(parser->callback != NULL)
						parser->callback(parser);
					parser->dataLen = 0;
					parser->isEsc = 0;
					return 1;
				}
			}
			parser->dataLen = 0;
			parser->isEsc = 0;
			break;
			
		default:
			if(parser->isEsc){
				parser->buf[parser->dataLen++] = value ^ 0x20;
				parser->isEsc = 0;
			}else
				parser->buf[parser->dataLen++] = value;
			if(parser->dataLen >= parser->bufSize)
			{
				parser->dataLen = 0;
				parser->isEsc = 0;
				return 0;
			}
				
			break;
	}
	
	return 0;
}

uint8_t PROTO_ParseByteUnFcs(PROTO_PARSER *parser, uint8_t value)
{
	switch(value){
		case 0x7D:
			parser->isEsc = 1;
			break;
			
		case 0x7E:
			parser->dataLen = 0;
			parser->isEsc = 0;
			break;
			
		case 0x7F:
			if(parser->dataLen >= 2)
			{
					if(parser->callback != NULL)
						parser->callback(parser);
					parser->dataLen = 0;
					parser->isEsc = 0;
					return 1;
			}
			parser->dataLen = 0;
			parser->isEsc = 0;
			break;
		default:
			if(parser->isEsc){
				parser->buf[parser->dataLen++] = value ^ 0x20;
				parser->isEsc = 0;
			}else
				parser->buf[parser->dataLen++] = value;
			if(parser->dataLen >= parser->bufSize)
			{
				parser->dataLen = 0;
				parser->isEsc = 0;
				return 0;
			}
				
			break;
	}
	return 0;
}

uint8_t PROTO_Parse(PROTO_PARSER *parser, uint8_t *value, uint16_t len)
{
	while(len--)
		PROTO_ParseByte(parser, *value++);
		
	return 0;
}

uint16_t PROTO_Add(uint8_t *buf,uint8_t *packet, uint16_t bufSize,uint16_t len)
{
	uint16_t i = 2,fcs;
	uint8_t c;
	if(bufSize < 1) return 0;
	// perform CRC16 checksum on the packet 
	fcs = FCS_Get((uint8_t *)packet, len);
	len += 2;
	*buf++ = 0x7E;
	bufSize --;
	while(len--)
	{
		if(len == 1)
			c = fcs & 0xFF;
		else if(len == 0)
			c = (fcs >> 8) & 0xFF;
		else
		{
			c = *packet;
			packet++;
		}
		switch(c)
		{
			case 0x7D:
			case 0x7E:
			case 0x7F:
				if(bufSize < 2) return 0;
				*buf++ = 0x7D;
				*buf++ = c ^ 0x20;
				i += 2;
				bufSize -= 2;
				break;
			default:
				if(bufSize < 1) return 0;
				*buf++ = c;
				i ++;
				bufSize --;
				break;
		}
	}
	
	if(bufSize < 1) return 0;
	*buf++ = 0x7F;
	
	return i;
}


uint16_t PROTO_AddUnFcs(uint8_t *buf, uint8_t *packet, uint16_t bufSize,uint16_t packetLen)
{
	uint16_t i = 2;
	
	if(bufSize < 1) return 0;
	// perform CRC16 checksum on the packet 
	*buf++ = 0x7E;
	bufSize --;
	while(packetLen--){
		switch(*packet){
			case 0x7D:
			case 0x7E:
			case 0x7F:
				if(bufSize < 2) return 0;
				*buf++ = 0x7D;
				*buf++ = *packet++ ^ 0x20;
				i += 2;
				bufSize -= 2;
				break;
			default:
				if(bufSize < 1) return 0;
				*buf++ = *packet++;
				i ++;
				bufSize --;
				break;
		}
	}
	
	if(bufSize < 1) return 0;
	*buf++ = 0x7F;
	
	return i;
}




