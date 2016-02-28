/**
* \file
*         I2C driver
* \author
*         Nguyen Van Hai <hainv@ivx.vn>
*/
/* Includes ------------------------------------------------------------------*/
#include "I2CRoutines.h"



void I2C_RoutinesInit(uint32_t pclk1)
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
// 	/* Configure speed in standard mode 100kHz*/
// 	result = (uint16_t)(pclk1 / 100000 / 2);
// 	I2C1->CCR	= result;
	result = (uint16_t)(pclk1 / (400000 * 3));
	I2C1->TRISE = (uint16_t)(((pclk1 / 1000000) * (uint16_t)300 / (uint16_t)1000) + (uint16_t)1);
	I2C1->CCR	= (uint16_t)(result | CCR_FS_Set);
	/* Enable the selected I2C peripheral */
	I2C1->CR1 |= I2C_CR1_PE;
	/* Clear ACK, SMBTYPE and  SMBUS bits */
	I2C1->CR1 &= CR1_CLEAR_Mask;
	I2C1->CR1 |= (uint16_t)(I2C_CR1_ACK);
}

uint8_t I2C_Master_BufferRead(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToRead,uint8_t SlaveAddress)

{
    __IO uint32_t temp = 0;
    __IO uint32_t Timeout = 0;

    /* Enable I2C errors interrupts (used in all modes: Polling, DMA and Interrupts */
    I2Cx->CR2 |= I2C_IT_ERR;

		if (NumByteToRead == 1)
		{
				Timeout = 0xFFFF;
				/* Send START condition */
				I2Cx->CR1 |= CR1_START_Set;
				/* Wait until SB flag is set: EV5  */
				while ((I2Cx->SR1&0x0001) != 0x0001)
				{
						if (Timeout-- == 0)
								return 1;
				}
				/* Send slave address */
				/* Reset the address bit0 for read */
				SlaveAddress |= OAR1_ADD0_Set;
				/* Send the slave address */
				I2Cx->DR = SlaveAddress;
				/* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
				and program the STOP just after ADDR is cleared. The EV6_3 
				software sequence must complete before the current byte end of transfer.*/
				/* Wait until ADDR is set */
				Timeout = 0xFFFF;
				while ((I2Cx->SR1&0x0002) != 0x0002)
				{
						if (Timeout-- == 0)
								return 1;
				}
				/* Clear ACK bit */
				I2Cx->CR1 &= CR1_ACK_Reset;
				/* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
				software sequence must complete before the current byte end of transfer */
				__disable_irq();
				/* Clear ADDR flag */
				temp = I2Cx->SR2;
				/* Program the STOP */
				I2Cx->CR1 |= CR1_STOP_Set;
				/* Re-enable IRQs */
				__enable_irq();
				/* Wait until a data is received in DR register (RXNE = 1) EV7 */
				while ((I2Cx->SR1 & 0x00040) != 0x000040);
				/* Read the data */
				*pBuffer = I2Cx->DR;
				/* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
				while ((I2Cx->CR1&0x200) == 0x200);
				/* Enable Acknowledgement to be ready for another reception */
				I2Cx->CR1 |= CR1_ACK_Set;

		}

		else if (NumByteToRead == 2)
		{
				/* Set POS bit */
				I2Cx->CR1 |= CR1_POS_Set;
				Timeout = 0xFFFF;
				/* Send START condition */
				I2Cx->CR1 |= CR1_START_Set;
				/* Wait until SB flag is set: EV5 */
				while ((I2Cx->SR1&0x0001) != 0x0001)
				{
						if (Timeout-- == 0)
								return 1;
				}
				Timeout = 0xFFFF;
				/* Send slave address */
				/* Set the address bit0 for read */
				SlaveAddress |= OAR1_ADD0_Set;
				/* Send the slave address */
				I2Cx->DR = SlaveAddress;
				/* Wait until ADDR is set: EV6 */
				while ((I2Cx->SR1&0x0002) != 0x0002)
				{
						if (Timeout-- == 0)
								return 1;
				}
				/* EV6_1: The acknowledge disable should be done just after EV6,
				that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
				ACK clearing */
				__disable_irq();
				/* Clear ADDR by reading SR2 register  */
				temp = I2Cx->SR2;
				/* Clear ACK */
				I2Cx->CR1 &= CR1_ACK_Reset;
				/*Re-enable IRQs */
				__enable_irq();
				/* Wait until BTF is set */
				while ((I2Cx->SR1 & 0x00004) != 0x000004);
				/* Disable IRQs around STOP programming and data reading because of the limitation ?*/
				__disable_irq();
				/* Program the STOP */
					I2Cx->CR1 |= CR1_STOP_Set;
				/* Read first data */
				*pBuffer = I2Cx->DR;
				/* Re-enable IRQs */
				__enable_irq();
				/**/
				pBuffer++;
				/* Read second data */
				*pBuffer = I2Cx->DR;
				/* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
				while ((I2Cx->CR1&0x200) == 0x200);
				/* Enable Acknowledgement to be ready for another reception */
				I2Cx->CR1  |= CR1_ACK_Set;
				/* Clear POS bit */
				I2Cx->CR1  &= CR1_POS_Reset;

		}

		else

		{

				Timeout = 0xFFFF;
				/* Send START condition */
				I2Cx->CR1 |= CR1_START_Set;
				/* Wait until SB flag is set: EV5 */
				while ((I2Cx->SR1&0x0001) != 0x0001)
				{
						if (Timeout-- == 0)
								return 1;
				}
				Timeout = 0xFFFF;
				/* Send slave address */
				/* Reset the address bit0 for write */
				SlaveAddress |= OAR1_ADD0_Set;;
				/* Send the slave address */
				I2Cx->DR = SlaveAddress;
				/* Wait until ADDR is set: EV6 */
				while ((I2Cx->SR1&0x0002) != 0x0002)
				{
						if (Timeout-- == 0)
								return 1;
				}
				/* Clear ADDR by reading SR2 status register */
				temp = I2Cx->SR2;
				/* While there is data to be read */
				while (NumByteToRead)
				{
						/* Receive bytes from first byte until byte N-3 */
						if (NumByteToRead != 3)
						{
								/* Poll on BTF to receive data because in polling mode we can not guarantee the
								EV7 software sequence is managed before the current byte transfer completes */
								while ((I2Cx->SR1 & 0x00004) != 0x000004);
								/* Read data */
								*pBuffer = I2Cx->DR;
								/* */
								pBuffer++;
								/* Decrement the read bytes counter */
								NumByteToRead--;
						}

						/* it remains to read three data: data N-2, data N-1, Data N */
						if (NumByteToRead == 3)
						{

								/* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
								while ((I2Cx->SR1 & 0x00004) != 0x000004);
								/* Clear ACK */
								I2Cx->CR1 &= CR1_ACK_Reset;

								/* Disable IRQs around data reading and STOP programming because of the
								limitation ? */
								__disable_irq();
								/* Read Data N-2 */
								*pBuffer = I2Cx->DR;
								/* Increment */
								pBuffer++;
								/* Program the STOP */
								I2Cx->CR1 |= CR1_STOP_Set;
								/* Read DataN-1 */
								*pBuffer = I2Cx->DR;
								/* Re-enable IRQs */
								__enable_irq();
								/* Increment */
								pBuffer++;
								/* Wait until RXNE is set (DR contains the last data) */
								while ((I2Cx->SR1 & 0x00040) != 0x000040);
								/* Read DataN */
								*pBuffer = I2Cx->DR;
								/* Reset the number of bytes to be read by master */
								NumByteToRead = 0;

						}
				}
				/* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
				while ((I2Cx->CR1&0x200) == 0x200);
				/* Enable Acknowledgement to be ready for another reception */
				I2Cx->CR1 |= CR1_ACK_Set;
		}
    return 0;
}



/**
  * @brief  Writes buffer of bytes.
  * @param pBuffer: Buffer of bytes to be sent to the slave.
  * @param NumByteToWrite: Number of bytes to be sent by the Master.
  * @param Mode: Polling or DMA or Interrupt having the highest priority in the application.
  * @param SlaveAddress: The address of the slave to be addressed by the Master.
  * @retval : None.
  */
uint8_t I2C_Master_BufferWrite(I2C_TypeDef* I2Cx, uint8_t* pBuffer,  uint32_t NumByteToWrite, uint8_t SlaveAddress )
{

	__IO uint32_t temp = 0;
	__IO uint32_t Timeout = 0;
	Timeout = 0xFFFF;
	/* Send START condition */
	I2Cx->CR1 |= CR1_START_Set;
	/* Wait until SB flag is set: EV5 */
	while ((I2Cx->SR1&0x0001) != 0x0001)
	{
			if (Timeout-- == 0)
					return 1;
	}

	/* Send slave address */
	/* Reset the address bit0 for write*/
	SlaveAddress &= OAR1_ADD0_Reset;
	/* Send the slave address */
	I2Cx->DR = SlaveAddress;
	Timeout = 0xFFFF;
	/* Wait until ADDR is set: EV6 */
	while ((I2Cx->SR1 &0x0002) != 0x0002)
	{
			if (Timeout-- == 0)
					return 1;
	}

	/* Clear ADDR flag by reading SR2 register */
	temp = I2Cx->SR2;
	/* Write the first data in DR register (EV8_1) */
	I2Cx->DR = *pBuffer;
	/* Increment */
	pBuffer++;
	/* Decrement the number of bytes to be written */
	NumByteToWrite--;
	/* While there is data to be written */
	while (NumByteToWrite--)
	{
			/* Poll on BTF to receive data because in polling mode we can not guarantee the
				EV8 software sequence is managed before the current byte transfer completes */
			while ((I2Cx->SR1 & 0x00004) != 0x000004);
			/* Send the current byte */
			I2Cx->DR = *pBuffer;
			/* Point to the next byte to be written */
			pBuffer++;
	}
	/* EV8_2: Wait until BTF is set before programming the STOP */
	while ((I2Cx->SR1 & 0x00004) != 0x000004);
	/* Send STOP condition */
	I2Cx->CR1 |= CR1_STOP_Set;
	/* Make sure that the STOP bit is cleared by Hardware */
	while ((I2Cx->CR1&0x200) == 0x200);
	return 0;
}





