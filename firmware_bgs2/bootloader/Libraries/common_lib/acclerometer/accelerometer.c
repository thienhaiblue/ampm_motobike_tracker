#include "accelerometer.h"


#define BIT_0	0x01
#define BIT_1	0x02
#define BIT_2	0x04
#define BIT_3	0x08
#define BIT_4	0x10
#define BIT_5	0x20
#define BIT_6	0x40
#define BIT_7	0x80

uint32_t acclInit = 0;

uint8_t ACCL_Write(uint8_t reg,uint8_t data)
{
	uint8_t buff[2];
	buff[0] = reg;
	buff[1] = data;
	return I2C_SendData(MMA7660_ADR,buff,2);
	//return I2C_Master_BufferWrite(I2C1,buff,2,MMA7660_ADR);
}
uint8_t ACCL_Read(uint8_t reg)
{
	return I2C_ReadByte(MMA7660_ADR,reg);
}

uint8_t AccelerometerInit(void)
{
	uint8_t res;
	I2C_Init(24000000);
	//if(I2C_CheckSlaveStatus(MMA7660_ADR) != 0) return 0xff;
	/*	Enter standby mode to configurate */
	//if(I2C_CheckSlaveStatus(MMA7660_ADR) != 0) return 0xff;
	/*	Enter standby mode to configurate */
	ACCL_Write(MODE_REG, 0x00);
	res = ACCL_Read(MODE_REG);
	
	ACCL_Write(MODE_REG, 0x00);
	res = ACCL_Read(MODE_REG);
	if(res != 0) return 0xff;
	
	ACCL_Write(SPCNT_REG, 0x00);
	res = ACCL_Read(SPCNT_REG);
	if(res != 0) return 0xff;
	
	ACCL_Write(INTSU_REG, 0xe7);
	res = ACCL_Read(INTSU_REG);
	if(res != 0xe7) return 0xff;	
	
	ACCL_Write(PDET_REG, 0x01);
	res = ACCL_Read(PDET_REG);
	if(res != 0x01) return 0xff;	
	
	ACCL_Write(SR_REG, 0x0a);
	res = ACCL_Read(SR_REG);
	if(res != 0x0a) return 0xff;
	
	ACCL_Write(PD_REG, 0x08);
	res = ACCL_Read(PD_REG);
	if(res != 0x08) return 0xff;
	
	ACCL_Write(MODE_REG,0x19);
	res = ACCL_Read(MODE_REG);
	if(res != 0x59) return 0xff;
	
	return 0;
}


uint8_t ACCL_ReadStatus(void)
{ 
	return ACCL_Read(TILT_REG);
}

