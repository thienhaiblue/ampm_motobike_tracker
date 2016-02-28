#ifndef _AT_GSM_TASK_H_
#define _AT_GSM_TASK_H_

#include <string.h>
#include <stdio.h>
#include "lib/ringbuf.h"
#include "at_command_parser.h"

typedef enum{
	MODEM_TURN_ON = 0,
	MODEM_SEND_AT,
	MODEM_CONFIG_0,
	MODEM_CONFIG_1,
	MODEM_REGISTRATION_NETWORK,
	MODEM_WAIT_REGISTRATION_NETWORK,
	MODEM_SYS_COVERAGE_OK,
} GSM_PHASE_TYPE;


extern uint8_t enableGprsRetryCnt;
extern uint8_t gprsEnableFlag;
extern uint8_t gprsDisable;
extern uint8_t gprsEnableCmd;

void GsmTask_InitApn(uint8_t *apn,uint8_t *usr,uint8_t *pwd);
void GsmTask_Init(void);
GSM_PHASE_TYPE GsmTask_GetPhase(void);
void GsmTask_SetPhase(GSM_PHASE_TYPE phase);
void Gsm_Task(void);
void MODEM_Sleep(void);
void MODEM_Wakeup(void);

#endif
