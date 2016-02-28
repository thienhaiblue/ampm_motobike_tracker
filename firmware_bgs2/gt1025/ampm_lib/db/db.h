#ifndef __DB_H__
#define __DB_H__

#include <stdint.h>
#include "lib/sys_time.h"
#include "system_config.h"
#include "lib/sys_time.h"
#include "sst25.h"
/*
* log db layout:
* year_month/mday/hour.log		speed log every second
*/

#define LOG_WRITE_BYTES 4
#define RINGLOG_MAX 	(LOG_DATA_SIZE_MAX / sizeof(MSG_STATUS_RECORD))

typedef struct
{
	uint32_t value;
	uint32_t oldValue;
	uint32_t cnt;
}DB_U32;

#if defined ( __ICCARM__ )
#pragma pack(1) 
typedef struct {
	uint32_t time; //
	double gpsLat;// kinh do
	double gpsLon;// vi do
	float gpsHdop;//  sai so trong don vi met
	float gpsSpeed;// toc do
	float gpsDir;// huong di chuyen
	double cellLat;// kinh do
	double cellLon;// vi do
	float cellHdop;//  sai so trong don vi met
	float mileage; // so km hien thi tren dong ho do cua xe
	float pulseSpeed;
	float speedFreq;
	uint32_t totalPulse; //
	uint32_t drivingTime; //  thoi gian lai xe lien tuc
	uint32_t totalDrivingTime;// tong thoi gian lai xe trong ngay
	uint32_t cellId;
	uint16_t mcc;
	uint16_t mnc;
	
	uint16_t lac;
	uint16_t IOStatus;// BIT0=ACC | BIT1=DOOR | BIT2=AIRCON | BIT3=PWR | BIT4=CHUYEN DONG BAT THUONG | BIT5=MAT GPS | BIT6 = QUA TOC DO | BIT7 = SOS;
	
	uint16_t adc1Value;// Gia tri cua cam bien 1
	uint16_t adc2Value;// Gia tri cua cam bien 2
	
	uint16_t overSpeedCnt;// Toc do gioi han cua xe
	uint16_t parkCnt;// so dan dung do xe
	
	uint16_t openDoorCnt;// so lan dong mo cua xe
	uint16_t onOffEngineCnt;// so lan tat mo may xe
	
	uint16_t parkingTime;// thoi gian dau xe 
	uint8_t drvIndex; //ID cua tai xe
	uint8_t serverSent; 
	
	uint8_t satinuse; 
	uint8_t batteryPercent;
	uint8_t gsmSignal;
	uint8_t dummy[4];
	uint8_t crc;
}MSG_STATUS_RECORD;
#pragma pack()
#elif defined (__CC_ARM)
typedef struct __attribute__((packed)){
	uint32_t time; //
	double gpsLat;// kinh do
	double gpsLon;// vi do
	float gpsHdop;//  sai so trong don vi met
	float gpsSpeed;// toc do
	float gpsDir;// huong di chuyen
	double cellLat;// kinh do
	double cellLon;// vi do
	float cellHdop;//  sai so trong don vi met
	float mileage; // so km hien thi tren dong ho do cua xe
	float pulseSpeed;
	float speedFreq;
	uint32_t totalPulse; //
	uint32_t drivingTime; //  thoi gian lai xe lien tuc
	uint32_t totalDrivingTime;// tong thoi gian lai xe trong ngay
	uint32_t cellId;
	uint16_t mcc;
	uint16_t mnc;
	
	uint16_t lac;
	uint16_t IOStatus;// BIT0=ACC | BIT1=DOOR | BIT2=AIRCON | BIT3=PWR | BIT4=CHUYEN DONG BAT THUONG | BIT5=MAT GPS | BIT6 = QUA TOC DO | BIT7 = SOS;
	
	uint16_t adc1Value;// Gia tri cua cam bien 1
	uint16_t adc2Value;// Gia tri cua cam bien 2
	
	uint16_t overSpeedCnt;// Toc do gioi han cua xe
	uint16_t parkCnt;// so dan dung do xe
	
	uint16_t openDoorCnt;// so lan dong mo cua xe
	uint16_t onOffEngineCnt;// so lan tat mo may xe
	
	uint16_t parkingTime;// thoi gian dau xe 
	uint8_t drvIndex; //ID cua tai xe
	uint8_t serverSent; 
	
	uint8_t satinuse; 
	uint8_t batteryPercent;
	uint8_t gsmSignal;
	uint8_t dummy[4];
	uint8_t crc;
}MSG_STATUS_RECORD;
#endif

extern MSG_STATUS_RECORD	logRecord,newestLog;

#if defined ( __ICCARM__ )
#pragma pack(1) 
typedef struct {
	uint32_t head;
	uint32_t tail;
	uint8_t 	crc;
}LOG_RING_TYPE;
#pragma pack()
#elif defined (__CC_ARM)
typedef struct __attribute__((packed)){
	uint32_t head;
	uint32_t tail;
	uint8_t 	crc;
}LOG_RING_TYPE;
#endif



extern LOG_RING_TYPE ringLog;
extern uint32_t flashInUse;
//flash log
uint8_t DB_ParaCheckErr(MSG_STATUS_RECORD *log);
int8_t DB_LoadEndLog(MSG_STATUS_RECORD *log);
void DB_InitLog(MSG_STATUS_RECORD *log);
int8_t DB_SaveLog(MSG_STATUS_RECORD *log);
int8_t DB_LoadLog(MSG_STATUS_RECORD *log,LOG_RING_TYPE *r);
int8_t DB_LoadNextLog(MSG_STATUS_RECORD *log);
void DB_RingLogReset(void);
void DB_RingLogSave(void);
void DB_RingLogNext(void);
uint8_t DbCalcCheckSum(uint8_t *buff, uint32_t length);

void DB_U32Save(DB_U32 *dbu32,uint32_t addr);
void DB_U32Load(DB_U32 *dbu32,uint32_t addr);
uint32_t DB_FloatToU32(double lf);
float DB_U32ToFloat(uint32_t *u32);

#endif

