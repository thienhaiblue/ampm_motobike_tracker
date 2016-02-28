/**
* \file
*         tick driver
* \author
*         Nguyen Van Hai <thienhaiblue@ampm.com.vn>
*/

#include <stdio.h>
#include <string.h>
#include "tick.h"

//#define __NVIC_PRIO_BITS 4
volatile uint32_t tickTimeSec = 0;
#define GetCurrentTime  TICK_Get


volatile uint32_t tickCounter32 = 0;
volatile uint64_t tickCounter64 = 0;

extern void AT_CommandCtl(void);
extern void disk_timerproc (void);

void TICK_Init(uint32_t timeMs) 
{
  SysTick->LOAD  = ((SystemCoreClock/1000)*timeMs - 1);      // set reload register
	/* preemption = TICK_PRIORITY, sub-priority = 1 */
	NVIC_SetPriority(SysTick_IRQn, ((0x01<<3)| 1));
  SysTick->VAL   =  0;                                          // clear  the counter
  SysTick->CTRL = 0x07;                       
} // end

void TICK_DeInit(void)
{
	SysTick->CTRL = 0x00; 
}


//interrupt
void SysTick_Handler(void)
{
	uint32_t temp;
	tickCounter32++;
	tickCounter64++;
	if((tickCounter32 % 999) == 0)
		tickTimeSec++;
}



uint32_t TICK_Get(void)
{
	return tickCounter32;
}

uint32_t TICK_Get64(void)
{
   return tickCounter64;
}

void DelayMs(uint32_t ms)
{
   	uint32_t currentTicks = TICK_Get();
		while(TICK_Get()- currentTicks < ms);
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
		NVIC_SystemReset();
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


