/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "accelerometer_task.h"


extern uint32_t  DbgCfgPrintf(const uint8_t *format, ...);


extern uint32_t alarmFlag;

#define ACCL_printf(...) //DbgCfgPrintf(__VA_ARGS__)


signed short accel_x, accel_y, accel_z;
signed short accel_x1, accel_y1, accel_z1;

signed short accel_x_min, accel_y_min, accel_z_min;
signed short accel_x_max, accel_y_max, accel_z_max;

uint8_t accelerometerAlarmCnt = 0;
uint32_t accelSampleCnt = 0;
uint8_t accelAlarmTimes = 0;

void AccelerometerTaskInit(void)
{
	//xl362Init();
	MMA7660_Init();
}

uint8_t Accelerometer_Task_MMA7660(void)
{
	uint8_t res;
	uint16_t temp;
	signed short delta_x,delta_y,delta_z;
	accelSampleCnt++;
	if(!acclError)
	{
		if(MMA7660_Read(MODE_REG) != 0x01)
			AccelerometerTaskInit();
		else
			MMA7660_ReadStatus();
	}
	else if(accelSampleCnt % 100 == 0)
		AccelerometerTaskInit();
	res = MMA7660_Read(0x00) & 0x3f;
	if(res > 0x3f)
		return 1;
	res &= 0x3f;
	
	accel_x1 = accel_x;
	if(res & 0x20)
	{
		accel_x = (signed short)res - 64;
	}
	else
	{
		accel_x = res & 0x1f;
	}
	delta_x = accel_x - accel_x1;
	
	accel_y1 = accel_y;
	res = MMA7660_Read(0x01) & 0x3f;
	if(res > 0x3f)
		return 1;
	res &= 0x3f;
	if(res & 0x20)
	{
		accel_y = (signed short)res - 64;
	}
	else
	{
		accel_y = res & 0x1f;
	}
	delta_y = accel_y - accel_y1;
	
	res = MMA7660_Read(0x02) & 0x3f;
	if(res > 0x3f)
		return 1;
	res &= 0x3f;
	accel_z1 = accel_z;
	if(res & 0x20)
	{
		accel_z = (signed short)res - 64;
	}
	else
	{
		accel_z = res & 0x1f;
	}
	delta_z = accel_z - accel_z1;
	if(delta_x < 0)
		delta_x = -delta_x;
	if(delta_y < 0)
		delta_y = -delta_y;
	if(delta_z < 0)
		delta_z = -delta_z;
	
	temp = (uint16_t)sysCfg.gSensorSense * 3 / 2 ;
	if(delta_x >= temp
			|| delta_y >= temp
			|| delta_z >= temp
			)
	{
		accelAlarmTimes++;
	}
	
	if(accelerometerAlarmCnt)
	{
		if( accelAlarmTimes >= 3)
		{
			alarmFlag |= CAR_BE_TOWED_AWAY_FLAG;
			accelAlarmTimes = 0;
		}
	}
	else
	{
		accel_x_min = accel_x;
		accel_y_min = accel_y;
		accel_z_min = accel_z;
		
		accel_x_max = accel_x;
		accel_y_max = accel_y;
		accel_z_max = accel_z;
		
		accelAlarmTimes = 0;
	}
		
	return 0xff;
}
