#ifndef __LOW_POWER_MODE_H__
#define __LOW_POWER_MODE_H__
#include <stdint.h>
#include "stm32f10x.h"
#include "hw_config.h"


/* --------- Cortex System Control register bit mask ---------------- */

/* Cortex System Control register address */
#define SCB_SysCtrl              ((uint32_t)0xE000ED10)

/* SLEEPDEEP bit mask */
#define SysCtrl_SLEEPDEEP_Set    ((uint32_t)0x00000004)


/* CR register bit mask */
#define CR_PDDS_Set              ((uint32_t)0x00000002)
#define CR_DS_Mask               ((uint32_t)0xFFFFFFFC)
#define CR_CWUF_Set              ((uint32_t)0x00000004)
#define CR_PLS_Mask              ((uint32_t)0xFFFFFF1F)

#define PWR_Regulator_ON          ((uint32_t)0x00000000)
#define PWR_Regulator_LowPower    ((uint32_t)0x00000001)
#define IS_PWR_REGULATOR(REGULATOR) (((REGULATOR) == PWR_Regulator_ON) || \
                                     ((REGULATOR) == PWR_Regulator_LowPower))


#define PWR_STOPEntry_WFI         ((uint8_t)0x01)
#define PWR_STOPEntry_WFE         ((uint8_t)0x02)
#define IS_PWR_STOP_ENTRY(ENTRY) (((ENTRY) == PWR_STOPEntry_WFI) || ((ENTRY) == PWR_STOPEntry_WFE))

void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry);
void PWR_EnterSTANDBYMode(void);

#endif

