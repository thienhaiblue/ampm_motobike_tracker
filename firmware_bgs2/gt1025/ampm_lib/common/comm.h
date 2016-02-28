
#ifndef __COMM_H__
#define __COMM_H__
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "lib/ringbuf.h"


extern uint32_t  DbgCfgPrintf(const uint8_t *format, ...);

#define PPP_Info(...)		DbgCfgPrintf(__VA_ARGS__)
#define PPP_Debug(...)	DbgCfgPrintf(__VA_ARGS__)
#define AT_CMD_Debug(...)	DbgCfgPrintf(__VA_ARGS__)
#define UIP_PPP_Info(...) DbgCfgPrintf(__VA_ARGS__)
#define AMPM_GSM_LIB_DBG(...) DbgCfgPrintf(__VA_ARGS__)
#define MODEM_Info(...)		//PrintfDebug(__VA_ARGS__)

extern RINGBUF USART1_TxRingBuff;
extern RINGBUF USART1_RxRingBuff;
extern uint8_t USART1_PutChar(uint8_t ch);
extern void USART1_PutString (uint8_t *s);
extern uint8_t MODEM_Dial(void);
extern uint8_t MODEM_CarrierDetected(void);
extern void MODEM_GoToCmdMode(void);

void COMM_Putc(uint8_t c);
void COMM_Puts(uint8_t *s);
extern RINGBUF *commTxRingBuf;
extern RINGBUF *commRxRingBuf;
void COMM_Open(uint32_t baudrate);
uint8_t COMM_Dial(void);
uint8_t COMM_CarrierDetected(void);
uint8_t COMM_ModemIsOnline(void);
uint32_t COMM_ModemSetOffline(void);
void COMM_GoToCmdMode(void);
uint8_t COMM_pppIsOpened(void);
void COMM_pppSetOpened(void);
void COMM_pppClrOpened(void);

#endif
