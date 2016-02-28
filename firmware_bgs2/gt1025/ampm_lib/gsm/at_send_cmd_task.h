#ifndef _AT_SEND_CMD_TASK_H_
#define _AT_SEND_CMD_TASK_H_

#include <string.h>
#include <stdio.h>
#include "lib/ringbuf.h"
#include "modem.h"
#include "at_command_parser.h"


#define AT_CMD_G100_PARSER	1
#define AT_U_BLOX_FTP_TASK	2
#define AT_U_BLOX_GPRS_TASK	3
#define AT_CMD_CALL_PARSER	4
#define AT_CMD_SMS_PARSER		5
#define AT_CMD_COMMON_PARSER		6


#define CRITICAL_CMD_TIME_OUT 120		// 120s



#define GSM_NO_REG_STATUS	0
#define GSM_REG_OK_STATUS 1
#define GSM_GPRS_OK_STATUS	2
#define GSM_CONNECTED_SERVER_STATUS	3

#define MODEM_RES_FAILS 1
#define MODEM_RES_OK		0

typedef enum{
	MODEM_CMD_SEND = 0,
	MODEM_CMD_WAIT,
	MODEM_CMD_ERROR,
	MODEM_CMD_OK
} MODEM_CMD_PHASE_TYPE;


typedef struct{
	uint8_t *command;
	uint8_t *response;
  uint32_t (*callback)(uint16_t cnt,uint8_t c);
	uint8_t *resOk;
	uint8_t *resFails;
	uint8_t retryNum;
	uint16_t timeout;/*in mS*/
	uint16_t delay;/*delay when retry: in mS*/
}AT_CMD_PACKET_TYPE;

typedef struct{
	struct AT_CMD_LIST_TYPE *next;
	AT_CMD_PACKET_TYPE *pt;
}AT_CMD_LIST_TYPE;

extern uint8_t modemIsBusy;
extern uint8_t modemIsBusySource;
extern uint8_t modemCancel;
uint8_t ModeBusy_IsLongTime(void);
uint8_t ModemCmdCheckList_IsEmpty(void);
void ModemCmdAdd(AT_CMD_LIST_TYPE *cmd);
void ModemCmdTask_SetCriticalCmd(void);
uint8_t ModemCmdTask_IsCriticalCmd(void);
MODEM_CMD_PHASE_TYPE ModemCmdTask_GetPhase(void);
uint8_t ModemCmdTask_IsIdle(void);
void ModemCmdTask_Reset(void);
void ModemCmd_Task(void);
void ModemCmdTask_Cancel(void);
uint8_t ModemCmdTask_SendCmd(uint32_t (*callback)(uint16_t cnt,uint8_t c),uint8_t *response,uint32_t delay, const uint8_t *resOk, const uint8_t *resFails, uint32_t timeout, uint8_t tryAgainNum, const uint8_t *format, ...);
void ModemCmdTask_SetWait(const uint8_t *resOk, const uint8_t *resFails, uint32_t timeout);
#endif
