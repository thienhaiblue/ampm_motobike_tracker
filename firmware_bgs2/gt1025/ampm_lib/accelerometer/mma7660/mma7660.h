#ifndef __MMA7660_H__
#define __MMA7660_H__
//#include "i2c_io.h"
#include "i2c.h"

#define MMA7660_ADR			(0x4C << 1)

/* REG MAP */
#define	XOUT_REG		0x00
#define YOUT_REG		0x01
#define ZOUT_REG		0x02
#define TILT_REG		0x03
#define SRST_REG		0x04
#define SPCNT_REG		0x05
#define INTSU_REG		0x06
#define	MODE_REG		0x07
#define	SR_REG			0x08
#define PDET_REG		0x09
#define PD_REG			0x0A

extern uint32_t acclInit;
extern uint32_t acclError;
uint8_t MMA7660_Read(uint8_t reg);
uint8_t  MMA7660_Init(void);
uint8_t MMA7660_ReadStatus(void);

#endif

