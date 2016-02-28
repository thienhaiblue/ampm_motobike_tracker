/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "modem.h"
#include "lib/sparser.h"
#include "lib/sys_tick.h"
#include "at_command_parser.h"
#include "at_gsm_task.h"
#include "at_send_cmd_task.h"
#include "lib/list.h"
#include "sms/pdu.h"


#define INFO(...)	//PrintfDebug(__VA_ARGS__)

#define smsScanf	sscanf

extern uint16_t CMD_CfgParse(char *cmdBuff, uint8_t *smsSendBuff, uint32_t smsLenBuf, uint32_t *dataOutLen, uint8_t pwdCheck,uint8_t sourceCmd);

uint32_t GetServiceCenterNumber(uint16_t cnt,uint8_t c);

LIST(smsList);

Timeout_Type tSmsTaskTimeOut;
SMS_LIST_TYPE *smsMsg;

uint8_t smsNewMessageFlag = 0;
uint8_t smsUnreadBuff[32] = {0};
RINGBUF smsUnreadRingBuff;

uint8_t smsSender[16];
uint8_t smsDataBuf[256];
uint8_t pdu_len_except_smsc;
uint8_t pdu_len;

SMS_PHASE_TYPE smsPhase = SMS_IDLE;

uint8_t smsCancelAll = 0;
uint8_t flagSmsReadFinish = 0;
uint8_t serviceCenterNumber[16];
uint8_t getServiceCenterNumber = 0;

void SmsTask_Init(void)
{
	AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS#SmsTask_Init();\r\n");
	list_init(smsList);
	getServiceCenterNumber = 0;
	smsPhase = SMS_IDLE;
}

//Check SMS need to send out
uint8_t SmsCheckMessage_IsEmpty(void)
{
	SMS_LIST_TYPE *smsMsg;
	if(smsPhase != SMS_IDLE)
		return 0;
	smsMsg = list_head(smsList);
	while(smsMsg != NULL)
	{
		if(smsMsg->msg != NULL && smsMsg->phone != NULL)
		{
			return 0;
		}
		smsMsg = list_item_next(smsMsg);
	}
	return 1;
}

void SmsAddMsg(SMS_LIST_TYPE *msg)
{
	list_add(smsList, msg);
}

uint8_t SmsCheckList_IsEmpty(void)
{
	if(smsList[0] == NULL)
		return 1;
	return 0;
}

void SmsTask_SendSms(void)
{
	if (smsPhase == SMS_IDLE)
	{
		smsPhase = SMS_IDLE;
	}
}

uint8_t SmsTask_IsIdle(void)
{
	if ((smsPhase == SMS_IDLE) || (smsPhase == SMS_PAUSED))
	{
		return 1;
	}
	
	return 0;
}

uint8_t SmsTask_IsModemBusy(void)
{
	if ((smsPhase == SMS_WAIT_MODEM_INIT) || (smsPhase == SMS_WAIT_MODEM_ANSWER) ||
			(smsPhase == SMS_WAIT_SEND))
	{
		return 1;
	}
	
	return 0;
}


SMS_PHASE_TYPE SmsTask_GetPhase(void)
{
	return smsPhase;
}

void SmsTask_CancelAll(void)
{
	smsCancelAll = 1;
	//if(smsCancelAll)
	{
		smsPhase = SMS_IDLE;
		smsCancelAll = 0;
		list_init(smsList);
		smsMsg = NULL;
	}
}

uint8_t SmsTask_IsCanceled(void)
{
	if(!smsCancelAll && (list_length(smsList) == 0))
		return 1;
	return 0;
}

uint8_t SmsTask_IsNoMsg(void)
{
	SMS_LIST_TYPE *smsMsg;
	smsMsg = list_head(smsList);
	if(smsPhase != SMS_IDLE)
		return 0;
	while(smsMsg != NULL)
	{
		if(smsMsg->msg != NULL && smsMsg->phone != NULL)
		{
			return 0;
		}
		else
			smsMsg = list_item_next(smsMsg);
	}
	return 1;
}

void SmsTask_Stop(void)
{
	if (SmsTask_IsModemBusy())
	{
		COMM_Putc(0x1B);				// send ESC;
	}
	smsPhase = SMS_IDLE;
}

void SmsTask_Pause(void)
{
		AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_PAUSED\r\n");
		smsPhase = SMS_PAUSED;
}

uint16_t Sms_SendMsg(SMS_LIST_TYPE *SMS,uint8_t *phone,uint8_t *msg,uint8_t len,SMS_MODE_TYPE mode,uint16_t timeout,uint8_t tryNum)
{
	//if(SMS->flag != SMS_NEW_MSG)
	{
		SMS->msg = msg;
		SMS->len = len;
		SMS->phone = phone;
		SMS->timeout = timeout;
		SMS->tryNum = tryNum;
		SMS->flag = SMS_NEW_MSG;
		SMS->mode = mode;
		SmsAddMsg(SMS);
		return 0;
	}
	return 0xff;
}

uint8_t Sms_Task(void)
{
	SMS_PHASE_TYPE smsOldPhase = smsPhase;
	uint8_t buf[8];
	uint16_t i;
	if(GsmTask_GetPhase() != MODEM_SYS_COVERAGE_OK)
		smsPhase = SMS_IDLE;
	if((modemIsBusy == 1 && modemIsBusySource != AT_CMD_SMS_PARSER && VoiceCallTask_GetPhase() != IN_CALLING)) 
		return 0;
	switch(smsPhase)
	{
		case SMS_IDLE:
			if(GsmTask_GetPhase() == MODEM_SYS_COVERAGE_OK)
			{
				if(smsCancelAll)
				{
					smsCancelAll = 0;
					list_init(smsList);
					smsMsg = NULL;
				}
				if(smsMsg != NULL)
				{
					if(smsMsg->msg != NULL && smsMsg->phone != NULL)
					{
						InitTimeout(&tSmsTaskTimeOut, SYSTICK_TIME_SEC(1));
						AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_DELAY_START_SEND\r\n");
						smsPhase = SMS_DELAY_START_SEND;
					}
					else
						smsMsg = list_item_next(smsMsg);
				}
				else
				{
					smsMsg = list_head(smsList);
				}
			}
			break;
			
		case SMS_DELAY_START_SEND:
			if (CheckTimeout(&tSmsTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				if(ModemCmdTask_IsIdle())
				{
					if(smsMsg->mode == SMS_TEXT_MODE)
					{
						ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 1, "AT+CMGF=1\r") ;
					}
					else
					{
						ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 1, "AT+CMGF=0\r") ;
					}
					InitTimeout(&tSmsTaskTimeOut, SYSTICK_TIME_SEC(3));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_START_SEND\r\n");
					smsPhase = SMS_START_SEND;
				}
			}
			break;
		case SMS_START_SEND:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				if(smsMsg->msg != NULL && smsMsg->phone != NULL)
				{
					if(smsMsg->mode != SMS_TEXT_MODE)
					{
						getServiceCenterNumber = 0;
						ModemCmdTask_SendCmd(GetServiceCenterNumber,"+CSCA: \"",100, modemOk, modemError, 5000, 1, "AT+CSCA?\r") ;
					}
					InitTimeout(&tSmsTaskTimeOut, SYSTICK_TIME_MS(500));
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_WAIT_MODEM_INIT\r\n");
					smsPhase = SMS_WAIT_MODEM_INIT;
				}
				else
				{
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_IDLE\r\n");
					smsPhase = SMS_IDLE;
				}
			}
			else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR || CheckTimeout(&tSmsTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				InitTimeout(&tSmsTaskTimeOut, SYSTICK_TIME_SEC(2));
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_IDLE\r\n");
				smsPhase = SMS_IDLE;
			}
			break;
			
		case SMS_WAIT_MODEM_INIT:
			if (CheckTimeout(&tSmsTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
				{
					if((smsMsg->msg[0] != NULL || smsMsg->mode != SMS_TEXT_MODE) && smsMsg->phone[0] != NULL)
					{
						if(smsMsg->mode == SMS_TEXT_MODE)
						{
							COMM_Puts("AT+CMGS=\"");
							COMM_Puts(smsMsg->phone);
							ModemCmdTask_SendCmd(NULL,NULL,100, "\"\r\r\n>", modemError, 5000, 0, "\"\r");
						}
						else
						{
							if(getServiceCenterNumber && serviceCenterNumber[0] != NULL)
							{
								if(smsMsg->mode == SMS_PDU16_MODE)
									pdu_len = pdu16_encode((const char *)serviceCenterNumber,(const char *)smsMsg->phone,( uint16_t *)smsMsg->msg,smsMsg->len,smsDataBuf,sizeof(smsDataBuf));
								else
									pdu_len = pdu_encode((const char *)serviceCenterNumber,(const char *)smsMsg->phone,(const char*)smsMsg->msg,smsDataBuf,sizeof(smsDataBuf));
								if(pdu_len < 0) //"error encoding to PDU"
								{
									smsMsg->phone = NULL;
									smsMsg->msg = NULL;
									smsMsg->flag = SMS_MSG_FAIL;
									smsPhase = SMS_IDLE;
									break;
								}
								pdu_len_except_smsc = pdu_len - 1 - smsDataBuf[0];
								ModemCmdTask_SendCmd(NULL,NULL,500, "\r\r\n> ", modemError, 5000, 0, "AT+CMGS=%d\r",pdu_len_except_smsc);
							}
						}
						AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_WAIT_MODEM_ANSWER\r\n");
						smsPhase = SMS_WAIT_MODEM_ANSWER;
					}
					else
					{
						AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_SENT\r\n");
						smsPhase = SMS_SENT;
					}
				}
				else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
				{
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_SEND_FAIL\r\n");
					smsPhase = SMS_SEND_FAIL;
				}
			}
			break;
				
		case SMS_WAIT_MODEM_ANSWER:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
					if(smsMsg->mode == SMS_TEXT_MODE)
					{
						COMM_Puts(smsMsg->msg);
						COMM_Putc(0x1A); 					// Send Ctl-Z
						ModemCmdTask_SetWait(modemOk, modemError, 10000);
					}
					else
					{
						for (i = 0; i < pdu_len; ++i) 
						{	
							sprintf((char *)buf, "%02X", smsDataBuf[i]);
							COMM_Puts(buf);
						}
						COMM_Putc(0x1A); 					// Send Ctl-Z
						ModemCmdTask_SetWait(modemOk, modemError, 10000);
					}
				smsPhase = SMS_WAIT_SEND;
			}
			else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_SEND_FAIL\r\n");
				COMM_Putc(0x1B);
				COMM_Putc(0x1B);
				smsPhase = SMS_SEND_FAIL;
			}
			break;
		
		case SMS_WAIT_SEND:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_SENT\r\n");
				smsPhase = SMS_SENT;
			}
			else if ((ModemCmdTask_GetPhase() == MODEM_CMD_ERROR))
			{
				COMM_Putc(0x1B);
				smsPhase = SMS_SEND_FAIL;
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_SEND_FAIL\r\n");
				InitTimeout(&tSmsTaskTimeOut, SYSTICK_TIME_SEC(3));
			}
			break;
		case SMS_SENT:
			smsMsg->phone = NULL;
			smsMsg->msg = NULL;
			smsMsg->flag = SMS_MSG_SENT;
			smsPhase = SMS_IDLE;
			AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_IDLE\r\n");
			break;
		case SMS_SEND_FAIL:
			if (CheckTimeout(&tSmsTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				smsMsg->flag = SMS_MSG_FAIL;
				if(smsMsg->tryNum)
				{
					smsMsg->tryNum--;
				}
				else
				{
					smsMsg->phone = NULL;
					smsMsg->msg = NULL;
				}
				if(ModemCmdTask_SendCmd(NULL,NULL,100,"+CREG: 0,1",modemError,1000,3,"AT+CREG?\r") == MODEM_RES_OK)
				{
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_CHECK_NETWORK\r\n");
					smsPhase = SMS_CHECK_NETWORK;
				}
			}
			break;
		case SMS_CHECK_NETWORK:
			if(ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_IDLE\r\n");
				smsPhase = SMS_IDLE;
			}
			else if( ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				if(VoiceCallTask_GetPhase() == IN_CALLING)
				{
					AMPM_GSM_LIB_DBG("\r\nAMPM_LIB:#SMS# SMS_IDLE\r\n");
					smsPhase = SMS_IDLE;
				}
				else
					GsmTask_Init();
			}
			break;
		case SMS_PAUSED:
			break;
	}
	if(smsPhase != SMS_IDLE)
	{
		modemIsBusy = 1;
		modemIsBusySource = AT_CMD_SMS_PARSER;
	}
	else if(modemIsBusy && modemIsBusySource == AT_CMD_SMS_PARSER)
	{
		modemIsBusy = 0;
	}
	if(smsOldPhase != SMS_IDLE && smsPhase == SMS_IDLE)
		modemIsBusy = 0;
	
	return 0;
}






void AT_CmdSmsParserInit(void)
{
	RINGBUF_Init(&smsUnreadRingBuff,smsUnreadBuff,sizeof(smsUnreadBuff));
}


/*+CMGR: "REC READ","+841645715282","","12/07/26,20:50:07+28"
"thienhailue"
*/
uint32_t SMS_NewMessage(uint16_t cnt,uint8_t c)
{
	smsNewMessageFlag = 1;
	return 0;
}

uint32_t SMS_ReadMsg(uint16_t cnt,uint8_t c)
{ 
	static uint8_t comma = 0,getSmsDataFlag = 0;
	static uint8_t *u8pt;
	static uint8_t *u8pt1;
	static uint8_t *u8pt2;
	if(cnt == 0)
	{
		comma = 0;
		getSmsDataFlag = 0;
		u8pt = smsSender;
		u8pt2 = smsDataBuf;
		u8pt1 = (uint8_t *)terminateStr;
		return 0xff;
	}
	if(c == ',') 
	{
		comma++;
	}
	
	if(getSmsDataFlag)
	{
		if(c == *u8pt1)
		{
			u8pt1++;
			if(*u8pt1 == '\0')
			{
				*u8pt2 = 0;
				flagSmsReadFinish = 1;
				return 0;
			}
		}
		else
		{
			u8pt1 = (uint8_t *)terminateStr;
			if(c == *u8pt1) u8pt1++;
		}
		if((u8pt2 - smsDataBuf) >= sizeof(smsDataBuf))
		{		
			*u8pt2 = 0;
			flagSmsReadFinish = 1;
			return 0;
		}
		*u8pt2 = c;
		 u8pt2++;
	}
	else
	{
		switch(comma)
		{
			case 0:
				break;
			case 1:
				if((u8pt - smsSender) >= sizeof(smsSender))
				{
					smsSender[0] = 0;
					return 0;
				}
				if(((c >= '0') && (c <= '9')) || (c == '+'))
				{
					*u8pt = c;
					u8pt++;
					*u8pt = 0;
				}
				break;
			default:
				break;
		}
	}
	if(c == '\n')
	{
		getSmsDataFlag = 1;
	}
	return 0xff;
}


/*
+CSCA: "+886931000099",145
*/
uint32_t GetServiceCenterNumber(uint16_t cnt,uint8_t c)
{
	static uint8_t len = 0;
	if(cnt == 0)
		len = 0;
	if((c >= '0') && (c <= '9') || (c == '+'))
	{
			if(c != '+')
			{
			 serviceCenterNumber[len] = c;
			 len++;
			 serviceCenterNumber[len] = '\0';
			}
	}
	else 
	{
		if(c == '"' && len >= 2)
		{
			getServiceCenterNumber = 1;
		}
		return 0;
	}
 return 0xff;
}
/*+CMTI: "ME",1*/
uint32_t SMS_URC_Incomming(uint16_t cnt,uint8_t c)
{ 
	static uint32_t length = 0;
	switch(cnt)
	{
		case 0:
				if((c >= '0') && (c <= '9'))
				{
					length = c - '0';
					smsNewMessageFlag = 1;
					break;
				}
				return 0;
		case 1:
		case 2:
		case 3:
				if((c >= '0') && (c <= '9'))
				{
					length *= 10;
					length += c - '0';
					break;
				}
				else
				{
					RINGBUF_Put(&smsUnreadRingBuff,(uint8_t)(length & 0x00ff));
					RINGBUF_Put(&smsUnreadRingBuff,(uint8_t)((length >> 8) & 0x00ff));
					return 0;
				}
		default:
			return 0;
	}
	return 0xff;
}


/*   14,"REC READ","+84972046096","","12/07/26,11:10:17+28"   */
/*
AT+CMGL="ALL"
+CMGL: 1,"REC READ","+84978779222",,"14/05/26,08:39:57+28"
*88:88#  
*/
uint32_t SMS_Process(uint16_t cnt,uint8_t c)
{ 
	static uint32_t length = 0;
	switch(cnt)
	{
		case 0:
				smsNewMessageFlag = 1;
				if((c >= '0') && (c <= '9'))
				{
					length = c - '0';
					break;
				}
				return 0;
		case 1:
		case 2:
		case 3:
				if((c >= '0') && (c <= '9'))
				{
					length *= 10;
					length += c - '0';
					break;
				}
				else
				{
					RINGBUF_Put(&smsUnreadRingBuff,(uint8_t)(length & 0x00ff));
					RINGBUF_Put(&smsUnreadRingBuff,(uint8_t)((length >> 8) & 0x00ff));
					return 0;
				}	
		default:
			return 0;
	}
	return 0xff;
}









