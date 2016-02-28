#ifndef _FIRMWARE_TASK_H_
#define _FIRMWARE_TASK_H_

#include "typedef.h"
#include "uip.h"
#include "resolver.h"
#include "lib/sys_tick.h"
#include "tcp_ip_task.h"


extern struct uip_conn *fwConn;
extern TCP_STATE_TYPE FIRMWARE_Manage(void);
extern void FIRMWARE_Reset(void);
extern void FIRMWARE_Callback(void);
extern void FIRMWARE_Init(uint32_t priority);
void FIRMWARE_Task(void);
uint8_t FirmwareTask_IsBusy(void);

#endif
