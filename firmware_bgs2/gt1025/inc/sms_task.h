#ifndef __SMS_TASK_H__
#define __SMS_TASK_H__

#include <stdint.h>
#include "stdio.h"
#include "string.h"
#include "lib/ringbuf.h"
#include "at_cmd_sms_parser.h"


extern SMS_LIST_TYPE replySms;

extern const uint8_t smsReplyOk[];

extern uint8_t replySmsBuf[160];
extern uint8_t firmwarePhoneSender[16];
extern uint8_t myAddr[160];
extern uint8_t getAddrFlag;
extern uint8_t gotAddrFlag;
extern uint8_t askAddrCnt;
extern uint8_t addrSendToUserFlag;

void SMS_Manage(uint8_t *buff,uint32_t lengBuf);
void SMS_TaskInit(void);
void RequestTableTask(uint32_t rtc);
uint16_t CMD_CfgParse(char *cmdBuff, uint8_t *smsSendBuff, uint32_t smsLenBuf, uint32_t *dataOutLen, uint8_t pwdCheck,uint8_t sourceCmd);
uint16_t UniConfigParser(uint16_t *in, uint16_t *out);

#endif
