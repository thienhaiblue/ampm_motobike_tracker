#ifndef __TICK_H__
#define __TICK_H__
//#include "stm32f10x.h"
#include <stdint.h>
//#include "core_cm3.h"
//#include "system_stm32f10x.h"
void TICK_DeInit(void);
void TICK_Init(uint32_t timeMs);

#endif

