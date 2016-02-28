/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "lib/sys_tick.h"
#include "lib/ringbuf.h"
#include "app_config_task.h"
#include "uart_config_task.h"
#include "system_config.h"
#include "uip.h"
#include "resolver.h"
#include "tcp_ip_task.h"

#define FW_DBG(...)		DbgCfgPrintf(__VA_ARGS__)

#define GPRS_PACKET_SIZE 			516

#define FIRMWARE_SIZE_MAX (128*1024) //128Kbytes

#define TIME_KEEP_CONNECT 60

#define TCPIP_ECHO_ON



#define FW_RECONNECT_TIMEOUT		30 * SYSTICK_SECOND

extern uint8_t CfgCalcCheckSum(uint8_t *buff, uint32_t length);

TCP_STATE_TYPE fwState = INITIAL;
uip_ipaddr_t fwServerIpAddr;
uip_ipaddr_t *fwServerIp;
struct uip_conn *fwConn;
uint32_t fwTick;
static int16_t fwRexmitCnt = 0;
int16_t fwTimedOutCnt = 0;
int16_t fwLen = 0;
volatile int8_t fwLoggedIn = 0;
extern Timeout_Type pppReset;
extern U32 rpTick;
uint8_t fwBuf[576];

extern Timeout_Type tDataIsSending;
#if defined ( __ICCARM__ )
#pragma pack(1) 
typedef struct
{
	uint8_t imei[18];
	uint8_t id[18];
	uint8_t ver[16];
}INFO_FIRMWARE_TYPE;
#pragma pack()
#elif defined (__CC_ARM)
typedef struct __attribute__((packed))
{
	uint8_t imei[18];
	uint8_t id[18];
	uint8_t ver[16];
}INFO_FIRMWARE_TYPE;
#endif


INFO_FIRMWARE_TYPE fInfo;

uint8_t loginSendBuff[sizeof(INFO_FIRMWARE_TYPE) + 5];

uint32_t fwCallBackCnt = 0;


Timeout_Type fdummySendPacket;


Timeout_Type tFrimwareUpdateTimeout;
Timeout_Type fwCallbackTimeout;
Timeout_Type tFwTcpDataOut;
uint8_t fSendBuff[64];
uint8_t GPRS_DataBuff[GPRS_PACKET_SIZE];
CFG_PROTOCOL_TYPE GPRS_ProtoRecv;
CFG_PROTOCOL_TYPE GPRS_ProtoSend;
PARSER_PACKET_TYPE GPRS_parserPacket;
uint8_t fFlagSendData = 0;
uint16_t fDataLength = 0;
uint32_t fCnt;
uint32_t fwSwitchServer = 0;



uint8_t FirmwareTask_IsBusy(void)
{
		if(firmwareStatus == 0xA5A5A5A5 && (CheckTimeout(&tFirmwareTryToUpdate) != SYSTICK_TIMEOUT))
			return 1;
		return 0;
}

void FIRMWARE_Task(void)
{
	uint32_t i;
	if(fDataLength)
	{
		GPRS_ProtoRecv.dataPt = GPRS_DataBuff;
		GPRS_parserPacket.lenMax = GPRS_PACKET_SIZE;
		GPRS_parserPacket.state = CFG_CMD_WAITING_SATRT_CODE;
		for(i = 0;i < fDataLength;i++)
		{
			if(CfgParserPacket(&GPRS_parserPacket,&GPRS_ProtoRecv,fwBuf[i]) == 0)
			{
				break;
			}
			fwBuf[i] = 0;
		}
		for(;i < fDataLength;i++) 
		{
				fwBuf[i] = 0;
		}
		CfgProcessData(&GPRS_ProtoRecv,&GPRS_ProtoSend,GPRS_DataBuff,GPRS_parserPacket.lenMax - 4,0);
		if(GPRS_ProtoSend.length)
		{
			fSendBuff[0] = GPRS_ProtoSend.start;
			fSendBuff[1] = ((uint8_t *)&GPRS_ProtoSend.length)[0];
			fSendBuff[2] = ((uint8_t *)&GPRS_ProtoSend.length)[1];
			fSendBuff[3] = GPRS_ProtoSend.opcode;
			for(i = 0;i < GPRS_ProtoSend.length;i++)
				fSendBuff[i+4] = GPRS_ProtoSend.dataPt[i];
			fSendBuff[i+4] = GPRS_ProtoSend.crc;
			GPRS_ProtoSend.length = 0;
			fFlagSendData = 1;
		}
		fDataLength = 0;

	}
	
}


void FIRMWARE_Init(uint32_t priority)
{
	fwConn = NULL;
	fwState = INITIAL;
	InitTimeout(&fwCallbackTimeout,SYSTICK_TIME_SEC(10));
	InitTimeout(&tFwTcpDataOut,SYSTICK_TIME_SEC(10));
}


void FIRMWARE_Reset(void)
{
	fwConn = NULL;
	fwState = INITIAL;
	InitTimeout(&fwCallbackTimeout,SYSTICK_TIME_SEC(10));
	InitTimeout(&tFwTcpDataOut,SYSTICK_TIME_SEC(10));
}

TCP_STATE_TYPE FIRMWARE_Manage(void)
{ 
	uint8_t i;
	uint32_t addr0,addr1,addr2,addr3;
	switch(fwState)
	{
		case INITIAL:
					
			if(Domain_IpCheck(sysCfg.priFserverName) == IS_IP)
			{
				if(fwSwitchServer == 0)
					sscanf((char *)sysCfg.priFserverName,"%d.%d.%d.%d",&addr0,&addr1,&addr2,&addr3);
				else
					sscanf((char *)sysCfg.secFserverName,"%d.%d.%d.%d",&addr0,&addr1,&addr2,&addr3);
				fwServerIpAddr.u8[0] = addr0;
				fwServerIpAddr.u8[1] = addr1;
				fwServerIpAddr.u8[2] = addr2;
				fwServerIpAddr.u8[3] = addr3;
				fwServerIp = &fwServerIpAddr;
			}
			else
			{
				if(fwSwitchServer == 0)
					fwServerIp = RESOLVER_Lookup((char *)sysCfg.priFserverName);
				else
					fwServerIp = RESOLVER_Lookup((char *)sysCfg.secFserverName);
			}
			if(fwServerIp == NULL) 
				break;
			else if(fwServerIp->u8[0] == 0 
			&& fwServerIp->u8[1]== 0 
			&& fwServerIp->u8[2] == 0
			&& fwServerIp->u8[3] == 0
			)
			{
				CFG_Load();
				break;
			}
			fwLoggedIn = 0;
			fwConn = NULL;
			FW_DBG("\r\nFW_SVR: Started, Server %d.%d.%d.%d:%d\r\n", ((uint8_t*)(fwServerIp))[0], ((uint8_t*)(fwServerIp))[1], 
					((uint8_t*)(fwServerIp))[2], ((uint8_t*)(fwServerIp))[3], 50000);
			
			fwConn = uip_connect(fwServerIp, uip_htons(50000));
			fwTick = SysTick_Get();
			if(fwConn == NULL)
			{
				FW_DBG("\r\nFW_SVR: uip_connect returns NULL\r\n");
				fwState = WAIT_TIMEOUT_RECONNECT;
				break;
			}
			
			memcpy((char  *)fInfo.imei,(char  *)sysCfg.imei,18);
			memcpy((char  *)fInfo.id,(char  *)sysCfg.id,18);
			memcpy((char  *)fInfo.ver,FIRMWARE_VERSION,sizeof(FIRMWARE_VERSION));
			loginSendBuff[1] = sizeof(INFO_FIRMWARE_TYPE);
			loginSendBuff[0] = 0xCA;
			loginSendBuff[2] = 0;
			loginSendBuff[3] = 0x77;
			memcpy((char *)&loginSendBuff[4],(char  *)&fInfo,sizeof(INFO_FIRMWARE_TYPE));
			loginSendBuff[loginSendBuff[1] + 4] = CfgCalcCheckSum((uint8_t *)&fInfo,sizeof(INFO_FIRMWARE_TYPE));
			
			InitTimeout(&fdummySendPacket,SYSTICK_TIME_SEC(10));
			InitTimeout(&tFrimwareUpdateTimeout,SYSTICK_TIME_MS(60));
			fwState = CONNECT;
			break;
		
		case LOGGED_IN:
			if(SysTick_Get() - fwTick >= SYSTICK_SECOND * 10)
			{
				fwTick = SysTick_Get();
				fwState = REPORTING;
			}
			break;
		case CONNECT:
		case WAIT_TIMEOUT_RECONNECT:
			if(SysTick_Get() - fwTick >= FW_RECONNECT_TIMEOUT)
			{
				fwTick = SysTick_Get();
				FIRMWARE_Reset();
			}
			break;
		case CONNECTED:
			//InitTimeout(&pppReset,SYSTICK_TIME_SEC(300));
			if(CheckTimeout(&fwCallbackTimeout) == SYSTICK_TIMEOUT)
			{
				FIRMWARE_Init(1);
				fwState = INITIAL;
				fwTimedOutCnt = 0;
				fwRexmitCnt = 0;
			}
		break;
		default:
			
			break;
	}
	
	return fwState;
}

void FIRMWARE_Callback(void)
{
	uint32_t i;
	uint8_t *u8pt;
	if(uip_conn != fwConn)
		return;
	fwCallBackCnt++;
	InitTimeout(&fwCallbackTimeout,SYSTICK_TIME_SEC(10));
	if(uip_connected())
	{
		FW_DBG("\r\nFW_SVR: connected\r\n");
		fwTimedOutCnt = 0;
		fwRexmitCnt = 0;
		fwLen = 0;
		fwState = CONNECTED;
	}
	
	if(uip_poll())
	{
		if(CheckTimeout(&tFwTcpDataOut) == SYSTICK_TIMEOUT)
			fwLen = 0;
		
		if(fwLen == 0)
		{
			InitTimeout(&tFwTcpDataOut,SYSTICK_TIME_SEC(10));
			if(fwLoggedIn)
			{
				if(fSendBuff[0] == 0xCA)
				{
					if(fFlagSendData)
					{
							InitTimeout(&tFrimwareUpdateTimeout,SYSTICK_TIME_SEC(60));
					}
					else if(CheckTimeout(&tFrimwareUpdateTimeout) == SYSTICK_TIMEOUT)
					{
						__disable_irq();
						firmwareStatus = 0xA5A5A5A5;
						firmwareFileOffSet = 0;
						InitTimeout(&tFrimwareUpdateTimeout,SYSTICK_TIME_SEC(60));
						fSendBuff[0] = 0;
						InitTimeout(&fdummySendPacket,SYSTICK_TIME_SEC(300));
						FIRMWARE_Reset();
						__enable_irq();
					}
					if((CheckTimeout(&fdummySendPacket) == SYSTICK_TIMEOUT) || fFlagSendData)
					{
						fFlagSendData = 0;
						fwLen = fSendBuff[1] + 5;
						memcpy(fwBuf,fSendBuff,fwLen);
						InitTimeout(&fdummySendPacket,SYSTICK_TIME_SEC(60));
					}
				}
				else if(CheckTimeout(&fdummySendPacket) == SYSTICK_TIMEOUT)
				{
					InitTimeout(&fdummySendPacket,SYSTICK_TIME_SEC(300));
					fwLen = loginSendBuff[1] + 5;
					memcpy(fwBuf,loginSendBuff,fwLen);
				}
			}
			else
			{
				if(firmwareStatus == 0xA5A5A5A5)
				{
					if((firmwareFileOffSet < firmwareFileSize) && (firmwareFileSize <= FIRMWARE_SIZE_MAX))
					{
						fSendBuff[0] = 0xCA;
						fSendBuff[1] = 4;
						fSendBuff[2] = 0;
						fSendBuff[3] = 0x12;
						u8pt = (uint8_t *)&firmwareFileOffSet;
						fSendBuff[4] = u8pt[0];
						fSendBuff[5] = u8pt[1];
						fSendBuff[6] = u8pt[2];
						fSendBuff[7] = u8pt[3];
						fSendBuff[8] = CfgCalcCheckSum(&fSendBuff[4],4);
					}
				}
				
				fwLen = loginSendBuff[1] + 5;
				memcpy(fwBuf,loginSendBuff,fwLen);
				InitTimeout(&tFrimwareUpdateTimeout,SYSTICK_TIME_SEC(60));
				fwLoggedIn = 1;
			}
			if(fwLen)
			{
				uip_send(fwBuf,fwLen);
				FW_DBG("\r\nFW_SVR: sending %d bytes",fwLen);
			}
		}
	}
	
	if(uip_newdata())
	{
		FW_DBG("\r\nFW_SVR: Got New Data:%d bytes",uip_len);
		if(uip_len && uip_len < sizeof(fwBuf))
			if(fDataLength == 0)
			{
				InitTimeout(&tDataIsSending,SYSTICK_TIME_SEC(30));
				memcpy(fwBuf,((uint8_t *)uip_appdata),uip_len);
				fDataLength = uip_len;
				InitTimeout(&fdummySendPacket,SYSTICK_TIME_SEC(10));
			}
	}	
	if(uip_acked())
	{
		FW_DBG("\r\nFW_SVR: sent:%d byte(s)\r\n",fwLen);
		fwLen = 0;
		fwTimedOutCnt = 0;
		fwRexmitCnt = 0;
		tcpIpTryCnt = 0;
		InitTimeout(&pppReset,SYSTICK_TIME_SEC(PPP_RESET_TIMEOUT));
	}
	
	if(uip_rexmit())
	{
		uip_send(fwBuf, fwLen);
		FW_DBG("\r\nFW_SVR: rexmit %d bytes\r\n",fwLen);
		fwRexmitCnt ++;
		if(fwRexmitCnt >= 5) 
		{
			fwRexmitCnt = 0;
			fwLen = 0;
			fwState = WAIT_TIMEOUT_RECONNECT;
		}
	}
	
	if(uip_closed() || uip_aborted())
	{
		fwConn = NULL;
		fwTick = SysTick_Get();
		FW_DBG("\r\nFW_SVR: timedout|closed|aborted %d\r\n", fwTimedOutCnt);
		fwState = WAIT_TIMEOUT_RECONNECT;		
	}
}


