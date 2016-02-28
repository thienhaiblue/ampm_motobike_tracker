#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "modem.h"
#include "tick.h"
#include "lib/data_cmp.h"
#include "at_command_parser.h"
#include "at_send_cmd_task.h"
#include "at_cinterion_gprs_task.h"
#include "at_gsm_task.h"
#include "lib/list.h"

LIST(gprsConnList);

const AT_CMD_PACKET_TYPE atGprsCmdStart[] = {
	"at^sics=0,conType,GPRS0\r",NULL,NULL,"OK","ERROR",1,1000,100,
	"at^sics=0,apn,\"internet\"\r",NULL,NULL,"OK","ERROR",1,1000,100,
	//"at^scfg=tcp/withurcs,off\r",NULL,NULL,"OK","ERROR",1,1000,100
};


AT_CMD_LIST_TYPE atGprsCmdListStart[sizeof(atGprsCmdStart)/sizeof(AT_CMD_PACKET_TYPE)];



TCPIP_CONN_TYPE	*tcpIpConn;
GPRS_PHASE_TYPE gprs_TaskPhase;
Timeout_Type tGprsTaskPhaseTimeOut;

uint8_t newDataInSocket = 0;
uint8_t newDataInSocketFlag = 0;
uint8_t closedSocket = 0;
uint8_t closedSocketFlag = 0;

uint32_t GPRS_NewRecvData(uint16_t cnt,uint8_t c)
{
	static uint8_t flag = 0;
	if(gprs_TaskPhase == GPRS_TASK_READ_DATA || gprs_TaskPhase == GPRS_TASK_GET_DATA)
		return 0;
 switch(cnt)
	{
		case 0:
			if(c >= '0' && c <= '9')
			{
				newDataInSocket = c - '0';
				closedSocket = newDataInSocket;
				newDataInSocketFlag = 0;
				flag = 0;
			}
			else
				return 0;
		break;
		case 1:
			if(c != ',')
				return 0;
		break;
		case 2:
			if(c != ' ')
				return 0;
		break;
		case 3:
			if(c == '1')
				flag = 1;
			else if(c == '2')
			{
				flag = 0;
				closedSocketFlag = 1;
			}
			else if(c == '-')
				break;
			else
				return 0;
		break;
		case 4:
			if(flag && c == '\r')
			{
				flag = 0;
				newDataInSocketFlag = 1;
				return 0;
			}
			else
				newDataInSocketFlag = 0;
			if(c == '2')
				closedSocketFlag = 1;
			flag = 0;
		return 0;
		break;
	}
 return 0xff;
}

uint8_t gprsCmdResBuf[32];
uint8_t gprsDataBuf[800];
uint16_t gprsDataLen = 0;
uint8_t gprsDataSocket = 0;
uint8_t gprsDataRecvFlag = 0;
uint32_t GPRS_CinterionGetData(uint16_t cnt,uint8_t c)
{
	static uint8_t gprsGetDataFlag = 0;
	static uint16_t dataLen = 0;
	if(gprsGetDataFlag)
	{
		gprsDataBuf[dataLen] = c;
		dataLen++;
		if(dataLen >= gprsDataLen)
		{
			gprsDataRecvFlag = 1;
			gprsGetDataFlag = 0;
			return 0;
		}
	}
	switch(cnt)
	{
		case 0:
			gprsGetDataFlag = 0;
			if(c >= '0' && c <= '9')
				gprsDataSocket = c - '0';
			break;
		case 1:
			if(c != ',')
				return 0;
			break;
		case 2:
			if(c != ' ')
				return 0;
			gprsDataLen = 0;
			break;
		case 3:
			if(c >= '1' && c <= '9')
			{
				gprsDataLen *= 10;
				gprsDataLen += c - '0';
			}
			else if(c == '-')
			{
				closedSocketFlag = 1;
				closedSocket = gprsDataSocket;
				return 0;
			}
			else
				return 0;
			break;
		case 4:
			if(c >= '0' && c <= '9')
			{
				gprsDataLen *= 10;
				gprsDataLen += c - '0';
			}
			else if(c != 0x0D)
				return 0;
			break;
		case 5:
			if(c >= '0' && c <= '9')
			{
				gprsDataLen *= 10;
				gprsDataLen += c - '0';
			}
			else if(c != 0x0D && c != 0x0A)
				return 0;
			else if(c == 0x0A)
			{
				gprsGetDataFlag = 1;
				dataLen = 0;
			}
			break;
			case 6:
			if(gprsGetDataFlag)
				break;
			if(c >= '0' && c <= '9')
			{
				gprsDataLen *= 10;
				gprsDataLen += c - '0';
			}
			else if(c != 0x0D && c != 0x0A)
				return 0;
			else if(c == 0x0A)
			{
				gprsGetDataFlag = 1;
				dataLen = 0;
			}
			break;
		default:
		break;
	}
	return 0xff;
}

void GprsTask_Init(void)
{
	list_init(gprsConnList);
	gprs_TaskPhase = GPRS_TASK_INIT;
}

void GprsTask_CreateTcpIpConnection(TCPIP_CONN_TYPE *conn)
{
	list_add(gprsConnList,conn);
}

void GprsTask_ConnInit(TCPIP_CONN_TYPE *conn,uint32_t ip,uint16_t port,uint8_t socketNum,uint16_t periodDataRecvCheck)
{
	memset(conn,0,sizeof(TCPIP_CONN_TYPE));
	memcpy(&conn->ip,(uint8_t *)&ip,4);
	conn->port = port;
	conn->cmd = CONN_CMD_START;
	conn->socketStatus = SOCKET_DISCONNECTED;
	conn->socketId =  socketNum;
	conn->periodDataRecvValue = periodDataRecvCheck;
	InitTimeout(&conn->periodDataRecvCheck,conn->periodDataRecvValue); 
	GprsTask_CreateTcpIpConnection(conn);
}



GPRS_PHASE_TYPE GprsTask_GetPhase(void)
{
	return gprs_TaskPhase;
}

void Gprs_Task(void)
{	
	uint16_t i;
	if(GsmTask_GetPhase() ==  MODEM_TURN_ON)
	{
		gprs_TaskPhase = GPRS_TASK_INIT;
		return;
	}
	if(GsmTask_GetPhase() != MODEM_SYS_COVERAGE_OK)
	{
		return;
	}
	switch(gprs_TaskPhase)
	{
		case GPRS_TASK_INIT:
			if(GsmTask_GetPhase() == MODEM_SYS_COVERAGE_OK)
			{
				for(i = 0;i < sizeof(atGprsCmdStart)/sizeof(AT_CMD_PACKET_TYPE);i++)
				{
					atGprsCmdListStart[i].pt = (AT_CMD_PACKET_TYPE *)&atGprsCmdStart[i];
					ModemCmdAdd(&atGprsCmdListStart[i]);
				}
				InitTimeout(&tGprsTaskPhaseTimeOut, TIME_SEC(3));
				gprs_TaskPhase = GPRS_TASK_START;
			}
		break;
		case GPRS_TASK_START:
			if(ModemCmdCheckList_IsEmpty() && (ModemCmdTask_GetPhase() == MODEM_CMD_OK))
			{
				gprs_TaskPhase = GPRS_TASK_IDLE;
			}
			else if (CheckTimeout(&tGprsTaskPhaseTimeOut) == TIMEOUT || ModemCmdTask_GetPhase() == MODEM_CMD_ERROR) 
			{
				GsmTask_Init();
				gprs_TaskPhase = GPRS_TASK_INIT;
			}
		break;
		case GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET:
			if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 1, "at^sisc=%d\r",tcpIpConn->socketId) == MODEM_RES_OK)
			{
				tcpIpConn->socketStatus = SOCKET_DISCONNECTED;
				gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				gprs_TaskPhase = GPRS_TASK_IDLE;
			}
			break;
		case GPRS_TASK_CREATE_SOCKET:
			if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 1, "at^siss=%d,srvType,socket\r",tcpIpConn->socketId) == MODEM_RES_OK)
			{
				gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET_1;
			}
		break;
		case GPRS_TASK_CREATE_SOCKET_1:
			tcpIpConn->socketStatus = SOCKET_DISCONNECTED;
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 1, "at^siss=%d,conId,0\r",tcpIpConn->socketId) == MODEM_RES_OK)
				{
					gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET_2;
				}
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				gprs_TaskPhase = GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET;
			}
		break;
		case GPRS_TASK_CREATE_SOCKET_2:
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 1, "at^siss=%d,address,\"socktcp://%d.%d.%d.%d:%d\"\r",
					tcpIpConn->socketId,tcpIpConn->ip[0],
															tcpIpConn->ip[1],
															tcpIpConn->ip[2],
															tcpIpConn->ip[3],tcpIpConn->port) == MODEM_RES_OK)
				{
					gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET_3;
				}
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				gprs_TaskPhase = GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET;
			}
		break;
		case GPRS_TASK_CREATE_SOCKET_3:
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				sprintf((char *)gprsCmdResBuf,"^SISW: %d, 1\r\n",tcpIpConn->socketId);
				if(ModemCmdTask_SendCmd(NULL,NULL,100, gprsCmdResBuf, modemError, 10000, 1, "at^siso=%d\r",tcpIpConn->socketId) == MODEM_RES_OK)
				{
					tcpIpConn->socketStatus = SOCKET_CONNECTING;
					gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET_4;
				}
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				gprs_TaskPhase = GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET;
			}
			break;
			
		case GPRS_TASK_CREATE_SOCKET_4:
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				tcpIpConn->socketStatus = SOCKET_CONNECTED;
				tcpIpConn->cmd = CONN_CMD_EMPTY;
				gprs_TaskPhase = GPRS_TASK_IDLE;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				gprs_TaskPhase = GPRS_TASK_CLOSE_BEFORE_CREATE_SOCKET;
			}
			break;
		case GPRS_TASK_READ_DATA:
			if(ModemCmdTask_SendCmd(GPRS_CinterionGetData,"^SISR: ",100, modemOk, modemError, 1000, 0, "at^sisr=%d,780\r",tcpIpConn->socketId) == MODEM_RES_OK)
			{
				newDataInSocketFlag = 0;
				tcpIpConn->cmd = CONN_CMD_EMPTY;
				gprs_TaskPhase = GPRS_TASK_GET_DATA;
			}
			break;
		case GPRS_TASK_GET_DATA:
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
					gprs_TaskPhase = GPRS_TASK_IDLE;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
					gprs_TaskPhase = GPRS_TASK_IDLE;
			}
			break;
		case GPRS_TASK_SEND_DATA: /*^SISW: 1,*/
			if(ModemCmdTask_IsIdle())
			{
				sprintf((char *)gprsCmdResBuf,"^SISW: %d, %d",tcpIpConn->socketId,tcpIpConn->dataOutLen);
				if(ModemCmdTask_SendCmd(NULL,NULL,100,gprsCmdResBuf, modemError, 1000, 0, "at^sisw=1,%d\r",tcpIpConn->dataOutLen) == MODEM_RES_OK)
				{
					tcpIpConn->cmd = CONN_CMD_EMPTY;
					gprs_TaskPhase = GPRS_TASK_SEND_OUT_DATA;
				}
			}
		break;
		case GPRS_TASK_SEND_OUT_DATA:	
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
					ModemCmdTask_SendCmd(NULL,NULL,100,modemOk, modemError, 1000, 0,NULL);
					for(i = 0;i < tcpIpConn->dataOutLen;i ++)
					{
						COMM_Putc(tcpIpConn->dataOutPt[i]);
					}
					gprs_TaskPhase = GPRS_TASK_CHECK_DATA_SENT;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				 gprs_TaskPhase = GPRS_TASK_IDLE;
			}
		break;
		case GPRS_TASK_CHECK_DATA_SENT:	
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
					gprs_TaskPhase = GPRS_TASK_IDLE;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				gprs_TaskPhase = GPRS_TASK_IDLE;
			}
		break;
		case GPRS_TASK_IDLE:
			if(tcpIpConn != NULL)
			{
				switch(tcpIpConn->cmd)
				{
					case CONN_CMD_START:
						gprs_TaskPhase = GPRS_TASK_CREATE_SOCKET;
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
					default:
						break;
				}
				
				if(gprs_TaskPhase == GPRS_TASK_IDLE)
				{
					if(tcpIpConn->socketStatus == SOCKET_CONNECTED)
					{
						if(gprsDataRecvFlag && tcpIpConn->socketId == gprsDataSocket)
						{
							if(tcpIpConn->echo)
							{
									gprsDataRecvFlag = 0;
									tcpIpConn->newDataFlag = 0;
									tcpIpConn->dataOutPt = gprsDataBuf;
									tcpIpConn->dataOutLen = gprsDataLen;
									tcpIpConn->cmd = CONN_SEND_DATA;
							}
							else
								tcpIpConn->newDataFlag = 1;
					}
					if(newDataInSocketFlag && tcpIpConn->socketId == newDataInSocket)
					{
						newDataInSocketFlag = 0;
						gprs_TaskPhase = GPRS_TASK_READ_DATA;
					}
					if(closedSocketFlag && tcpIpConn->socketId == closedSocket)
					{
						closedSocketFlag = 0;
						tcpIpConn->socketStatus = SOCKET_DISCONNECTED;
						tcpIpConn->cmd = CONN_CMD_START;
					}
					}
					else
					{
						InitTimeout(&tcpIpConn->periodDataRecvCheck,tcpIpConn->periodDataRecvValue); 
					}
					
					if(CheckTimeout(&tcpIpConn->periodDataRecvCheck) == TIMEOUT)
					{
						InitTimeout(&tcpIpConn->periodDataRecvCheck,tcpIpConn->periodDataRecvValue); 
						gprs_TaskPhase = GPRS_TASK_READ_DATA;
					}
					else if(gprs_TaskPhase == GPRS_TASK_IDLE)
						tcpIpConn = list_item_next(tcpIpConn);
				}
			}
			else
			{
				tcpIpConn = list_head(gprsConnList);
			}
			break;
		default:
			// re-init modem
			gprs_TaskPhase = GPRS_TASK_START;
			break;
	}
}

