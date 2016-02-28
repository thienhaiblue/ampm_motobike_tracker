#ifndef _AT_U_BLOX_FTP_TASK_H_
#define _AT_U_BLOX_FTP_TASK_H_

#include <string.h>
#include <stdio.h>
#include "lib/ringbuf.h"
#include "lib/sys_tick.h"
#include "at_command_parser.h"

typedef enum{
	FTP_TASK_IDLE = 0,
	FTP_DEFINE_SERVER_NAME,
	FTP_DEFINE_USER,
	FTP_DEFINE_PASS,
	FTP_LOGIN,
	FTP_WAIT_LOGIN,
	FTP_LOGOUT,
	FTP_GET_FILE,
	FTP_WAITING_GET_FILE,
	FTP_SEND_FILE,
	FTP_TASK_FAIL,
	FTP_TASK_OK
} FTP_PHASE_TYPE;


typedef enum{
	FTP_CMD_GET_FILE,
	FTP_CMD_SEND_FILE,
	FTP_CMD_OK,
	FTP_CMD_FAIL,
}FTP_CONN_CMD;

typedef enum{
	FTP_NO_ERROR,
	FTP_CONFIG_FAIL,
	FTP_LOGIN_FAIL,
	FTP_SEND_FILE_FAIL,
	FTP_RECV_FILE_FAIL
}FTP_CONN_ERROR;

typedef struct{
	struct FTP_CONN_TYPE *next;
	uint8_t *serverName;
	uint8_t *user;
	uint8_t *pass;
	uint8_t *dlFileName;
	uint8_t *ulFileName;
	FTP_CONN_CMD cmd;
	FTP_CONN_ERROR error;
	uint8_t tryNum;
}FTP_CONN_TYPE;

void FtpTask_Init(void);
void Ftp_Task(void);
void FtpTask_ConnInit(FTP_CONN_TYPE *ftp,uint8_t *server,uint8_t *user,uint8_t *pass,FTP_CONN_CMD cmd,uint8_t *fileName,uint8_t tryNum);
FTP_PHASE_TYPE FtpTask_GetPhase(void);
#endif
