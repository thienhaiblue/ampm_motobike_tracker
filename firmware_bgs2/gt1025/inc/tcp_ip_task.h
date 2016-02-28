#ifndef	__TCP_IP_TASK__H__
#define __TCP_IP_TASK__H__
#include <stdint.h>

#define PPP_RESET_TIMEOUT	120
typedef enum {
	INITIAL,
	CONNECT,
	CONNECTED,
	LOGGED_IN,
	REPORTING,
	WAIT_TIMEOUT_RECONNECT
}TCP_STATE_TYPE;

extern Timeout_Type tDataIsSending;
extern uint8_t tcpIpTryCnt;
extern void TcpIpInit(uint32_t priority);
extern void vTcpIpTask(void);
extern void UIP_UDPCallback(void);
extern void UIP_TCPCallback(void);
void vTcpIpTaskInit(void);

#endif

