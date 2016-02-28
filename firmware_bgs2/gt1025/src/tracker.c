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
#include "tracker.h"
#include <math.h>
#include "system_config.h"
#include "gps/gps.h"
#include "db.h"
#include "rtc.h"
#include "lib/sys_time.h"
#include "adc_task.h"
#include "database_app.h"
#include "modem.h"
#include "lib/sys_tick.h"
#include "alarm_task.h"

#define TRACKER(...)	//DbgCfgPrintf(__VA_ARGS__)

#define TICK_RATE			CONFIG_SYS_CLOCK
#define REMIND_INTERVAL		60*60				// VEHICLE_INFO report interval (sec)
#define OVERSPEED_INTERVAL	30					// sec
#define PARKING_INTERVAL	15

//#define USER_TEST_CODE	1			  /*user for test speed, GT402 will genarate internal speed to use*/
#define USER_SPEED_FOR_CAR_STATUS	 /*if not zezo then will use speed for detect car status is runing or stoping*/
																			/*else will use ACC for detect car status*/
#define USER_STOP_TIME_CONDITION	(15*60) /*sec*/
#define USER_START_TIME_CONDITION	(2*60) /*sec*/
//#define ENABLE_OVER_SPEED_WARNING

uint8_t trackerTaskRun = 0;
extern uint16_t batteryPercent;
extern uint32_t rtcCnt;
float mileage;
float an1Voltage = 0;
uint8_t trackerSaveLog;
uint32_t trackerTaskCnt = 0;
uint32_t adcChannel8Value = 0;
uint8_t newUpdateGps = 0;


extern DATE_TIME fwUpdateDateHour;
extern DATE_TIME fwUpdatedLastDateHour;
extern uint32_t firmwareUpdateAt;
extern uint32_t flagFirmwareUpdateNow;

MSG_STATUS_RECORD	logSend;
MSG_STATUS_RECORD	*logSendPt = NULL;

Timeout_Type tSpeedAlarmTimeout;

void TrackerInit(void)
{
	DB_InitLog(&logRecord);
}

void TRACKER_Pause(void)
{
	trackerTaskRun = 0;
}
void TRACKER_Resume(void)
{
	trackerTaskRun = 1;
}

uint8_t GetDataFromLog(void)
{
	if((ringLog.head != ringLog.tail))
	{
		if(logSendPt == NULL)
		{
			if(logSend.serverSent)
			{
				logSend.serverSent = 0;
				DB_RingLogNext();
			}
			if(DB_LoadNextLog(&logSend) == 0)
			{
				logSendPt = &logSend; 
				return 0;
			}
		}
	}
	return 1;
}

void TrackerTask(void)
{
	float currentSpeed = 0,gpsCurrentSpeed;
	static float speedSamples[5];
	uint32_t i;
	DATE_TIME timeTemp;
	uint32_t gpsTimeSec;
//	if(trackerTaskRun)
	{
		trackerTaskCnt++;
		//GPS get info
		GPS_GetInfo();
		TIME_FromGps(&timeTemp,&nmeaInfo);
		gpsTimeSec = TIME_GetSec(&timeTemp);
		
		if(nmeaInfo.fix  == 1 || nmeaInfo.fix == 2)
		{
			if(nmeaInfo.satinfo.inuse && nmeaInfo.satinfo.inview)
				UpdateRtcTime(gpsTimeSec);
		}
		else if((nmeaInfo.fix == 3) && 
				(nmeaInfo.utc.year >= 113) && 
				(nmeaInfo.sig > 0) && 
				(nmeaInfo.lat != 0) && 
				(nmeaInfo.lon != 0) && 
				(nmeaInfo.lat <= 90 || nmeaInfo.lat >= -90) &&
				(nmeaInfo.lon <= 180  || nmeaInfo.lon >= -180)
			)
		{

			if(nmeaInfo.speed >= sysCfg.speedLimit && (CheckTimeout(&tSpeedAlarmTimeout) == SYSTICK_TIMEOUT))
			{
				InitTimeout(&tSpeedAlarmTimeout,SYSTICK_TIME_SEC(30));
				alarmFlag |= SPEED_ALARM_FLAG;
			}

			newUpdateGps = 1;
			lastNmeaInfo = nmeaInfo;
			UpdateRtcTime(gpsTimeSec);

		}

		
		if(trackerTaskCnt % 30 == 0)
		{
			if(lastNmeaInfo.fix == 3 && newUpdateGps)
			{
				newUpdateGps = 0;
				gpsLat.value = DB_FloatToU32(lastNmeaInfo.lat);
				gpsLon.value = DB_FloatToU32(lastNmeaInfo.lon);
				gpsHDOP.value = DB_FloatToU32(lastNmeaInfo.HDOP);
				gpsDir.value = DB_FloatToU32(lastNmeaInfo.direction);
				DB_U32Save(&gpsLat,GPS_LAT_ADDR);
				DB_U32Save(&gpsLon,GPS_LON_ADDR);
				DB_U32Save(&gpsHDOP,GPS_HDOP_ADDR);
				DB_U32Save(&gpsDir,GPS_DIR_ADDR);
			}
		}
		TIME_FromSec(&sysTime,rtcTimeSec);
		
		
		// Speed
		gpsCurrentSpeed = nmeaInfo.speed;
		if(gpsCurrentSpeed < CONFIG_SPEED_STOP || nmeaInfo.fix < 3 || nmeaInfo.utc.year < 111 || nmeaInfo.sig < 1 || nmeaInfo.lat == 0 || nmeaInfo.lon == 0 || nmeaInfo.HDOP == 0)
		{
			gpsCurrentSpeed = 0;
		}
		speedSamples[sizeof speedSamples / sizeof speedSamples[0] - 1] = gpsCurrentSpeed;
		if(speedSamples[0] == 0 && speedSamples[sizeof speedSamples / sizeof speedSamples[0] - 1] == 0)
		{
			for(i=1;i<sizeof speedSamples / sizeof speedSamples[0] - 1;i++)
				speedSamples[i] = 0;
		}
		for(i=1;i<sizeof speedSamples / sizeof speedSamples[0]; i++)
			speedSamples[i-1] = speedSamples[i];
			
		gpsCurrentSpeed = speedSamples[0];
		
		currentSpeed = gpsCurrentSpeed;
		
		mileage += currentSpeed / 3600;

		if(trackerSaveLog)
		{
			trackerSaveLog = 0;
			logRecord.time = TIME_GetSec(&sysTime);
			logRecord.drvIndex = 0;
			logRecord.adc1Value = (uint16_t)(ADC1_10_Value*1000);
			logRecord.adc2Value = (uint16_t)(ADC1_11_Value*1000);
			logRecord.gpsLat = lastNmeaInfo.lat;
			logRecord.gpsLon = lastNmeaInfo.lon;
			logRecord.gpsHdop = lastNmeaInfo.HDOP;
			logRecord.cellLat = lastNmeaInfo.lat;
			logRecord.cellLon = lastNmeaInfo.lon;
			logRecord.cellHdop = lastNmeaInfo.HDOP;
			logRecord.gpsSpeed = currentSpeed;
			logRecord.pulseSpeed = 0;
			logRecord.gpsDir = lastNmeaInfo.direction;
			logRecord.drivingTime = 0;
			logRecord.totalDrivingTime = 0;
			logRecord.overSpeedCnt = 0;
			logRecord.parkCnt = 0;
			logRecord.openDoorCnt =  0;
			logRecord.onOffEngineCnt = 0;
			logRecord.parkingTime = 0;
			logRecord.mileage = mileage;
			logRecord.totalPulse = rtcCnt;
			logRecord.speedFreq = 0;
			logRecord.batteryPercent = batteryPercent;//ADC1_11_Value*100/4.2;
			logRecord.gsmSignal = (uint8_t)csqValue;
			
			logRecord.IOStatus = 0;
			logRecord.satinuse = lastNmeaInfo.satinfo.inuse;
			logRecord.IOStatus = 0;
			if(GET_AVLIO1_PIN)
				logRecord.IOStatus |= 1<<0; //ACC
			if(GET_AVLIO2_PIN)
				logRecord.IOStatus |= 1<<1;
			if(GET_AVLIO3_PIN)
				logRecord.IOStatus |= 1<<2; 
			if(GET_AVLIO4_PIN)
				logRecord.IOStatus |= 1<<3; 
			
			if((!ACC_IS_ON) && currentSpeed > 15/*km/h*/) //ACC
				logRecord.IOStatus |= 1<<4; //chuyen dong bat thuong
			//logRecordTemp = logRecord;
			//newestLog = logRecord;
			//newestLog.serverSent = 0;
			DB_SaveLog(&logRecord);
		}
	}
}

void TrackerGetNewData(void)
{
	trackerSaveLog = 1;
}
	

uint8_t TrackerCrc(uint8_t *buff, uint32_t length)
{
	uint32_t i;
	uint8_t crc = 0;
	for(i = 0;i < length; i++)
	{
		crc += buff[i];
	}
	return crc;
}

uint16_t AddTrackerPacket(MSG_STATUS_RECORD *logRecordSend,char *data,uint16_t *len)
{
	uint32_t u32Temp0,u32Temp1;
//	int32_t i32Temp0,i32Temp1;
	char buff[256];
	char tmpBuf[32];
	double fTemp;
	DATE_TIME time;
	buff[0] = 0;
	data[0] = 0;
	TIME_FromSec(&time,logRecordSend->time);
	//3.	Company ID
	strcat(buff, ",AB,");
	//4.	Device ID
	strcat(buff, (char *)sysCfg.id);
	//5.	Driver ID
	sprintf(tmpBuf, ",%03d",0);
	strcat(buff,tmpBuf);
	//6.	HHMMSS
	sprintf(tmpBuf, ",%02d%02d%02d", time.hour, time.min, time.sec);
	strcat(buff, tmpBuf);
	//7.	YYMMDD
	sprintf(tmpBuf, ",%02d%02d%02d",time.year%100, time.month, time.mday);
	strcat(buff, tmpBuf);
	//8. GPS Lattidue
	if(logRecordSend->gpsLat)
	{
		sprintf(tmpBuf,",%.06lf",logRecordSend->gpsLat);
		strcat(buff, tmpBuf);
	}
	else
		strcat(buff, ",0");
	
	//9. GPS Longitude
	if(logRecordSend->gpsLon)
	{
		sprintf(tmpBuf,",%.06lf",logRecordSend->gpsLon);
		strcat(buff, tmpBuf);
	}
	else
		strcat(buff, ",0");
	//10.	Warning bits
	strcat(buff, ",0");
	strcat(buff, "0");
	strcat(buff, "0");
	strcat(buff, "0");
	if(logRecordSend->IOStatus & (1 << 4)) //Chuyen dog bat thuong
		strcat(buff, "1");
	else
		strcat(buff, "0");
	if(logRecordSend->IOStatus & (1 << 5)) //MAT GPS
		strcat(buff, "1");
	else
		strcat(buff, "0");
	if(logRecordSend->IOStatus & (1 << 6)) //QUA TOC DO
		strcat(buff, "1");
	else
		strcat(buff, "0");
	if(logRecordSend->IOStatus & (1 << 7)) //SOS
		strcat(buff, "1");
	else
		strcat(buff, "0");
	
	//11.	Analog IO
	// 8 bytes adc
	an1Voltage = (float)logRecordSend->adc1Value;
	fTemp = an1Voltage;
	u32Temp0 = (uint32_t)fTemp;
	u32Temp1 = (uint32_t)((fTemp - (double)u32Temp0)*100);
	sprintf(tmpBuf, ",%d.%02d",u32Temp0,u32Temp1);
	strcat(buff, tmpBuf);
	//12.	Pulse IO
	fTemp = logRecordSend->speedFreq;
	u32Temp0 = (uint32_t)fTemp;
	u32Temp1 = (uint32_t)((fTemp - (double)u32Temp0)*100);
	sprintf(tmpBuf, ",%d.%02d",u32Temp0,u32Temp1);
	strcat(buff,tmpBuf);
	sprintf(tmpBuf, ",%d",logRecordSend->totalPulse);
	strcat(buff, tmpBuf);
	//13.	Digital IO
	
		strcat(buff, ",0");
		strcat(buff, "0");
	
	if(logRecordSend->IOStatus & (1 << 9)) 
		strcat(buff, "1");
	else
		strcat(buff, "0");
	
	if(logRecordSend->IOStatus & (1 << 8)) 
		strcat(buff, "1");
	else
		strcat(buff, "0");
	
	if(logRecordSend->IOStatus & (1 << 3)) 
		strcat(buff, "0");
	else
		strcat(buff, "1");
	
	if(logRecordSend->IOStatus & (1 << 2))
		strcat(buff, "1");
	else
		strcat(buff, "0");
	
	if(logRecordSend->IOStatus & (1 << 1))
		strcat(buff, "0");
	else
		strcat(buff, "1");
	
	if(logRecordSend->IOStatus & (1 << 0))
		strcat(buff, "0");
	else
		strcat(buff, "1");
	
	//14.	Vgps 
	fTemp = logRecordSend->gpsSpeed;
	u32Temp0 = (uint32_t)fTemp;
	u32Temp1 = (uint32_t)((fTemp - (double)u32Temp0)*10);
	sprintf(tmpBuf, ",%d.%d",u32Temp0,u32Temp1);
	strcat(buff, tmpBuf);
	//15.	Direction
	fTemp = logRecordSend->gpsDir;
	u32Temp0 = (uint32_t)fTemp;
	u32Temp1 = (uint32_t)((fTemp - (double)u32Temp0)*10);
	sprintf(tmpBuf, ",%d.%d",u32Temp0,u32Temp1);
	strcat(buff, tmpBuf);
	//16.	Vp
	fTemp = logRecordSend->pulseSpeed;
	u32Temp0 = (uint32_t)fTemp;
	u32Temp1 = (uint32_t)((fTemp - (double)u32Temp0)*10);
	sprintf(tmpBuf, ",%d.%d",u32Temp0,u32Temp1);
	strcat(buff, tmpBuf);
	//17.	Mileage
	sprintf(tmpBuf, ",%d", (uint32_t)(logRecordSend->mileage));
	strcat(buff, tmpBuf);
	//18.	GPRS status
	strcat(buff, ",0");//du lieu moi
	//19.	GPS satinuse
	sprintf(tmpBuf,",%d",logRecordSend->satinuse);
	strcat(buff,tmpBuf);
	//20.	HW version
	strcat(buff, ",");
	strcat(buff,HARDWARE_VERSION);
	//21.	FW version
	strcat(buff, ",");
	strcat(buff,FIRMWARE_VERSION);
	//22.	Cell_lat,Cell_long
	if(logRecordSend->cellLat)
	{
		sprintf(tmpBuf,",%.06lf",logRecordSend->cellLat);
		strcat(buff, tmpBuf);
	}
	else
		strcat(buff, ",0");
	if(logRecordSend->cellLon)
	{
		sprintf(tmpBuf,",%.06lf",logRecordSend->cellLon);
		strcat(buff, tmpBuf);
	}
	else
		strcat(buff, ",0");
	//23.	GSM Signal
	sprintf(tmpBuf,",%d",logRecordSend->gsmSignal);
	strcat(buff,tmpBuf);
	//24.	HDOP: Horizontal Dilution of Precision.
	if(logRecordSend->IOStatus & (1 << 5)) //MAT GPS
		fTemp = logRecordSend->cellHdop;
	else
		fTemp = logRecordSend->gpsHdop;
	
	u32Temp0 = (uint32_t)fTemp;
	u32Temp1 = (uint32_t)((fTemp - (double)u32Temp0)*100);
	sprintf(tmpBuf, ",%d.%02d",u32Temp0,u32Temp1);
	strcat(buff, tmpBuf);
	//25.	Bat
	sprintf(tmpBuf,",%d",logRecordSend->batteryPercent);
	strcat(buff,tmpBuf);
	//26.	Ex_PWR adc1Value
	sprintf(tmpBuf,",%2.3f",(float)logRecordSend->adc1Value / 1000);
	strcat(buff,tmpBuf);
	//27.	Total Driving Time
	sprintf(tmpBuf, ",%02d%02d",logRecordSend->totalDrivingTime/3600,(logRecordSend->totalDrivingTime%3600)/60);
	strcat(buff, tmpBuf);
	//28.	Continuous Driving Time
	//thoi gian lai xe lien tuc va tong thoi gian lai xe trong ngay
	sprintf(tmpBuf, ",%02d%02d,",logRecordSend->drivingTime/3600,(logRecordSend->drivingTime%3600)/60);
	strcat(buff, tmpBuf);
	
	//1.Header
	strcat(data, "$ATZ");
	//2 Length
	sprintf(tmpBuf,",%d",strlen(buff));
	strcat(data,tmpBuf);
	//3-28
	strcat(data,buff);
	//29 checksum
	sprintf(tmpBuf,"%d",TrackerCrc((uint8_t *)data,strlen(data)));
	strcat(data,tmpBuf);
	strcat(data,"\r\n");
	*len = strlen(data);
	return 0;
}


void PrintTrackerInfo(MSG_STATUS_RECORD *log)
{

}



