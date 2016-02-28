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
#include "lib/data_cmp.h"
#include "lib/sys_tick.h"
#include "gsm_hwr.h"




uint8_t modemDialFlag = 0;
uint8_t modemToCommandModeFlag = 0;
uint8_t modemToDataModeFlag = 0;
uint8_t modem_IsOnline = 0;
uint8_t *modemApn;

const uint8_t *modemOk = "OK";
const uint8_t *modemError = "ERROR";
uint8_t modemCmdMode = 0;

uint8_t csqBuff[16];
float csqValue = 20;
uint8_t gsmSignal = 0;

void MODEM_GoToCmdMode(void)
{
	if(!modemCmdMode)
		modemToCommandModeFlag = 1;
}
void MODEM_GoToDataMode(void)
{
	if(modemCmdMode)
		modemToDataModeFlag = 1;
}

void MODEM_IsOfline(void)
{
	modem_IsOnline = 0;
	modemCmdMode = 1;
}
void MODEM_SetApn(uint8_t *apn)
{
	modemApn = apn;
}

uint8_t MODEM_TurnOn(void)
{
	MODEM_PowerOn();
	if(MODEM_SendCommand(modemOk,modemError,1000,2,"AT\r")) return 0xff;
	MODEM_SendCommand(modemOk,modemError,1000,2,"AT+CBC\r"); //Batery check status
	return 0;
}

uint8_t MODEM_PowerOff(void)
{
	while(MODEM_TurnOn());
	if(MODEM_SendCommand(modemOk,modemError,1000,3,"AT+CPWROFF\r")) return 0xff; //Power off modem
	return 0;
	
}


uint8_t MODEM_GetCSQ(void)
{
	uint8_t i,c;
	float fTemp;
	SysTick_DelayMs(100);
	MODEM_FlushBuffer();
	COMM_Puts("AT+CSQ\r");				// cancel calling if busy.
	if(!MODEM_CheckResponse("+CSQ:", 2000))
	{
		i = 0;
		while(RINGBUF_Get(commRxRingBuf,&c) == 0)
		{
				if((c == '\r') || (c == '\n'))
					break;
				csqBuff[i] = c;
				i++;
				if(i >= sizeof(csqBuff)) 
				{
					break;		
				}
		}
		csqBuff[i] = '\0';
		sscanf((char *)csqBuff,"%f",&fTemp);
		csqValue = (uint8_t)fTemp;
		if((csqValue < 4) || (csqValue == 99))
			gsmSignal = 0;
		else if(csqValue < 10)
			gsmSignal = 1;
		else if(csqValue < 16)
			gsmSignal = 2;
		else if(csqValue < 22)
			gsmSignal = 3;
		else if(csqValue < 28)
			gsmSignal = 4;
		else if(csqValue < 51)
			gsmSignal = 5;
		else 
			gsmSignal = 0;
		return 0;
	}
	return 0xff;
}




uint8_t MODEM_CheckResponse(uint8_t *str,uint32_t t)
{
	COMPARE_TYPE cmp;
	uint8_t c;
	InitFindData(&cmp,str);
	while(t--)
	{
		SysTick_DelayMs(1);
		if(RINGBUF_Get(commRxRingBuf,&c) == 0)
		{
			if(FindData(&cmp,c) == 0)
			{
				return 0;
			}
		}
	}
	return 0xff;
}


uint8_t VoiceSetup(void)
{
	
	return 0;
}


uint8_t MODEM_SendCommand(const uint8_t *resOk,const uint8_t *resFails,uint32_t timeout,uint8_t tryAgainNum,const uint8_t *format, ...)
{
	static  uint8_t  buffer[128];
	COMPARE_TYPE cmp1,cmp2;
	Timeout_Type tOut;
	uint8_t c;
        #if defined ( __ICCARM__ )
        va_list     vArgs;
        #elif defined (__CC_ARM)
         __va_list     vArgs;
        #endif
	
	
	va_start(vArgs, format);
	vsprintf((char *)&buffer[0], (char const *)format, vArgs);
	va_end(vArgs);
	COMM_Putc(0x1A);
	COMM_Putc(0x1A);
	if (tryAgainNum == 0)
	{
		tryAgainNum = 1;
	}
	while (tryAgainNum)
	{
		SysTick_DelayMs(100);
		InitFindData(&cmp1, (uint8_t *)resOk);
		InitFindData(&cmp2, (uint8_t *)resFails);
		InitTimeout(&tOut, SYSTICK_TIME_MS(timeout));
		MODEM_FlushBuffer();
		MODEM_Info("\r\nGSM->%s\r\n", buffer);
		COMM_Puts(buffer);
		while(CheckTimeout(&tOut))
		{
			if(RINGBUF_Get(commRxRingBuf, &c) == 0)
			{
				if(FindData(&cmp1, c) == 0)
				{
					MODEM_Info("\r\nGSM->%s OK\r\n",buffer);
					return 0;
				}
				if(FindData(&cmp2, c) == 0)
				{
					break;
				}
			}
		}
		tryAgainNum--;
	}
	MODEM_Info("\r\nGSM->%s FAILS\r\n",buffer);
	return 0xff;
}

void MODEM_FlushBuffer(void)
{
	uint8_t c;
	while(RINGBUF_Get(commRxRingBuf, &c)==0);
}




// is Carrier Detected
uint8_t MODEM_CarrierDetected(void)
{
	return 0;//MODEM_DCD();
}





