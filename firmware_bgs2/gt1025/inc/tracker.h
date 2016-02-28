#ifndef __TRACKER_H__
#define __TRACKER_H__

#include <stdint.h>
#include "lib/ringbuf.h"
#include "db.h"

#define	M_PI		3.14159265358979323846	/* pi */
#define d2r (M_PI / 180.0)

extern uint32_t rpSampleCount;
extern uint32_t flagPrint;
extern float pulseCurrentSpeed;
extern float currentSpeed;
extern uint8_t trackerSaveLog;
#if defined ( __ICCARM__ )
#pragma pack(1) 
typedef struct {
	uint8_t oldMsgIsEmpty;
	uint8_t newMsgIsEmpty;
	uint8_t lastMsgIsEmpty;
	MSG_STATUS_RECORD	oldMsg;
	MSG_STATUS_RECORD	newMsg;
	MSG_STATUS_RECORD lastMsg;
} TRACKER_MSG_TYPE;


typedef struct {
	uint8_t type;
	uint8_t size;
	uint32_t random;
	uint8_t id[18];
	uint8_t drvNo;
	MSG_STATUS_RECORD msg;
}AMBO_TRAKER_PROTOCOL_TYPE;
#pragma pack()
#elif defined (__CC_ARM)
typedef struct __attribute__((packed)){
	uint8_t oldMsgIsEmpty;
	uint8_t newMsgIsEmpty;
	uint8_t lastMsgIsEmpty;
	MSG_STATUS_RECORD	oldMsg;
	MSG_STATUS_RECORD	newMsg;
	MSG_STATUS_RECORD lastMsg;
} TRACKER_MSG_TYPE;


typedef struct __attribute__((packed)){
	uint8_t type;
	uint8_t size;
	uint32_t random;
	uint8_t id[18];
	uint8_t drvNo;
	MSG_STATUS_RECORD msg;
}AMBO_TRAKER_PROTOCOL_TYPE;
#endif



typedef struct {
	int din1:1;
	int din2:1;
	int din3:1;
	int din4:1;
	int overSpeed:1;
	volatile int newDriver:1;
	volatile int newDriverSub:1;
	volatile int idle:1;
}TrackerStatusType;

extern TrackerStatusType  trackerStatus;

extern uint32_t serverSendDataFlag;
extern RINGBUF trackerRbOldMsg;
extern RINGBUF trackerRbNewMsg;
extern uint32_t trackerEnable;
extern DATE_TIME printfReportTime;
extern TrackerStatusType  trackerStatus;
extern MSG_STATUS_RECORD	logSend;
extern MSG_STATUS_RECORD	*logSendPt;

void TrackerInit(void);
void TrackerTask(void);
void TankerTask(void);
void TRACKER_Pause(void);
void TRACKER_Resume(void);
uint16_t CalculateCRC(const int8_t* buf, int16_t len);
float haversine_km(float lat1, float long1, float lat2, float long2);
uint32_t GetTrackerMsg(char *buff,uint32_t *len);
uint32_t GetLastTrackerMsg(char *buff,uint32_t *len);
uint32_t CheckTrackerMsgIsReady(void);
void TrackerGetNewData(void);
uint16_t AddTrackerPacket(MSG_STATUS_RECORD *logRecordSend,char *data,uint16_t *len);
uint8_t GetDataFromLog(void);

#endif

