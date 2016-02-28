/**
* \file
*         i2c driver
* \author
*         Nguyen Van Hai <hainv@ivx.vn>
*/

#include "i2c.h"

/* --EV5 */
#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */
/* --EV6 */
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */
/* --EV9 */
#define  I2C_EVENT_MASTER_MODE_ADDRESS10                   ((uint32_t)0x00030008)  /* BUSY, MSL and ADD10 flags */


/* Master RECEIVER mode -----------------------------*/ 
/* --EV7 */
#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */

/* Master TRANSMITTER mode --------------------------*/
/* --EV8 */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */
/* --EV8_2 */
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */

#define FLAG_Mask               ((uint32_t)0x00FFFFFF)

void I2C_Init(uint32_t pclk1)
{
	uint32_t result;
	/* Clear frequency FREQ[5:0] bits */
	I2C1->CR2 &= ~I2C_CR2_FREQ;
	 /* Set frequency bits depending on pclk1 value */
	I2C1->CR2 |= (uint16_t)(pclk1 / 1000000);
	 /* Disable the selected I2C peripheral to configure TRISE */
	I2C1->CR1 &= ~I2C_CR1_PE;
	 /* Clear F/S, DUTY and CCR[11:0] bits */
	I2C1->TRISE = (pclk1 / 1000000) + 1;
	/* Configure speed in standard mode 100kHz*/
	result = (uint16_t)(pclk1 / 100000 / 2);
	I2C1->CCR	= result;
	/* Enable the selected I2C peripheral */
	I2C1->CR1 |= I2C_CR1_PE;
	/* Clear ACK, SMBTYPE and  SMBUS bits */
	I2C1->CR1 |= (uint16_t)(I2C_CR1_ACK);
	/* I2C1 Enable */
	I2C1->CR1 = I2C_CR1_PE;
}

uint32_t I2C_CheckStatus(uint32_t event)
{
	uint32_t temp;
		temp = I2C1->SR2;
		temp <<= 16;
		temp |= I2C1->SR1;
		temp &= FLAG_Mask;
		if((temp & event) == event) return 0;
		else return 1;
}


uint8_t I2C_ReadByte(uint8_t slaveAddr,uint8_t dataAddr)
{
	uint32_t I2C_timeOut = 10000000;
	uint8_t res;
	/* Disable the acknowledgement */
	I2C1->CR1 &= ~I2C_CR1_ACK;
	/* Generate a START condition */
	I2C1->CR1 |= I2C_CR1_START;
	/* Test on I2C1 EV5 and clear it */
	while ((I2C1->SR1&0x0001) != 0x0001)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	
	/* Send slave address for write */
	I2C1->DR = slaveAddr & 0xfe;	
	 /* Test on I2C1 EV6  and clear it */
	while ((I2C1->SR1 &0x0002) != 0x0002)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	/* Clear ADDR flag by reading SR2 register */
  res = I2C1->SR2;
	/*Send data*/
	I2C1->DR = dataAddr;
	/* Test on I2C1 EV8  and clear it */
	while ((I2C1->SR1 & 0x00004) != 0x000004);
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	/* Generate a START condition */
	I2C1->CR1 |= I2C_CR1_START;
	/* Test on I2C1 EV5 and clear it */
	while ((I2C1->SR1&0x0001) != 0x0001)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	/* Send slave address for write */
	I2C1->DR = slaveAddr | I2C_OAR1_ADD0;
	 /* Test on I2C1 EV6  and clear it */
	while ((I2C1->SR1&0x0002) != 0x0002)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}	
	 I2C1->CR1 &= ~I2C_CR1_ACK;
	res = I2C1->SR2;
	/* Send I2C1 STOP Condition */
	I2C1->CR1 |= I2C_CR1_STOP;
	/* Wait until a data is received in DR register (RXNE = 1) EV7 */
  while ((I2C1->SR1 & 0x00040) != 0x000040);
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}	
	res = (uint8_t)I2C1->DR;
	/* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
	while ((I2C1->CR1&0x200) == 0x200);
	/* Enable Acknowledgement to be ready for another reception */
	I2C1->CR1 |= I2C_CR1_ACK;
	return res;
}

uint8_t I2C_SendByte(uint8_t addr,uint8_t data)
{
	uint32_t I2C_timeOut = 10000000,temp;
	/* Disable the acknowledgement */
	I2C1->CR1 &= ~I2C_CR1_ACK;
	/* Generate a START condition */
	I2C1->CR1 |= I2C_CR1_START;
	/* Test on I2C1 EV5 and clear it */
	while ((I2C1->SR1&0x0001) != 0x0001)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	I2C1->DR = addr & 0xfe;	/* Send slave address for write */
	/* Test on I2C1 EV6  and clear it */
	while ((I2C1->SR1 &0x0002) != 0x0002)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	/* Clear ADDR flag by reading SR2 register */
   temp = I2C1->SR2;
	I2C1->DR = data;
	/* EV8_2: Wait until BTF is set before programming the STOP */
  while ((I2C1->SR1 & 0x00004) != 0x000004)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	/* Send I2C1 STOP Condition */
	I2C1->CR1 |= I2C_CR1_STOP;
	/* Make sure that the STOP bit is cleared by Hardware */
  while ((I2C1->CR1&0x200) == 0x200)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	return 0;
}

// uint8_t I2C_SendData(uint8_t addr,uint8_t *data,uint32_t len)
// {
// 	uint32_t I2C_timeOut = 0x3ffff,i;
// 	/* Disable the acknowledgement */
// 	I2C1->CR1 &= ~I2C_CR1_ACK;
// 	/* Generate a START condition */
// 	I2C1->CR1 |= I2C_CR1_START;
// 	/* Test on I2C1 EV5 and clear it */
// 	while(I2C_CheckStatus(I2C_EVENT_MASTER_MODE_SELECT)) 
// 	{
// 		//if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
// 	}
// 	I2C1->DR = addr & 0xfe;	/* Send slave address for write */
// 	/* Test on I2C1 EV6  and clear it */
// 	while(I2C_CheckStatus(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
// 	{
// 		//if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
// 	}
// 	for(i = 0;i < len; i++)
// 	{
// 		/*Send data*/
// 		I2C1->DR = data[i];
// 		/* Test on I2C1 EV8  and clear it */
// 		while(I2C_CheckStatus(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) 
// 		{
// 			//if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
// 		}
// 	}
// 	/* EV8_2: Wait until BTF is set before programming the STOP */
//    while ((I2C1->SR1 & 0x00004) != 0x000004);
// 	/* Send I2C1 STOP Condition */
// 	I2C1->CR1 |= I2C_CR1_STOP;
// 	/* Make sure that the STOP bit is cleared by Hardware */
//    while ((I2C1->CR1&0x200) == 0x200);
// 	return 0;
// }

uint8_t I2C_SendData(uint8_t addr,uint8_t *data,uint32_t len)
{
	uint32_t I2C_timeOut = 10000000,temp;
	/* Disable the acknowledgement */
	I2C1->CR1 &= ~I2C_CR1_ACK;
	/* Generate a START condition */
	I2C1->CR1 |= I2C_CR1_START;
	/* Test on I2C1 EV5 and clear it */
	while ((I2C1->SR1&0x0001) != 0x0001)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	I2C1->DR = addr & 0xfe;	/* Send slave address for write */
	/* Test on I2C1 EV6  and clear it */
	while ((I2C1->SR1 &0x0002) != 0x0002)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	/* Clear ADDR flag by reading SR2 register */
   temp = I2C1->SR2;
	I2C1->DR = *data;
	data++;
	len--;
	while(len--)
	{
		/* Test on I2C1 EV8  and clear it */
		while ((I2C1->SR1 & 0x00004) != 0x000004);
		{
			if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
		}
		/*Send data*/
		I2C1->DR = *data;
		data++;
	}
	/* EV8_2: Wait until BTF is set before programming the STOP */
  while ((I2C1->SR1 & 0x00004) != 0x000004)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	/* Send I2C1 STOP Condition */
	I2C1->CR1 |= I2C_CR1_STOP;
	/* Make sure that the STOP bit is cleared by Hardware */
  while ((I2C1->CR1&0x200) == 0x200)
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	return 0;
}


uint8_t I2C_CheckSlaveStatus(uint8_t addr)
{
	uint32_t I2C_timeOut = 10000000;
	I2C1->SR1 = (uint16_t)~(I2C_SR1_AF & FLAG_Mask);
	/* Disable the acknowledgement */
	I2C1->CR1 &= ~I2C_CR1_ACK;
	/* Generate a START condition */
	I2C1->CR1 |= I2C_CR1_START;
	/* Test on I2C1 EV5 and clear it */
	while(I2C_CheckStatus(I2C_EVENT_MASTER_MODE_SELECT))
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
		/* Send slave address for write */
	I2C1->DR = addr & 0xfe;	
	/* Test on I2C1 EV6  and clear it */
	while(I2C_CheckStatus(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
	{
		if(I2C_timeOut-- == 0){I2C1->CR1 |= I2C_CR1_STOP; return 0xff;}
	}
	/* Check Acknowledge failure*/
	if(I2C1->SR1 & I2C_SR1_AF) {I2C1->CR1 |= I2C_CR1_STOP; return 0xff;} /*Acknowledge failure*/
	/* Send I2C1 STOP Condition */
	I2C1->CR1 |= I2C_CR1_STOP;
	return 0;
}

