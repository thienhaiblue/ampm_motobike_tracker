/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "modem.h"
#include "ppp.h"
#include "resolv.h"
#include "resolver.h"
#include "reporter.h"
#include "firmware_task.h"
#include "lib/sys_tick.h"
#include "lib/sys_time.h"
#include "gps/gps.h"
#include "tracker.h"

uint32_t gprsDataSending = 0;
extern uint32_t pgoodStatus;
uint32_t tcpCnt = 0;
Timeout_Type pppReset;
extern Timeout_Type timeReconnect;

Timeout_Type tDataIsSending;
extern uint8_t tcpIpTryCnt;

void vTcpIpTaskInit(void)
{
	MODEM_SetApn((uint8_t *)sysCfg.gprsApn);	
	PPP_Init();
	PPP_SetAuthentication(PPP_AUTH_PAP, (char *)sysCfg.gprsUsr, (char *)sysCfg.gprsPwd);
	InitTimeout(&pppReset,SYSTICK_TIME_SEC(PPP_RESET_TIMEOUT));
}


void vTcpIpTask(void)
{	
	if(!modemCmdMode)
	{
		
		if(CheckTimeout(&pppReset) == SYSTICK_TIMEOUT || tcpIpTryCnt >= 3)
		{
			PPP_Init();
			tcpIpTryCnt  = 0;
			InitTimeout(&pppReset,SYSTICK_TIME_SEC(PPP_RESET_TIMEOUT));
		}
		
		PPP_ManageLink();		// ppp states handling
		
		if(PPP_IsLinked())
		{					
			if(rpConn && fwConn && (rpConn == fwConn))
			{
				RESOLVER_Reset();
				FIRMWARE_Reset();
			}
			RESOLVER_Manage();
			REPORTER_Manage();
			FIRMWARE_Manage();
		}
		else
		{
			InitTimeout(&pppReset,SYSTICK_TIME_SEC(PPP_RESET_TIMEOUT));
			RESOLVER_Reset();
			FIRMWARE_Reset();	// reset those tasks so it can quickly recover after a link down event
			tcpIpTryCnt = 0;
		}
	}
}

extern void FIRMWARE_Callback(void);
// uip callback
void UIP_TCPCallback(void)
{
	REPORTER_Callback();
	FIRMWARE_Callback();
}
// uip callback
void UIP_UDPCallback(void)
{
	resolv_appcall();
}
