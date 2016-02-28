
#ifndef __APP_CONFIG_TASK_H__
#define __APP_CONFIG_TASK_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "lib/sys_tick.h"
#include "lib/ringbuf.h"
#include "system_config.h"

typedef enum{
	NO_LOG = 0,
	TCP_IP_LOG,
	GSM_AT_CMD_LOG,
	GPS_LOG,
	ALL_LOG,
}LOG_TYPE;

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

extern uint32_t firmwareFileCrc;
extern uint32_t firmwareFileSize;
extern uint32_t firmwareFileOffSet;
extern uint32_t firmwareFileOffSetOld;
extern uint32_t firmwareSendFileOffSetFlag;
extern uint32_t firmwareSaveFileOffsetCnt;
extern uint32_t firmwareStatus;
extern RINGBUF configTaskRingBuf;
extern Timeout_Type tFirmwareTryToUpdate;
extern LOG_TYPE logSource;
void AppConfigTaskInit(void);
uint8_t CfgParserPacket(PARSER_PACKET_TYPE *parserPacket,CFG_PROTOCOL_TYPE *cfgProtoRecv,uint8_t c);
uint8_t CfgProcessData(CFG_PROTOCOL_TYPE *cfgProtoRecv,CFG_PROTOCOL_TYPE *cfgProtoSend,uint8_t *cfgSendDataBuff,uint32_t maxPacketSize,uint8_t logSendEnable);
uint32_t  DbgCfgPrintf(const uint8_t *format, ...);
void LoadFirmwareFile(void);

#endif
