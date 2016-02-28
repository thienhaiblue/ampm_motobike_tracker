#ifndef __SYSTEM_CONFIG_H__
#define __SYSTEM_CONFIG_H__
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x_flash.h"
#include "hw_config.h"



#define POWER_OFF	0x0FF0FF0F
#define POWER_ON	0x01101101

#define FIRMWARE_VERSION	"2.925"
#define HARDWARE_VERSION	"1.0"

#define WARNING_SMS							4
#define WARNING_CALL						8


#define DEFAULT_SMS_PWD			"12345678"
#define DEFAULT_GPSR_APN		"internet"
#define DEFAULT_GPRS_USR		""
#define DEFAULT_GPRS_PWD		""



#define DEFAULT_ROOT_NUMBER	"0978770856"
#define DEFAULT_ID					"BGS2_GT1010"
#define DEFAULT_VEHICLE_ID	""
#define DEFAULT_USER_PHONE_NUMBER "0978770856"


//#define DEFAULT_PRI_DSERVER_NAME	"118.69.60.174"
//#define DEFAULT_SEC_DSERVER_NAME	"118.69.60.174"
//#define DEFAULT_DSERVER_PORT	12349

#define DEFAULT_PRI_DSERVER_NAME	"211.21.204.19"
#define DEFAULT_SEC_DSERVER_NAME	"211.21.204.23"
#define DEFAULT_DSERVER_PORT	4050


#define DEFAULT_FSERVER_NAME	"211.21.204.19"
#define DEFAULT_FSERVER_PORT	50000
#define DEFAULT_F_USER "GT1025"
#define DEFAULT_F_PASS	"0913812777"

#define DEFAULT_ACC_ON_REPORT_INTERVAL		30
#define DEFAULT_ACC_OFF_REPORT_INTERVAL		1800
#define DEFAULT_SLEEP_TIMER		60
#define DEFAULT_TIMEZONE	8 //Taiwan
#define DEFAULT_G_SENSOR_SENS	3
#define DEFAULT_SPEED_LIMIT	40 /* km/hr */

#define DEFAULT_RFID_TAG_ID		"9999999999999999"

// system definitions
#define CONFIG_HW_VERSION			1
#define CONFIG_FW_VERSION			1
#define CONFIG_RELEASE_DATE			__TIMESTAMP__
#define CONFIG_MAX_USER			10


#define CONFIG_SIZE_GPRS_APN		16
#define CONFIG_SIZE_GPRS_USR		32
#define CONFIG_SIZE_GPRS_PWD		16
#define CONFIG_SIZE_SERVER_NAME		31
#define CONFIG_SIZE_SMS_PWD			16
#define CONFIG_SIZE_PLATE_NO		12
#define CONFIG_SIZE_USER_NAME		32
#define CONFIG_SIZE_PHONE_NUMBER	16
#define CONFIG_SIZE_SMS						80
#define CONFIG_SIZE_VEHICLE_ID	16
#define CONFIG_SIZE_IP				32
#define CONFIG_SIZE_ID				18
#define CONFIG_SIZE_IMEI			18
#define CONFIG_SIZE_FWVERSION	16
#define CONFIG_SIZE_RFID_PASS	32
#define CONFIG_SPEED_STOP	5 //km/h


#define DEFAULT_FEATURE	0

#if defined ( __ICCARM__ )
#pragma pack(1) 
typedef struct{
	uint32_t status;
	uint32_t fileSize;
	uint32_t offset;
	uint32_t saveSector;
	uint32_t crc;
}FW_UPDATE_TYPE;
#pragma pack()
#elif defined (__CC_ARM)
typedef struct __attribute__((packed)){
	uint32_t status;
	uint32_t fileSize;
	uint32_t offset;
	uint32_t saveSector;
	uint32_t crc;
}FW_UPDATE_TYPE;
#endif



extern FW_UPDATE_TYPE firmware;
#if defined ( __ICCARM__ )
#pragma pack(1) 
typedef struct 
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}DAILY_REPORT;
#pragma pack()
#elif defined (__CC_ARM)
typedef struct __attribute__((packed))
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}DAILY_REPORT;
#endif

#if defined ( __ICCARM__ )
#pragma pack(1) 

typedef struct
{
	uint16_t size;
	int8_t fwVersion[CONFIG_SIZE_FWVERSION];
	int8_t imei[CONFIG_SIZE_IMEI];
	int8_t id[CONFIG_SIZE_ID];
	// GPRS config
	int8_t gprsApn[CONFIG_SIZE_GPRS_APN];
	int8_t gprsUsr[CONFIG_SIZE_GPRS_USR];
	int8_t gprsPwd[CONFIG_SIZE_GPRS_PWD];
	uint16_t onReportInterval; /*1 min to 30 mins or 1 hour to 24 hours*/
	uint16_t offReportInterval; /*1 min to 30 mins or 1 hour to 24 hours*/
	// primary server config
	uint8_t  priDserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint8_t  secDserverName[CONFIG_SIZE_SERVER_NAME];
	uint16_t dServerPort;	/**< port */
	// primary server config
	uint8_t  priFserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint8_t  secFserverName[CONFIG_SIZE_SERVER_NAME];
	uint8_t fUserName[CONFIG_SIZE_SERVER_NAME];
	uint8_t fPassword[CONFIG_SIZE_SERVER_NAME];
// sms user
	uint8_t userPhone[CONFIG_SIZE_PHONE_NUMBER];
	uint8_t rfidPass[CONFIG_SIZE_RFID_PASS];
	uint8_t vehicleID[CONFIG_SIZE_VEHICLE_ID];
	int8_t timezone;
	uint8_t gSensorSense;
	uint8_t speedLimit;
	uint32_t featureSet;			/*< feature set mapping */
	uint8_t language;
	uint32_t crc;
}CONFIG_POOL;

#pragma pack()
#elif defined (__CC_ARM)
typedef struct __attribute__((packed))
{
	uint16_t size;
	int8_t fwVersion[CONFIG_SIZE_FWVERSION];
	int8_t imei[CONFIG_SIZE_IMEI];
	int8_t id[CONFIG_SIZE_ID];
	// GPRS config
	int8_t gprsApn[CONFIG_SIZE_GPRS_APN];
	int8_t gprsUsr[CONFIG_SIZE_GPRS_USR];
	int8_t gprsPwd[CONFIG_SIZE_GPRS_PWD];
	uint16_t onReportInterval; /*1 min to 30 mins or 1 hour to 24 hours*/
	uint16_t offReportInterval; /*1 min to 30 mins or 1 hour to 24 hours*/
	// primary server config
	uint8_t  priDserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint8_t  secDserverName[CONFIG_SIZE_SERVER_NAME];
	uint16_t dServerPort;	/**< port */
	// primary server config
	uint8_t  priFserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint8_t  secFserverName[CONFIG_SIZE_SERVER_NAME];
	uint8_t fUserName[CONFIG_SIZE_SERVER_NAME];
	uint8_t fPassword[CONFIG_SIZE_SERVER_NAME];
// sms user
	uint8_t userPhone[CONFIG_SIZE_PHONE_NUMBER];
	uint8_t rfidPass[CONFIG_SIZE_RFID_PASS];
	uint8_t vehicleID[CONFIG_SIZE_VEHICLE_ID];
	int8_t timezone;
	uint8_t gSensorSense;
	uint8_t speedLimit;
	uint32_t featureSet;			/*< feature set mapping */
	uint8_t language;
	uint32_t crc;
}CONFIG_POOL;

#endif


extern CONFIG_POOL sysCfg;
extern uint8_t sysUploadConfig;
extern uint8_t sysResetMcuFlag;

void CFG_Save(void);
void CFG_Load(void);
void CFG_ReLoad(void);
void CFG_Write(uint8_t *buff, uint32_t address, int offset, int length);
uint8_t CFG_CheckSum(CONFIG_POOL *sysCfg);
uint8_t CFG_Check(CONFIG_POOL *sysCfg);
#endif
