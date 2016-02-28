#ifndef __I2C_H__
#define __I2C_H__
#include "stm32f10x.h"
#include <stdint.h>
#include "hw_config.h"


void I2C_Init(uint32_t pclk);
uint32_t I2C_CheckStatus(uint32_t event);
uint8_t I2C_ReadByte(uint8_t slaveAddr,uint8_t dataAddr);
uint8_t I2C_SendByte(uint8_t addr,uint8_t data);
uint8_t I2C_SendData(uint8_t addr,uint8_t *data,uint32_t len);
uint8_t I2C_CheckSlaveStatus(uint8_t addr);

#endif
