
#include "xl362/xl362_io.h"
#include "spi.h"


void xl362Init(void)
{
		uint32_t i;
		uint8_t buffer[16];
		/* soft reset for safety */
		buffer[0] = XL362_SOFT_RESET_KEY;
		xl362Write(1,XL362_SOFT_RESET,buffer);
		/* wait for soft reset to pass */
		i = 10000;
		while(i--); 

		/* set up a buffer with all the initialization for activity and inactivity */
		buffer[0] = 200;  /* XL362_THRESH_ACTL about 15 degrees*/
		buffer[1] = 0  ;  /* THRESH_ACTH */
		buffer[2] = 3  ;  /* TIME_ACT */
		buffer[3] = 105;  /* THRESH_INACTL*/
		buffer[4] = 0  ;  /* THRESH_INACTH */
		buffer[5] = 37 ;  /* TIME_INACTL 3 seconds at 12.5 hz*/
		buffer[6] = 0  ;  /* TIME_INACTH */
		buffer[7] =     /* ACT_INACT_CTL */
								 XL362_ACT_ENABLE | XL362_ACT_AC | XL362_INACT_ENABLE
			| XL362_INACT_AC | XL362_ACT_INACT_LINK | XL362_ACT_INACT_LOOP;
		buffer[8] =     XL362_FIFO_MODE_STREAM;/* FIFO CONTROL */
		buffer[9] =    150;/*FIFO SAMPLES*/
		xl362Write(10,XL362_THRESH_ACTL,buffer);

		/* set up a buffer with all the initization for intmaps filter and power*/
		buffer[0] = XL362_INT_AWAKE | XL362_INT_LOW; /* INTMAP1 */
		buffer[1] = 0; /* INTMAP2 */
		buffer[2] =     /* FILTER_CTL */
			XL362_RATE_50 |  XL362_RANGE_2G;
		buffer[3] =     /* POWER_CTL */
			XL362_MEASURE_3D |  XL362_LOW_POWER;
		xl362Write(4,XL362_INTMAP1,buffer);
		i = 10000;
		while(i--); 
}

/* Wrapper functions for reading and writing bursts to / from the XL362
   Will need to be modified for your hardware 
*/

/*
  The read function takes a byte count, a register address and a
  pointer to the buffer where to return the data.  When the read
  function runs, it goes through the following sequence:

    1) CS_N Low
    2) Send the correct command
    3) Send the register address
    4) Read each byte
    5) CS_N high
*/
void xl362Read(uint8_t count, uint8_t regaddr, uint8_t *buf)
{
	HAL_SPI1_ACCEL_CS_ASSERT;
	halSpiWriteByte(SPI1,XL362_REG_READ);
	halSpiWriteByte(SPI1,regaddr);
	while(count--)
		*buf++ = halSpiWriteByte(SPI1,0xff);
	HAL_SPI1_ACCEL_CS_DEASSERT;
}
/*
  The fifo read function takes a byte count as an int and a
  pointer to the buffer where to return the data.  When the read
  function runs, it goes through the following sequence:

    1) CS_N Low
    2) Send the correct command
    4) Read each byte
    5) CS_N high
*/
void xl362FifoRead(uint16_t count, uint8_t *buf)
{
	HAL_SPI1_ACCEL_CS_ASSERT;
	halSpiWriteByte(SPI1,XL362_FIFO_READ);
	while(count--)
		*buf++ = halSpiWriteByte(SPI1,0xff);
	HAL_SPI1_ACCEL_CS_DEASSERT;
}
/*
  The write function takes a byte count, and a pointer to the buffer
  with the data.  The first byte of the data should be the start
  register address, the remaining bytes will be written starting at
  that register.  The mininum bytecount that shoudl be passed is 2,
  one byte of address, followed by a byte of data.  Multiple
  sequential registers can be written with longer byte counts. When
  the write function runs, it goes through the following sequence:

    1) CS_N Low
    2) Send the correct command
    3) Send the register address
    4) Send each byte
    5) CS_N high
*/
void xl362Write(uint8_t count, uint8_t regaddr, uint8_t *buf)
{
	HAL_SPI1_ACCEL_CS_ASSERT;
	halSpiWriteByte(SPI1,XL362_REG_WRITE);
	halSpiWriteByte(SPI1,regaddr);
	while(count)
	{
		halSpiWriteByte(SPI1,*buf++);
		
		count--;
	}
	HAL_SPI1_ACCEL_CS_DEASSERT;
}





