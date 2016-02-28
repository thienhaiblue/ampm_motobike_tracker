#ifndef _AT_GPRS_CINTERION_TASK_H_
#define _AT_GPRS_CINTERION_TASK_H_

#include <string.h>
#include <stdio.h>
#include "lib/ringbuf.h"
#include "lib/sys_tick.h"
#include "at_command_parser.h"

#define TCPIP_FEATURE_ECHO_ENABLE	1
#define TCP_IP_BUF_SIZE	800
#define GPRS_MAX_SOCKET_NUM 5

#define TCPIP_NO_DATA	0
#define TCPIP_NEW_DATA_RECV 1
#define TCPIP_NEW_DATA_SEND	2

typedef enum{
	GPRS_TASK_IDLE = 0,
	GPRS_TASK_LOCATION_SEND_CMD,
	TASK_GPRS_GET_CELL_LOCATION,
	GPRS_TASK_CREATE_SOCKET,
	GPRS_TASK_CREATE_SOCKET_1,
	GPRS_TASK_CREATE_SOCKET_2,
	GPRS_TASK_CREATE_SOCKET_3,
	GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET,
	GPRS_TASK_READ_DATA,
	GPRS_TASK_GET_DATA,
	GPRS_TASK_SEND_DATA,
	GPRS_TASK_SEND_OUT_DATA,
	GPRS_TASK_CHECK_DATA_SENT,
	GPRS_TASK_CHECK_DATA_SENT1,
	GPRS_TASK_CLOSE_CONNECTION,
	GPRS_TASK_WAIT_CONNECTION_CLOSE,
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
	CONN_CMD_CLOSE,
	CONN_CMD_EMPTY
}TCP_IP_CONN_CMD;

typedef enum{
	CONN_NO_ERROR,
	CONN_ENABLE_GPRS_FAIL,
	CONN_OPEN_SOCKET_FAIL,
	CONN_CREATE_TCP_CONNECTION_FAIL,
	CONN_SEND_DATA_FAIL,
}TCP_IP_CONN_ERROR;

typedef struct{
	struct TCPIP_CONN_TYPE *next;
	uint8_t *ip;
	uint16_t port;
	uint8_t socketId;
	uint8_t newDataFlag;
	uint8_t sentDataFlag;
	TCPIP_SOCKET_STATUS socketStatus;
	TCP_IP_CONN_ERROR error;
	uint8_t *dataOutPt;
	uint16_t dataOutLen;
	TCP_IP_CONN_CMD cmd;
	uint16_t periodDataRecvValue;
	Timeout_Type periodDataRecvCheck;
	uint8_t tryNum;
	uint8_t echo;
}TCPIP_CONN_TYPE;


typedef struct{
	uint8_t data[TCP_IP_BUF_SIZE];
	uint16_t dataUnReadLen[GPRS_MAX_SOCKET_NUM];
	uint16_t len;
	uint8_t socket;
	uint8_t flag;
}TCPIP_DATA_TYPE;

extern TCPIP_DATA_TYPE tcpIpBuf;
extern uint8_t gprsGetLocation;

uint32_t GPRS_NewRecvData(uint16_t cnt,uint8_t c);
void GprsTask_Init(void);
GPRS_PHASE_TYPE GprsTask_GetPhase(void);
void Gprs_Task(void);
void GprsTask_ConnInit(TCPIP_CONN_TYPE *conn,uint8_t* ip,uint16_t port,uint8_t socketNum,uint16_t periodDataRecvCheck);
uint32_t GPRS_TCPCloseSocket1(uint16_t cnt,uint8_t c);
uint32_t GPRS_TCPCloseSocket(uint16_t cnt,uint8_t c);
uint32_t GPRS_Deactivation(uint16_t cnt,uint8_t c);
uint8_t RemaskSocket(uint8_t socket);
uint32_t GPRS_TcpGetLength(uint16_t cnt,uint8_t c);
uint32_t GPRS_TcpGetData(uint16_t cnt,uint8_t c);
uint8_t  GprsTask_SendData(TCPIP_CONN_TYPE *conn,uint8_t *data,uint16_t len);
uint8_t GprsTask_GprsDisable(void);
uint32_t GprsTask_Stop(void);


#endif
