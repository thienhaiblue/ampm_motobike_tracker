#ifndef _REPORTER_H_
#define _REPORTER_H_

#include "typedef.h"
#include "uip.h"
#include "resolver.h"
#include "lib/sys_tick.h"
#include "tcp_ip_task.h"

extern RINGBUF rpRxBufRing,rpTxBufRing; 
extern uint32_t gprsDataSending;
extern struct uip_conn *rpConn;

extern TCP_STATE_TYPE REPORTER_Manage(void);
extern void REPORTER_Reset(void);
extern void REPORTER_Callback(void);
extern void REPORTER_Init(uint32_t priority);
extern void vReporterTask(void *pvParameters);

#endif
