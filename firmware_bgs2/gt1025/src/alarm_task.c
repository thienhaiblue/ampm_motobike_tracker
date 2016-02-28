/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#include "app_config_task.h"
#include "accelerometer_task.h"
#include "lib/encoding.h"

#define ALARM_Info(...) // DbgCfgPrintf(__VA_ARGS__)

#define ACC_OFF_TIME 30


const uint8_t smsgSensorAlarmWarning[] = "Xe Chuyển động";

SMS_LIST_TYPE gSensorAlarmSms;

uint8_t gSensorAlarmSmsBuf[160];

uint32_t alarmFlag = 0;


uint8_t gSensorAlarmTimes = 3;




Timeout_Type tCarAccident;

uint8_t accOffTime = ACC_OFF_TIME;

void AlarmTaskInit(void)
{
	InitTimeout(&tCarAccident,SYSTICK_TIME_SEC(1));
	memset((uint8_t *)&gSensorAlarmSms,NULL,sizeof(SMS_LIST_TYPE));
}



void AlarmTask(uint32_t rtc)
{
	uint8_t pdu2uniBuf[160];
	uint8_t smsLen;
	static uint32_t time;
	if(time != rtc)
	{
		//Check error
		if(SmsCheckMessage_IsEmpty())
		{
			if(gSensorAlarmSms.flag == SMS_NEW_MSG){gSensorAlarmSms.flag = SMS_MSG_FAIL;}
		}
		time = rtc;
		if(ACC_IS_ON)
			accOffTime = ACC_OFF_TIME; //3min
		else if(accOffTime)
			accOffTime--;
	}
	

	
	if(accOffTime)
	{
			alarmFlag &= ~ACCLEROMETER_FLAG;
			alarmFlag &= ~ CAR_BE_TOWED_AWAY_FLAG;
			gSensorAlarmTimes = 3;
	}
	else
	{
	/*Car accident (G-Sensor more than 2G) */
		if(gSensorAlarmSms.flag != SMS_NEW_MSG
		&& alarmFlag & CAR_BE_TOWED_AWAY_FLAG
		&& gSensorAlarmTimes
		)
		{
			if(CheckTimeout(&tCarAccident) == SYSTICK_TIMEOUT)
			{
				getAddrFlag = 1;
				askAddrCnt = 0;
				InitTimeout(&tCarAccident,SYSTICK_TIME_SEC(30));
				
				ALARM_Info("ALARM:Car accident!\r\n");  
				alarmFlag &= ~CAR_BE_TOWED_AWAY_FLAG;
				gSensorAlarmTimes--;
				smsLen =sprintf((char *)pdu2uniBuf,"%s!\nhttp://maps.google.com/maps?q=%.6f,%.6f",smsgSensorAlarmWarning,lastGpsInfo.lat,lastGpsInfo.lon);
				smsLen = utf8s_to_ucs2s((int16_t *)gSensorAlarmSmsBuf,pdu2uniBuf);
				big2litel_endian((uint16_t *)gSensorAlarmSmsBuf,unilen((uint16_t *)gSensorAlarmSmsBuf));
				smsLen *= 2;
				Sms_SendMsg(&gSensorAlarmSms,sysCfg.userPhone,(uint8_t *)gSensorAlarmSmsBuf,smsLen,SMS_PDU16_MODE,30000,3);
				addrSendToUserFlag = 1;
			}
			else
				alarmFlag &= ~CAR_BE_TOWED_AWAY_FLAG;
		}
	}
}

