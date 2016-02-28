
#ifndef __ACCELEROMETER_TASK_H__
#define __ACCELEROMETER_TASK_H__

#include "spi.h"
#include "system_config.h"
#include "mma7660.h"
#include "xl362/xl362.h"
#include "xl362/xl362_io.h"

extern XL362_Type xl362;
extern uint8_t accelerometerAlarmCnt;

void AccelerometerTaskInit(void);
uint8_t Accelerometer_Task_MMA7660(void);
void Accelerometer_Task_ADXL362(void);


#endif


