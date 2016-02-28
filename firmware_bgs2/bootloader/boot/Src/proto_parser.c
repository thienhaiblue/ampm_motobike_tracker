
#include <stdio.h>
#include "proto_parser.h"




uint8_t CfgParserPacket(PARSER_PACKET_TYPE *parserPacket,CFG_PROTOCOL_TYPE *cfgProtoRecv,uint8_t c)
{
	switch(parserPacket->state)
	{
		case CFG_CMD_WAITING_SATRT_CODE:
			if(c == 0xCA)
			{
				parserPacket->state = CFG_CMD_GET_LENGTH;
				parserPacket->len = 0;
				parserPacket->crc = 0;
				parserPacket->cnt = 0;
			}
			break;
		case CFG_CMD_GET_LENGTH:
				parserPacket->len |= (uint16_t)c<<parserPacket->cnt;
				parserPacket->cnt += 8;
				if(parserPacket->cnt >= 16)
				{
					parserPacket->state = CFG_CMD_GET_OPCODE;
					parserPacket->cnt = 0;
				}
			break;
		case CFG_CMD_GET_OPCODE:		
					parserPacket->opcode = c;
					parserPacket->state = CFG_CMD_GET_DATA;
			break;
		case CFG_CMD_GET_DATA:
				if((parserPacket->cnt >= parserPacket->len) || (parserPacket->len > parserPacket->lenMax))
				{
					parserPacket->state = CFG_CMD_WAITING_SATRT_CODE;
				}
				else
				{
					parserPacket->crc += c;
					cfgProtoRecv->dataPt[parserPacket->cnt]= c;
					parserPacket->cnt++;
					if(parserPacket->cnt == parserPacket->len)
					{
						parserPacket->state = CFG_CMD_CRC_CHECK;
					}
				}
			break;
		case CFG_CMD_CRC_CHECK:
				parserPacket->state= CFG_CMD_WAITING_SATRT_CODE;
				if(parserPacket->crc  == c)
				{	
					cfgProtoRecv->length = parserPacket->len;
					cfgProtoRecv->opcode = parserPacket->opcode;
					return 0;
				}
			break;
		default:
			parserPacket->state = CFG_CMD_WAITING_SATRT_CODE;
			break;				
	}
	return 0xff;
}


uint8_t CfgCalcCheckSum(uint8_t *buff, uint32_t length)
{
	uint32_t i;
	uint8_t crc = 0;
	for(i = 0;i < length; i++)
	{
		crc += buff[i];
	}
	return crc;
}
