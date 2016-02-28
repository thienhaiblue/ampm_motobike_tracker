#ifndef __RTC_H__
#define __RTC_H__
#include "stm32f10x.h"
#include <stdint.h>
#include "exti.h"
#include "system_config.h"
#include "hw_config.h"

extern Timeout_Type MCU_WakeupTimeout;
void RTC_Init(void);
uint32_t RTC_GetCounter(void);
void RTC_SetAlarm(uint32_t AlarmValue);
#endif

