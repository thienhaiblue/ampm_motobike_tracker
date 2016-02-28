#include "at_command_parser.h"
#include "modem.h"
#include "lib/sparser.h"
#include "lib/list.h"
#include "lib/sys_tick.h"
#include "at_command_parser.h"
#include "at_gsm_task.h"
#include "at_send_cmd_task.h"



#define CALL_PICKUP_BUF_LEN	6

//LIST(callList);

AT_CMD_CALL_TYPE atCmdCallParser;
VOICE_CALL_PHASE_TYPE voiceCall_TaskPhase = CALL_IDLE,voiceCall_OldTaskPhase = CALL_IDLE;
Timeout_Type tCallRingCheckTimeOut;
Timeout_Type tVoiceCallTaskTimeOut;
Timeout_Type tCallTaskDelay;
Timeout_Type tStartCallErrorTimeOut;
Timeout_Type tWaitCallActionTimeOut;
Timeout_Type tVoiceCallTaskTimeout;
uint8_t dialingPhoneNumber[16];
const char callPickUpBuf[6] = ";\r\r\nOK";
char pickUpPos = 0;
uint8_t callSentATH = 0;

void AT_CmdCallParserInit(void)
{
	AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# AT_CmdCallParserInit();\r\n");
	//VoiceCallTask_Init();
}



void VoiceCallTask_Init(void)
{
	memset((char *)&atCmdCallParser,NULL,sizeof(AT_CMD_CALL_TYPE));
	atCmdCallParser.incommingCallTalkingTimeout = 60;//60s
	voiceCall_TaskPhase = CALL_IDLE;
}


void VoiceCallTask_StartCall(char* phoneNo, uint16_t calloutTimeout)
{
	if (voiceCall_TaskPhase == CALL_IDLE)
 	{
		if ((phoneNo != NULL) && (phoneNo[0] != 0))
		{
			strcpy((char*)dialingPhoneNumber, phoneNo);
			atCmdCallParser.startCallRetry = 0;
			atCmdCallParser.startCall = 1;
			atCmdCallParser.cancelCall = 0;
			atCmdCallParser.answerCall = 0;
			atCmdCallParser.extendedErrorCode = 0;
			atCmdCallParser.error = CALL_RESPONSE_NO_ERROR;
			atCmdCallParser.calloutTalkingTimeout = calloutTimeout;
			InitTimeout(&tWaitCallActionTimeOut, SYSTICK_TIME_SEC(30));
		}
	}
}

void VoiceCallTask_AnswerCall(void)
{
	if (voiceCall_TaskPhase == INCOMING_CALL)
	{
		atCmdCallParser.answerCall = 1;
		atCmdCallParser.calloutTalkingTimeout = 1800;
		InitTimeout(&tWaitCallActionTimeOut, SYSTICK_TIME_SEC(30));
	}
}

void VoiceCallTask_EndCall(void)
{
	atCmdCallParser.cancelCall = 1;
}

uint8_t VoiceCallTask_GetCallerPhoneNo(void)
{
	return 0;
}

uint8_t VoiceCallTask_IsModemBusy(void)
{
	if ((voiceCall_TaskPhase == INCOMING_CALL) || (voiceCall_TaskPhase == IN_CALLING) ||
			(voiceCall_TaskPhase == WAIT_START_CALL) || (voiceCall_TaskPhase == WAIT_ANSWER_CALL))
	{
		return 1;
	}
	
	return 0;
}

VOICE_CALL_PHASE_TYPE VoiceCallTask_GetPhase(void)
{
	return voiceCall_TaskPhase;
}

void VoiceCallTask_ClearEndCallStatus(void)
{
	switch (voiceCall_TaskPhase)
	{
		case END_CALL:
		case MISS_CALL:
		case BUSY_CALL:
		case NO_ANSWER_CALL:
		case UNSUCCESS_CALL:
			voiceCall_TaskPhase = CALL_IDLE;
			break;
	}
}

void VoiceCallTask_ClearCallAction(void)
{
	VoiceCallTask_ClearEndCallStatus();
}


uint8_t VoiceCall_Task(void)
{
	if(GsmTask_GetPhase() != MODEM_SYS_COVERAGE_OK)
	{
		atCmdCallParser.incomingCall = 0;
		voiceCall_TaskPhase = CALL_IDLE;
		VoiceCallTask_Init();
		return 0;
	}
	if(modemIsBusy == 1 && modemIsBusySource != AT_CMD_CALL_PARSER)
		return 0;
	if(CheckTimeout(&tVoiceCallTaskTimeout) == SYSTICK_TIMEOUT)
		VoiceCallTask_Init();
	switch (voiceCall_TaskPhase)
	{
		case CALL_IDLE:
			InitTimeout(&tVoiceCallTaskTimeout,SYSTICK_TIME_SEC(30*60));
			if (atCmdCallParser.incomingCall)
			{
				InitTimeout(&tCallTaskDelay,SYSTICK_TIME_MS(100));
				InitTimeout(&tCallRingCheckTimeOut, SYSTICK_TIME_SEC(10));
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(60));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# INCOMING_CALL\r\n");
				voiceCall_TaskPhase = INCOMING_CALL;
			}
			else if (atCmdCallParser.startCall)
			{
				if (ModemCmdTask_IsIdle())
				{
					atCmdCallParser.startCall = 0;
					atCmdCallParser.startCallStatus = 0;
					InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(5));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# SETUP_START_CALL\r\n");
					voiceCall_TaskPhase = SETUP_START_CALL;
				}
				else if (CheckTimeout(&tWaitCallActionTimeOut) == SYSTICK_TIMEOUT)
				{
					atCmdCallParser.startCall = 0;
					InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# UNSUCCESS_CALL\r\n");
					voiceCall_TaskPhase = UNSUCCESS_CALL;
				}
				else if (CheckTimeout(&tCallTaskDelay) == SYSTICK_TIMEOUT)
				{
					InitTimeout(&tCallTaskDelay,SYSTICK_TIME_MS(100));
					COMM_Putc(0x1B);
				}
			}
			else if(atCmdCallParser.getIncomingNumber)
			{
				atCmdCallParser.getIncomingNumber = 0;
			}
				
			break;
			
		case SETUP_START_CALL:
			if (VoiceSetup() == 0)
			{
				atCmdCallParser.startCallRetry = 0;
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# START_CALL\r\n");
				voiceCall_TaskPhase = START_CALL;
			}
			else if (CheckTimeout(&tVoiceCallTaskTimeOut))
			{
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# UNSUCCESS_CALL\r\n");
				voiceCall_TaskPhase = UNSUCCESS_CALL;
			}
			else
			{
				if (atCmdCallParser.startCallRetry++ >= 3)
				{
					atCmdCallParser.startCallRetry = 0;
					InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# UNSUCCESS_CALL\r\n");
					voiceCall_TaskPhase = UNSUCCESS_CALL;
				}
			}
			break;
			
		case START_CALL:
			COMM_Putc(0x1A);
			ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 3000, 0, "ATD%s;\r", dialingPhoneNumber);
			InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(3));
			AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# WAIT_START_CALL\r\n");
			voiceCall_TaskPhase = WAIT_START_CALL;
			break;
			
		case WAIT_START_CALL:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				if (atCmdCallParser.cancelCall)
				{
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# HANG_UP_CALL\r\n");
					voiceCall_TaskPhase = HANG_UP_CALL;
				}
				else
				{
					atCmdCallParser.incomingCall = 0;
					InitTimeout(&tStartCallErrorTimeOut, SYSTICK_TIME_SEC(10));
					InitTimeout(&tVoiceCallTaskTimeOut,SYSTICK_TIME_SEC(atCmdCallParser.calloutTalkingTimeout));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# IN_CALLING\r\n");
					voiceCall_TaskPhase = IN_CALLING;
				}
			}
			else if ((CheckTimeout(&tVoiceCallTaskTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				if (atCmdCallParser.cancelCall)
				{
					InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# END_CALL\r\n");
					voiceCall_TaskPhase = END_CALL;
				}
				else
				{
					if (atCmdCallParser.startCallRetry++ < 3)
					{
						AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# START_CALL\r\n");
						voiceCall_TaskPhase = START_CALL;
					}
					else
					{
						atCmdCallParser.startCallRetry = 0;
						InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
						AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# UNSUCCESS_CALL\r\n");
						voiceCall_TaskPhase = UNSUCCESS_CALL;
					}
				}
			}
			break;
			
		case INCOMING_CALL:
			if (atCmdCallParser.missCall)
			{
				atCmdCallParser.missCall = 0;
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# MISS_CALL\r\n");
				voiceCall_TaskPhase = MISS_CALL;
				break;
			}
			if(atCmdCallParser.cancelCall)
			{
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# HANG_UP_CALL\r\n");
				voiceCall_TaskPhase = HANG_UP_CALL;
			}
			else if(atCmdCallParser.autoAnswer || atCmdCallParser.answerCall)
			{
				if (atCmdCallParser.getIncomingNumber)
				{
					if (ModemCmdTask_IsIdle())
					{
						atCmdCallParser.answerCall = 0;
						InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(15));
						AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# SETUP_ANSWER_CALL\r\n");
						voiceCall_TaskPhase = SETUP_ANSWER_CALL;
					}
					else if (CheckTimeout(&tCallTaskDelay) == SYSTICK_TIMEOUT)
					{
						InitTimeout(&tCallTaskDelay,SYSTICK_TIME_MS(100));
						COMM_Putc(0x1B);
					}
				}
			}
			if (CheckTimeout(&tVoiceCallTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# MISS_CALL\r\n");
				voiceCall_TaskPhase = MISS_CALL;
			}
			break;
			
		case SETUP_ANSWER_CALL:
			if (VoiceSetup() == 0)
			{
				atCmdCallParser.answerCallRetry = 0;
				voiceCall_TaskPhase = ANSWER_CALL;
			}
			else if (CheckTimeout(&tVoiceCallTaskTimeOut))
			{
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
				voiceCall_TaskPhase = END_CALL;
			}
			else
			{
				if (atCmdCallParser.answerCallRetry++ >= 3)
				{
					atCmdCallParser.answerCallRetry = 0;
					InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# END_CALL\r\n");
					voiceCall_TaskPhase = END_CALL;
				}
			}
			break;
			
		case ANSWER_CALL:
			COMM_Putc(0x1A);
			if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 3000, 0, "ATA\r") == 0)
			{
				SysTick_DelayMs(2000);
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(3));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# WAIT_ANSWER_CALL\r\n");
				voiceCall_TaskPhase = WAIT_ANSWER_CALL;
			}
			break;
			
		case WAIT_ANSWER_CALL:
			if(atCmdCallParser.missCall)
			{
				atCmdCallParser.missCall = 0;
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# MISS_CALL\r\n");
				voiceCall_TaskPhase = MISS_CALL;
			}
			else if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				if (atCmdCallParser.cancelCall)
				{
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# HANG_UP_CALL\r\n");
					voiceCall_TaskPhase = HANG_UP_CALL;
				}
				else 
				{
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# IN_CALLING\r\n");
					voiceCall_TaskPhase = IN_CALLING;
					InitTimeout(&tVoiceCallTaskTimeOut,SYSTICK_TIME_SEC(atCmdCallParser.incommingCallTalkingTimeout));
				}
			}
			else if ((CheckTimeout(&tVoiceCallTaskTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# UNSUCCESS_CALL\r\n");
				voiceCall_TaskPhase = UNSUCCESS_CALL;
			}
			break;
			
		case IN_CALLING:
			if (atCmdCallParser.cancelCall || (CheckTimeout(&tVoiceCallTaskTimeOut) == SYSTICK_TIMEOUT))
			{
				if (ModemCmdTask_IsIdle())
				{
					atCmdCallParser.cancelCall = 0;
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# HANG_UP_CALL\r\n");
					voiceCall_TaskPhase = HANG_UP_CALL;
				}
			}
			else if (atCmdCallParser.farHangUp)
			{
				atCmdCallParser.farHangUp = 0;
				atCmdCallParser.callUnsuccess = 0;
				atCmdCallParser.extendedErrorCode = 0; 
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 0, "AT+CEER\r");
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_MS(500));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# CHECK_END_CALL\r\n");
				voiceCall_TaskPhase = CHECK_END_CALL;
			}
			else if (atCmdCallParser.busyFlag)
			{
				atCmdCallParser.busyFlag = 0;
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# BUSY_CALL\r\n");
				voiceCall_TaskPhase = BUSY_CALL;
			}
			else if (atCmdCallParser.noAnswerFlag)
			{
				atCmdCallParser.noAnswerFlag = 0;
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# NO_ANSWER_CALL\r\n");
				voiceCall_TaskPhase = NO_ANSWER_CALL;
			}
			else if (atCmdCallParser.callUnsuccess)
			{
				atCmdCallParser.callUnsuccess = 0;
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# UNSUCCESS_CALL\r\n");
				voiceCall_TaskPhase = UNSUCCESS_CALL;
			}
			break;
			
		case HANG_UP_CALL:
			COMM_Putc(0x1A);
			ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, "NO CARRIER", 2000, 2, "ATH\r");
			callSentATH = 1;
			InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(3));
			AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# WAIT_HANG_UP_CALL\r\n");
			voiceCall_TaskPhase = WAIT_HANG_UP_CALL;
			break;
		
		case WAIT_HANG_UP_CALL:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(3));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# ENSURE_HANG_UP_CALL\r\n");
				voiceCall_TaskPhase = ENSURE_HANG_UP_CALL;
			}
			else if ((CheckTimeout(&tVoiceCallTaskTimeOut) == SYSTICK_TIMEOUT) || (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				if (atCmdCallParser.hangUpCallRetry++ < 5)
				{
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# HANG_UP_CALL\r\n");
					voiceCall_TaskPhase = HANG_UP_CALL;
				}
				else
				{
					atCmdCallParser.hangUpCallRetry = 0;
					InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# END_CALL\r\n");
					voiceCall_TaskPhase = END_CALL;
				}
			}
			break;
			
		case ENSURE_HANG_UP_CALL:
			if (CheckTimeout(&tVoiceCallTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# END_CALL\r\n");
				voiceCall_TaskPhase = END_CALL;
			}
			break;
			
		case CHECK_END_CALL:
			if ((ModemCmdTask_GetPhase() == MODEM_CMD_OK) || (CheckTimeout(&tVoiceCallTaskTimeOut) == SYSTICK_TIMEOUT))
			{
				if (atCmdCallParser.callUnsuccess)
				{
					atCmdCallParser.callUnsuccess = 0;
					if ((atCmdCallParser.extendedErrorCode == 290) && (CheckTimeout(&tStartCallErrorTimeOut) != SYSTICK_TIMEOUT))
					{
						if (atCmdCallParser.startCallRetry++ < 3)
						{
							AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# START_CALL\r\n");
							voiceCall_TaskPhase = START_CALL;
						}
						else
						{
							atCmdCallParser.startCallRetry = 0;
							InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
							AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# UNSUCCESS_CALL\r\n");
							voiceCall_TaskPhase = UNSUCCESS_CALL;
						}
					}
					else
					{
						InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
						AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# UNSUCCESS_CALL\r\n");
						voiceCall_TaskPhase = UNSUCCESS_CALL;
					}
				}
				else
				{
					InitTimeout(&tVoiceCallTaskTimeOut, SYSTICK_TIME_SEC(10));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# END_CALL\r\n");
					voiceCall_TaskPhase = END_CALL;
				}
			}
			break;
			
		case END_CALL:
			atCmdCallParser.error = CALL_RESPONSE_NO_ERROR;
			voiceCall_TaskPhase = END_CALL_PHASE;
		break;
		case MISS_CALL:
			atCmdCallParser.error = CALL_RESPONSE_MISS;
			voiceCall_TaskPhase = END_CALL_PHASE;
		break;
		case BUSY_CALL:
			atCmdCallParser.error = CALL_RESPONSE_BUSY;
			voiceCall_TaskPhase = END_CALL_PHASE;
		break;
		case NO_ANSWER_CALL:
			atCmdCallParser.error = CALL_RESPONSE_NO_ANSWER;

			voiceCall_TaskPhase = END_CALL_PHASE;
		break;
		case UNSUCCESS_CALL:
			atCmdCallParser.error = CALL_RESPONSE_UNSUCCESS;	
			voiceCall_TaskPhase = END_CALL_PHASE;
		break;
		case END_CALL_PHASE:
			AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# END_CALL_PHASE\r\n");
			AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#CALL# CALL_IDLE\r\n");
			voiceCall_TaskPhase = CALL_IDLE;
			atCmdCallParser.getIncomingNumber = 0;
			atCmdCallParser.cancelCall = 0;
			atCmdCallParser.incomingCall = 0;
			atCmdCallParser.getIncomingNumber = 0;
		break;
	}
	if(voiceCall_TaskPhase != CALL_IDLE)
	{
		modemIsBusy = 1;
		modemIsBusySource = AT_CMD_CALL_PARSER;
	}
	else if(modemIsBusy && modemIsBusySource == AT_CMD_CALL_PARSER)
	{
		modemIsBusy = 0;
	}
	if(voiceCall_OldTaskPhase != SMS_IDLE && voiceCall_TaskPhase == CALL_IDLE)
		modemIsBusy = 0;
	return 0;
}

uint8_t Modem_IsRinging(void)
{
	if(atCmdCallParser.ringingFlag 
	||  atCmdCallParser.ringingDetectFlag
	||  atCmdCallParser.ringingCountDown
	)
		return 1;
	return 0;
}

void RING_ProcessReset(void)
{
	atCmdCallParser.ringingFlag = 0;
	atCmdCallParser.ringingCountDown = 0;
	atCmdCallParser.ringingDetectFlag = 0;
	atCmdCallParser.missCall = 0;
	atCmdCallParser.missCall = 0;
	atCmdCallParser.incomingCall = 0;
}

void RING_Process(void)
{
	static uint8_t cnt = 0;
	if(atCmdCallParser.ringingFlag && atCmdCallParser.ringingDetectFlag == 0)
	{
			cnt = 0;
		 COMM_GoToCmdMode();
		 atCmdCallParser.ringingFlag = 0;
		 atCmdCallParser.ringingCountDown = 5;
		 atCmdCallParser.ringingDetectFlag = 1;
		 atCmdCallParser.missCall = 0;
	}
	if( atCmdCallParser.ringingDetectFlag)
	{
		if( atCmdCallParser.ringingCountDown) 
		{
			 atCmdCallParser.ringingCountDown--;
		}
		else
		{
			atCmdCallParser.ringingDetectFlag = 0;
			if( atCmdCallParser.ringingFlag)
			{
				 cnt = 0;
				 atCmdCallParser.incomingCall = 1;
			}
			else if(atCmdCallParser.incomingCall)
			{
				 atCmdCallParser.missCall = 1;
				 atCmdCallParser.incomingCall = 0;
			}
			else
			{
				smsNewMessageFlag = 1;
				atCmdCallParser.missCall = 0;
				atCmdCallParser.incomingCall = 0;
			}
		}
	}
	if(atCmdCallParser.incomingCall)
	{
		if(cnt++ > 10)
		{
			cnt = 0;
			atCmdCallParser.missCall = 0;
			atCmdCallParser.incomingCall = 0;
		}
	}
}


uint32_t CallOutProcess(uint16_t cnt,uint8_t c)
{
	switch(cnt)
	{
		case 0:
			pickUpPos = 0;
			 atCmdCallParser.pickUpCallFlag = 0;
			AT_CmdParser.timeoutSet = 120000;// 2min
		break;
		default:
			if(pickUpPos)
			{
				if(c == callPickUpBuf[pickUpPos])
				{
					pickUpPos++;
					if(pickUpPos >= CALL_PICKUP_BUF_LEN)
					{
						 atCmdCallParser.pickUpCallFlag = 1;
						return 0;
					}
				}
				else
				{
					 atCmdCallParser.pickUpCallFlag = 0;
					pickUpPos = 0;
					//CallCancel(c);
					return 0;
				}
			}
			else
			{
				if(c == ';')
					pickUpPos = 1;
				else if(c < '0' || c > '9')
				{
					 atCmdCallParser.pickUpCallFlag = 0;
					//CallCancel(c);
					return 0;
				}
			}
		break;
	}
	return 0xff;
}


uint32_t CallCancel(uint16_t cnt,uint8_t c)
{
	//COMM_ModemSetOffline();
	//COMM_GoToCmdMode();
	 atCmdCallParser.farHangUp = 1;
	RING_ProcessReset();
	modem_IsOnline = 0;
	return 0;
}


uint32_t CallErrorReport(uint16_t cnt,uint8_t c)
{
	static uint16_t errorCode = 0;
	
	if(cnt >= 4) 
	{
		errorCode = 0;
		 atCmdCallParser.callUnsuccess = 1;
		return 0;
	}
	else if (cnt == 0)
	{
		errorCode = 0;
	}
	if((c >= '0') && (c <= '9'))
	{
		errorCode *= 10;
		errorCode += c - '0';
	}
	else if (c == ',')
	{
		if (errorCode == 0)
		{
			errorCode = 9999;
		}
		if ( atCmdCallParser.extendedErrorCode == 0)		// check only the first received code
		{
			 atCmdCallParser.extendedErrorCode = errorCode;
			if (errorCode != 16)
			{
				 atCmdCallParser.callUnsuccess = 1;
			}
		}
		return 0;
	}
	else
	{
		errorCode = 0;
		 atCmdCallParser.callUnsuccess = 1;
		return 0;
	}
	
	return 0xff;
}

uint32_t CallNoAnswer(uint16_t cnt,uint8_t c)
{
	 atCmdCallParser.noAnswerFlag = 1;
	return 0;
}

uint32_t CallBusy(uint16_t cnt,uint8_t c)
{
	 atCmdCallParser.busyFlag = 1;
 return 0;
}


/*
+CLIP: "0978779222",161,,,,0
*/
uint32_t GetCallerNumber(uint16_t cnt,uint8_t c)
{
	atCmdCallParser.incomingCall = 1;
	atCmdCallParser.ringingFlag = 1;
	atCmdCallParser.ringingCountDown = 5;
	atCmdCallParser.ringingDetectFlag = 0;
	atCmdCallParser.missCall = 0;
	if((c >= '0') && (c <= '9'))
	{
		 atCmdCallParser.incomingCallPhoneNo[cnt] = c;
		 atCmdCallParser.incomingCallPhoneNo[cnt+1] = '\0';
	}
	else 
	{
		if(c == '"' && cnt >= 2)
		{
			atCmdCallParser.getIncomingNumber = 1;
			atCmdCallParser.ringingFlag = 1;
			atCmdCallParser.ringingCountDown = 5;
			atCmdCallParser.ringingDetectFlag = 0;
			atCmdCallParser.missCall = 0;
		}
		return 0;
	}
 return 0xff;
}

uint32_t RingCallProcess(uint16_t cnt,uint8_t c)
{
	if( atCmdCallParser.ringingFlag == 0)
	{
		 atCmdCallParser.ringingFlag = 1;
	}
	return 0;
}

uint8_t ComparePhoneNumber(char* phone1, char* phone2)
{
	uint8_t i = strlen(phone1);
	uint8_t j = strlen(phone2);
	uint8_t l1;
	uint8_t l2;
	uint8_t minL;
	uint8_t count = 0;
	
	l1 = i;
	if (phone1[0] == '+')
	{
		if (i > 4)
		{
			l1 = i - 4;
		}
	}
	else if (phone1[0] == '0')
	{
		if (i > 1)
		{
			l1 = i - 1;
		}
	}
	
	l2 = j;
	if (phone2[0] == '+')
	{
		if (j > 4)
		{
			l2 = j - 4;
		}
	}
	else if (phone2[0] == '0')
	{
		if (j > 1)
		{
			l2 = j - 1;
		}
	}
	
	minL = l1;
	if (l2 < l1)
	{
		minL = l2;
	}
	
	while ((i != 0) && (j != 0))
	{
		i--;
		j--;
		if (phone1[i] == phone2[j])
		{
			count++;
		}
		else
		{
			break;
		}
	}
	
	if ((count >= PHONE_DIGIT_THRESHOLD) && (count >= minL))
	{
		return SAME_NUMBER;
	}
	
	return DIFFERENT_NUMBER;
}


