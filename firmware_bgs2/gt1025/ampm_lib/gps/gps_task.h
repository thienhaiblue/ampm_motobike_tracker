

#ifndef __GPS_TASK_H__
#define __GPS_TASK_H__
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "gps/gps.h"
#include "lib/sys_time.h"

#define GPS_WALKING_INTERVAL 300
#define GPS_WAITING_TIME	180

#define lastNmeaInfo lastGpsInfo


typedef enum{
	GPS_IDLE = 0,
	GPS_ENABLE_PHASE,
	GPS_GET_LOCATION_PHASE,
	GPS_FIXED_PHASE,
	GPS_STOP_PHASE
}GPS_CTRL_PHASE;

extern uint32_t gpsTimeSec;
extern uint32_t gpsInterval;
extern uint32_t GPS_timeoutCheck;
extern uint32_t GPS_timeCnt;
extern uint8_t movementToCheckGps;
extern uint8_t timeGetGPS;
extern uint32_t lastGpsGotTime;
extern uint8_t serverWaitGpsFlag;
extern uint32_t gpsSleepCntTime;
extern uint8_t lastGpsFixed;
extern uint8_t gpsPromptFlag;
extern uint8_t saveGpsFlag;
extern uint8_t sendGpsFlag;
extern uint8_t gpsPedometerFlag;
extern uint8_t intervalPedometerFlag;
extern volatile float currentSpeed;
extern uint32_t flagSaveLog;
extern uint8_t flagSaveTime;


void GpsTask_SetPhase(GPS_CTRL_PHASE phase);
void GpsTask_Init(void);
GPS_CTRL_PHASE GpsTask_GetPhase(void);
void GpsTask_EnableGps(void);
void GpsTask_Control(void);

#endif
