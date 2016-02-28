/**
* \file
*         tick driver
* \author
*         Nguyen Van Hai <thienhaiblue@ampm.com.vn>
*/

#include <stdio.h>
#include <string.h>
#include "lib/sys_tick.h"
#include "tick.h"
#include "alarm_task.h"
#include "stm32f10x_iwdg.h"

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
	SysTick_Task();
}



