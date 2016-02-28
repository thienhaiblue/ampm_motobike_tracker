/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "system_config.h"
#include "lib/sys_tick.h"
#include <stdlib.h>
#include "rtc.h"

CONFIG_POOL sysCfg;
uint8_t sysUploadConfig = 2;
uint8_t sysResetMcuFlag = 0;
FW_UPDATE_TYPE firmware;

void CFG_ReLoad(void)
{
	memcpy(&sysCfg, (void*)CONFIG_AREA_START, sizeof(CONFIG_POOL));
}
void CFG_Load(void)
{
	uint32_t saveFlag = 0,u32Temp = 0;
	int16_t i;
	uint8_t *u8Pt;
	memcpy(&sysCfg, (void*)CONFIG_AREA_START, sizeof(CONFIG_POOL));
	//memset((void*)&sysCfg, 0xFF, sizeof sysCfg);
	u8Pt = (uint8_t *)&sysCfg;
	u32Temp = 0;
	for(i = 0;i < sizeof(CONFIG_POOL)-sizeof(sysCfg.crc);i++)
	{
		u32Temp += u8Pt[i];
	}
	if(u32Temp != sysCfg.crc)
	{
		memset((void*)&sysCfg, 0xFF, sizeof sysCfg);
		saveFlag = 1;
	}
	
	if(sysCfg.size != sizeof(CONFIG_POOL))
	{
		sysCfg.size = sizeof(CONFIG_POOL);
		saveFlag = 1;
	}
	//memset((void*)&sysCfg, 0xFF, sizeof sysCfg);
	
	
	if((uint8_t)sysCfg.imei[0] == 0xFF)
	{
		saveFlag = 1;
		sysCfg.imei[0] = 0;
	}
	if((uint8_t)sysCfg.fwVersion[0] == 0xFF)
	{
		strcpy((char *)sysCfg.fwVersion, FIRMWARE_VERSION);
		saveFlag = 1;
	}
	if((uint8_t)sysCfg.id[0] == 0xFF)
	{
		strcpy((char *)sysCfg.id, DEFAULT_ID);
		saveFlag = 1;
	}
	if(sysCfg.onReportInterval == 0xffff)
	{
		sysCfg.onReportInterval = DEFAULT_ACC_ON_REPORT_INTERVAL;
		saveFlag = 1;
	}
	if(sysCfg.offReportInterval == 0xffff)
	{
		sysCfg.offReportInterval = DEFAULT_ACC_OFF_REPORT_INTERVAL;
		saveFlag = 1;
	}
	// load default GPRS config
	if((uint8_t)sysCfg.gprsApn[0] == 0xFF || sysCfg.gprsApn[0] == 0)
	{
		strcpy((char *)sysCfg.gprsApn, DEFAULT_GPSR_APN);
		strcpy((char *)sysCfg.gprsUsr, DEFAULT_GPRS_USR);
		strcpy((char *)sysCfg.gprsPwd, DEFAULT_GPRS_PWD);
		saveFlag = 1;
	}
	// load default data server name
	if((uint8_t)sysCfg.priDserverName[0] == 0xFF)
	{
		strcpy((char *)sysCfg.priDserverName, DEFAULT_PRI_DSERVER_NAME);
		strcpy((char *)sysCfg.secDserverName, DEFAULT_SEC_DSERVER_NAME);
		sysCfg.dServerPort = DEFAULT_DSERVER_PORT;
		saveFlag = 1;
	}
	
	// load default firmware server name
	if((uint8_t)sysCfg.priFserverName[0] == 0xFF)
	{
		strcpy((char *)sysCfg.priFserverName, DEFAULT_FSERVER_NAME);
		strcpy((char *)sysCfg.secFserverName, DEFAULT_FSERVER_NAME);
		saveFlag = 1;
	}
	if(sysCfg.fUserName[0] == 0xff)
	{
		strcpy((char *)sysCfg.fUserName, DEFAULT_F_USER);
		saveFlag = 1;
	}
	if(sysCfg.fPassword[0] == 0xff)
	{
		strcpy((char *)sysCfg.fPassword, DEFAULT_F_PASS);
		saveFlag = 1;
	}
	if(sysCfg.userPhone[0] == 0xff)
	{
		strcpy((char *)sysCfg.userPhone, DEFAULT_USER_PHONE_NUMBER);
		saveFlag = 1;
	}


	if(sysCfg.vehicleID[0] == 0xff)
	{
		strcpy((char *)sysCfg.vehicleID, DEFAULT_VEHICLE_ID);
		saveFlag = 1;
	}
	
	if((uint8_t)sysCfg.timezone == 0xff)
	{
		sysCfg.timezone = DEFAULT_TIMEZONE; 
		saveFlag = 1;
	}
	
	if(sysCfg.gSensorSense == 0xff)
	{
		sysCfg.gSensorSense = DEFAULT_G_SENSOR_SENS; 
		saveFlag = 1;
	}
	
	if(sysCfg.speedLimit == 0xff)
	{
		sysCfg.speedLimit = DEFAULT_SPEED_LIMIT;
		saveFlag = 1;
	}
	
	if(sysCfg.featureSet == 0xffffffff)
	{
		sysCfg.featureSet = DEFAULT_FEATURE;
		saveFlag = 1;
	}
	
	if(sysCfg.language == 0xff)
	{
		sysCfg.language = 0;
		saveFlag = 1;
	}

	if(saveFlag)
	{
		CFG_Save();
	}
}


uint8_t CFG_CheckSum(CONFIG_POOL *sysCfg)
{
	uint32_t u32Temp = 0;
	int16_t i;
	uint8_t *u8Pt;
	u8Pt = (uint8_t *)sysCfg;
	u32Temp = 0;
	for(i = 0;i < sizeof(CONFIG_POOL)-sizeof(sysCfg->crc);i++)
	{
		u32Temp += u8Pt[i];
	}
	if(u32Temp != sysCfg->crc)
	{
		return 0;
	}
	return 1;
}
void CFG_Save(void)
{
	int16_t i;
	uint32_t u32Temp;
	uint8_t *u8Pt;
	uint32_t *cfgdest;
	uint16_t  FlashStatus;

	
	if(memcmp((void *)CONFIG_AREA_START,&sysCfg,sizeof(CONFIG_POOL)) == NULL)
		return;
	__disable_irq();	
	
	u8Pt = (uint8_t *)&sysCfg;
	u32Temp = 0;
	for(i = 0;i < sizeof(CONFIG_POOL)-sizeof(sysCfg.crc);i++)
	{
		u32Temp += u8Pt[i];
	}
	sysCfg.crc = u32Temp;
	
	FLASH_Unlock();
	for(i=0; i <= sizeof(CONFIG_POOL)/ PAGE_SIZE; i++) 
	{
		FlashStatus = FLASH_ErasePage((uint32_t)(CONFIG_AREA_START + i*PAGE_SIZE));
		if(FLASH_COMPLETE != FlashStatus) 
		{
			FLASH_Lock();
			__enable_irq();
			return;
		}
	}
	cfgdest = (uint32_t*)&sysCfg;
	for(i=0; i < sizeof(CONFIG_POOL); i+=4) 
	{
		FlashStatus = FLASH_ProgramWord(CONFIG_AREA_START + i, *(uint32_t*)(cfgdest + i/4));
		if(*(uint32_t*)(cfgdest + i/4) != *(uint32_t*)(CONFIG_AREA_START + i)) //check wrote data
		{
			FLASH_Lock();
		__enable_irq();
			return;
		}
	}
	FLASH_Lock();

	__enable_irq();
	RTC_SetAlarm(RTC_GetCounter() + RTC_ALARM_INTERVAL);
}
