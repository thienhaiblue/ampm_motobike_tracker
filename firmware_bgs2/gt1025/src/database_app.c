/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "db.h"
#include "rtc.h"

DB_U32 sysTimeSleep;
DB_U32 gpsLat; 
DB_U32 gpsLon;
DB_U32 gpsHDOP;
DB_U32 gpsDir;
DB_U32 cellLat;
DB_U32 cellLon;
DB_U32 powerOfTimeDelay;
DB_U32 firmwareDownloadTimeout;
DB_U32 timeSave;
DB_U32 agpsTimeSave;



uint32_t DB_FlashMemInit(void)
{
	float lat,lon;
	DB_InitLog(&logRecord);
	DB_U32Load(&sysTimeSleep,SYS_SLEEP_TIME_ADDR);
	DB_U32Load(&gpsLat,GPS_LAT_ADDR);
	DB_U32Load(&gpsLon,GPS_LON_ADDR);
	DB_U32Load(&gpsHDOP,GPS_HDOP_ADDR);
	DB_U32Load(&gpsDir,GPS_DIR_ADDR);
	DB_U32Load(&cellLat,CELL_LAT_ADDR);
	DB_U32Load(&cellLon,CELL_LON_ADDR);
	DB_U32Load(&powerOfTimeDelay,CELL_HDOP_ADDR);
	DB_U32Load(&firmwareDownloadTimeout,FIRMWARE_DOWNLOAD_TIMEOUT_ADDR); 
	
	lat = *((float *)(&gpsLat.value));
	lon = *((float *)(&gpsLon.value));

	if(lat < 0)
		lastNmeaInfo.ns = 'S';
	else 
		lastNmeaInfo.ns = 'N';
	if(lon < 0)
		lastNmeaInfo.ew = 'W';
	else 
		lastNmeaInfo.ew = 'E';
	lastNmeaInfo.HDOP = *((float *)(&gpsHDOP.value));
	lastNmeaInfo.direction = *((float *)(&gpsDir.value));
	lastNmeaInfo.lon = lon;
	lastNmeaInfo.lat = lat;
	
	if((lastNmeaInfo.lat <= 90 || lastNmeaInfo.lat >= -90) 
		&& (lastNmeaInfo.lon <= 180  || lastNmeaInfo.lon >= -180)
		)
		{
			lastNmeaInfo.fix = 3;
			lastNmeaInfo.sig = 1;
		}
		else
		{
			lastNmeaInfo.lat = 0;
			lastNmeaInfo.lon = 0;
		}

	DB_U32Load(&timeSave,TIME_SAVE_ADDR);
	DB_U32Load(&agpsTimeSave,AGPS_TIME_SAVE_ADDR);

	return 0;
}


