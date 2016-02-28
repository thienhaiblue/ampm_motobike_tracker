
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
#include "option/cc950.h"

const uint8_t smsChinese_HelpHelp[] = "求救、求救";
const uint8_t smsChinese_LowPowerPleaseCheckBattery[] = "車上主電源電壓過低請檢查";
const uint8_t smsChinese_BackupBatteryLowPower[] = "衛星定位備用電源電壓過低";
const uint8_t smsChinese_MotionDetect[] = "觸發震動感應器";
const uint8_t smsChinese_CarAlarmWarning[] = "原車防盜器觸發";
const uint8_t smsChinese_MicrowaveAlarmWarning[] = "超音波感應器觸發";

//const uint8_t smsChinese_HelpHelp[] = "Help Help!";
//const uint8_t smsChinese_LowPowerPleaseCheckBattery[] = "Low power!";
//const uint8_t smsChinese_BackupBatteryLowPower[] = "Low Bakup Battery";
//const uint8_t smsChinese_MotionDetect[] = "MotionDetect";
//const uint8_t smsChinese_CarAlarmWarning[] = "Car Alarm";
//const uint8_t smsChinese_MicrowaveAlarmWarning[] = "Mictowave Alarm";



SMS_LIST_TYPE carAlarmU22Sms;
SMS_LIST_TYPE carAlarmU23Sms;
SMS_LIST_TYPE carAlarmU24Sms;

SMS_LIST_TYPE carAlarmHoldU22Sms;
SMS_LIST_TYPE carAlarmHoldU23Sms;
SMS_LIST_TYPE carAlarmHoldU24Sms;

SMS_LIST_TYPE microwaveU22Sms;
SMS_LIST_TYPE microwaveU25Sms;

SMS_LIST_TYPE powerU23Sms;
SMS_LIST_TYPE powerU24Sms;

SMS_LIST_TYPE batteryU23Sms;
SMS_LIST_TYPE batteryU24Sms;

SMS_LIST_TYPE sosU26Sms;
SMS_LIST_TYPE sosU27Sms;
SMS_LIST_TYPE sosU28Sms;

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
uint16_t alarmPulseCnt = 0;

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
uint8_t alarmAlarmSentTimes = 0;
uint8_t haveAlarmSignal = 0;

uint8_t sosChangeCnt = 0;
uint32_t sosStartTime = 0;
uint8_t sosStatusChange = 0;
uint32_t sosStatusHoldStartTime = 0;
uint8_t sosAlarmSentTimes = 0;
uint8_t accOffTime = 180;



void AlarmHoldDetect(void)
{
	static uint32_t time = 0;
	
	if(time != rtcTimeSec) //pulse per 1 sec
	{
		time = rtcTimeSec;
		if(alarmPulseCnt >= 4)
		{
			haveAlarmSignal = 4;
		}
		else
		{
			if(alarmPulseCnt <= 2 && (!ACC_IS_ON))
				accOffTime = 0;
			haveAlarmSignal = 0;
			alarmStatusHoldStartTime = time;
		}
		alarmPulseCnt = 0;
	}

	if(haveAlarmSignal)
	{
		if(rtcTimeSec - alarmStatusHoldStartTime >= 4)
		{
			if(alarmAlarmSentTimes < 3)
			{
				alarmStatusHoldStartTime = rtcTimeSec;
				alarmFlag |= CAR_ALARM_HOLD_IN_4SEC_FLAG;
			}
		}
	}
	else
	{
		alarmAlarmSentTimes = 0;
		alarmStatusChange = 0;
	}

}

void SosHoldDetect(void)
{
	if(sosStatusChange)
	{
		if(GET_AVLIO3_PIN == 0)
		{
			if(rtcTimeSec - sosStatusHoldStartTime >= 4)
			{
				if(sosAlarmSentTimes < 3)
				{
					sosStatusHoldStartTime = rtcTimeSec;
					alarmFlag |= SOS_ALARM_FLAG;
				}
			}
		}
		else
		{
			sosAlarmSentTimes = 0;
			sosStatusChange = 0;
		}
	}
}

void EXTI0_IRQHandler(void)
{
	EXTI->PR = EXTI_Line0;
	IO_Control(NULL);
	alarmFlag |= ACC_ON_OFF_FLAG;
}

void EXTI1_IRQHandler(void)
{
	EXTI->PR = EXTI_Line1;
	IO_Control(NULL);
	alarmPulseCnt++;
	if(alarmStatusChange == 0)
	{
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
		EXTI->PR = EXTI_Line5;
		
		IO_Control(NULL);
		
		sosStatusChange = 1;
		sosStatusHoldStartTime = rtcTimeSec;
		if(rtcTimeSec - sosStartTime > 4)
			sosChangeCnt = 0;
		if(sosChangeCnt == 0)
			sosStartTime = rtcTimeSec;
		sosChangeCnt++;
		if(sosChangeCnt >= 3)
		{
			if(rtcTimeSec - sosStartTime <= 4)
				alarmFlag |= SOS_ALARM_FLAG;
			sosChangeCnt = 0;
		}
		
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
		
		IO_Control(NULL);
		
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



void AlarmTask(uint32_t rtc)
{
	uint16_t wchar;
	uint8_t smsLen;
	static uint32_t time;
	if(time != rtc)
	{
		time = rtc;
		if(ACC_IS_ON)
			accOffTime = 180; //3min
		else if(accOffTime)
			accOffTime--;
	}
	
	if(accOffTime)
	{
			alarmFlag &= ~CAR_ALARM_HOLD_IN_4SEC_FLAG;
			alarmFlag &= ~MICROWAVE_ALARM_FLAG;
			alarmFlag &= ~MILEAGE_REMIND_FLAG;
			alarmFlag &= ~ACC_OFF_3MIN_FLAG;
			alarmFlag &= ~SPEED_ALARM_FLAG;
	}
	else
	{
	
		/*Car Alarm*/
		if(carAlarmHoldU22Sms.flag != SMS_NEW_MSG
		&& carAlarmHoldU23Sms.flag != SMS_NEW_MSG
		&& carAlarmHoldU24Sms.flag != SMS_NEW_MSG
		&& alarmFlag & CAR_ALARM_HOLD_IN_4SEC_FLAG
		)
		{
			alarmFlag &= ~CAR_ALARM_HOLD_IN_4SEC_FLAG;
			
			alarmAlarmSentTimes++;
			
			smsLen = ConverBig5_Unicode((uint16_t *)carAlarmHoldSmsBuf,smsChinese_CarAlarmWarning);
			sprintf((char *)pdu2uniBuf,"!\nhttp://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
			smsLen += ConverASCII_Unicode(&carAlarmHoldSmsBuf[smsLen*2],pdu2uniBuf);
			smsLen *= 2;
			
			//sprintf((char *)&carAlarmHoldSmsBuf,"Car Alarm the status changed more than 4 second: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
			Sms_SendMsg(&carAlarmHoldU22Sms,sysCfg.u22Phone,(uint8_t *)carAlarmHoldSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u22Phone,(char *)sysCfg.u23Phone) == NULL)
			{
				Sms_SendMsg(&carAlarmHoldU23Sms,sysCfg.u23Phone,(uint8_t *)carAlarmHoldSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
				VoiceCallTask_StartCall(sysCfg.u23Phone,14);
			}
			if(ComparePhoneNumber((char *)sysCfg.u22Phone,(char *)sysCfg.u24Phone) == NULL)
				Sms_SendMsg(&carAlarmHoldU24Sms,sysCfg.u24Phone,(uint8_t *)carAlarmHoldSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
		}
		
	/*Car accident (G-Sensor more than 2G) */

	/*Microwave detect Alarm*/
		if(microwaveU22Sms.flag != SMS_NEW_MSG
		&& microwaveU25Sms.flag != SMS_NEW_MSG
		&& alarmFlag & MICROWAVE_ALARM_FLAG
		)
		{
			alarmFlag &= ~MICROWAVE_ALARM_FLAG;
			
			smsLen = ConverBig5_Unicode((uint16_t *)microwaveSmsBuf,smsChinese_MicrowaveAlarmWarning);
			sprintf((char *)pdu2uniBuf,"!\nhttp://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
			smsLen += ConverASCII_Unicode(&microwaveSmsBuf[smsLen*2],pdu2uniBuf);
			smsLen *= 2;
			
			//sprintf((char *)&microwaveSmsBuf,"Microwave: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
			Sms_SendMsg(&microwaveU22Sms,sysCfg.u22Phone,(uint8_t *)microwaveSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u22Phone,(char *)sysCfg.u24Phone) == NULL)
				Sms_SendMsg(&microwaveU25Sms,sysCfg.u25Phone,(uint8_t *)microwaveSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			VoiceCallTask_StartCall(sysCfg.u23Phone,14);
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
			Sms_SendMsg(&mileageRemindU23Sms,sysCfg.u23Phone,(uint8_t *)mileageRemindSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
				Sms_SendMsg(&mileageRemindU36Sms,sysCfg.u36Phone,(uint8_t *)mileageRemindSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
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
			Sms_SendMsg(&accOffU23Sms,sysCfg.u23Phone,(uint8_t *)accOffSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u30Phone) == NULL)
				Sms_SendMsg(&accOffU30Sms,sysCfg.u30Phone,(uint8_t *)accOffSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
				Sms_SendMsg(&accOffU31Sms,sysCfg.u31Phone,(uint8_t *)accOffSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
				Sms_SendMsg(&accOffU36Sms,sysCfg.u36Phone,(uint8_t *)accOffSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
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
			Sms_SendMsg(&speedAlarmU23Sms,sysCfg.u23Phone,(uint8_t *)speedAlarmSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u24Phone) == NULL)
				Sms_SendMsg(&speedAlarmU24Sms,sysCfg.u30Phone,(uint8_t *)speedAlarmSmsBuf,smsLen,SMS_PDU_MODE,30000,3);

		}
	}
	
	/*SOS Alarm (4second)*/
		if(sosU26Sms.flag != SMS_NEW_MSG 
			&& sosU27Sms.flag != SMS_NEW_MSG
			&& sosU28Sms.flag != SMS_NEW_MSG
			&& alarmFlag & SOS_ALARM_FLAG
		)
		{
			alarmFlag &= ~SOS_ALARM_FLAG;
			sosAlarmSentTimes++;
			smsLen = ConverBig5_Unicode((uint16_t *)sosSmsBuf,smsChinese_HelpHelp);
			sprintf((char *)pdu2uniBuf,"!\nhttp://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
			smsLen += ConverASCII_Unicode(&sosSmsBuf[smsLen*2],pdu2uniBuf);

			Sms_SendMsg(&sosU26Sms,sysCfg.u26Phone,(uint8_t *)sosSmsBuf,smsLen*2,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u26Phone,(char *)sysCfg.u27Phone) == NULL)
				Sms_SendMsg(&sosU27Sms,sysCfg.u27Phone,(uint8_t *)sosSmsBuf,smsLen*2,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u26Phone,(char *)sysCfg.u28Phone) == NULL)
				Sms_SendMsg(&sosU28Sms,sysCfg.u28Phone,(uint8_t *)sosSmsBuf,smsLen*2,SMS_PDU_MODE,30000,3);
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
			Sms_SendMsg(&dailyReportU23Sms,sysCfg.u23Phone,(uint8_t *)dailyReportSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u30Phone) == NULL)
				Sms_SendMsg(&dailyReportU30Sms,sysCfg.u30Phone,(uint8_t *)dailyReportSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u31Phone) == NULL)
				Sms_SendMsg(&dailyReportU31Sms,sysCfg.u31Phone,(uint8_t *)dailyReportSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
				Sms_SendMsg(&dailyReportU36Sms,sysCfg.u36Phone,(uint8_t *)dailyReportSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
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
			Sms_SendMsg(&timeRemindU23Sms,sysCfg.u23Phone,(uint8_t *)timeRemindSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u36Phone) == NULL)
				Sms_SendMsg(&timeRemindU36Sms,sysCfg.u36Phone,(uint8_t *)timeRemindSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
		}
		
		/*Car Battery Low Alarm (10.5V)*/
		if(ADC1_10_Value < 9.5 && ADC_Updated
			&& powerU23Sms.flag != SMS_NEW_MSG
			&& powerU24Sms.flag != SMS_NEW_MSG
			&& ((alarmFlag & POWER_ALARM_FLAG) == 0)
		)
		{
			alarmFlag |= POWER_ALARM_FLAG;
			
			smsLen = ConverBig5_Unicode((uint16_t *)powerSmsBuf,smsChinese_LowPowerPleaseCheckBattery);
			sprintf((char *)pdu2uniBuf,"!\nhttp://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
			smsLen += ConverASCII_Unicode(&powerSmsBuf[smsLen*2],pdu2uniBuf);
			smsLen *= 2;
			
			//sprintf((char *)&powerSmsBuf,"Low power: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
			Sms_SendMsg(&powerU23Sms,sysCfg.u23Phone,(uint8_t *)powerSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			if(ComparePhoneNumber((char *)sysCfg.u23Phone,(char *)sysCfg.u24Phone) == NULL)
				Sms_SendMsg(&powerU24Sms,sysCfg.u24Phone,(uint8_t *)powerSmsBuf,smsLen,SMS_PDU_MODE,30000,3);
		}
		else if(ADC1_10_Value > 11.0 && ADC_Updated)
		{
			alarmFlag &= ~POWER_ALARM_FLAG;
		}
		
	/*internal Backup Battery Low Alarm (3.6V)*/
		if(ADC1_11_Value < 3.6 && ADC_Updated
			&& batteryU23Sms.flag != SMS_NEW_MSG
			&& batteryU24Sms.flag != SMS_NEW_MSG
			&& ((alarmFlag & BATTERY_ALARM_FLAG) == 0)
		)
		{
			alarmFlag |= BATTERY_ALARM_FLAG;
		
			smsLen = ConverBig5_Unicode((uint16_t *)batterySmsBuf,smsChinese_BackupBatteryLowPower);
			sprintf((char *)pdu2uniBuf,"!\nhttp://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
			smsLen += ConverASCII_Unicode(&batterySmsBuf[smsLen*2],pdu2uniBuf);
			smsLen *= 2;
		
			//sprintf((char *)&batterySmsBuf,"Battery Low: http://maps.google.com/maps?q=%.8f,%.8f\n",lastGpsInfo.lat,lastGpsInfo.lon);
			Sms_SendMsg(&batteryU23Sms,sysCfg.u23Phone,(uint8_t *)batterySmsBuf,smsLen,SMS_PDU_MODE,30000,3);
			Sms_SendMsg(&batteryU24Sms,sysCfg.u24Phone,(uint8_t *)batterySmsBuf,smsLen,SMS_PDU_MODE,30000,3);
		}
		else if(ADC1_11_Value >= 3.7 && ADC_Updated)
		{
			alarmFlag &= ~BATTERY_ALARM_FLAG;
		}
	
}

