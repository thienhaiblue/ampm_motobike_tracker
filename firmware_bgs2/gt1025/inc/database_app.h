
#ifndef __DATABASE_APP_H__
#define __DATABASE_APP_H__

#include "db.h"

extern DB_U32 dbTimeZone;
extern DB_U32 sysTimeSleep;
extern DB_U32 gpsLat;
extern DB_U32 gpsLon;
extern DB_U32 gpsHDOP;
extern DB_U32 gpsDir;
extern DB_U32 cellLat;
extern DB_U32 cellLon;
extern DB_U32 powerOfTimeDelay;
extern DB_U32 firmwareDownloadTimeout;
extern DB_U32 timeSave;
extern DB_U32 agpsTimeSave;

uint32_t DB_FlashMemInit(void);


#endif

