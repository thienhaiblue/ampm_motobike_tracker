#ifndef __PROTO_PARSER_H__
#define __PROTO_PARSER_H__


#include <stdint.h>
#include "ringbuf.h"



#define USB_PACKET_SIZE		36
#define UART_PACKET_SIZE		516
#define FLASH_PACKET_SIZE		32
#define GPRS_PACKET_SIZE	516


typedef struct
{
	uint32_t packetNo;
	uint16_t packetLengthToSend;
	uint8_t  *dataPt;
} CFG_DATA_TYPE;

typedef enum{
	CFG_CMD_NEW_STATE,
	CFG_CMD_GET_LENGTH,
	CFG_CMD_GET_OPCODE,
	CFG_CMD_GET_DATA,
	CFG_CMD_CRC_CHECK,
	CFG_CMD_WAITING_SATRT_CODE
}CFG_CMD_STATE_TYPE;


typedef struct
{
	uint8_t start;
	uint16_t length;
	uint8_t opcode;
	uint8_t *dataPt;
	uint8_t crc;
} CFG_PROTOCOL_TYPE;

typedef struct
{
	CFG_CMD_STATE_TYPE state;
	uint16_t len;
	uint16_t lenMax;
	uint16_t cnt;
	uint8_t opcode;
	uint8_t crc;
} PARSER_PACKET_TYPE;

uint8_t CfgParserPacket(PARSER_PACKET_TYPE *parserPacket,CFG_PROTOCOL_TYPE *cfgProtoRecv,uint8_t c);
uint8_t CfgCalcCheckSum(uint8_t *buff, uint32_t length);


#endif
