#ifndef __RTC_H__
#define __RTC_H__
#include "stm32f10x.h"
#include <stdint.h>
#include "exti.h"
#include "system_config.h"
#include "hw_config.h"

#define WTD_MAIN_LOOP	0
#define WTD_TIMER0_LOOP	1
#define WTD_TIMER1_LOOP	2
#define WATCHDOG_NUM	1 /*MAIN*/ 
#define RTC_ALARM_INTERVAL 3

#define WAKEUP_TIME2	30
#define WAKEUP_TIME1	120
#define WAKEUP_TIME		20*60 //20Min
#define RTC_FREQUENCY  1//Hz
#define ALARM_FREQUENCY	4096

#define MOVING_MODE	0
#define STOP_MODE	1
 
extern volatile uint32_t watchdogFeed[WATCHDOG_NUM]; 
extern uint32_t watchdogEnable[WATCHDOG_NUM];
extern uint32_t rtcCnt; 

void RTC_Init(void);
uint32_t RTC_GetCounter(void);
void RTC_SetAlarm(uint32_t AlarmValue);
uint8_t RTC_SetCounter(uint32_t CounterValue);
void RTC_SetCounterX(uint32_t CounterValue);
void RTC_Reset(void);
#endif

