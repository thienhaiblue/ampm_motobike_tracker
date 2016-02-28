#ifndef __SYS_TICK_H__
#define __SYS_TICK_H__
#include <stdint.h>

#define SYSTICK_SECOND 1000

#define SYSTICK_TIME_MS(x)	x
#define SYSTICK_TIME_SEC(x)	(x*1000)

#define SYSTICK_TIMEOUT		0
#define SYSTICK_TIMEIN		1


typedef struct {
	uint32_t start_time; 		
	uint32_t timeout;
	uint32_t crc; 
} Timeout_Type;

void SysTick_Task(void);
void SysTick_Init(uint32_t timeMs);
void SysTick_DeInit(void);
uint32_t SysTick_Get(void);
uint32_t SysTick_Get64(void);
void SysTick_DelayMs(uint32_t ms);
void InitTimeout(Timeout_Type *t,uint32_t timeout);
uint32_t CheckTimeout(Timeout_Type *t);

#endif

