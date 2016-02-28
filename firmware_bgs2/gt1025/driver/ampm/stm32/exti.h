#ifndef __EXTI_H__
#define __EXTI_H__
#include <stdint.h>
#include "stm32f10x.h"
#include "hw_config.h"
#include "system_config.h"
#include "dbg.h"

#define EXTI_Line0       ((uint32_t)0x00001)  /* External interrupt line 0 */
#define EXTI_Line1       ((uint32_t)0x00002)  /* External interrupt line 1 */
#define EXTI_Line2       ((uint32_t)0x00004)  /* External interrupt line 2 */
#define EXTI_Line3       ((uint32_t)0x00008)  /* External interrupt line 3 */
#define EXTI_Line4       ((uint32_t)0x00010)  /* External interrupt line 4 */
#define EXTI_Line5       ((uint32_t)0x00020)  /* External interrupt line 5 */
#define EXTI_Line6       ((uint32_t)0x00040)  /* External interrupt line 6 */
#define EXTI_Line7       ((uint32_t)0x00080)  /* External interrupt line 7 */
#define EXTI_Line8       ((uint32_t)0x00100)  /* External interrupt line 8 */
#define EXTI_Line9       ((uint32_t)0x00200)  /* External interrupt line 9 */
#define EXTI_Line10      ((uint32_t)0x00400)  /* External interrupt line 10 */
#define EXTI_Line11      ((uint32_t)0x00800)  /* External interrupt line 11 */
#define EXTI_Line12      ((uint32_t)0x01000)  /* External interrupt line 12 */
#define EXTI_Line13      ((uint32_t)0x02000)  /* External interrupt line 13 */
#define EXTI_Line14      ((uint32_t)0x04000)  /* External interrupt line 14 */
#define EXTI_Line15      ((uint32_t)0x08000)  /* External interrupt line 15 */
#define EXTI_Line16      ((uint32_t)0x10000)  /* External interrupt line 16
                                                 Connected to the PVD Output */
#define EXTI_Line17      ((uint32_t)0x20000)  /* External interrupt line 17 
                                                 Connected to the RTC Alarm event */
#define EXTI_Line18      ((uint32_t)0x40000)  /* External interrupt line 18 
                                                 Connected to the USB Wakeup from 
                                                 suspend event */


extern uint8_t accelerometerFlagCnt;
extern uint8_t accelerometerFlagResetTime;
void ClearAllEventFlag(void);
void EXTI_Init(void);
uint8_t EventInit(void);

#endif

