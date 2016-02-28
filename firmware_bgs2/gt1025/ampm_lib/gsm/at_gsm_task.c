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
#include "modem.h"
#include "lib/sys_tick.h"
#include "lib/sys_time.h"
#include "lib/data_cmp.h"
#include "at_command_parser.h"
#include "at_send_cmd_task.h"
#include "at_gsm_task.h"


uint32_t GSM_GetIMEI(uint16_t cnt,uint8_t c);
uint32_t GSM_GetCID(uint16_t cnt,uint8_t c);

const AT_CMD_PACKET_TYPE atGsmCmdStart[] = {
	"AT\r",NULL,NULL,"OK","ERROR",1,1000,100,
	"ATI\r",NULL,NULL,"OK","ERROR",1,1000,100,
	"AT+CGSN\r","+CGSN\r\r\n",GSM_GetIMEI,"OK","ERROR",1,2000,100,
	//"AT&D1\r\r",NULL,NULL,"OK","ERROR",1,1000,100,
	//"AT&C1\r\r",NULL,NULL,"OK","ERROR",1,1000,100,
	"AT\\Q3\r\r",NULL,NULL,"OK","ERROR",1,2000,100,
	"AT+CMGF=1\r",NULL,NULL,"OK","ERROR",1,1000,100,
	//"AT^SCFG=\"URC/Datamode/Ringline\",\"on\"\r",NULL,NULL,"OK","ERROR",1,3000,100,
	"AT^SCFG=\"URC/Ringline\",\"asc0\"\r",NULL,NULL,"OK","ERROR",1,2000,100,
	"AT+CNMI=3,1,0,0,1\r",NULL,NULL,"OK","ERROR",1,2000,100,
	"AT+CLIP=1\r",NULL,NULL,"OK","ERROR",1,1000,100,
	"AT+CFUN=9\r",NULL,NULL,"OK","ERROR",1,1000,100,
	//"AT^SIND?\r",NULL,NULL,"OK","ERROR",1,1000,100,
	"AT^SIND= nitz,1\r","^SIND: nitz,1,\"",GsmGetUTCTime,"OK","ERROR",1,3000,100,
};


AT_CMD_LIST_TYPE atGsmCmdListStart[sizeof(atGsmCmdStart)/sizeof(AT_CMD_PACKET_TYPE)];

GSM_PHASE_TYPE gsm_TaskPhase;
Timeout_Type tGsmTaskPhaseTimeOut;
Timeout_Type tGsmTaskDelay;
Timeout_Type tGprsEnable;
uint8_t registerNetworkRetry = 0;
uint8_t flagGsmStatus = GSM_NO_REG_STATUS;

uint8_t *gprsApn;
uint8_t *gprsUsr;
uint8_t *gprsPwd;
uint8_t enableGprsRetryCnt = 0;
uint8_t gprsEnableFlag = 0;
uint8_t gprsDisable = 0;
uint8_t gprsEnableCmd = 0;

void GsmTask_InitApn(uint8_t *apn,uint8_t *usr,uint8_t *pwd)
{
	gprsApn = apn;
	gprsUsr = usr;
	gprsPwd = pwd;
}

void GsmTask_Init(void)
{
	modemIsBusy = 0; 
	modemIsBusySource = 0;
	gsm_TaskPhase = MODEM_TURN_ON;
	gprsEnableFlag  = 0;
	gprsDisable = 0;
	gprsEnableCmd = 0;
	modemCmdMode = 1;
	modem_IsOnline = 0;
	InitTimeout(&tGprsEnable,SYSTICK_TIME_SEC(5));
}

void GsmTask_SetPhase(GSM_PHASE_TYPE phase)
{
	gsm_TaskPhase = phase;
}

GSM_PHASE_TYPE GsmTask_GetPhase(void)
{
	return gsm_TaskPhase;
}



void Gsm_Task(void)
{	
	uint16_t i;
	switch(gsm_TaskPhase)
	{
		case MODEM_TURN_ON:
			flagGsmStatus = GSM_NO_REG_STATUS;
			flagGotIMEI = 0;
			modemIMEIPass = 0;
			MODEM_UartInit(115200);
			MODEM_PowerOn();
			InitTimeout(&tGsmTaskDelay, SYSTICK_TIME_SEC(5));
			gsm_TaskPhase = MODEM_SEND_AT;
			break;
		case MODEM_SEND_AT:
			if (CheckTimeout(&tGsmTaskDelay) == SYSTICK_TIMEOUT)
			{
					if(ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, modemError, 3000, 3, "AT\r") == MODEM_RES_OK)
					{	
						InitTimeout(&tGsmTaskPhaseTimeOut, SYSTICK_TIME_SEC(3));
						gsm_TaskPhase = MODEM_CONFIG_0;
					}
			}
		break;
		case MODEM_CONFIG_0:
			//COMM_Puts("AT+IPR=115200\r");
			//COMM_Puts("AT+IPR=115200\r");
			InitTimeout(&tGsmTaskDelay, SYSTICK_TIME_SEC(2));
			gsm_TaskPhase = MODEM_CONFIG_1;
		break;
		case MODEM_CONFIG_1:
			if (ModemCmdTask_GetPhase() == MODEM_CMD_OK && CheckTimeout(&tGsmTaskDelay) == SYSTICK_TIMEOUT)
			{
				//MODEM_UartInit(115200);
				for(i = 0;i < sizeof(atGsmCmdStart)/sizeof(AT_CMD_PACKET_TYPE);i++)
				{
					atGsmCmdListStart[i].pt = (AT_CMD_PACKET_TYPE *)&atGsmCmdStart[i];
					ModemCmdAdd(&atGsmCmdListStart[i]);
				}
				gsm_TaskPhase = MODEM_REGISTRATION_NETWORK;
				InitTimeout(&tGsmTaskPhaseTimeOut, SYSTICK_TIME_SEC(30));
			}
			else if (ModemCmdTask_GetPhase() == MODEM_CMD_ERROR)
			{
				gsm_TaskPhase = MODEM_TURN_ON;
			}
			break;
		
		case MODEM_REGISTRATION_NETWORK:
			if(ModemCmdCheckList_IsEmpty() && (ModemCmdTask_GetPhase() == MODEM_CMD_OK))
			{
				ModemCmdTask_SendCmd(NULL,NULL,100, modemOk, "NOT USE", 3000, 20,"AT+COPS=0,2\r");
				InitTimeout(&tGsmTaskPhaseTimeOut, SYSTICK_TIME_SEC(60));
				gsm_TaskPhase = MODEM_WAIT_REGISTRATION_NETWORK;
			}
			else if (CheckTimeout(&tGsmTaskPhaseTimeOut) == SYSTICK_TIMEOUT || ModemCmdTask_GetPhase() == MODEM_CMD_ERROR) 
			{
				gsm_TaskPhase = MODEM_TURN_ON;
			}
		break;
		case MODEM_WAIT_REGISTRATION_NETWORK:
			if(ModemCmdCheckList_IsEmpty() && (ModemCmdTask_GetPhase() == MODEM_CMD_OK))
			{
				if(modemIMEIPass)
				{
					modemIMEIPass = 0;
					gsm_TaskPhase = MODEM_SYS_COVERAGE_OK;
				}
				else
				{
					gsm_TaskPhase = MODEM_TURN_ON;
				}
				
				if(gsmGetTimeFlag)
				{
					gsmGetTimeFlag = 0;
					sysTimeZone = sysGsmTimezone;
					UpdateRtcTime(TIME_GetSec((DATE_TIME *)&sysGsmTime));
					MODEM_SendCommand(modemOk,modemError,1000,1,"AT+CCLK=\"%02d/%02d/%02d,%02d:%02d:%02d\"\r",sysTime.year % 100,sysTime.month,sysTime.mday,sysTime.hour,sysTime.min,sysTime.sec);
				}

			}
			else if (CheckTimeout(&tGsmTaskPhaseTimeOut) == SYSTICK_TIMEOUT || ModemCmdTask_GetPhase() == MODEM_CMD_ERROR) 
			{
				gsm_TaskPhase = MODEM_TURN_ON;
			}
		break;
		case MODEM_SYS_COVERAGE_OK:
			break;
		default:
			gsm_TaskPhase = MODEM_TURN_ON;
			break;
	}
}
