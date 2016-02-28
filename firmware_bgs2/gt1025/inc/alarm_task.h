
#ifndef __ALARM_TASK_H__
#define __ALARM_TASK_H__

#include <stdint.h>

extern uint32_t alarmFlag;

extern uint8_t gSensorAlarmTimes;
extern Timeout_Type tGetAddr;
void AlarmTaskInit(void);
void AlarmTask(uint32_t rtc);
void SosHoldDetect(void);
void AlarmHoldDetect(void);
void MicrowaveDetect(void);

#endif
