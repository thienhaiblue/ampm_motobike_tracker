#ifndef __TICK_H__
#define __TICK_H__
#include "stm32f10x.h"
#include <stdint.h>
#include "core_cm3.h"
#include "system_stm32f10x.h"

#define TICK_SECOND 1000

#define TIME_MS(x)	x
#define TIME_SEC(x)	(x*1000)

#define TIMEOUT		0
#define TIMEIN		1

#define WATCHDOG_NUM	3 /*MAIN + Timer0 + Timer1*/

typedef struct {
	uint32_t start_time; 		
	uint32_t timeout;
	uint32_t crc; 
} Timeout_Type;


void TICK_Init(uint32_t timeMs);
void TICK_DeInit(void);
uint32_t TICK_Get(void);
uint32_t TICK_Get64(void);
void DelayMs(uint32_t ms);
void InitTimeout(Timeout_Type *t,uint32_t timeout);
uint32_t CheckTimeout(Timeout_Type *t);

#endif

