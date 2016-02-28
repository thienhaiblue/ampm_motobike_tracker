#ifndef __AT_CMD_SMS_PARSER_H__
#define __AT_CMD_SMS_PARSER_H__

#include <stdint.h>
#include "stdio.h"
#include "string.h"
#include "lib/ringbuf.h"

#define CMD_FROM_SMS	0
#define CMD_FROM_TCPIP	1

#define	SMS_CHECK_TIMEOUT			300
#define SMS_PHONE_LENGTH			16
#define SMS_MAX_NUMBER				10
#define SMS_DATA_MAX					159
#define SMS_SEND_TIMEOUT			10000 	// 10 sec
#define SMS_RESEND_CNT_MAX		3 			// resend 3 time


typedef enum {
	SMS_IDLE = 0,
	SMS_DELAY_START_SEND,
	SMS_START_SEND,
	SMS_WAIT_MODEM_INIT,
	SMS_WAIT_MODEM_ANSWER,
	SMS_WAIT_SEND,
	SMS_SENT,
	SMS_SEND_FAIL,
	SMS_CHECK_NETWORK,
	SMS_PAUSED
} SMS_PHASE_TYPE;

#define SMS_MSG_SENT	0
#define SMS_MSG_FAIL	1
#define SMS_NEW_MSG		2

typedef enum {
	SMS_PDU8_MODE = 0,
	SMS_PDU16_MODE,
	SMS_TEXT_MODE
	
}SMS_MODE_TYPE;

typedef struct{
	struct SMS_LIST_TYPE *next;
	uint8_t 	*phone;
	uint8_t 	*msg; 				//  maximun 159 character
	uint8_t len;
	uint16_t 	timeout; //second
	uint8_t 	tryNum;
	uint8_t flag;
	SMS_MODE_TYPE mode;
}SMS_LIST_TYPE;


extern uint8_t smsNewMessageFlag;
extern uint8_t smsSender[];
extern RINGBUF smsUnreadRingBuff;
extern uint8_t flagSmsReadFinish;
extern uint8_t smsDataBuf[256];

uint8_t Modem_IsRinging(void);
void RING_Process(void);
void RING_ProcessReset(void);
void AT_CmdSmsParserInit(void);
uint32_t SMS_Process(uint16_t cnt,uint8_t c);
uint32_t SMS_URC_Incomming(uint16_t cnt,uint8_t c);
uint32_t SMS_NewMessage(uint16_t cnt,uint8_t c);
uint32_t SMS_ReadMsg(uint16_t cnt,uint8_t c);
uint32_t SMS_NewMessage(uint16_t cnt,uint8_t c);
uint32_t SMS_ModemOk(uint16_t cnt,uint8_t c);

uint8_t Sms_Task(void);
void SmsTask_Init(void);
void SmsAddMsg(SMS_LIST_TYPE *msg);
uint8_t SmsCheckList_IsEmpty(void);
void SmsTask_SendSms(void);
uint8_t SmsTask_IsIdle(void);
SMS_PHASE_TYPE SmsTask_GetPhase(void);
uint8_t Sms_Task(void);
uint16_t Sms_SendMsg(SMS_LIST_TYPE *SMS,uint8_t *phone,uint8_t *msg,uint8_t len,SMS_MODE_TYPE mode,uint16_t timeout,uint8_t tryNum);


uint8_t SmsCheckMessage_IsEmpty(void);

uint8_t SmsTask_IsCanceled(void);
uint8_t SmsTask_IsNoMsg(void);

void SmsTask_CancelAll(void);


#endif
