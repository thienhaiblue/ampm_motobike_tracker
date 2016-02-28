/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "modem.h"
#include "system_config.h"
#include "lib/sparser.h"
#include "lib/sys_tick.h"
#include "at_command_parser.h"
#include "at_gsm_task.h" 
#include "at_send_cmd_task.h"
#include "at_cmd_sms_parser.h"
#include "lib/list.h"
#include "sst25.h"
#include "gps/gps.h"
#include "adc_task.h"
#include "database_app.h"
#include "lib/encoding.h"

#define smsScanf	sscanf

#define INFO(...)	//DbgCfgPrintf(__VA_ARGS__)
#define SMS_TASK_DBG(...)	//PrintfDebug(__VA_ARGS__)

extern Timeout_Type tFwDownload;
extern uint32_t deviceTurnOffTime;
extern uint8_t flagSendLocationSms;
extern uint32_t firmwareStatus;

const uint8_t smsReplyOk[] = "Thành Công!";
const uint8_t addrString[] = "@ADDR:";

uint8_t smsReplyBuf[32];

uint8_t askAddrCnt = 0;
uint8_t getAddrFlag = 0;
uint8_t gotAddrFlag = 0;
uint8_t addrSendToUserFlag = 0;

SMS_LIST_TYPE replySms;


uint8_t replySmsBuf[160];

uint8_t myAddr[160];

uint8_t systemReset = 0;
uint8_t firmwarePhoneSender[16] = {0};

Timeout_Type tGetAddrTimeout;
Timeout_Type tGotAddrTimeout;
Timeout_Type tGetAddr;


void SMS_TaskInit(void)
{
	InitTimeout(&tGetAddrTimeout,SYSTICK_TIME_SEC(300));
	InitTimeout(&tGotAddrTimeout,SYSTICK_TIME_SEC(1));
	InitTimeout(&tGetAddr,SYSTICK_TIME_SEC(1));
	memset((uint8_t *)&replySms,NULL,sizeof(SMS_LIST_TYPE));
}




void RequestTableTask(uint32_t rtc)
{
	static uint32_t time = 0;
	uint8_t smsLen = 0;
	if(time != rtcTimeSec)
	{
		time = rtcTimeSec;

		if(replySms.flag != SMS_NEW_MSG)
		{
			if(strcmp((char *)sysCfg.fwVersion,FIRMWARE_VERSION) != NULL)
			{
				strcpy((char *)sysCfg.fwVersion,FIRMWARE_VERSION);
				CFG_Save();
				//send sms
				smsLen = sprintf((char *)replySmsBuf,	"%s:Ver%s\n",replySmsBuf,FIRMWARE_VERSION);
				Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)replySmsBuf,smsLen,SMS_TEXT_MODE,30000,3);
			}
		}
	}
	
	if(newIMEI_IsUpdate
			&& replySms.flag != SMS_NEW_MSG
	)
	{
		newIMEI_IsUpdate = 0;
		sprintf((char *)replySmsBuf,"New IMEI is Updated:%s",sysCfg.imei);
		smsLen = strlen((char *)replySmsBuf);
		Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)replySmsBuf,smsLen,SMS_TEXT_MODE,30000,3);
		CFG_Save();
	}
		
	if(replySms.flag != SMS_NEW_MSG 
	&& sysTimeZone 
	&& (sysTimeZone/3600) != sysCfg.timezone)
	{
			sysCfg.timezone = sysTimeZone/3600;
			sprintf((char *)replySmsBuf,"New timezone is Updated:%d",sysCfg.timezone);
			smsLen = strlen((char *)replySmsBuf);
			Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)replySmsBuf,smsLen,SMS_TEXT_MODE,30000,3);
			CFG_Save();
	}
	//Addr 
	if(getAddrFlag == 0)
	{
		InitTimeout(&tGetAddrTimeout,SYSTICK_TIME_SEC(300));
		InitTimeout(&tGetAddr,SYSTICK_TIME_SEC(1));
	}
	else if(CheckTimeout(&tGetAddrTimeout) == SYSTICK_TIMEOUT
	|| (lastGpsInfo.lat == 0 && lastGpsInfo.lon == 0)	
	)
	{
		askAddrCnt = 0;
		getAddrFlag = 0;
	}
	
	if(gotAddrFlag == 0)
	{
		InitTimeout(&tGotAddrTimeout,SYSTICK_TIME_SEC(300));
	}
	else if(CheckTimeout(&tGotAddrTimeout) == SYSTICK_TIMEOUT
	|| (lastGpsInfo.lat == 0 && lastGpsInfo.lon == 0)		
	)
	{
		gotAddrFlag = 0;
	}
	
	if(gotAddrFlag
	&& replySms.flag != SMS_NEW_MSG 
	)
	{
		gotAddrFlag = 0;
		if(addrSendToUserFlag)
		{
				addrSendToUserFlag = 0;
				smsLen = unilen((uint16_t *)myAddr);
				smsLen *= 2;
				Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)myAddr,smsLen,SMS_PDU16_MODE,30000,3);
		}
	}
}


uint16_t UniConfigParser(uint16_t *in, uint16_t *out)
{
	uint8_t u8temp;
	uint16_t *u16pt;
	uint8_t buf[32];
	uint16_t len = 0;
	
	utf8s_to_ucs2s((int16_t *)buf,(uint8_t *)addrString);
	u16pt = uni_srtsrt((uint16_t *)in,(uint16_t *)buf); 
	if (u16pt != 0)
	{
			askAddrCnt = 0;
			getAddrFlag = 0;
			gotAddrFlag = 1;
			u8temp = unilen((uint16_t *)&u16pt[6]);
			u8temp *= 2;
			if(u8temp && u8temp < sizeof(myAddr))
			{
				memset(myAddr,0,sizeof(myAddr));
				memcpy(myAddr,&u16pt[6],u8temp);
				big2litel_endian((uint16_t *)myAddr,unilen((uint16_t *)myAddr));
				//Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)myAddr,u8temp,SMS_PDU16_MODE,30000,3);
			}
	}
	return len;
}


uint16_t CMD_CfgParse(char *cmdBuff, uint8_t *smsSendBuff, uint32_t smsLenBuf, uint32_t *dataOutLen, uint8_t pwdCheck,uint8_t sourceCmd)
{
		char *buff, *pt;
	uint32_t  t1;
	uint8_t smsLen = 0,len = 0;
	char tempBuff0[34];
	uint8_t flagCfgSave = 0;
	buff = cmdBuff;
	smsLen = utf8s_to_ucs2s((int16_t *)smsReplyBuf,(uint8_t *)smsReplyOk);
	big2litel_endian((uint16_t *)smsReplyBuf,unilen((uint16_t *)smsReplyBuf));
	smsLen *= 2;
	
	for(t1 = 0; t1 < strlen(buff);t1++)
	{
			if(islower(buff[t1]))
			 buff[t1] = toupper(buff[t1]);
	}
	//change device id
	pt = strstr(buff,"ABCFG,14,");
	if(pt != NULL)
	{
		if(ComparePhoneNumber((char *)smsSender,(char *)sysCfg.userPhone) != NULL)
		{
			Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)smsReplyBuf,smsLen,SMS_PDU16_MODE,30000,3);
		}
		else if(ComparePhoneNumber((char *)smsSender,"+84978779222") != NULL)
		{
			Sms_SendMsg(&replySms,"84978779222",(uint8_t *)smsReplyBuf,smsLen,SMS_PDU16_MODE,30000,3);
		}
		else
			return 0xff;
		INFO("\n\rSMS->CHANGE DEVICE ID:%s\n\r",sysCfg.id);
		smsScanf(pt,"ABCFG,14,%s",tempBuff0);
		memcpy((char *)sysCfg.id,tempBuff0,sizeof(sysCfg.id));
		flagCfgSave = 1;
	}
	pt = strstr(buff, "RFID RESET");
	if (pt != NULL)
	{
		if(ComparePhoneNumber((char *)smsSender,(char *)sysCfg.userPhone) != NULL)
		{
			Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)smsReplyBuf,smsLen,SMS_PDU16_MODE,30000,3);
		}
		else if(ComparePhoneNumber((char *)smsSender,"+84978779222") != NULL)
		{
			Sms_SendMsg(&replySms,"84978779222",(uint8_t *)smsReplyBuf,smsLen,SMS_PDU16_MODE,30000,3);
		}
		else
			return 0xff;
		
		strcpy((char *)sysCfg.rfidPass ,DEFAULT_RFID_TAG_ID);
		CFG_Save();
		strcpy((char *)smsSendBuff,(char *)smsReplyOk);
		*dataOutLen = strlen((char *)smsSendBuff);
		return 0;
	}

	pt = strstr(buff, "*C88:88#");
	if (pt != NULL)
	{
		if(ComparePhoneNumber((char *)smsSender,(char *)sysCfg.userPhone) != NULL)
		{
			Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)smsReplyBuf,smsLen,SMS_PDU16_MODE,30000,3);
		}
		else if(ComparePhoneNumber((char *)smsSender,"+84978779222") != NULL)
		{
			Sms_SendMsg(&replySms,"84978779222",(uint8_t *)smsReplyBuf,smsLen,SMS_PDU16_MODE,30000,3);
		}
		else
			return 0xff;
		DB_RingLogReset();
		strcpy((char *)tempBuff0, (char *)sysCfg.id);
		memset((void*)&sysCfg, 0xFF, sizeof sysCfg);
		CFG_Save();
		CFG_Load();
		strcpy((char *)sysCfg.id ,(char *)tempBuff0);
		CFG_Save();
		sysResetMcuFlag = 1;
	}
	
	pt = strstr(buff, "*C30:30#");
	if (pt != NULL)
	{
		smsLen = sprintf((char *)replySmsBuf,	"ID:%s,Ver:%s",sysCfg.id,FIRMWARE_VERSION);
		if(ComparePhoneNumber((char *)smsSender,(char *)sysCfg.userPhone) != NULL)
		{
			Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)replySmsBuf,smsLen,SMS_TEXT_MODE,30000,3);
		}
		else if(ComparePhoneNumber((char *)smsSender,"+84978779222") != NULL)
		{
			Sms_SendMsg(&replySms,"84978779222",(uint8_t *)replySmsBuf,smsLen,SMS_TEXT_MODE,30000,3);
		}
		else
			return 0xff;
		
		strcpy((char *)smsSendBuff,(char *)smsReplyOk);
		*dataOutLen = strlen((char *)smsSendBuff);
	}

	pt = strstr(buff, "GT1025FWUD");
	if (pt != NULL)
	{
		if(ComparePhoneNumber((char *)smsSender,(char *)sysCfg.userPhone) != NULL)
		{
			Sms_SendMsg(&replySms,sysCfg.userPhone,(uint8_t *)smsReplyBuf,smsLen,SMS_PDU16_MODE,30000,3);
		}
		else if(ComparePhoneNumber((char *)smsSender,"+84978779222") != NULL)
		{
			Sms_SendMsg(&replySms,"84978779222",(uint8_t *)smsReplyBuf,smsLen,SMS_PDU16_MODE,30000,3);
		}
		else
			return 0xff;


		if(smsSender[0] == '+')
			strcpy((char *)firmwarePhoneSender,(char *)&smsSender[1]);
		else 
			strcpy((char *)firmwarePhoneSender,(char *)&smsSender[0]);
		InitTimeout(&tFwDownload,SYSTICK_TIME_SEC(600)); //wait for 10min

		strcpy((char *)smsSendBuff,(char *)smsReplyOk);
		*dataOutLen = strlen((char *)smsSendBuff);
		return 0;
	}

	if(flagCfgSave)
	{
		CFG_Save();
	}
	return 0xff;
}

// untested
void SMS_Manage(uint8_t *buff,uint32_t lengBuf)
{
	Timeout_Type t,timeOutFunc;
	uint8_t tmpBuf[32],c;	
	uint16_t smsLoc,i,res;
	uint32_t len;
//	SysTick_DelayMs(500);
//	MODEM_SendCommand(modemOk,modemError,1000,2,"AT+CBC\r"); //Batery check status
	// read the newly received SMS
	SysTick_DelayMs(500);
	if(MODEM_SendCommand(modemOk,modemError,1000,3,"AT+CMGF=1\r"))	 return;
	SysTick_DelayMs(500);
	MODEM_SendCommand(modemOk,modemError,3000,0,"AT+CMGL=\"ALL\"\r");
	InitTimeout(&timeOutFunc,10000);//10s
	SysTick_DelayMs(1000);
	while(RINGBUF_GetFill(&smsUnreadRingBuff) >=2)
	{
		if(CheckTimeout(&timeOutFunc) == SYSTICK_TIMEOUT) 
		{
			return;
		}
		if(RINGBUF_Get(&smsUnreadRingBuff,&c) == 0)
			smsLoc = c;
		else break;
		if(RINGBUF_Get(&smsUnreadRingBuff,&c) == 0)
			smsLoc |= (((uint16_t)c) << 8 & 0xff00);
		else break;
		// read the newly received SMS
		SMS_TASK_DBG("\r\nSMS->READ SMS\r\n");
		flagSmsReadFinish = 0;
		sprintf((char *)tmpBuf, "AT+CMGR=%d\r", smsLoc);
		COMM_Puts(tmpBuf);
		InitTimeout(&t,3000);//3s
		while(!flagSmsReadFinish)
		{
			if(CheckTimeout(&t) == SYSTICK_TIMEOUT) 
			{
				break;
			}
		}
		smsDataBuf[sizeof(smsDataBuf) - 1] = 0;
		SMS_TASK_DBG("\n\rSMS->PHONE:%s\n\r", smsSender);	
		SMS_TASK_DBG("\r\nSMS->DATA:%s\r\n",smsDataBuf);
		// delete just received SMS
		SMS_TASK_DBG("\n\rSMS->DELETE:%d\n\r",smsLoc);
		SysTick_DelayMs(1000);
		MODEM_SendCommand(modemOk, modemError, 1000, 3, "AT+CMGD=%d\r",smsLoc);
		if(flagSmsReadFinish == 0)
		{
			continue;
		}
		i = strlen((char *)smsDataBuf);
		if(i >= sizeof(smsDataBuf))
			i = sizeof(smsDataBuf) - 2;
		smsDataBuf[i] = 0;
		smsDataBuf[i + 1] = 0;
		res = CMD_CfgParse((char *)smsDataBuf, buff, lengBuf, &len, 1,CMD_FROM_SMS);
		 
		CFG_Save();
		
	}
}

