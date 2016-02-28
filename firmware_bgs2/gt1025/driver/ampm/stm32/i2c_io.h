
#ifndef __I2C_IO_H_
#define __I2C_IO_H_
#include <stdint.h>
#include "stm32f10x.h"


#define I2C_SCL_PIN_OPEN_DRAIN		GPIOB->CRL	&= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);GPIOB->CRL   |=  ( GPIO_CRL_CNF6_0  | GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1)	//Output Open-drain
#define I2C_SDA_PIN_OPEN_DRAIN		GPIOB->CRL	&= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);GPIOB->CRL   |=  ( GPIO_CRL_CNF7_0  | GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1)	//Output Open-drain
#define I2C_SDA_PORT_IN		GPIOB->IDR
#define I2C_SDA_BIT				GPIO_IDR_IDR7
#define I2C_SCL_PORT_IN		GPIOB->IDR
#define I2C_SCL_BIT				GPIO_IDR_IDR6
#define I2C_SDA_SET				GPIOB->BSRR = I2C_SDA_BIT
#define I2C_SDA_CLR				GPIOB->BRR = I2C_SDA_BIT
#define I2C_SCL_SET				GPIOB->BSRR = I2C_SCL_BIT
#define I2C_SCL_CLR				GPIOB->BRR = I2C_SCL_BIT

int I2C_Init(uint32_t freq);
int I2C_SendData(uint8_t slaveAddr,uint16_t dataAddr, uint8_t *dat, uint16_t len);
uint8_t I2C_ReadByte(uint8_t slaveAddr,uint8_t dataAddr,uint8_t *data);

#endif
