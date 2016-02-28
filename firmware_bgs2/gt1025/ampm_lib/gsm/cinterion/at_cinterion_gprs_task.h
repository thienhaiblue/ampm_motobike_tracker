#ifndef _AT_GPRS_CINTERION_TASK_H_
#define _AT_GPRS_CINTERION_TASK_H_

#include <string.h>
#include <stdio.h>
#include "lib/ringbuf.h"
#include "tick.h"
#include "at_command_parser.h"

#define TCPIP_FEATURE_ECHO_ENABLE	1

typedef enum{
	GPRS_TASK_INIT = 0,
	GPRS_TASK_START,
	GPRS_TASK_IDLE,
	GPRS_TASK_CREATE_SOCKET,
	GPRS_TASK_CREATE_SOCKET_1,
	GPRS_TASK_CREATE_SOCKET_2,
	GPRS_TASK_CREATE_SOCKET_3,
	GPRS_TASK_CREATE_SOCKET_4,
	GPRS_TASK_CREATE_SOCKET_5,
	GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET,
	GPRS_TASK_READ_DATA,
	GPRS_TASK_GET_DATA,
	GPRS_TASK_SEND_DATA,
	GPRS_TASK_SEND_OUT_DATA,
	GPRS_TASK_CHECK_DATA_SENT
} GPRS_PHASE_TYPE;

typedef enum{
	SOCKET_DISCONNECTED,
	SOCKET_CONNECTING,
	SOCKET_CONNECTED
}TCPIP_SOCKET_STATUS;

typedef enum{
	CONN_CMD_STOP,
	CONN_CMD_START,
	CONN_SEND_DATA,
	CONN_READ_DATA,
	CONN_CMD_PAUSE,
	CONN_CMD_EMPTY
}TCP_IP_CONN_CMD;

typedef struct{
	struct TCPIP_CONN_TYPE *next;
	uint8_t ip[4];
	uint16_t port;
	uint8_t socketId;
	uint8_t newDataFlag;
	TCPIP_SOCKET_STATUS socketStatus;
	uint8_t *dataOutPt;
	uint16_t dataOutLen;
	TCP_IP_CONN_CMD cmd;
	uint16_t periodDataRecvValue;
	Timeout_Type periodDataRecvCheck;
	uint8_t echo;
}TCPIP_CONN_TYPE;

uint32_t GPRS_NewRecvData(uint16_t cnt,uint8_t c);
void GprsTask_Init(void);
GPRS_PHASE_TYPE GprsTask_GetPhase(void);
void Gprs_Task(void);
void GprsTask_ConnInit(TCPIP_CONN_TYPE *conn,uint32_t ip,uint16_t port,uint8_t socketNum,uint16_t periodDataRecvCheck);

#endif
