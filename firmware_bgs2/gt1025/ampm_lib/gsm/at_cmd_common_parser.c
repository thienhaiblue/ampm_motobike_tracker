/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "at_command_parser.h"
#include "at_cmd_common_parser.h"
#include "at_gsm_task.h"
#include "at_send_cmd_task.h"
#define INFO(...)	//PrintfDebug(__VA_ARGS__)


uint16_t batteryPercent = 100;

uint8_t gsmSimCIDBuf[32];
uint8_t flagGotSimCID = 0;


uint8_t gsmIMEIBuf[18];
uint8_t flagGotIMEI = 0;
uint8_t newIMEI_IsUpdate = 0;
uint8_t modemIMEIPass = 0;

GSM_DATE_TIME sysGsmTime;
uint8_t gsmGetTimeFlag = 0;
int32_t sysGsmTimezone = 0;

/*

+CBC: 0,46

*/

uint32_t GetBatteryStatus(uint16_t cnt,uint8_t c)
{
	static uint32_t level = 0;
	switch(cnt)
	{
		case 0:
		case 1:
			break;
		default:
			if(cnt >= 6) 
			{
				level = 0;
				return 0;
			}
			if((c >= '0') && (c <= '9'))
			{
				level *= 10;
				level += c - '0';
			}
			else
			{
				if(level > 0 && level < 100)
					batteryPercent = level;
				level = 0;
				return 0;
			}
			break;
	}
	return 0xff;
}




uint32_t GSM_GetIMEI(uint16_t cnt,uint8_t c)
{
	if(cnt >= 18)
		return 0;
	if(c >= '0' && c <= '9')
		gsmIMEIBuf[cnt] = c;
	else
	{
		flagGotIMEI = 1;
		modemIMEIPass = 1;
		return 0;
	}
	return 0xff;
}

uint32_t GSM_GetSimCID(uint16_t cnt,uint8_t c)
{
	if(cnt >= 32)
		return 0;
	if(c >= '0' && c <= '9')
		gsmSimCIDBuf[cnt] = c;
	else
	{
		flagGotSimCID = 1;
		return 0;
	}
	return 0xff;
}

//^SIND: nitz,1,"13/12/11,11:15:41",+28,0
uint32_t GsmGetUTCTime(uint16_t cnt,uint8_t c)
{
	static uint16_t number,timezone_pos = 0;
	if(c < '0' && c > '9' && c != '/' && c != ':' && c != ',' && c != '+')
	{
		return 0;
	}
	switch(cnt)
	{
		case 0:
			number = c - '0';
		break;
		case 1:
			number *= 10;
			number += c - '0';
			sysGsmTime.year = number + 2000;
			if(sysGsmTime.year < 2013)
				return 0;
		break;
		case 3:
			number = c - '0';
		break;
		case 4:
			number *= 10;
			number += c - '0';
			sysGsmTime.month = number;
			if(sysGsmTime.month > 12)
					return 0;
		break;
		case 6:
			number = c - '0';
		break;
		case 7:
			number *= 10;
			number += c - '0';
			sysGsmTime.mday = number;
			if(sysGsmTime.mday > 31)
					return 0;
		break;

		case 9:
			number = c - '0';
		break;
		case 10:
			number *= 10;
			number += c - '0';
			sysGsmTime.hour = number;
			if(sysGsmTime.hour > 24)
				return 0;
		break;
		case 12:
			number = c - '0';
		break;
		case 13:
			number *= 10;
			number += c - '0';
			sysGsmTime.min = number;
			if(sysGsmTime.min > 59)
					return 0;
		break;

		case 15:
			number = c - '0';
		break;
		case 16:
			number *= 10;
			number += c - '0';
			sysGsmTime.sec = number;
			if(sysGsmTime.sec > 59)
					return 0;
		break;
		case 19:
			if(c == '+')
			{
				timezone_pos = 1;
			}
			else
				return 0;
		break;
		case 20:
			number = c - '0';
		break;
		case 21:
			if(c >= '0' && c <= '9')
			{
				number *= 10;
				number += c - '0';
			}
			if(timezone_pos)
				sysGsmTimezone = (int32_t)number*15*60;
			else
				sysGsmTimezone = -((int32_t)number*15*60);
				gsmGetTimeFlag = 1;
			return 0;
		default:
		break;
	}
	return 0xff;
}


//+CCLK: "13/12/11,11:15:41+00
uint32_t GsmGetTime(uint16_t cnt,uint8_t c)
{
	static uint16_t number;
	if(c < '0' && c > '9' && c != '/' && c != ':' && c != ',' && c != '+')
	{
		return 0;
	}
	switch(cnt)
	{
		case 0:
			number = c - '0';
		break;
		case 1:
			number *= 10;
			number += c - '0';
			sysGsmTime.year = number + 2000;
			if(sysGsmTime.year < 2013)
				return 0;
		break;
		case 3:
			number = c - '0';
		break;
		case 4:
			number *= 10;
			number += c - '0';
			sysGsmTime.month = number;
			if(sysGsmTime.month > 12)
					return 0;
		break;
		case 6:
			number = c - '0';
		break;
		case 7:
			number *= 10;
			number += c - '0';
			sysGsmTime.mday = number;
			if(sysGsmTime.mday > 31)
					return 0;
		break;

		case 9:
			number = c - '0';
		break;
		case 10:
			number *= 10;
			number += c - '0';
			sysGsmTime.hour = number;
			if(sysGsmTime.hour > 24)
				return 0;
		break;
		case 12:
			number = c - '0';
		break;
		case 13:
			number *= 10;
			number += c - '0';
			sysGsmTime.min = number;
			if(sysGsmTime.min > 59)
					return 0;
		break;

		case 15:
			number = c - '0';
		break;
		case 16:
			number *= 10;
			number += c - '0';
			sysGsmTime.sec = number;
			if(sysGsmTime.sec > 59)
					return 0;
		break;
		case 17:
			if(c == '+')
			{
				gsmGetTimeFlag = 1;
			}
			return 0;
		default:
		break;
	}
	return 0xff;
}

Timeout_Type tGsmCommonTaskTimeOut;

typedef enum {
	GSM_COMMON_IDLE = 0,
	GSM_COMMON_DIALUP0,
	GSM_COMMON_DIALUP0_1,
	GSM_COMMON_DIALUP0_2,
	GSM_COMMON_DIALUP1,
	GSM_COMMON_DIALUP2,
	GSM_COMMON_DIALUP3,
	GSM_COMMON_DIALUP4,
	GSM_COMMON_DIALUP5,
	GSM_COMMON_FAIL,
	GSM_COMMON_TO_DATA_MODE,
	GSM_COMMON_WAIT_TO_DATA_MODE,
	GSM_COMMON_TO_CMD_MODE,
	GSM_COMMON_CHECK_CMD_MODE,
	GSM_COMMON_WAIT_TO_CMD_MODE
} GSM_COMMON_PHASE_TYPE;
GSM_COMMON_PHASE_TYPE gsmCommonPhase = GSM_COMMON_IDLE;

uint8_t gsmCmdRetry = 3;
Timeout_Type tGsmCommonTaskDelay;
uint8_t GsmCommonTask_IsIdle(void)
{
	if (gsmCommonPhase == GSM_COMMON_IDLE)
	{
		return 1;
	}
	return 0;
}

uint32_t ATO_Func(uint16_t cnt,uint8_t c)
{
   modem_IsOnline = 0;
		return 0;
}

extern void vTcpIpTaskInit(void);

uint8_t GsmCommon_Task(void)
{
	GSM_COMMON_PHASE_TYPE gsmCommonOldPhase = gsmCommonPhase;
	if(GsmTask_GetPhase() != MODEM_SYS_COVERAGE_OK)
		gsmCommonPhase = GSM_COMMON_IDLE;
	if((modemIsBusy == 1 && modemIsBusySource != AT_CMD_COMMON_PARSER && VoiceCallTask_GetPhase() != IN_CALLING)) 
		return 0;
	switch(gsmCommonPhase)
	{
		case GSM_COMMON_IDLE:
			if(callSentATH)
			{
				callSentATH = 0;
				modem_IsOnline = 0;
			}
			if(modem_IsOnline == 0)
				modemCmdMode = 1;
			if(modemDialFlag)
			{
				modem_IsOnline = 0;
				modemDialFlag = 0;
				gsmCommonPhase = GSM_COMMON_DIALUP0;
				vTcpIpTaskInit();
				gsmCmdRetry = 3;
			}
			else if(modemToCommandModeFlag )
			{
					modemToCommandModeFlag = 0;
					if(modemCmdMode == 0)
					{
						gsmCommonPhase = GSM_COMMON_TO_CMD_MODE;
				//	MODEM_DTR_DeAssert();
						gsmCmdRetry = 3;
						InitTimeout(&tGsmCommonTaskDelay, SYSTICK_TIME_SEC(2));
					}
			}
			else if(modemToDataModeFlag)
			{
				modemToDataModeFlag = 0;
				gsmCommonPhase = GSM_COMMON_TO_DATA_MODE;
				gsmCmdRetry = 3;
			}
		break;
		case GSM_COMMON_DIALUP0:
			if(ModemCmdTask_IsIdle())
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 0, "+++") ;
				InitTimeout(&tGsmCommonTaskTimeOut, SYSTICK_TIME_SEC(3));
				gsmCommonPhase = GSM_COMMON_DIALUP0_1;
			}
		break;
		case GSM_COMMON_DIALUP0_1:
			if(ModemCmdTask_IsIdle())
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 0, "AT") ;
				InitTimeout(&tGsmCommonTaskTimeOut, SYSTICK_TIME_SEC(3));
				gsmCommonPhase = GSM_COMMON_DIALUP0_2;
			}
		break;
		case GSM_COMMON_DIALUP0_2:
			if(ModemCmdTask_IsIdle())
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 1000, 0, "ATH\r") ;
				InitTimeout(&tGsmCommonTaskTimeOut, SYSTICK_TIME_SEC(3));
				gsmCommonPhase = GSM_COMMON_DIALUP1;
			}
		break;
		case GSM_COMMON_DIALUP1:
			if (ModemCmdTask_IsIdle())
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 3000, 0, "AT+CGACT=0\r") ;
				InitTimeout(&tGsmCommonTaskTimeOut, SYSTICK_TIME_SEC(3));
				gsmCommonPhase = GSM_COMMON_DIALUP2;
			}
		break;
		case GSM_COMMON_DIALUP2:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 2000, 0, "AT+CGDCONT=1,\"IP\",\"%s\"\r",modemApn) ;
				InitTimeout(&tGsmCommonTaskTimeOut, SYSTICK_TIME_SEC(3));
				gsmCommonPhase = GSM_COMMON_DIALUP3;
			}
			else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR || CheckTimeout(&tGsmCommonTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				gsmCommonPhase = GSM_COMMON_FAIL;
			}
		break;
		case GSM_COMMON_DIALUP3:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, "CONNEC", modemError, 10000, 1, "ATDT*99***1#\r") ;
				InitTimeout(&tGsmCommonTaskTimeOut, SYSTICK_TIME_SEC(30));
				gsmCommonPhase = GSM_COMMON_DIALUP4;
			}
			else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR || CheckTimeout(&tGsmCommonTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				gsmCommonPhase = GSM_COMMON_IDLE;
			}
		break;
		case GSM_COMMON_DIALUP4:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
			//	gsmCommonPhase = GSM_COMMON_TO_CMD_MODE;
				//break;
					// put a dummy PPP packet
				COMM_Putc(0x7E);
				COMM_Putc(0xFF);
				COMM_Putc(0x7D);
				COMM_Putc(0x23);
				COMM_Putc(0xC0);
				COMM_Putc(0x21);
				COMM_Putc(0x7E);
				modemCmdMode = 0;
				modem_IsOnline = 1;
				COMM_pppClrOpened();
				InitTimeout(&tGsmCommonTaskDelay, SYSTICK_TIME_SEC(3));
				gsmCommonPhase = GSM_COMMON_IDLE;
			}
			else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR || CheckTimeout(&tGsmCommonTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				gsmCommonPhase = GSM_COMMON_FAIL;
			}
		break;
		case GSM_COMMON_DIALUP5:
			if (CheckTimeout(&tGsmCommonTaskDelay) == SYSTICK_TIMEOUT)
				gsmCommonPhase = GSM_COMMON_IDLE;
		break;
		case GSM_COMMON_FAIL:
			if(gsmCmdRetry)
					gsmCmdRetry--;
				else
				{
					gsmCmdRetry = 3;
					GsmTask_Init();
					gsmCommonPhase = GSM_COMMON_IDLE;
					break;
				}
			gsmCommonPhase = GSM_COMMON_DIALUP0;
		break;
		case GSM_COMMON_TO_DATA_MODE:
			if(!modemCmdMode)
					gsmCommonPhase = GSM_COMMON_IDLE;
			if(ModemCmdTask_IsIdle())
			{
				ModemCmdTask_SendCmd(ATO_Func,"NO CARRIER",1000, "CONNECT", modemError, 10000, 1, "ATO\r") ;
				MODEM_FlushBuffer();
				InitTimeout(&tGsmCommonTaskTimeOut, SYSTICK_TIME_SEC(3));
				gsmCommonPhase = GSM_COMMON_WAIT_TO_DATA_MODE;
			}
		break;
		case GSM_COMMON_WAIT_TO_DATA_MODE:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				modemCmdMode = 0;
				gsmCommonPhase = GSM_COMMON_IDLE;
			}
			else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR || CheckTimeout(&tGsmCommonTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				COMM_ModemSetOffline();
				gsmCommonPhase = GSM_COMMON_IDLE;
			}
			
		break;
		case GSM_COMMON_TO_CMD_MODE:	
			if (CheckTimeout(&tGsmCommonTaskDelay) == SYSTICK_TIMEOUT)
			{
				COMM_Puts("+++");
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, "NO CARRIER", 3000, 0,NULL) ;
				//MODEM_DTR_Assert();
				modemCmdMode = 1;
				InitTimeout(&tGsmCommonTaskDelay, SYSTICK_TIME_SEC(2));
				gsmCommonPhase = GSM_COMMON_CHECK_CMD_MODE;
			}
		break;
		case GSM_COMMON_CHECK_CMD_MODE:
		if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
		{
				gsmCommonPhase = GSM_COMMON_IDLE;
				break;
		}	
		else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
		{
			ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 3000, 1, "AT\r") ;
			InitTimeout(&tGsmCommonTaskTimeOut, SYSTICK_TIME_SEC(3));
			gsmCommonPhase = GSM_COMMON_WAIT_TO_CMD_MODE;
			break;
		}
		if (ModemCmdTask_IsIdle() && CheckTimeout(&tGsmCommonTaskDelay) == SYSTICK_TIMEOUT)
		{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 3000, 1, "AT\r") ;
				InitTimeout(&tGsmCommonTaskTimeOut, SYSTICK_TIME_SEC(3));
				gsmCommonPhase = GSM_COMMON_WAIT_TO_CMD_MODE;
		}
		
		break;
		case GSM_COMMON_WAIT_TO_CMD_MODE:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK)
			{
				gsmCommonPhase = GSM_COMMON_IDLE;
			}
			else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR || CheckTimeout(&tGsmCommonTaskTimeOut) == SYSTICK_TIMEOUT)
			{
				if(gsmCmdRetry)
					gsmCmdRetry--;
				else
				{
					gsmCmdRetry = 3;
					GsmTask_Init();
					gsmCommonPhase = GSM_COMMON_IDLE;
					break;
				}
				InitTimeout(&tGsmCommonTaskDelay, SYSTICK_TIME_SEC(2));
				gsmCommonPhase = GSM_COMMON_TO_CMD_MODE;
			}
		break;
	}
	
	if(gsmCommonPhase != GSM_COMMON_IDLE)
	{
		modemIsBusy = 1;
		modemIsBusySource = AT_CMD_COMMON_PARSER;
	}
	else if(modemIsBusy && modemIsBusySource == AT_CMD_COMMON_PARSER)
	{
		modemIsBusy = 0;
	}
	if(gsmCommonOldPhase != GSM_COMMON_IDLE && gsmCommonPhase == SMS_IDLE)
		modemIsBusy = 0;
	
	return 0;
	
}

