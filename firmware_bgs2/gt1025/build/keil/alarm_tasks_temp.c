
#include "modem.h"
#include "system_config.h"
#include "lib/sparser.h"
#include "lib/sys_tick.h"
#include "lib/sys_time.h"
#include "at_command_parser.h"
#include "at_gsm_task.h"
#include "at_send_cmd_task.h"
#include "at_cmd_sms_parser.h"
#include "at_cmd_call_parser.h"
#include "lib/list.h"
#include "sst25.h"
#include "gps/gps.h"
#include "adc_task.h"
#include "sms_task.h"
#include "call_task.h"
#include "exti.h"
#include "hw_config.h"
#include "rtc.h"
#include "ff.h"

SMS_LIST_TYPE carAlarmU22Sms;
SMS_LIST_TYPE carAlarmU23Sms;
SMS_LIST_TYPE carAlarmU24Sms;

SMS_LIST_TYPE carAlarmHoldU22Sms;
SMS_LIST_TYPE carAlarmHoldU23Sms;
SMS_LIST_TYPE carAlarmHoldU24Sms;

SMS_LIST_TYPE microwaveU22Sms;
SMS_LIST_TYPE microwaveU24Sms;

SMS_LIST_TYPE powerU23Sms;
SMS_LIST_TYPE powerU24Sms;

SMS_LIST_TYPE batteryU23Sms;
SMS_LIST_TYPE batteryU24Sms;

SMS_LIST_TYPE sosU26Sms;
SMS_LIST_TYPE sosU27Sms;

SMS_LIST_TYPE accOffU23Sms;
SMS_LIST_TYPE accOffU30Sms;
SMS_LIST_TYPE accOffU31Sms;
SMS_LIST_TYPE accOffU36Sms;

SMS_LIST_TYPE dailyReportU23Sms;
SMS_LIST_TYPE dailyReportU30Sms;
SMS_LIST_TYPE dailyReportU31Sms;
SMS_LIST_TYPE dailyReportU36Sms;

SMS_LIST_TYPE speedAlarmU23Sms;
SMS_LIST_TYPE speedAlarmU24Sms;

SMS_LIST_TYPE timeRemindU23Sms;
SMS_LIST_TYPE timeRemindU36Sms;

SMS_LIST_TYPE mileageRemindU23Sms;
SMS_LIST_TYPE mileageRemindU36Sms;

uint8_t pdu2uniBuf[256];

uint8_t carAlarmSmsBuf[160];
uint8_t carAlarmHoldSmsBuf[160];
uint8_t microwaveSmsBuf[160];
uint8_t powerSmsBuf[160];
uint8_t batterySmsBuf[160];
uint8_t sosSmsBuf[160];
uint8_t accOffSmsBuf[160];
uint8_t timeRemindSmsBuf[160];
uint8_t mileageRemindSmsBuf[160];
uint8_t dailyReportSmsBuf[160];
uint8_t speedAlarmSmsBuf[160];

uint32_t alarmFlag = 0;

extern uint32_t RingCallProcess(uint16_t cnt,uint8_t c); 

uint8_t carAlarmFlag = 0;
uint8_t microwaveAlarmFlag = 1;
uint8_t powerAlarmFlag = 1;
uint8_t batteryAlarmFlag = 0;
uint8_t sosAlarmActiveFlag = 0;
uint8_t timeRemindActiveFlag = 0;
uint8_t accOffAlarmFlag = 0;
uint8_t mileageRemindActiveFlag = 0;
uint8_t dailyAlarmFlag = 0;
uint8_t speedAlarmFlag = 0;

uint8_t alarmChangeCnt = 0;
uint32_t alarmStartTime = 0;
uint8_t alarmStatusChange = 0;
uint32_t alarmStatusHoldStartTime = 0;
void AlarmHoldDetect(void)
{
	if(alarmStatusChange)
	{
		if(GET_AVLIO2_PIN == 0)
		{
			if(rtcTimeSec - alarmStatusHoldStartTime >= 4)
			{
				alarmStatusChange = 0;
				alarmFlag |= CAR_ALARM_HOLD_IN_4SEC_FLAG;
			}
		}
		else
		{
			alarmStatusChange = 0;
		}
	}
}

void EXTI0_IRQHandler(void)
{
	EXTI->PR = EXTI_Line0;
	alarmFlag |= ACC_ON_OFF_FLAG;
}

void EXTI1_IRQHandler(void)
{
	EXTI->PR = EXTI_Line1;
	alarmStatusChange = 1;
	alarmStatusHoldStartTime = rtcTimeSec;
	if(rtcTimeSec - alarmStartTime > 4)
		alarmChangeCnt = 0;
	if(alarmChangeCnt == 0)
		alarmStartTime = rtcTimeSec;
	alarmChangeCnt++;
	if(alarmChangeCnt >= 3)
	{
		if(rtcTimeSec - alarmStartTime <= 4)
			alarmFlag |= CAR_ALARM_PRESS_3TIMES_IN_4SEC_FLAG;
		alarmChangeCnt = 0;
		
	}
}

void EXTI2_IRQHandler(void)
{
	EXTI->PR = EXTI_Line2;
	alarmFlag |= ACCLEROMETER_FLAG;
	
}


void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR & EXTI_Line5)
	{
		alarmFlag |= SOS_ALARM_FLAG;
		EXTI->PR = EXTI_Line5;
	}
	if(EXTI->PR & EXTI_Line6)
	{
		EXTI->PR = EXTI_Line6;
	}
	if(EXTI->PR & EXTI_Line8)
	{
		RingCallProcess(NULL,NULL);
		EXTI->PR = EXTI_Line8;
	}
	if(EXTI->PR & EXTI_Line6)
	{
		//GPS_SetPulse();
		EXTI->PR = EXTI_Line6;
	}
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & EXTI_Line14)
	{
		EXTI->PR = EXTI_Line14;
		alarmFlag |= MICROWAVE_ALARM_FLAG;
	}
}

void USBWakeUp_IRQHandler(void)
{
	EXTI->PR = EXTI_Line18;
}


void AlarmTaskInit(void)
{
	
}


#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x40
#define _DS1E	0x7E
#define _DS2S	0xA1
#define _DS2E	0xFE

/* Character code support macros */
#define IsUpper(c)	(((c)>='A')&&((c)<='Z'))
#define IsLower(c)	(((c)>='a')&&((c)<='z'))
#define IsDigit(c)	(((c)>='0')&&((c)<='9'))

#if _DF1S		/* Code page is DBCS */

#ifdef _DF2S	/* Two 1st byte areas */
#define IsDBCS1(c)	(((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E) || ((BYTE)(c) >= _DF2S && (BYTE)(c) <= _DF2E))
#else			/* One 1st byte area */
#define IsDBCS1(c)	((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E)
#endif

#ifdef _DS3S	/* Three 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E) || ((BYTE)(c) >= _DS3S && (BYTE)(c) <= _DS3E))
#else			/* Two 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E))
#endif

#else			/* Code page is SBCS */

#define IsDBCS1(c)	0
#define IsDBCS2(c)	0

#endif /* _DF1S */

int16_t ConverBig5_Unicode(uint8_t *out	/* Pointer to pointer to the segment in the path string */,
		const uint8_t *path	/* Pointer to pointer to the segment in the path string */
)
{
	uint32_t i, ni, si, di;
	uint16_t w,*wo = (uint16_t *)out;
	uint8_t b, cf;
	const uint8_t *p = path;
	si = di = 0;
	for (;;) {
		w = p[si++];					/* Get a character */
		if(w ==NULL)
		{
			return di;
		}
		if (di >= 256)				/* Reject too long name */
			return di;
		w = ff_convert(w, 1);			/* Convert ANSI/OEM to Unicode */
		if (!w) return 0;	/* Reject invalid code */
		wo[di++] = w;					/* Store the Unicode char */
	}
}

void AlarmTask(uint32_t rtc)
{
	uint16_t wchar;
	 /*Car Alarm*/
	if(carAlarmU22Sms.flag != SMS_NEW_MSG
	&& carAlarmU23Sms.flag != SMS_NEW_MSG
	&& carAlarmU24Sms.flag != SMS_NEW_MSG
	&& alarmFlag & CAR_ALARM_PRESS_3TIMES_IN_4SEC_FLAG
	)
	{
		alarmFlag &= ~CAR_ALARM_PRESS_3TIMES_IN_4SEC_FLAG;
		sprintf((char *)&carAlarmSmsBuf,"Car Alarm in 4 second the status changed more than 3 times: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		Sms_SendMsg(&carAlarmU22Sms,sysCfg.u22Phone,(uint8_t *)carAlarmSmsBuf,strlen(carAlarmSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u22Phone,(char *)sysCfg.u23Phone) == NULL)
			Sms_SendMsg(&carAlarmU23Sms,sysCfg.u23Phone,(uint8_t *)carAlarmSmsBuf,strlen(carAlarmSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u22Phone,(char *)sysCfg.u24Phone) == NULL)
			Sms_SendMsg(&carAlarmU24Sms,sysCfg.u24Phone,(uint8_t *)carAlarmSmsBuf,strlen(carAlarmSmsBuf),30000,3);
	}
	/*Car Alarm*/
	else if(carAlarmHoldU22Sms.flag != SMS_NEW_MSG
	&& carAlarmHoldU23Sms.flag != SMS_NEW_MSG
	&& carAlarmHoldU24Sms.flag != SMS_NEW_MSG
	&& alarmFlag & CAR_ALARM_HOLD_IN_4SEC_FLAG
	)
	{
		alarmFlag &= ~CAR_ALARM_HOLD_IN_4SEC_FLAG;
		sprintf((char *)&carAlarmHoldSmsBuf,"Car Alarm the status changed more than 4 second: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		Sms_SendMsg(&carAlarmHoldU22Sms,sysCfg.u22Phone,(uint8_t *)carAlarmHoldSmsBuf,strlen(carAlarmHoldSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u22Phone,(char *)sysCfg.u23Phone) == NULL)
			Sms_SendMsg(&carAlarmHoldU23Sms,sysCfg.u23Phone,(uint8_t *)carAlarmHoldSmsBuf,strlen(carAlarmHoldSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u22Phone,(char *)sysCfg.u24Phone) == NULL)
			Sms_SendMsg(&carAlarmHoldU24Sms,sysCfg.u24Phone,(uint8_t *)carAlarmHoldSmsBuf,strlen(carAlarmHoldSmsBuf),30000,3);
	}
	
/*Car accident (G-Sensor more than 2G) */

/*Microwave detect Alarm*/
	if(microwaveU22Sms.flag != SMS_NEW_MSG
	&& microwaveU24Sms.flag != SMS_NEW_MSG
	&& alarmFlag & MICROWAVE_ALARM_FLAG
	)
	{
		alarmFlag &= ~MICROWAVE_ALARM_FLAG;
		sprintf((char *)&microwaveSmsBuf,"Microwave: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		Sms_SendMsg(&microwaveU22Sms,sysCfg.u22Phone,(uint8_t *)microwaveSmsBuf,strlen(microwaveSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u22Phone,(char *)sysCfg.u24Phone) == NULL)
			Sms_SendMsg(&microwaveU24Sms,sysCfg.u24Phone,(uint8_t *)microwaveSmsBuf,strlen(microwaveSmsBuf),30000,3);
	}

/*Car Battery Low Alarm (10.5V)*/
	if(ADC1_10_Value < 9.5 && ADC_Updated
		&& powerU23Sms.flag != SMS_NEW_MSG
		&& powerU24Sms.flag != SMS_NEW_MSG
		&& ((alarmFlag & POWER_ALARM_FLAG) == 0)
	)
	{
		alarmFlag |= POWER_ALARM_FLAG;
		sprintf((char *)&powerSmsBuf,"Low power: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		Sms_SendMsg(&powerU23Sms,sysCfg.u23Phone,(uint8_t *)powerSmsBuf,strlen(powerSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u24Phone) == NULL)
			Sms_SendMsg(&powerU24Sms,sysCfg.u24Phone,(uint8_t *)powerSmsBuf,strlen(powerSmsBuf),30000,3);
	}
	else if(ADC1_10_Value > 11.0 && ADC_Updated)
	{
		alarmFlag &= ~POWER_ALARM_FLAG;
	}
/*internal Backup Battery Low Alarm (3.6V)*/
//	if(ADC1_11_Value < 3.6 && ADC_Updated
//		&& batteryU23Sms.flag != SMS_NEW_MSG
//		&& batteryU24Sms.flag != SMS_NEW_MSG
//		&& ((alarmFlag & BATTERY_ALARM_FLAG) == 0)
//	)
//	{
//		alarmFlag |= BATTERY_ALARM_FLAG;
//		sprintf((char *)&batterySmsBuf,"Battery Low: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
//		Sms_SendMsg(&batteryU23Sms,sysCfg.u23Phone,(uint8_t *)batterySmsBuf,strlen(batterySmsBuf),30000,3);
//		Sms_SendMsg(&batteryU24Sms,sysCfg.u24Phone,(uint8_t *)batterySmsBuf,strlen(batterySmsBuf),30000,3);
//	}
//	else if(ADC1_11_Value >= 3.7 && ADC_Updated)
//	{
//		alarmFlag &= ~BATTERY_ALARM_FLAG;
//	}
/*SOS Alarm (4second)*/
	if(sosU26Sms.flag != SMS_NEW_MSG 
		&& sosU27Sms.flag != SMS_NEW_MSG
		&& alarmFlag & SOS_ALARM_FLAG
	)
	{
		alarmFlag &= ~SOS_ALARM_FLAG;
		wchar = ff_convert('©I',1);
		sosSmsBuf[0] = ((uint8_t *)&wchar)[0];
		sosSmsBuf[1] = ((uint8_t *)&wchar)[1];
		sprintf((char *)pdu2uniBuf,"SOS Alarm http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		ConverBig5_Unicode(&sosSmsBuf[2],pdu2uniBuf);
		
		
		Sms_SendMsg(&sosU26Sms,sysCfg.u26Phone,(uint8_t *)sosSmsBuf,strlen(sosSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u26Phone,(char *)sysCfg.u27Phone) == NULL)
			Sms_SendMsg(&sosU27Sms,sysCfg.u27Phone,(uint8_t *)sosSmsBuf,strlen(sosSmsBuf),30000,3);
	}
/*Inspection Time Remind*/
	if(
		timeRemindU23Sms.flag != SMS_NEW_MSG 
		&& timeRemindU36Sms.flag != SMS_NEW_MSG 
		&& alarmFlag & TIME_REMIND_FLAG
	)
	{
		alarmFlag &= ~TIME_REMIND_FLAG;
		sprintf((char *)batterySmsBuf,"Time Remind: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		Sms_SendMsg(&timeRemindU23Sms,sysCfg.u23Phone,(uint8_t *)timeRemindSmsBuf,strlen(timeRemindSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
			Sms_SendMsg(&timeRemindU36Sms,sysCfg.u36Phone,(uint8_t *)timeRemindSmsBuf,strlen(timeRemindSmsBuf),30000,3);
	}
/*Inspection mileage Remind*/
	if(
		mileageRemindU23Sms.flag != SMS_NEW_MSG 
		&& mileageRemindU36Sms.flag != SMS_NEW_MSG 
		&&  alarmFlag & MILEAGE_REMIND_FLAG
	)
	{
		alarmFlag &= ~MILEAGE_REMIND_FLAG;
		sprintf((char *)mileageRemindSmsBuf,"Mileage Remind: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		Sms_SendMsg(&mileageRemindU23Sms,sysCfg.u23Phone,(uint8_t *)mileageRemindSmsBuf,strlen(mileageRemindSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
			Sms_SendMsg(&mileageRemindU36Sms,sysCfg.u36Phone,(uint8_t *)mileageRemindSmsBuf,strlen(mileageRemindSmsBuf),30000,3);
	}
/*ACC OFF 3 minutes*/
	if(
		accOffU23Sms.flag != SMS_NEW_MSG 
		&& accOffU30Sms.flag != SMS_NEW_MSG 
		&& accOffU31Sms.flag != SMS_NEW_MSG 
		&& accOffU36Sms.flag != SMS_NEW_MSG 
	  && alarmFlag & ACC_OFF_3MIN_FLAG
	)
	{
		alarmFlag &= ~ACC_OFF_3MIN_FLAG;
		sprintf((char *)accOffSmsBuf,"ACC OFF: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		Sms_SendMsg(&accOffU23Sms,sysCfg.u23Phone,(uint8_t *)accOffSmsBuf,strlen(accOffSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u30Phone) == NULL)
			Sms_SendMsg(&accOffU30Sms,sysCfg.u30Phone,(uint8_t *)accOffSmsBuf,strlen(accOffSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
			Sms_SendMsg(&accOffU31Sms,sysCfg.u31Phone,(uint8_t *)accOffSmsBuf,strlen(accOffSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
			Sms_SendMsg(&accOffU36Sms,sysCfg.u36Phone,(uint8_t *)accOffSmsBuf,strlen(accOffSmsBuf),30000,3);
	}
/*Daily Report*/
	if(
		dailyReportU23Sms.flag != SMS_NEW_MSG 
		&& dailyReportU30Sms.flag != SMS_NEW_MSG 
		&& dailyReportU31Sms.flag != SMS_NEW_MSG 
		&& dailyReportU36Sms.flag != SMS_NEW_MSG 
		&& alarmFlag & DAILY_ALARM_FLAG
	)
	{
		alarmFlag &= ~DAILY_ALARM_FLAG;
		sprintf((char *)dailyReportSmsBuf,"Daily Report: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		Sms_SendMsg(&dailyReportU23Sms,sysCfg.u23Phone,(uint8_t *)dailyReportSmsBuf,strlen(dailyReportSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u30Phone) == NULL)
			Sms_SendMsg(&dailyReportU30Sms,sysCfg.u30Phone,(uint8_t *)dailyReportSmsBuf,strlen(dailyReportSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u31Phone) == NULL)
			Sms_SendMsg(&dailyReportU31Sms,sysCfg.u31Phone,(uint8_t *)dailyReportSmsBuf,strlen(dailyReportSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
			Sms_SendMsg(&dailyReportU36Sms,sysCfg.u36Phone,(uint8_t *)dailyReportSmsBuf,strlen(dailyReportSmsBuf),30000,3);
	}
/*Speed Alarm*/
	if(
		speedAlarmU23Sms.flag != SMS_NEW_MSG 
		&& speedAlarmU24Sms.flag != SMS_NEW_MSG 
		&&  alarmFlag & SPEED_ALARM_FLAG
	)
	{
		alarmFlag &= ~SPEED_ALARM_FLAG;
		sprintf((char *)speedAlarmSmsBuf,"Speed Alarm: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
		Sms_SendMsg(&speedAlarmU23Sms,sysCfg.u23Phone,(uint8_t *)speedAlarmSmsBuf,strlen(speedAlarmSmsBuf),30000,3);
		if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u24Phone) == NULL)
			Sms_SendMsg(&speedAlarmU24Sms,sysCfg.u30Phone,(uint8_t *)speedAlarmSmsBuf,strlen(speedAlarmSmsBuf),30000,3);

	}
}
