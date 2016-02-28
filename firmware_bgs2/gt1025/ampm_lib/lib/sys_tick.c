/**
* \file
*         tick driver
* \author
*         Nguyen Van Hai <thienhaiblue@ampm.com.vn>
*/

#include <stdio.h>
#include <string.h>
#include "sys_tick.h"

volatile uint32_t sysTickTimeSec = 0;
#define GetCurrentTime  SysTick_Get


volatile uint32_t sysTickCounter32 = 0;
volatile uint64_t sysTickCounter64 = 0;


void SysTick_Init(uint32_t timeMs) 
{
					
} // end

void SysTick_DeInit(void)
{
	 
}


//interrupt
void SysTick_Task(void)
{
	sysTickCounter32++;
	sysTickCounter64++;
	if((sysTickCounter32 % 999) == 0)
		sysTickTimeSec++;
}



uint32_t SysTick_Get(void)
{
	return sysTickCounter32;
}

uint32_t SysTick_Get64(void)
{
   return sysTickCounter64;
}

void SysTick_DelayMs(uint32_t ms)
{
   	uint32_t currentTicks = SysTick_Get();
		while(SysTick_Get()- currentTicks < ms);
}



void InitTimeout(Timeout_Type *t,uint32_t timeout)
{
	t->start_time = GetCurrentTime();
	t->timeout = timeout;
	t->crc = t->start_time + t->timeout;
}

uint32_t CheckTimeout(Timeout_Type *t)
{
	uint32_t u32temp,u32temp1;
	u32temp = t->start_time + t->timeout;
	if(u32temp != t->crc) 
		return 0;
	u32temp = GetCurrentTime();
	t->crc = t->start_time + t->timeout;
	if(u32temp >= t->start_time)
		u32temp1 = u32temp - t->start_time;
	else
		u32temp1 = (0xFFFFFFFF - t->start_time) + u32temp;
	if(u32temp1 >= t->timeout) return 0;
	return (t->timeout - u32temp1);
}

void TimerDelayms(uint32_t time)
{
	uint32_t start_time,current_time;
	start_time = GetCurrentTime();
	while(1)
	{
		current_time = GetCurrentTime();
		if(current_time >= start_time)
		{
			if((current_time - start_time) >= time) break;
		}
		else
		{
			if(((0xFFFFFFFF - start_time) + current_time) >= time) break;
		}
	}
}


