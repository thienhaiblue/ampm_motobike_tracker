#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdlib.h>
#include <stdint.h>
#include "fcs.h"
typedef void(PROTO_PARSE_CALLBACK)(void *parser);

typedef struct
{
	uint8_t *buf;
	uint16_t bufSize;
	uint16_t dataLen;
	uint8_t isEsc;
	PROTO_PARSE_CALLBACK* callback;
}PROTO_PARSER;

uint8_t PROTO_ParseByteUnFcs(PROTO_PARSER *parser, uint8_t value);
uint8_t PROTO_ParseByte(PROTO_PARSER *parser, uint8_t value);
uint8_t PROTO_InitParser(PROTO_PARSER *parser, PROTO_PARSE_CALLBACK *completeCallback, uint8_t *buf, uint16_t bufSize);
uint8_t PROTO_Parse(PROTO_PARSER *parser, uint8_t *value, uint16_t len);
uint16_t PROTO_Add(uint8_t *buf, uint8_t *packet, uint16_t bufSize,uint16_t packetLen);
uint16_t PROTO_AddUnFcs(uint8_t *buf, uint8_t *packet, uint16_t bufSize,uint16_t packetLen);

#endif
