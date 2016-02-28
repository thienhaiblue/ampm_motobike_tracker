#ifndef __AT_CMD_COMMON_PARSER_H__
#define __AT_CMD_COMMON_PARSER_H__

#include <stdint.h>
#include "stdio.h"
#include "string.h"

#if defined ( __ICCARM__ )
#pragma pack(1) 
typedef struct {
	int16_t year;			// year with all 4-digit (2011)
	int8_t month;			// month 1 - 12 (1 = Jan)
	int8_t mday;			// day of month 1 - 31
	int8_t wday;			// day of week 1 - 7 (1 = Sunday)
	int8_t hour;			// hour 0 - 23
	int8_t min;				// min 0 - 59
	int8_t sec;				// sec 0 - 59
}GSM_DATE_TIME;
#pragma pack()
#elif defined (__CC_ARM)
typedef struct __attribute__((packed)){
	int16_t year;			// year with all 4-digit (2011)
	int8_t month;			// month 1 - 12 (1 = Jan)
	int8_t mday;			// day of month 1 - 31
	int8_t wday;			// day of week 1 - 7 (1 = Sunday)
	int8_t hour;			// hour 0 - 23
	int8_t min;				// min 0 - 59
	int8_t sec;				// sec 0 - 59
}GSM_DATE_TIME;
#endif



extern uint16_t batteryPercent;

extern uint8_t gsmSimCIDBuf[32];
extern uint8_t flagGotSimCID;
extern uint8_t gsmIMEIBuf[18];
extern uint8_t flagGotIMEI;
extern uint8_t newIMEI_IsUpdate;
extern uint8_t modemIMEIPass;

extern GSM_DATE_TIME sysGsmTime;
extern uint8_t gsmGetTimeFlag;
extern int32_t sysGsmTimezone;


extern uint8_t modem_IsOnline;

uint32_t GSM_GetIMEI(uint16_t cnt,uint8_t c);
uint32_t GSM_GetSimCID(uint16_t cnt,uint8_t c);
uint32_t GsmGetTime(uint16_t cnt,uint8_t c);
uint32_t GsmGetUTCTime(uint16_t cnt,uint8_t c);
uint32_t GetBatteryStatus(uint16_t cnt,uint8_t c);
void MODEM_GoToCmdMode(void);
void MODEM_GoToDataMode(void);
void MODEM_SetApn(uint8_t *apn);
uint8_t GsmCommon_Task(void);
uint8_t GsmCommonTask_IsIdle(void);


#endif
