/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "mma7660.h"
#include "exti.h"
#include "lib/sys_tick.h"

#define BIT_0	0x01
#define BIT_1	0x02
#define BIT_2	0x04
#define BIT_3	0x08
#define BIT_4	0x10
#define BIT_5	0x20
#define BIT_6	0x40
#define BIT_7	0x80

uint32_t acclInit = 0;
uint32_t acclError = 1;

uint8_t MMA7660_Write(uint8_t reg,uint8_t data)
{
	uint8_t buff[2];
	buff[0] = reg;
	buff[1] = data;
//	return I2C_SendData(MMA7660_ADR,reg,&data,1);
		return I2C_SendData(MMA7660_ADR,buff,2);
	//return I2C_Master_BufferWrite(I2C1,buff,2,MMA7660_ADR);
}
uint8_t MMA7660_Read(uint8_t reg)
{
	uint8_t res = 0;
	if(!acclError)
		I2C_ReadByte(MMA7660_ADR,reg,&res);
	return res;
}

uint8_t MMA7660_Init(void)
{
	uint8_t res,retry = 5;
	acclError = 0;
	while(retry--)
	{
		I2C_Init();
		
//		if(I2C_CheckSlaveStatus(MMA7660_ADR))  continue;
		/*	Enter standby mode to configurate */
		MMA7660_Write(MODE_REG, 0x00);
		res = MMA7660_Read(MODE_REG);
		
		MMA7660_Write(MODE_REG, 0x00);
		res = MMA7660_Read(MODE_REG);
		if(res != 0) continue;
		
		MMA7660_Write(SPCNT_REG, 0x00);
		res = MMA7660_Read(SPCNT_REG);
		if(res != 0) continue;
		
		MMA7660_Write(INTSU_REG, 0xe7);
		res = MMA7660_Read(INTSU_REG);
		if(res != 0xe7) continue;	
		
		MMA7660_Write(PDET_REG, 0x01);
		res = MMA7660_Read(PDET_REG);
		if(res != 0x01) continue;	
		
		MMA7660_Write(SR_REG, 0x01);
		res = MMA7660_Read(SR_REG);
		if(res != 0x01) continue;
		
		MMA7660_Write(PD_REG, 0x00);
		res = MMA7660_Read(PD_REG);
		if(res != 0x00) continue;
		
		MMA7660_Write(MODE_REG,0x01);
		res = MMA7660_Read(MODE_REG);
		if(res != 0x01) continue;
		//sysEventFlag &= ~ACCELEROMETER_FLAG;
		EXTI->PR = EXTI_Line2;
		MMA7660_ReadStatus();
		acclError = 0;
		return 0;
	}
	acclError = 1;
	return 1;
}


uint8_t MMA7660_ReadStatus(void)
{ 
	return MMA7660_Read(TILT_REG);
}

