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
#include "at_u_blox_ftp_task.h"
#include "at_gsm_task.h"
#include "lib/list.h"

#define DEFAULT_GPSR_APN		"internet"
#define DEFAULT_GPRS_USR		""
#define DEFAULT_GPRS_PWD		""

LIST(ftpConnList);

const AT_CMD_PACKET_TYPE atFtpCmdStart[] = {
	"AT\r",NULL,NULL,"OK","ERROR",1,1000,100
};


AT_CMD_LIST_TYPE atFtpCmdListStart[sizeof(atFtpCmdStart)/sizeof(AT_CMD_PACKET_TYPE)];

FTP_PHASE_TYPE ftp_TaskPhase;

FTP_CONN_TYPE	*ftpConn;

Timeout_Type tFtpTaskPhaseTimeOut;

uint8_t ftpCmdResBuf[32];



//+UUFTPCR: 
uint32_t FTP_UrcControl(uint16_t cnt,uint8_t c)
{
	
	switch(cnt)
	{
		case 0:
			if(c >= '0' && c <= '9')
			{
				return 0;
			}
		break;
	}
	return 0xff;
}



void FtpTask_Init(void)
{
	list_init(ftpConnList);
}
void FtpTask_CreateConnection(FTP_CONN_TYPE *conn)
{
	list_add(ftpConnList,conn);
}

void FtpTask_ConnInit(FTP_CONN_TYPE *ftp,uint8_t *server,uint8_t *user,uint8_t *pass,FTP_CONN_CMD cmd,uint8_t *fileName,uint8_t tryNum)
{
	memset(ftp,0,sizeof(FTP_CONN_TYPE));
	ftp->cmd = cmd;
	ftp->serverName = server;
	ftp->user = user;
	ftp->pass = pass;
	ftp->tryNum = tryNum;
	ftp->dlFileName = fileName;
	ftp->ulFileName = fileName;
	ftp->error = FTP_NO_ERROR;
	FtpTask_CreateConnection(ftp);
}


uint32_t FTP_CmdFails(uint16_t cnt,uint8_t c)
{
	ftp_TaskPhase = FTP_TASK_IDLE;
	return 0;
}

uint32_t FTP_GSM_ReInit(uint16_t cnt,uint8_t c)
{
	gprsDisable = 0;
	gprsEnableFlag = 0;
	ftp_TaskPhase = FTP_TASK_IDLE;
	return 0;
}

FTP_PHASE_TYPE FtpTask_GetPhase(void)
{
	return ftp_TaskPhase;
}



void Ftp_Task(void)
{	
	uint16_t i,j;
	FTP_PHASE_TYPE ftp_TaskPhaseOld = ftp_TaskPhase;
	if(GsmTask_GetPhase() ==  MODEM_TURN_ON)
	{
		ftp_TaskPhase = FTP_TASK_IDLE;
		return;
	}
	if(GsmTask_GetPhase() != MODEM_SYS_COVERAGE_OK)
	{
		return;
	}
	if(modemIsBusy == 1 && modemIsBusySource != AT_U_BLOX_FTP_TASK)
		return;
	switch(ftp_TaskPhase)
	{
		case FTP_DEFINE_SERVER_NAME:
			if (ModemCmdTask_IsIdle())
			{
				if (ftpConn)
				{
					ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 1, "AT+UFTP=1,\"%s\"\r", ftpConn->serverName);
				}
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
				ftp_TaskPhase = FTP_DEFINE_USER;
			}
			else if ((CheckTimeout(&tFtpTaskPhaseTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				ftpConn->error = FTP_CONFIG_FAIL;
				ftp_TaskPhase = FTP_LOGOUT;
			}
		break;
		case FTP_DEFINE_USER:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 1, "AT+UFTP=2,\"%s\"\r", ftpConn->user);
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
				ftp_TaskPhase = FTP_DEFINE_PASS;
			}
			else if ((CheckTimeout(&tFtpTaskPhaseTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				ftpConn->error = FTP_CONFIG_FAIL;
				ftp_TaskPhase = FTP_LOGOUT;
			}
		break;
		case FTP_DEFINE_PASS:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 1, "AT+UFTP=3,\"%s\"\r", ftpConn->pass);
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
				ftp_TaskPhase = FTP_LOGIN;
			}
			else if ((CheckTimeout(&tFtpTaskPhaseTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				ftpConn->error = FTP_CONFIG_FAIL;
				ftp_TaskPhase = FTP_LOGOUT;
			}
		break;
			
		case FTP_LOGIN:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				ModemCmdTask_SendCmd(NULL,NULL,100,"+UUFTPCR: 1,1","+UUFTPCR: 1,0",20000,0, "AT+UFTPC=1\r");
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(20));
				ftp_TaskPhase = FTP_WAIT_LOGIN;
			}
			else if ((CheckTimeout(&tFtpTaskPhaseTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				ftpConn->error = FTP_CONFIG_FAIL;
				ftp_TaskPhase = FTP_LOGOUT;
			}
		break;
		case FTP_WAIT_LOGIN:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
				if(ftpConn->cmd == FTP_CMD_GET_FILE)
					ftp_TaskPhase = FTP_GET_FILE;
				else 
					ftp_TaskPhase = FTP_SEND_FILE;
			}
			else if ((CheckTimeout(&tFtpTaskPhaseTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				ftpConn->error = FTP_LOGIN_FAIL;
				ftp_TaskPhase = FTP_LOGOUT;
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
			}
		break;
		case FTP_LOGOUT:
			if (ModemCmdTask_IsIdle())
			{
				ModemCmdTask_SendCmd(NULL,NULL,100,"+UUFTPCR: 0,1","+UUFTPCR: 0,0",3000,0, "AT+UFTPC=0\r");
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
				ftp_TaskPhase = FTP_TASK_OK;
				if(ftpConn->error != FTP_NO_ERROR)
					ftp_TaskPhase = FTP_TASK_FAIL;
			}
			else if ((CheckTimeout(&tFtpTaskPhaseTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
				ftpConn->error = FTP_CONFIG_FAIL;
				ftp_TaskPhase = FTP_TASK_FAIL;
			}
		break;
		case FTP_GET_FILE:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				ModemCmdTask_SendCmd(NULL,NULL,100,"+UUFTPCR: 4,1","+UUFTPCR: 4,0",60000,0, "AT+UFTPC=4,\"%s\",\"%s\"\r",ftpConn->dlFileName,ftpConn->dlFileName);
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(60));
				ftpConn->error = FTP_NO_ERROR;
				ftp_TaskPhase = FTP_WAITING_GET_FILE;
			}
			else if ((CheckTimeout(&tFtpTaskPhaseTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
				ftpConn->error = FTP_CONFIG_FAIL;
				ftp_TaskPhase = FTP_TASK_FAIL;
			}
		break;
		case FTP_WAITING_GET_FILE:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
				ftp_TaskPhase = FTP_LOGOUT;
			}
			else if ((CheckTimeout(&tFtpTaskPhaseTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				ftpConn->error = FTP_LOGIN_FAIL;
				ftp_TaskPhase = FTP_LOGOUT;
				InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
			}
		break;
		case FTP_SEND_FILE:
			
		break;
		case FTP_TASK_FAIL:
			if(ftpConn->tryNum)
				ftpConn->tryNum--;
			else
			{
				ftpConn->cmd = FTP_CMD_FAIL;
			}
			InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
			ftp_TaskPhase = FTP_TASK_IDLE;
		break;
		case FTP_TASK_OK:
			ftpConn->cmd = FTP_CMD_OK;
			ftp_TaskPhase = FTP_TASK_IDLE;
		break;
		case FTP_TASK_IDLE:
			if(ftpConn != NULL)
			{
				switch(ftpConn->cmd)
				{
						case FTP_CMD_GET_FILE:
							if(gprsEnableFlag == 0)
							{
								gprsEnableCmd = 1;
							}
							else
							{
								ftp_TaskPhase = FTP_DEFINE_SERVER_NAME;
								InitTimeout(&tFtpTaskPhaseTimeOut, SYSTICK_TIME_SEC(10));
							}
							break;
						case FTP_CMD_SEND_FILE:
							if(gprsEnableFlag == 0)
							{
								gprsEnableCmd = 1;
							}
							else
								ftp_TaskPhase = FTP_DEFINE_SERVER_NAME;
							break;
						case FTP_CMD_OK:
							
							break;
						case FTP_CMD_FAIL:

							break;
						default:
							break;
				}
				if(ftp_TaskPhase == FTP_TASK_IDLE)
				{
					ftpConn = list_item_next(ftpConn);
				}
			}
			else
			{
				ftpConn = list_head(ftpConnList);
			}
			break;
		default:
			// re-init modem
			ftp_TaskPhase = FTP_TASK_IDLE;
			break;
	}
	if(ftp_TaskPhase != FTP_TASK_IDLE)
	{
		modemIsBusy = 1;
		modemIsBusySource = AT_U_BLOX_FTP_TASK;
	}
	else if(modemIsBusy && modemIsBusySource == AT_U_BLOX_FTP_TASK)
	{
		modemIsBusy = 0;
	}
	if(ftp_TaskPhaseOld != FTP_TASK_IDLE && ftp_TaskPhase == FTP_TASK_IDLE)
		modemIsBusy = 0;
	if((atCmdCallParser.incomingCall || VoiceCallTask_GetPhase() != CALL_IDLE) && modemIsBusy)
	{
		modemIsBusy = 0;
		ftp_TaskPhase = FTP_TASK_IDLE;
	}
}

