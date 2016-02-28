#ifndef __AT_CMD_CALL_PARSER_H__
#define __AT_CMD_CALL_PARSER_H__
#include <stdint.h>
#include "stdio.h"
#include "string.h"
#include "lib/ringbuf.h"

#define PHONE_NO_LENGTH									16
#define SAME_NUMBER											1
#define DIFFERENT_NUMBER								0
#define PHONE_DIGIT_THRESHOLD						6

typedef enum {
	CALL_IS_IDLE,
	CALL_IS_RINGING,
	CALL_IS_TALKING,
	CALL_FAR_HANGUP,
	CALL_MISS_CALL,
	CALL_UNSUCCESS,
	CALL_SUCCESS
}CALL_STATUS;

typedef enum {
	CALL_NO_ACTION,
	CALL_START,
	CALL_PICKUP,
	CALL_HANGUP,
	CALL_STOP
}CALL_ACTION;



typedef struct{
	struct CALL_TYPE *next;
	uint8_t *phoneNumber;
	uint16_t callingTime;
	uint16_t endTime;
	uint8_t tryNum;
	CALL_STATUS status;
	CALL_ACTION action;
}CALL_TYPE;

typedef enum{
	CALL_RESPONSE_NO_ERROR = 0,
	CALL_RESPONSE_MISS,
	CALL_RESPONSE_BUSY,
	CALL_RESPONSE_NO_ANSWER,
	CALL_RESPONSE_UNSUCCESS,
}CALL_RESPONSE;

typedef struct{
	CALL_RESPONSE error;
	uint8_t ringing;
	uint8_t inCalling;
	uint8_t busyFlag;
	uint8_t noAnswerFlag;
	uint8_t callUnsuccess;
	uint8_t incomingCall;
	uint8_t getIncomingNumber;
	uint8_t farHangUp;
	uint16_t extendedErrorCode;
	uint8_t pickUpCallFlag;
	uint8_t incomingCallPhoneNo[16];
	uint8_t ringingDetectFlag;
	uint8_t ringingCountDown;
	uint8_t ringingFlag;
	uint8_t missCall;
	uint8_t startCall;
	uint8_t autoAnswer;
	uint8_t cancelCall;
	uint8_t answerCall;
	uint8_t answerCallRetry;
	uint8_t hangUpCallRetry;
	uint8_t startCallRetry;
	uint8_t startCallStatus;
	uint16_t calloutTalkingTimeout;
	uint16_t incommingCallTalkingTimeout;
}AT_CMD_CALL_TYPE;

typedef enum {
	CALL_IDLE = 0,
	INCOMING_CALL,
	SETUP_START_CALL,
	START_CALL,
	WAIT_START_CALL,
	SETUP_ANSWER_CALL,
	ANSWER_CALL,
	WAIT_ANSWER_CALL,
	IN_CALLING,
	HANG_UP_CALL,
	WAIT_HANG_UP_CALL,
	ENSURE_HANG_UP_CALL,
	CHECK_END_CALL,
	END_CALL,
	MISS_CALL,
	BUSY_CALL,
	NO_ANSWER_CALL,
	UNSUCCESS_CALL,
	END_CALL_PHASE
} VOICE_CALL_PHASE_TYPE;


extern AT_CMD_CALL_TYPE atCmdCallParser;
extern uint8_t callSentATH;

void AT_CmdCallParserInit(void);
uint32_t GetCallerNumber(uint16_t cnt,uint8_t c);
uint32_t RingCallProcess(uint16_t cnt,uint8_t c);

uint32_t CallBusy(uint16_t cnt,uint8_t c);
uint32_t CallNoAnswer(uint16_t cnt,uint8_t c);
uint32_t CallCancel(uint16_t cnt,uint8_t c);

uint32_t CallUnsuccess(uint16_t cnt,uint8_t c);
uint32_t CallOutProcess(uint16_t cnt,uint8_t c);

uint32_t CallPickUp(uint16_t cnt,uint8_t c);
uint32_t CallErrorReport(uint16_t cnt,uint8_t c);

VOICE_CALL_PHASE_TYPE VoiceCallTask_GetPhase(void);

void VoiceCallTask_StartCall(char* phoneNo, uint16_t calloutTimeout);
uint8_t ComparePhoneNumber(char* phone1, char* phone2);
uint8_t VoiceCall_Task(void);
void VoiceCallTask_Init(void);
void VoiceCallTask_AnswerCall(void);
void VoiceCallTask_EndCall(void);
void VoiceCallTask_Init(void);
VOICE_CALL_PHASE_TYPE VoiceCallTask_GetPhase(void);

#endif
