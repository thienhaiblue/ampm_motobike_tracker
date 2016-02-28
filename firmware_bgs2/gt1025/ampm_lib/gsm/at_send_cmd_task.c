/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lib/sys_tick.h"
#include "modem.h"
#include "lib/data_cmp.h"
#include "at_command_parser.h"
#include "at_send_cmd_task.h"
#include "at_gsm_task.h"
#include "lib/list.h"


LIST(atCmdList);

MODEM_CMD_PHASE_TYPE modemCmd_TaskPhase = MODEM_CMD_OK;
uint8_t  modemAtCmdBuf[128];
COMPARE_TYPE modemCmpOk, modemCmpFails;
Timeout_Type tModemAtTimeout;
Timeout_Type tModemCancelTimeout;
Timeout_Type tModemIsBusyTimeOut;

uint8_t cmdTimeoutCount = 0;
uint8_t modemCancel = 0;
uint8_t criticalCmd = 0;
AT_CMD_PACKET_TYPE cmdPckSend;
STR_PARSER_Type atCallBackParser;
uint8_t cmdParsedChar[1] =  {0};
STR_INFO_Type atProcessCmd[1];
uint8_t modemIsBusy = 0;
uint8_t modemIsBusySource = 0;

uint8_t ModeBusy_IsLongTime(void)
{
	if(modemIsBusy)
	{
		if(CheckTimeout(&tModemIsBusyTimeOut) == SYSTICK_TIMEOUT)
		{
				InitTimeout(&tModemIsBusyTimeOut,SYSTICK_TIME_SEC(30*30));
				return 1;
		}
	}
	else
	{
		InitTimeout(&tModemIsBusyTimeOut,SYSTICK_TIME_SEC(30*30));
	}
	return 0;
}

void ModemCmdAdd(AT_CMD_LIST_TYPE *cmd)
{
	list_add(atCmdList, cmd);
}

uint8_t ModemCmdCheckList_IsEmpty(void)
{
	if(atCmdList[0] == NULL)
		return 1;
	return 0;
}

void ModemCmdTask_SetCriticalCmd(void)
{
	if (modemCmd_TaskPhase == MODEM_CMD_SEND)
	{
		criticalCmd = 1;
	}
}

uint8_t ModemCmdTask_SendCmd(uint32_t (*callback)(uint16_t cnt,uint8_t c),uint8_t *response,
	uint32_t delay, const uint8_t *resOk, const uint8_t *resFails, 
	uint32_t timeout, uint8_t tryAgainNum, const uint8_t *format, ...)
{
	
	#if defined ( __ICCARM__ )
        va_list     vArgs;
        #elif defined (__CC_ARM)
        __va_list     vArgs;
        #endif
	if(ModemCmdTask_IsIdle() && (criticalCmd == 0))
	{
		if(format != NULL)
		{
			AT_CMD_Debug("\r\n\r\nat_send: ");
			AT_CMD_Debug(format);
			va_start(vArgs, format);
			vsprintf((char *)&modemAtCmdBuf[0], (char const *)format, vArgs);
			va_end(vArgs);
		}
		else
		{
			modemAtCmdBuf[0] = 0;
		}
		cmdPckSend.response = response;
		cmdPckSend.callback = callback;
		if(cmdPckSend.callback != NULL && cmdPckSend.response != NULL)
		{
			StrParserInit(&atCallBackParser,cmdParsedChar,1,1024);
			atCallBackParser.timeoutSet = timeout;
			atProcessCmd[0].callback  = cmdPckSend.callback;
			atProcessCmd[0].command  = cmdPckSend.response;
		}
		cmdPckSend.resOk = (uint8_t *)resOk;
		cmdPckSend.resFails = (uint8_t *)resFails;
		InitFindData(&modemCmpOk, cmdPckSend.resOk);
		InitFindData(&modemCmpFails, cmdPckSend.resFails);
		cmdPckSend.delay = delay;
		InitTimeout(&tModemAtTimeout, SYSTICK_TIME_MS(cmdPckSend.delay));
		cmdPckSend.timeout = timeout;
		cmdPckSend.retryNum  = tryAgainNum;
		modemCmd_TaskPhase = MODEM_CMD_SEND;
		MODEM_FlushBuffer();
		
		return MODEM_RES_OK;
	}
	
	return MODEM_RES_FAILS;
}



MODEM_CMD_PHASE_TYPE ModemCmdTask_GetPhase(void)
{
	return modemCmd_TaskPhase;
}

uint8_t ModemCmdTask_IsIdle(void)
{
	switch(modemCmd_TaskPhase)
	{
		case MODEM_CMD_ERROR:
		case MODEM_CMD_OK:
			return 1;
		default:
			return 0;
	}
}

void ModemCmdTask_Reset(void)
{
	modemCmd_TaskPhase = MODEM_CMD_OK;
}

void ModemCmdTask_SetWait(const uint8_t *resOk, const uint8_t *resFails, uint32_t timeout)
{
	if(ModemCmdTask_IsIdle())
	{
		cmdPckSend.resOk = (uint8_t *)resOk;
		cmdPckSend.resFails = (uint8_t *)resFails;
		InitFindData(&modemCmpOk, cmdPckSend.resOk);
		InitFindData(&modemCmpFails, cmdPckSend.resFails);
		cmdPckSend.retryNum = 0;
		InitTimeout(&tModemAtTimeout, SYSTICK_TIME_MS(timeout));
		modemCmd_TaskPhase = MODEM_CMD_WAIT;
	}
}

void ModemCmdTask_Cancel(void)
{
	modemCancel = 1;
	InitTimeout(&tModemCancelTimeout, SYSTICK_TIME_SEC(10));
	while ((modemCmd_TaskPhase == MODEM_CMD_WAIT) && (CheckTimeout(&tModemCancelTimeout) != SYSTICK_TIMEOUT))
	{
		COMM_Putc(0x1B);
		ModemCmd_Task();
	}
	modemCancel = 0;
}

void ModemCmd_Task(void)
{
	uint8_t c;
	AT_CMD_LIST_TYPE *cmdList;
// 	if ((cmdRetry >= 30) || (CheckTimeout(&tModemAtTimeout) >= 60000))
// 	{
// 		cmdRetry = 0;
// 		modemCmd_TaskPhase = MODEM_CMD_ERROR;
// 	}
	
	switch(modemCmd_TaskPhase)
	{
		case MODEM_CMD_SEND:
			//if ((criticalCmd == 0) /*|| (atCmdCallParser.ringingDetectFlag == 0)) && (atCmdCallParser.ringingFlag == 0)*/)
			{
				if(CheckTimeout(&tModemAtTimeout) == SYSTICK_TIMEOUT)
				{
					if (modemAtCmdBuf[0])
					{
						MODEM_FlushBuffer();
						COMM_Puts(modemAtCmdBuf);
					}
//					if (criticalCmd)
//					{
//						InitTimeout(&tModemAtTimeout, SYSTICK_TIME_SEC(CRITICAL_CMD_TIME_OUT));
//					}
//					else
					{
						InitTimeout(&tModemAtTimeout, SYSTICK_TIME_MS(cmdPckSend.timeout));
					}
					modemCmd_TaskPhase = MODEM_CMD_WAIT;
					AT_CMD_Debug("\r\n\r\nat_recv: ");
				}
			}
//			else
//			{
//				modemCmd_TaskPhase = MODEM_CMD_ERROR;
//			}
			break;
			
		case MODEM_CMD_WAIT:
			if(CheckTimeout(&tModemAtTimeout) == SYSTICK_TIMEOUT)
			{
				if (cmdPckSend.retryNum)
				{
					cmdPckSend.retryNum--;
					InitTimeout(&tModemAtTimeout, SYSTICK_TIME_MS(cmdPckSend.delay));
					modemCmd_TaskPhase = MODEM_CMD_SEND;
				}
				else
				{
					criticalCmd = 0;
					modemCmd_TaskPhase = MODEM_CMD_ERROR;
					if(cmdPckSend.callback != NULL && cmdPckSend.response == NULL)
					{
						cmdPckSend.callback(NULL,NULL);
					}
					if (cmdTimeoutCount++ > 10)
					{
						cmdTimeoutCount = 0;
						GsmTask_Init();
					}
				}
			}
			else
			{			
				while (RINGBUF_Get(commRxRingBuf, &c) == 0)
				{
					AT_CMD_Debug("%c",c);
					if ((c != 0x1A) && (c != 0x1B))
					{
						if(cmdPckSend.callback != NULL && cmdPckSend.response != NULL)
						{
							StrComnandParser(SysTick_Get(),atProcessCmd,&atCallBackParser,cmdParsedChar,c);
						}
						if (FindData(&modemCmpOk, c) == 0)
						{
							criticalCmd = 0;
							cmdTimeoutCount = 0;
							modemCmd_TaskPhase = MODEM_CMD_OK;
						}
						else if (FindData(&modemCmpFails, c) == 0)
						{
							cmdTimeoutCount = 0;
							if (cmdPckSend.retryNum)
							{
								cmdPckSend.retryNum--;
								InitTimeout(&tModemAtTimeout, SYSTICK_TIME_MS(cmdPckSend.delay));
								modemCmd_TaskPhase = MODEM_CMD_SEND;
							}
							else
							{
								criticalCmd = 0;
								if(cmdPckSend.callback != NULL && cmdPckSend.response == NULL)
								{
									cmdPckSend.callback(NULL,NULL);
								}
								modemCmd_TaskPhase = MODEM_CMD_ERROR;
							}
						}
					}
					else if (modemCancel)
					{
						modemCancel = 0;
						modemCmd_TaskPhase = MODEM_CMD_OK;
						break;
					}						
				}
			}
			break;
			
		case MODEM_CMD_ERROR:		
		case MODEM_CMD_OK:
			if(atCmdList[0] != NULL)
			{
				if(modemCmd_TaskPhase != MODEM_CMD_ERROR)
				{
					cmdList = list_pop(atCmdList);
					if(cmdList != NULL)
					{
						cmdPckSend = *cmdList->pt;
						if(cmdPckSend.callback != NULL)
						{
							StrParserInit(&atCallBackParser,cmdParsedChar,1,1024);
							atProcessCmd[0].callback  = cmdPckSend.callback;
							atProcessCmd[0].command  = cmdPckSend.response;
						}
						strcpy((char *)modemAtCmdBuf,(char *)cmdPckSend.command);
						InitFindData(&modemCmpOk, (uint8_t *)cmdPckSend.resOk);
						InitFindData(&modemCmpFails, (uint8_t *)cmdPckSend.resFails);
						InitTimeout(&tModemAtTimeout, SYSTICK_TIME_MS(cmdPckSend.delay));
						modemCmd_TaskPhase = MODEM_CMD_SEND;
						MODEM_FlushBuffer();
					}
				}
				else
					list_init(atCmdList);
			}
			else
			{
				criticalCmd = 0;
			}
			break;
		default:
			modemCmd_TaskPhase = MODEM_CMD_ERROR;
			break;
	}
}
