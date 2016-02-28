

#include "system_config.h"
#include "hw_config.h"
#include "gps_task.h"
#include "rtc.h"


uint32_t gpsTimeSec = 0;
uint32_t gpsInterval = GPS_WALKING_INTERVAL;
uint32_t GPS_timeoutCheck = 0;
uint32_t GPS_timeCnt = 0;
uint8_t movementToCheckGps = 0;
uint8_t timeGetGPS = 0;
uint32_t lastGpsGotTime = 0xfffff;
GPS_CTRL_PHASE GPS_phase = GPS_ENABLE_PHASE;
uint8_t serverWaitGpsFlag = 0;
uint32_t gpsSleepCntTime = 0;
uint8_t lastGpsFixed = 0;
uint8_t gpsPromptFlag = 0;
uint8_t saveGpsFlag = 0;
uint8_t sendGpsFlag = 0;
uint8_t gpsPedometerFlag;
uint8_t intervalPedometerFlag;
volatile float currentSpeed;
uint32_t flagSaveLog = 0;
uint8_t flagSaveTime = 0;

void GpsTask_Init(void)
{
	GPS_phase = GPS_ENABLE_PHASE;
	gpsInterval = sysCfg.reportInterval*60;
}

GPS_CTRL_PHASE GpsTask_GetPhase(void)
{
	return GPS_phase;
}


void GpsTask_EnableGps(void)
{
	gpsSleepCntTime = 0;
	GPS_phase = GPS_ENABLE_PHASE;
}

void GpsTask_SetPhase(GPS_CTRL_PHASE phase)
{
	GPS_phase = phase;
}

void GpsTask_Control(void)
{
	float fTemp;
	DATE_TIME timeTemp;

	if(gpsSleepCntTime) gpsSleepCntTime--;
	lastGpsGotTime++;
	switch(GPS_phase)
	{
		case GPS_IDLE:
			if(gpsPedometerFlag && (gpsSleepCntTime > GPS_WALKING_INTERVAL))
			{ 
				gpsSleepCntTime = 0;
				gpsPedometerFlag = 0;
			}
			if(gpsSleepCntTime == 0)
			{
				GPS_phase = GPS_ENABLE_PHASE;
			}
		break;
		case GPS_ENABLE_PHASE:
			GPS_EN_PIN_SET;
			nmea_zero_INFO(&nmeaInfo);
			GPS_phase = GPS_GET_LOCATION_PHASE;
			timeGetGPS = 0;
			GPS_timeoutCheck = GPS_WAITING_TIME; //3min
		break;
		case GPS_GET_LOCATION_PHASE:
			//GPS get info
			GPS_GetInfo();
			TIME_FromGps(&timeTemp,&nmeaInfo);
			gpsTimeSec = TIME_GetSec(&timeTemp);

			if((nmeaInfo.fix == 3) && 
				(nmeaInfo.utc.year >= 113) && 
				(nmeaInfo.sig > 0) && 
				(nmeaInfo.lat != 0) && 
				(nmeaInfo.lon != 0) && 
				(nmeaInfo.lat <= 90 || nmeaInfo.lat >= -90) &&
				(nmeaInfo.lon <= 180  || nmeaInfo.lon >= -180)
			)
			{
				if(timeGetGPS ==0)
					lastNmeaInfo = nmeaInfo;
				else if(nmeaInfo.HDOP < lastNmeaInfo.HDOP)
					lastNmeaInfo = nmeaInfo;
				timeGetGPS++;
				if(timeGetGPS >= 10)
				{
					nmeaInfo = lastNmeaInfo; //update best location
					saveGpsFlag = 1;
					lastGpsGotTime = 0;
					
					if(lastGpsFixed == 0)
					{
						lastGpsFixed = 1;
						gpsPromptFlag = 1;			
					}
					
					if(serverWaitGpsFlag)
					{
						serverWaitGpsFlag = 0;
					}
					
					GPS_phase = GPS_FIXED_PHASE;
					GPS_timeoutCheck = 0;
					currentSpeed = nmeaInfo.speed;
					flagSaveLog = 1;
				}
				break;
			}
			GPS_timeoutCheck--;
			if(GPS_timeoutCheck == 0)
			{
				if((lastNmeaInfo.lat <= 90 || lastNmeaInfo.lat >= -90) 
				&& (lastNmeaInfo.lon <= 180  || lastNmeaInfo.lon >= -180)
				)
				{
					sendGpsFlag = 1;
				}
				lastGpsFixed = 0;
				if(gpsPedometerFlag) // if GPS cant fixed and have movement
				{
					gpsSleepCntTime = GPS_WALKING_INTERVAL;
					gpsPedometerFlag = 0;
				}
				else
					gpsSleepCntTime = 4*3600; //if gps cant fixed and no movement
				GPS_phase = GPS_STOP_PHASE;
			}
		break;
		case GPS_FIXED_PHASE:
				if(nmeaInfo.utc.year >= 113)
				{
					UpdateRtcTime(gpsTimeSec);
				}
				gpsInterval = sysCfg.reportInterval*60;
				if(gpsPedometerFlag) // If GPS fixed and have moving
				{
					gpsSleepCntTime = GPS_WALKING_INTERVAL;
					gpsPedometerFlag = 0;
				}
				else //If GPS fixed and have no moving
				{
					gpsSleepCntTime = 4*3600; //if gps cant fixed and no movement
				}
				GPS_phase = GPS_STOP_PHASE;
		break;
		case GPS_STOP_PHASE:
			GPS_EN_PIN_CLR;
			GPS_phase = GPS_IDLE;
		break;
	}
}



