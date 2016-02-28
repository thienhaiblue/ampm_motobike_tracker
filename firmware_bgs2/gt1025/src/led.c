/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "led.h"


LED_TYPE	led1Ctr;	

void LedInit(void)			
{
	LED_SET_OUTPUT;
	LedSetStatus(&led1Ctr,LED_ON_TIME_DFG,LED_OFF_TIME_DFG,LED_TURN_ON,0xffffffff);
	
}

void LedSetStatus(LED_TYPE *ledCtr,uint32_t onTime,uint32_t offTime,uint32_t ledEnable,uint32_t times)		
{
	ledCtr->ledOnTime = onTime;
	ledCtr->ledOffTime = offTime;
	ledCtr->ledCounter = 0;
	ledCtr->ledEnable = ledEnable;
	ledCtr->ledTimes = times;
}													


void CtrLed(uint32_t time)
{
		static uint64_t ledTimeNew = 0,ledTimeOld = 0;
		static uint32_t timeOld = 0;
		ledTimeNew += time - timeOld;
		LedCtr(&led1Ctr,ledTimeNew - ledTimeOld);
		ledTimeOld = ledTimeNew; 
		timeOld = time;
	
	if(led1Ctr.ledStatus == LED_ON)
	{
		LED_PIN_SET;
	}
	else 
	{
		LED_PIN_CLR;
	}
}	
											
void LedCtr(LED_TYPE *ledCtr, uint32_t times)	
{
	if(ledCtr->ledEnable == LED_TURN_ON) 
	{
			if(ledCtr->ledCounter > times)
				ledCtr->ledCounter -= times;
			else ledCtr->ledCounter = 0;
				
			if(ledCtr->ledCounter == 0) 
			{
				if(ledCtr->ledTimes) 
				{
					ledCtr->ledTimes--;
					ledCtr->ledCounter = ledCtr->ledOffTime + ledCtr->ledOnTime;
					ledCtr->ledStatus = LED_ON;
				}
			}
			
			if(ledCtr->ledCounter <= ledCtr->ledOffTime) 
				ledCtr->ledStatus = LED_OFF;
	}
}

