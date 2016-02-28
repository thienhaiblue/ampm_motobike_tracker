#ifndef __EXTI_H__
#define __EXTI_H__
#include <stdint.h>
#include "stm32f10x.h"
#include "hw_config.h"
#include "system_config.h"
#include "dbg_cfg_app.h"

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


#define MCU_WAKEUP_FLAG			0x00000001
#define RTC_ALARM_FLAG			0x00000002
#define ENGINE_ON_FLAG			0x00000004
#define MODEM_SMS_FLAG			0x00000008
#define MODEM_CALL_FLAG			0x00000010
#define MODEM_RI_FLAG				0x00000020
#define ACC_FLAG	          0x00000040
#define RF_GDO0_FLAG	      0x00000080
#define SOS_FLAG	          0x00000100
#define ACCELEROMETER_FLAG	0x00000200
#define SECURITY_ON_FLAG		0x00000400
#define GPS_SMS_SEND_FLAG		0x00000800
#define USB_WAKEUP_FLAG			0x00001000
#define RF_RECIEVED_DATA_FLAG	0x00002000
#define THEFT_ALARM_FLAG	0x00004000
#define HORN_ON_FLAG			0x00008000
#define POWER_FAILS_FLAG	0x00010000


typedef struct {
	uint8_t flagAccelerometer;
	uint8_t flagSOS;
	uint8_t flagACC;
	uint8_t flagModemRI;
	uint8_t flagUSBWakeUp;
	uint8_t flagSMS;
	uint8_t flagCall;
	uint8_t flagRTCAlarm;
	uint8_t flagMCUWakeUp;
	uint8_t flagGPSSendToSMS;
	uint8_t flagEngineStatus;
	uint8_t flagSecurity;
	uint8_t flagRF;
	uint8_t flagGDO0;
	uint8_t crc;
}EVENT_FLAG_TYPE;

//extern EVENT_FLAG_TYPE	event;
extern EVENT_FLAG_TYPE	event __attribute__((zero_init));
extern uint32_t sysEventFlag __attribute__((zero_init));
extern uint32_t accelerometerFlagCnt;

void ClearAllEventFlag(void);
void EXTI_Init(void);
uint8_t EventInit(void);

#endif

