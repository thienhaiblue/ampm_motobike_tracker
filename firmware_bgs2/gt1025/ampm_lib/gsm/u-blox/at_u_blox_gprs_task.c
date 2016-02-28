#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "modem.h"
#include "lib/sys_tick.h"
#include "lib/data_cmp.h"
#include "at_command_parser.h"
#include "at_send_cmd_task.h"
#include "at_u_blox_gprs_task.h"
#include "at_gsm_task.h"
#include "lib/list.h"

#define DEFAULT_GPSR_APN		"internet"
#define DEFAULT_GPRS_USR		""
#define DEFAULT_GPRS_PWD		""



#define SOCKET_CLOSE	0
#define SOCKET_OPEN 	1
#define IS_IP 0
#define IS_DOMAIN 1

#define TCPIP_NO_DATA	0
#define TCPIP_NEW_DATA_RECV 1
#define TCPIP_NEW_DATA_SEND	2

LIST(gprsConnList);

const AT_CMD_PACKET_TYPE atGprsCmdStart[] = {
	"AT\r",NULL,NULL,"OK","ERROR",1,1000,100
};


AT_CMD_LIST_TYPE atGprsCmdListStart[sizeof(atGprsCmdStart)/sizeof(AT_CMD_PACKET_TYPE)];



TCPIP_CONN_TYPE	*tcpIpConn;
GPRS_PHASE_TYPE gprs_TaskPhase;
TCPIP_DATA_TYPE tcpIpBuf;

Timeout_Type tGprsTaskPhaseTimeOut;

uint8_t gprsCmdResBuf[32];

uint8_t createSocketNow = 0;
uint8_t gprsSocketStatus[GPRS_MAX_SOCKET_NUM];
uint8_t gprsSocketMask[GPRS_MAX_SOCKET_NUM];
uint32_t socketNo = 0;
uint8_t DNS_serverIp[16];
uint8_t tryOpenSocketNum = 0;
uint16_t GPRS_dataUnackLength = 0;
uint8_t gprsCheckDataCnt = 0;
uint8_t socketTryToConnectCnt = 0;
uint8_t tryToCreateSocketCnt = 0;
uint8_t gprsGetLocation = 0;

void GprsTask_Init(void)
{
	memset(gprsSocketStatus,NULL,sizeof(gprsSocketStatus));
	memset(gprsSocketMask,NULL,sizeof(gprsSocketMask));
	tcpIpBuf.flag = TCPIP_NO_DATA;
	tcpIpBuf.socket = 0;
	tcpIpBuf.len = 0;
	list_init(gprsConnList);
	gprs_TaskPhase = GPRS_TASK_IDLE;
	
}

uint8_t GprsTask_GprsDisable(void)
{
	if(gprsEnableFlag && gprsDisable == 0)
		gprsDisable = 1;
	return !gprsEnableFlag;
}

void GprsTask_CreateTcpIpConnection(TCPIP_CONN_TYPE *conn)
{
	list_add(gprsConnList,conn);
}

uint8_t Domain_IpCheck(uint8_t *domain_ip)
{
	uint8_t *pt = domain_ip;
	while(*pt != NULL)
	{
		if(!((*pt >= '0' && *pt <= '9') || *pt == '.'))
		{
			return IS_DOMAIN;
		}
		pt++;
	}
	return IS_IP;
}
void GprsTask_ConnInit(TCPIP_CONN_TYPE *conn,uint8_t* ip,uint16_t port,uint8_t socketNum,uint16_t periodDataRecvCheck)
{
	memset(conn,0,sizeof(TCPIP_CONN_TYPE));
	memset(&tcpIpBuf,0,sizeof(TCPIP_DATA_TYPE));
	conn->error = CONN_NO_ERROR;
	conn->ip = ip;
	conn->port = port;
	conn->cmd = CONN_CMD_START;
	conn->socketStatus = SOCKET_DISCONNECTED;
	conn->socketId =  socketNum;
	conn->periodDataRecvValue = periodDataRecvCheck;
	conn->tryNum = 1;
	
	InitTimeout(&conn->periodDataRecvCheck,conn->periodDataRecvValue); 
	GprsTask_CreateTcpIpConnection(conn);
	gprsSocketStatus[tcpIpConn->socketId] = SOCKET_CLOSE;
}

uint8_t GprsTask_SendData(TCPIP_CONN_TYPE *conn,uint8_t *data,uint16_t len)
{
	if(tcpIpBuf.flag == TCPIP_NO_DATA)
	{
		memcpy(&tcpIpBuf.data,data,len);
		tcpIpBuf.len = len;
		tcpIpBuf.flag = TCPIP_NEW_DATA_SEND;
		conn->dataOutPt = tcpIpBuf.data;
		conn->dataOutLen = tcpIpBuf.len;
		conn->cmd = CONN_SEND_DATA;
		return 0;
	}
	return 1;
}

/*				
				  \/
+UUSORD: 0,14
AT_cmdRecvBuff = 0,14
*/
uint32_t GPRS_TcpGetLength(uint16_t cnt,uint8_t c)
{
	static uint32_t length = 0;
	static uint8_t socketNo = 0;
	static uint8_t flag = 1;
	switch(cnt)
	{
		case 0:
				socketNo = (c - '0');
				length = 0;
				flag = 1;
			break;
		case 1:
				if((c >= '0') && (c <= '9'))
				{
					flag = 0;
					socketNo *= 10;
					socketNo += c - '0';
				}
				socketNo = RemaskSocket(socketNo);
				if(socketNo >= GPRS_MAX_SOCKET_NUM)
				{
					return 0;
				}
			break;
		default:
			if(flag)
			{
				if((c >= '0') && (c <= '9'))
				{
					length *= 10;
					length += c - '0';
				}
				else
				{
					tcpIpBuf.dataUnReadLen[socketNo] += length;
					return 0;
				}
			}
			else if(c == ',')
				flag = 1;
			else
				return 0;
		break;
	}
	return 0xff;
}

//+UUPSDD: 0
uint32_t GPRS_Deactivation(uint16_t cnt,uint8_t c)
{
	gprsEnableFlag = 0;
	return 0;
}

enum{
	GPRS_GET_SOCKET,
	GPRS_GET_LENGTH,
	GPRS_PREPARE_GET_DATA,
	GPRS_GET_DATA,
	GPRS_START_RECV_DATA,
	GPRS_FINISH
}gprsRecvDataPhase = GPRS_FINISH;

/*
+USORD: 0,12,"thienhaiblue"
									 1,"
AT_cmdRecvBuff = 0,12,"thienhaiblue"
									 123,"
                 0,0,""
*/
uint32_t GPRS_TcpGetData(uint16_t cnt,uint8_t c)
{
	static uint32_t dataLen = 0;
	static uint8_t socketNo = 0;
	switch(cnt)
	{
		case 0:
				socketNo = (c - '0');
				dataLen = 0;
				gprsRecvDataPhase = GPRS_GET_SOCKET;
		break;
		default:
			switch(gprsRecvDataPhase)
			{
				case GPRS_GET_SOCKET:
					if((c >= '0') && (c <= '9'))
					{
						socketNo *= 10;
						socketNo += c - '0';
					}
					else if(c == ',')
					{
						tcpIpBuf.socket = socketNo;
						gprsRecvDataPhase = GPRS_GET_LENGTH;
					}
					else
						return 0;
					socketNo = RemaskSocket(socketNo);
					if(socketNo >= GPRS_MAX_SOCKET_NUM)
					{
						tcpIpBuf.flag = TCPIP_NO_DATA;
						return 0;
					}
				break;
				case GPRS_GET_LENGTH:
						if((c >= '0') && (c <= '9'))
						{
							dataLen *= 10;
							dataLen += c - '0';
						}
						else if(c == ',')
						{
							if(dataLen == 0)
								tcpIpBuf.dataUnReadLen[socketNo] = 0;
							else if(tcpIpBuf.dataUnReadLen[socketNo] >= dataLen)
								tcpIpBuf.dataUnReadLen[socketNo] -= dataLen;
							else
								tcpIpBuf.dataUnReadLen[socketNo] = 0;
							gprsRecvDataPhase = GPRS_PREPARE_GET_DATA;
						}
					break;
				case GPRS_PREPARE_GET_DATA:
					if(dataLen == 0){tcpIpBuf.len = 0; return 0;}
					if(c == '"')
					{
						tcpIpBuf.len = 0;
						gprsRecvDataPhase = GPRS_GET_DATA;
					}
					else 
						return 0;
					break;
				case GPRS_GET_DATA:
					tcpIpBuf.data[tcpIpBuf.len] = c;
					tcpIpBuf.len++;
					if(tcpIpBuf.len >= dataLen || tcpIpBuf.len >= TCP_IP_BUF_SIZE)
					{
						tcpIpBuf.socket = socketNo;
						gprsRecvDataPhase = GPRS_FINISH;
						tcpIpBuf.flag = TCPIP_NEW_DATA_RECV;
						return 0;
					}
					break;
				case GPRS_FINISH:
				default:
					break;
			}
			break;
	}
	return 0xff;
}


uint32_t GPRS_TCPCreateSocket(uint16_t cnt,uint8_t c)
{
	if((c >= '0') && (c <= '9'))
	{
		gprsSocketMask[createSocketNow] = c - '0';
	}
	return 0;
}

uint32_t GPRS_GetIpFromDNS(uint16_t cnt,uint8_t c)
{
	static uint8_t *pt;
	switch(cnt)
	{
		case 0:
				pt = DNS_serverIp;
				*pt = c;
				pt++;
			break;
		default:
				if((c == '"') || (pt >= DNS_serverIp + sizeof(DNS_serverIp)))
				{
					return 0;
				}
				else
				{
					*pt = c;
					pt++;
					*pt = 0;
				}
			break;
	}
	
	return 0xff;
}

uint8_t RemaskSocket(uint8_t socket)
{
	uint8_t i;
	for(i = 0;i < GPRS_MAX_SOCKET_NUM;i++)
	{
		if(gprsSocketStatus[i] == SOCKET_OPEN)
			if(gprsSocketMask[i] == socket)
				return i;
	}
	return 0xff;
}

/*+UUSOCL: */
uint32_t GPRS_TCPCloseSocket1(uint16_t cnt,uint8_t c)
{ uint8_t socketNo;
	socketNo = RemaskSocket((c - '0'));
	if(socketNo < GPRS_MAX_SOCKET_NUM)
	{
		gprsSocketStatus[socketNo] = SOCKET_CLOSE;
	}
	return 0;
}
/*+USOWR: 0,0*/
uint32_t GPRS_TCPCloseSocket(uint16_t cnt,uint8_t c)
{
	switch(cnt)
	{
		case 0:
				socketNo = RemaskSocket((c - '0'));
				if(socketNo >= GPRS_MAX_SOCKET_NUM)
				{
					return 0;
				}
			break;
		case 2:
				if(c == '0')
				{
					gprsSocketStatus[socketNo] = SOCKET_CLOSE;
				}
				return 0;
		default:
			break;
	}
	return 0xff;
}


void GPRS_CheckSocketFree(uint8_t socket)
{
	uint8_t i;
	gprsSocketMask[socket] = 0;
	for(i = 0; i < GPRS_MAX_SOCKET_NUM;)
	{
		if(gprsSocketStatus[i] == SOCKET_OPEN)
		{
			if((gprsSocketMask[i] == gprsSocketMask[socket]) && (i != socket))
			{
				gprsSocketMask[socket]++;
				i = 0;
				continue;
			}
		}
		i++;
	}
}

/*
AT+USOCTL=0,11 +USOCTL: 0,11,2501
*/
uint32_t GPRS_TcpSocketControl(uint16_t cnt,uint8_t c)
{
	static uint32_t length = 0;
	static uint8_t socketNo = 0;
	switch(cnt)
	{
		case 0:
				socketNo = (c - '0');
				length = 0;
				if(socketNo != gprsSocketMask[tcpIpConn->socketId])
				{
					return 0;
				}
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		default:
			if(cnt > 9) 
			{
				GPRS_dataUnackLength = length;
				if(GPRS_dataUnackLength >= 32768)
					GPRS_dataUnackLength = 0;
				return 0;
			}
			if((c >= '0') && (c <= '9'))
			{
				length *= 10;
				length += c - '0';
			}
			else
			{
				GPRS_dataUnackLength = length;
				if(GPRS_dataUnackLength == 32768)
					GPRS_dataUnackLength = 0;
				return 0;
			}
			break;
	}
	return 0xff;
}



uint32_t GPRS_CmdFails(uint16_t cnt,uint8_t c)
{
	gprs_TaskPhase = GPRS_TASK_IDLE;
	return 0;
}

uint32_t GPRS_GSM_ReInit(uint16_t cnt,uint8_t c)
{
	gprsEnableFlag = 0;
	gprsEnableCmd = 1;
	gprs_TaskPhase = GPRS_TASK_IDLE;
	return 0;
}

GPRS_PHASE_TYPE GprsTask_GetPhase(void)
{
	return gprs_TaskPhase;
}


uint32_t GprsTask_Stop(void)
{
	if(gprs_TaskPhase == GPRS_TASK_SEND_OUT_DATA
		|| gprs_TaskPhase == GPRS_TASK_SEND_DATA
	)
	{
		return 0;
	}
	gprs_TaskPhase = GPRS_TASK_IDLE;
	return 1;
}


void Gprs_Task(void)
{	
	uint16_t i,j;
	GPRS_PHASE_TYPE gprs_TaskPhaseOld = gprs_TaskPhase;
	if(GsmTask_GetPhase() ==  MODEM_TURN_ON)
	{
		gprs_TaskPhase = GPRS_TASK_IDLE;
		return;
	}
	if(GsmTask_GetPhase() != MODEM_SYS_COVERAGE_OK)
	{
		memset(gprsSocketStatus,0x00,sizeof(gprsSocketStatus));
		memset(gprsSocketMask,0x00,sizeof(gprsSocketMask));
		return;
	}
	if(modemIsBusy == 1 && modemIsBusySource != AT_U_BLOX_GPRS_TASK)
		return;
	switch(gprs_TaskPhase)
	{
		case GPRS_TASK_LOCATION_SEND_CMD:
			if(ModemCmdTask_SendCmd(NULL,NULL,500, modemOk, modemError, 5000, 0, "AT+ULOC=2,2,0,100,5000\r") == MODEM_RES_OK)
			{
				gprs_TaskPhase = TASK_GPRS_GET_CELL_LOCATION;
			}
			break;
		case TASK_GPRS_GET_CELL_LOCATION:
			if (ModemCmdTask_IsIdle())
			{
				gprs_TaskPhase = GPRS_TASK_IDLE;
			}
			break;
		case GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET:
				if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
				gprsSocketStatus[tcpIpConn->socketId] = SOCKET_CLOSE;
				GPRS_CheckSocketFree(tcpIpConn->socketId);
				createSocketNow = tcpIpConn->socketId;
				if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 3000, 2, "AT+USOCL=%d\r", gprsSocketMask[tcpIpConn->socketId]) == MODEM_RES_OK)
				{
					tcpIpConn->socketStatus = SOCKET_DISCONNECTED;
					gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET;
				}
				else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
				{
					gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET;
				}
		break;
		case GPRS_TASK_CREATE_SOCKET:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			if(gprsEnableFlag == 0)
			{
				gprs_TaskPhase = GPRS_TASK_IDLE;
				break;
			}
			if(gprsSocketStatus[tcpIpConn->socketId] == SOCKET_OPEN)
			{
				COMM_Putc(0x1B);
				gprs_TaskPhase = GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET;
				break;
			}
			if(ModemCmdTask_IsIdle())
			{
				COMM_Putc(0x1B);
				createSocketNow = tcpIpConn->socketId;
				ModemCmdTask_SendCmd(GPRS_TCPCreateSocket,"+USOCR: ",100, modemOk, modemError, 5000, 1, "AT+USOCR=6\r");
				gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET_1;
			}

		break;
		case GPRS_TASK_CREATE_SOCKET_1:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			tcpIpConn->socketStatus = SOCKET_DISCONNECTED;
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				if(Domain_IpCheck(tcpIpConn->ip) == IS_IP)
				{
					ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 15000, 0, 
							"AT+USOCO=%d,\"%s\",%d\r",gprsSocketMask[tcpIpConn->socketId],tcpIpConn->ip,tcpIpConn->port);
					gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET_3;
				}
				else
				{
					DNS_serverIp[0] = 0;
					ModemCmdTask_SendCmd(GPRS_GetIpFromDNS,"+UDNSRN: \"",100, modemOk, modemError, 60000, 0, "AT+UDNSRN=0,\"%s\"\r", tcpIpConn->ip);
					gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET_2;
				}
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				if(tcpIpConn->tryNum++ >= 3)
				{
					GsmTask_Init();
					gprs_TaskPhase = GPRS_TASK_IDLE;
					tcpIpConn->error = CONN_OPEN_SOCKET_FAIL;
					tcpIpConn->cmd = CONN_CMD_EMPTY;
					break;
				}
				COMM_Putc(0x1B);
				gprs_TaskPhase = GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET;
			}
		break;
		case GPRS_TASK_CREATE_SOCKET_2:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 15000, 0,"AT+USOCO=%d,\"%s\",%d\r",gprsSocketMask[tcpIpConn->socketId],DNS_serverIp,tcpIpConn->port); 
					gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET_3;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				gprsDisable = 1;
				tcpIpConn = list_item_next(tcpIpConn);
				gprs_TaskPhase = GPRS_TASK_IDLE;
			}
		break;
		case GPRS_TASK_CREATE_SOCKET_3:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				tcpIpConn->tryNum = 1;
				gprsSocketStatus[tcpIpConn->socketId] = SOCKET_OPEN;
				tcpIpConn->socketStatus = SOCKET_CONNECTED;
				tcpIpConn->cmd = CONN_CMD_EMPTY;
				gprs_TaskPhase = GPRS_TASK_IDLE;
				tcpIpConn = list_item_next(tcpIpConn);
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				if(tcpIpConn->tryNum != 0)
				{
					tcpIpConn->tryNum--;
					tcpIpConn = list_item_next(tcpIpConn);
					gprs_TaskPhase = GPRS_TASK_IDLE;
					break;
				}
				else
				{
					tcpIpConn->error = CONN_CREATE_TCP_CONNECTION_FAIL;
					tcpIpConn->cmd = CONN_CMD_EMPTY;
					tcpIpConn = list_item_next(tcpIpConn);
					gprs_TaskPhase = GPRS_TASK_IDLE;
					break;
				}
				COMM_Putc(0x1B);
				gprs_TaskPhase = GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET;
			}
			break;
		case GPRS_TASK_READ_DATA:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 0, "AT+USORD=%d,%d\r",gprsSocketMask[tcpIpConn->socketId],TCP_IP_BUF_SIZE-32) == MODEM_RES_OK)
			{
				tcpIpConn->cmd = CONN_CMD_EMPTY;
				gprs_TaskPhase = GPRS_TASK_GET_DATA;
			}
			break;
		case GPRS_TASK_GET_DATA:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
					gprs_TaskPhase = GPRS_TASK_IDLE;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
					
					if(tcpIpConn->tryNum != 0)
					{
						tcpIpConn->tryNum--;
						tcpIpConn = list_item_next(tcpIpConn);
						gprs_TaskPhase = GPRS_TASK_IDLE;
					}
					else
					{
						tcpIpConn->error = CONN_CREATE_TCP_CONNECTION_FAIL;
						tcpIpConn->socketStatus = SOCKET_DISCONNECTED;
						tcpIpConn->cmd = CONN_CMD_EMPTY;
						tcpIpConn = list_item_next(tcpIpConn);
						gprs_TaskPhase = GPRS_TASK_IDLE;
						break;
					}
			}
			break;
		case GPRS_TASK_SEND_DATA:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			if(ModemCmdTask_SendCmd(NULL,NULL,500, "@", modemError, 5000, 0, "AT+USOWR=%d,%d\r", gprsSocketMask[tcpIpConn->socketId], tcpIpConn->dataOutLen) == MODEM_RES_OK)
			{
				gprs_TaskPhase = GPRS_TASK_SEND_OUT_DATA;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				gprs_TaskPhase = GPRS_TASK_SEND_OUT_DATA;
			}
		break;
		case GPRS_TASK_SEND_OUT_DATA:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
					for(i = 0;i < tcpIpConn->dataOutLen;i ++)
					{
						COMM_Putc(tcpIpConn->dataOutPt[i]);
					}
					ModemCmdTask_SendCmd(NULL,NULL,100,modemOk, modemError, 2000, 0,NULL);
					gprs_TaskPhase = GPRS_TASK_CHECK_DATA_SENT;
					gprsCheckDataCnt = 0;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
					COMM_Putc(0x1A);
					tcpIpBuf.flag = TCPIP_NO_DATA;
					tcpIpConn->cmd  = CONN_CMD_EMPTY;
					gprs_TaskPhase = GPRS_TASK_IDLE;
			}
		break;
		case GPRS_TASK_CHECK_DATA_SENT:	
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				ModemCmdTask_SendCmd(GPRS_TcpSocketControl,"+USOCTL: ",100, modemOk, modemError, 1000, 0, "AT+USOCTL=%d,11\r",gprsSocketMask[tcpIpConn->socketId]);
				InitTimeout(&tGprsTaskPhaseTimeOut, SYSTICK_TIME_SEC(1));
				gprs_TaskPhase = GPRS_TASK_CHECK_DATA_SENT1;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				tcpIpBuf.flag = TCPIP_NO_DATA;
				tcpIpConn->cmd  = CONN_CMD_EMPTY;
				gprs_TaskPhase = GPRS_TASK_IDLE;
			}
		break;
		case GPRS_TASK_CHECK_DATA_SENT1:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				if(GPRS_dataUnackLength == 0)
				{
					tcpIpBuf.flag = TCPIP_NO_DATA;
					tcpIpConn->cmd  = CONN_CMD_EMPTY;
					tcpIpConn->error = CONN_NO_ERROR;
					tcpIpConn->sentDataFlag = 1;
					gprs_TaskPhase = GPRS_TASK_IDLE;
				}
				else if(GPRS_dataUnackLength >= 2000)
				{
						gprsDisable = 1;
						tcpIpConn = list_item_next(tcpIpConn);
						gprs_TaskPhase = GPRS_TASK_IDLE;
				}
				else
				{
					if(CheckTimeout(&tGprsTaskPhaseTimeOut) == SYSTICK_TIMEOUT)
					{
						gprs_TaskPhase = GPRS_TASK_CHECK_DATA_SENT;
						if(gprsCheckDataCnt++ > 3)
						{
							tcpIpConn->error = CONN_SEND_DATA_FAIL;
							tcpIpBuf.flag = TCPIP_NO_DATA;
							tcpIpConn->cmd  = CONN_CMD_EMPTY;
							gprsCheckDataCnt = 0;
							gprs_TaskPhase = GPRS_TASK_IDLE;
						}
					}
				}
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				tcpIpConn->error = CONN_SEND_DATA_FAIL;
				tcpIpBuf.flag = TCPIP_NO_DATA;
				tcpIpConn->cmd  = CONN_CMD_EMPTY;
				gprs_TaskPhase = GPRS_TASK_IDLE;
			}
		break;
			
		case GPRS_TASK_CLOSE_CONNECTION:
			if(tcpIpConn ==  NULL){gprs_TaskPhase = GPRS_TASK_IDLE; break;}
			gprsSocketStatus[tcpIpConn->socketId] = SOCKET_CLOSE;
			GPRS_CheckSocketFree(tcpIpConn->socketId);
			createSocketNow = tcpIpConn->socketId;
			if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 3000, 0, "AT+USOCL=%d\r", gprsSocketMask[tcpIpConn->socketId]) == MODEM_RES_OK)
			{
				tcpIpConn->socketStatus = SOCKET_DISCONNECTED;
				gprs_TaskPhase = GPRS_TASK_WAIT_CONNECTION_CLOSE;
			}
		break;
		case GPRS_TASK_WAIT_CONNECTION_CLOSE:
			if(ModemCmdTask_IsIdle())
			{
				tcpIpConn->cmd = CONN_CMD_EMPTY;
				gprs_TaskPhase = GPRS_TASK_IDLE;
			}
		break;
		
		case GPRS_TASK_IDLE:
			
			if(gprsEnableFlag == 0)
			{
				memset(gprsSocketStatus,0x00,sizeof(gprsSocketStatus));
				memset(gprsSocketMask,0x00,sizeof(gprsSocketMask));
				tcpIpBuf.flag = TCPIP_NO_DATA;
			}
			for(i = 0;i < GPRS_MAX_SOCKET_NUM;i++)
			{
				if(tcpIpBuf.dataUnReadLen[i] && tcpIpBuf.flag == TCPIP_NO_DATA)
				{
					tcpIpConn = list_head(gprsConnList);
					for(j = 0; j < GPRS_MAX_SOCKET_NUM;j++)
					{
						if(tcpIpConn != NULL)
							if(tcpIpConn->socketId == i )
							{
								if(tcpIpConn->socketStatus == SOCKET_DISCONNECTED)
								{
									tcpIpBuf.dataUnReadLen[i] = 0;
								}
								else
									break;
							}
						tcpIpConn = list_item_next(tcpIpConn);
					}
					if(tcpIpConn != NULL)
						gprs_TaskPhase = GPRS_TASK_READ_DATA;
					break;
				}
				else if(tcpIpBuf.flag == TCPIP_NEW_DATA_RECV && i == tcpIpBuf.socket)
				{
					tcpIpConn = list_head(gprsConnList);
					for(j = 0; j < GPRS_MAX_SOCKET_NUM;j++)
					{
						if(tcpIpConn != NULL)
							if(tcpIpConn->socketId == i)
							{
								if(tcpIpConn->socketStatus == SOCKET_DISCONNECTED)
								{
									tcpIpBuf.flag = TCPIP_NO_DATA;
								}
								else
									break;
							}
						tcpIpConn = list_item_next(tcpIpConn);
					}
					if(tcpIpConn != NULL)
					{
						if(tcpIpConn->echo)
						{
							tcpIpBuf.flag = TCPIP_NO_DATA;
							tcpIpConn->newDataFlag = 0;
							tcpIpConn->dataOutPt = tcpIpBuf.data;
							tcpIpConn->dataOutLen = tcpIpBuf.len;
							gprs_TaskPhase = GPRS_TASK_SEND_DATA;
							tcpIpBuf.len = 0;
							break;
						}
						else
							tcpIpConn->newDataFlag = 1;
					}
				}
			}
			if(gprs_TaskPhase != GPRS_TASK_IDLE)
				break;
			if(tcpIpConn != NULL)
			{
				
				if(tcpIpConn->cmd == CONN_CMD_EMPTY)
				{
					tcpIpBuf.flag = TCPIP_NO_DATA;
				}
				if(gprs_TaskPhase == GPRS_TASK_IDLE)
				{
					if(tcpIpConn->socketStatus == SOCKET_CONNECTED)
					{
						if(gprsSocketStatus[tcpIpConn->socketId] == SOCKET_CLOSE)
							tcpIpConn->socketStatus = SOCKET_DISCONNECTED;
					}
					if(tcpIpConn->socketStatus == SOCKET_CONNECTED)
					{
						if(tcpIpBuf.flag == TCPIP_NO_DATA && CheckTimeout(&tcpIpConn->periodDataRecvCheck) == SYSTICK_TIMEOUT && tcpIpBuf.dataUnReadLen[tcpIpConn->socketId] == 0)
						{
							InitTimeout(&tcpIpConn->periodDataRecvCheck,tcpIpConn->periodDataRecvValue); 
							gprs_TaskPhase = GPRS_TASK_READ_DATA;
							break;
						}
					}
					else
					{
						InitTimeout(&tcpIpConn->periodDataRecvCheck,tcpIpConn->periodDataRecvValue); 
					}
					
					switch(tcpIpConn->cmd)
					{
						case CONN_CMD_START:
							if(tcpIpConn->tryNum)
							{
								tryOpenSocketNum = 0;
								gprsCheckDataCnt = 0;
								socketTryToConnectCnt = 0;
								tryToCreateSocketCnt = 0;
								gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET;
								if(gprsEnableFlag == 0)
								{
									gprsEnableCmd = 1;
								}
							}
							else
								tcpIpConn->cmd = CONN_CMD_EMPTY;
							break;
						case CONN_CMD_STOP:
							break;
						case CONN_SEND_DATA:
							gprs_TaskPhase = GPRS_TASK_SEND_DATA;
							
							break;
						case CONN_READ_DATA:
							gprs_TaskPhase = GPRS_TASK_READ_DATA;
							break;
						case CONN_CMD_PAUSE:
							break;
						case CONN_CMD_EMPTY:
							break;
						case CONN_CMD_CLOSE:
							gprs_TaskPhase = GPRS_TASK_CLOSE_CONNECTION;
							break;
						default:
							break;
					}
					
					
					if(gprs_TaskPhase == GPRS_TASK_IDLE)
					{
						tcpIpConn = list_item_next(tcpIpConn);
					}
				}
			}
			else
			{
				tcpIpConn = list_head(gprsConnList);
			}
			break;
		default:
			// re-init modem
			gprs_TaskPhase = GPRS_TASK_IDLE;
			break;
	}
	if(gprs_TaskPhase != GPRS_TASK_IDLE)
	{
		modemIsBusy = 1;
		modemIsBusySource = AT_U_BLOX_GPRS_TASK;
	}
	else if(modemIsBusy && modemIsBusySource == AT_U_BLOX_GPRS_TASK)
	{
		modemIsBusy = 0;
	}
	if(gprs_TaskPhaseOld != GPRS_TASK_IDLE && gprs_TaskPhase == GPRS_TASK_IDLE)
		modemIsBusy = 0;
	if((atCmdCallParser.incomingCall || VoiceCallTask_GetPhase() != CALL_IDLE) && modemIsBusy)
	{
		modemIsBusy = 0;
		
		memset(gprsSocketStatus,NULL,sizeof(gprsSocketStatus));
		memset(gprsSocketMask,NULL,sizeof(gprsSocketMask));
		tcpIpBuf.flag = TCPIP_NO_DATA;
		tcpIpBuf.socket = 0;
		tcpIpBuf.len = 0;
		
		gprs_TaskPhase = GPRS_TASK_IDLE;
		
	}
}

