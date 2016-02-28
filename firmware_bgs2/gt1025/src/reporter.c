/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "reporter.h"
#include "xtea.h"
#include "lib/sys_tick.h"
#include "lib/sys_tick.h"
#include "system_config.h"
#include "app_config_task.h"
#include "led.h"
#include "tcp_ip_task.h"
#include "gps/gps.h"
#include "tracker.h"
#include "sms_task.h"
#include "alarm_task.h"

#define RP_DBG(...)		DbgCfgPrintf(__VA_ARGS__)

#define TIME_KEEP_CONNECT 60

#define TCPIP_ECHO_ON


#define RECONNECT_TIMEOUT		10 * SYSTICK_SECOND

TCP_STATE_TYPE rpState = INITIAL;

uip_ipaddr_t rpServerIpAddr;
uip_ipaddr_t *rpServerIp;
struct uip_conn *rpConn;
U32 rpTick;
I16 rexmitCnt = 0;
I16 timedOutCnt = 0;
uint16_t rpLen = 0;
I16 loginLen = 0;
volatile I8 loggedIn = 0;

uint32_t askCmd = 0;
Timeout_Type tCallbackTimeout;
extern uint8_t sendStatus;
extern uint8_t statusSentFlag;
extern Timeout_Type fdummySendPacket;
extern uint32_t fwTick;
extern Timeout_Type pppReset;

extern Timeout_Type tAppTimeout;

extern void FIRMWARE_Reset(void);
uint32_t rpTaskLen = 0;	


uint8_t rpBuf[256];			// must big enough to hold a packet of all types

uint8_t tcpIpTryCnt = 0;
uint32_t callBackCnt = 0;
uint32_t reportCnt = 0;

uint32_t rpSwitchServer = 0;

void REPORTER_Init(uint32_t priority)
{
	rpConn = NULL;
	rpState = INITIAL;
	/*Ring Buff*/
	InitTimeout(&tCallbackTimeout,SYSTICK_TIME_SEC(10));
}

void REPORTER_Reset(void)
{
	rpConn = NULL;
	rpState = INITIAL;
}
uint32_t sentDataCnt = 0;
uint32_t recvDataCnt = 0;
TCP_STATE_TYPE REPORTER_Manage(void)
{ 
	uint32_t addr0,addr1,addr2,addr3;
	reportCnt++;
	switch(rpState)
	{
		case INITIAL:		
			if(Domain_IpCheck(sysCfg.priDserverName) == IS_IP)
			{
				if(rpSwitchServer == 0)
					sscanf((char *)sysCfg.priDserverName,"%d.%d.%d.%d",&addr0,&addr1,&addr2,&addr3);
				else
					sscanf((char *)sysCfg.secDserverName,"%d.%d.%d.%d",&addr0,&addr1,&addr2,&addr3);
				rpServerIpAddr.u8[0] = addr0;
				rpServerIpAddr.u8[1] = addr1;
				rpServerIpAddr.u8[2] = addr2;
				rpServerIpAddr.u8[3] = addr3;
				rpServerIp = &rpServerIpAddr;
			}
			else
			{
				if(rpSwitchServer == 0)
					rpServerIp = RESOLVER_Lookup((char *)sysCfg.priDserverName);
				else
					rpServerIp = RESOLVER_Lookup((char *)sysCfg.secDserverName);
			}
			
			if(rpServerIp == NULL) 
				break;
			else if(rpServerIp->u8[0] == 0 
			&& rpServerIp->u8[1]== 0 
			&& rpServerIp->u8[2] == 0
			&& rpServerIp->u8[3] == 0
			)
			{
				CFG_Load();
				break;
			}

			LedSetStatus(&led1Ctr,500,500,LED_TURN_ON,0xffffffff);
			loggedIn = 0;
			rpConn = NULL;
			RP_DBG("\r\nDATA_SVR: Started, Server %d.%d.%d.%d:%d\r\n", ((uint8_t*)(rpServerIp))[0], ((uint8_t*)(rpServerIp))[1], 
					((uint8_t*)(rpServerIp))[2], ((uint8_t*)(rpServerIp))[3], sysCfg.dServerPort);
			
			rpConn = uip_connect(rpServerIp, uip_htons(sysCfg.dServerPort));
			rpTick = SysTick_Get();
			if(rpConn == NULL)
			{
				RP_DBG("\r\nDATA_SVR: uip_connect returns NULL\r\n");
				rpState = WAIT_TIMEOUT_RECONNECT;
				break;
			}
			rpState = CONNECT;
			
			InitTimeout(&tCallbackTimeout,SYSTICK_TIME_SEC(10));
			break;
		
		case LOGGED_IN:
			if(SysTick_Get() - rpTick >= SYSTICK_SECOND * 10)
			{
				rpState = REPORTING;
			}
			break;
		case CONNECT:
		case WAIT_TIMEOUT_RECONNECT:
			if(SysTick_Get() - rpTick >= RECONNECT_TIMEOUT)
			{
				tcpIpTryCnt++;
				rpSwitchServer ^= 1;
				REPORTER_Reset();
			}
			break;
		case CONNECTED:
			//InitTimeout(&pppReset,SYSTICK_TIME_SEC(300));
			if(CheckTimeout(&tCallbackTimeout) == SYSTICK_TIMEOUT)
			{
				REPORTER_Init(1);
				rpState = INITIAL;
				timedOutCnt = 0;
				rexmitCnt = 0;
			}
		break;
		default:
			
			break;
	}
	return rpState;
}

void REPORTER_Callback()
{
	uint32_t i;
	uint16_t len1;
	if(uip_conn != rpConn)
		return;
	callBackCnt++;
	InitTimeout(&tCallbackTimeout,SYSTICK_TIME_SEC(10));
	if(uip_connected())
	{
		LedSetStatus(&led1Ctr,100,2900,LED_TURN_ON,0xffffffff);
		RP_DBG("\r\nDATA_SVR: connected\r\n");
		timedOutCnt = 0;
		rexmitCnt = 0;
		rpLen = 0;
		rpState = CONNECTED;
	}
	
	if(uip_poll())
	{
		GetDataFromLog();
		rpLen = 0;
		if(logSendPt)
		{
			
			if(memcmp((char *)&newestLog,(char *)&logRecord,sizeof(MSG_STATUS_RECORD)) != NULL
				&& memcmp((char *)&logRecord,(char *)logSendPt,sizeof(MSG_STATUS_RECORD)) != NULL
			)
			{
				newestLog = logRecord;
				AddTrackerPacket(&newestLog,(char *)rpBuf,&rpLen);
			}
			else
				AddTrackerPacket(logSendPt,(char *)rpBuf,&rpLen);
		}
		//Add ask Addr CMD
		else if(getAddrFlag && CheckTimeout(&tGetAddr) == SYSTICK_TIMEOUT) 
		{
				InitTimeout(&tGetAddr,SYSTICK_TIME_SEC(10));
				rpLen = sprintf((char *)rpBuf,"@GETADDR,vi,%.8f,%.8f\r\n",lastGpsInfo.lat,lastGpsInfo.lon);
				askAddrCnt++;
		}
		if(rpLen)
			uip_send(rpBuf,rpLen);
	}
	
	if(uip_newdata())
	{
		if(uip_len)
		{
			((uint8_t *)uip_appdata)[uip_len] = 0;
			((uint8_t *)uip_appdata)[uip_len + 1] = 0;
			((uint8_t *)uip_appdata)[uip_len + 2] = 0;
			UniConfigParser((uint16_t *)uip_appdata,(uint16_t *)rpBuf);
			InitTimeout(&pppReset,SYSTICK_TIME_SEC(PPP_RESET_TIMEOUT));
			InitTimeout(&tAppTimeout,SYSTICK_TIME_SEC(120));
		}
	}	
	if(uip_acked())
	{
		RP_DBG("\r\nDATA_SVR: sent:%d byte(s)\r\n",rpLen);
		tcpIpTryCnt = 0;
		if(logSendPt)
		{	
			if(trackerSaveLog == 0 && (sendStatus || (!ACC_IS_ON)))
			{
				sendStatus = 0;
				statusSentFlag = 1;
			}
			logSendPt->serverSent = 1;
			logSendPt = NULL;
		}
		if(askAddrCnt >= 6 && getAddrFlag)
		{
			askAddrCnt = 0;
			getAddrFlag = 0;	
		}
		GetDataFromLog();
		InitTimeout(&tAppTimeout,SYSTICK_TIME_SEC(120));
		rpLen = 0;
		timedOutCnt = 0;
		rexmitCnt = 0;	
		InitTimeout(&pppReset,SYSTICK_TIME_SEC(PPP_RESET_TIMEOUT));
	}
	
	if((uip_rexmit()))
	{
		uip_send(rpBuf, rpLen);
		RP_DBG("\r\nDATA_SVR: rexmit\r\n");
		rexmitCnt ++;
		if(rexmitCnt >= 5) 
		{
			rexmitCnt = 0;
			rpLen = 0;
			rpState = WAIT_TIMEOUT_RECONNECT;
		}
	}
	
	if(uip_closed() || uip_aborted())
	{
		LedSetStatus(&led1Ctr,500,500,LED_TURN_ON,0xffffffff);
		rpConn = NULL;
		rpTick = SysTick_Get();
		RP_DBG("\r\nDATA_SVR: timedout|closed|aborted %d\r\n", timedOutCnt);
		rpState = WAIT_TIMEOUT_RECONNECT;		
	}
}


