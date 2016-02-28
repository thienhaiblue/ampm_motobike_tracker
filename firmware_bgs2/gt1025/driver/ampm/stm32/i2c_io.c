
#include "i2c_io.h"

//**************************************************************************
//              thienhaiblue@ictuonglai.com
//* ***********************************************************************/
#include <stdint.h>
#include "stm32f10x.h"

#define MAX_I2C_TRIES 200

/* This delay is needed for EEPROMs */
static void delay(int i)
{
		i *= 10;
    while(i--);
}

static void I2C_Start(void)
{
    /* Send a Start to the EEPROM */
    /* A Start is the data bit falling while the clock is high */
    I2C_SDA_SET;
    delay(2);   //At least ???
    I2C_SCL_SET;
    delay(5);   //At least 4.7us
    I2C_SDA_CLR;
    delay(5);   //At least 4.7us
    I2C_SCL_CLR;
}

static void I2C_Stop(void)
{
    /* Send a Stop to the EEPROM */
    /* A Stop is the data bit rising while the clock is high */
    I2C_SDA_CLR;
    delay(2);   //At least ???
    I2C_SCL_SET;
    delay(5);   //At least 4.7us
    I2C_SDA_SET;
    delay(5);   //At least 4.7us
    I2C_SCL_CLR;
}

int I2C_Send(uint8_t data)
{
    int i;
    
    /* Send 8 bits of data */
    I2C_SCL_CLR;
    for (i = 8;  i > 0;  i--)
    {
        /* The data can now change without delay */
        if (data & 0x80)
            I2C_SDA_SET;
        else
            I2C_SDA_CLR;
        data <<= 1;
        /* Pulse the clock high while the data bit is steady */
        delay(5);   //At least 4.7us
        I2C_SCL_SET;
        delay(5);   //At least 4.7us
        I2C_SCL_CLR;
    }
    /* Check the acknowledgement from the EEPROM */
    /* Pulse the clock high, and see what the device says while the clock is high */
    delay(5);
    I2C_SDA_SET;
    delay(5);
    I2C_SCL_SET;
    delay(5);
    i = I2C_SDA_PORT_IN & I2C_SDA_BIT;
    I2C_SCL_CLR;
    delay(4);
    return i;
}

uint8_t I2C_Receive(int ok)
{
    uint8_t data;
    int i;

    /* Get 8 bits of data */
    I2C_SDA_SET;     //Input
    data = 0;
    for (i = 8;  i > 0;  i--)
    {
        I2C_SCL_SET;
        delay(5);   //At least 4.7us
        data <<= 1;
        if (I2C_SDA_PORT_IN & I2C_SDA_BIT)
            data |= 0x01;
        I2C_SCL_CLR;
        delay(5);   //At least 4.7us
    }
    /* Send the acknowledgement to the EEPROM */
    if (ok)
        I2C_SDA_CLR;
    else
        I2C_SDA_SET;
    delay(4);
    I2C_SCL_SET;
    delay(4);
    I2C_SCL_CLR;
    I2C_SDA_SET;
    return data;
}

static int I2C_Test_SDA(void)
{
    int i;
  
    I2C_Stop();
    I2C_SDA_SET;
    delay(4);
    for (i = 16;  i > 0;  i--)
    {
        delay(5);
        if (!(I2C_SDA_PORT_IN & I2C_SDA_BIT))
            break;
    }
    return i;
}

int I2C_Init(uint32_t freq)
{
    int i;
		I2C_SCL_PIN_OPEN_DRAIN;
		I2C_SDA_PIN_OPEN_DRAIN;
    /* While idling, the EEPROM clock should be low */
    I2C_SCL_CLR;
    /* If we happen to have reStarted in the middle of a read from
       the DEVICE, we need to regain control of the device. If we
       give it enough clocks, and do no acknowledge it we should get out
       of any odd conditions. Then we do a Stop, and we should be OK. If
       the device was mid write when the reStart occurred we cannot really
       act in a clean way. */
    delay(5);   //At least 4.7us
    for (i = 0;  i < 16;  i++)
    {
        I2C_SCL_SET;
        delay(5);   //At least 4.7us
        I2C_SCL_CLR;
        delay(5);   //At least 4.7us
    }
    I2C_Stop();
    return 0;
}

uint8_t I2C_ReadByte(uint8_t slaveAddr,uint8_t dataAddr,uint8_t *data)
{
	int i;
	__disable_irq();
	for (i = 0;  i < MAX_I2C_TRIES;  ++i)
	{
		if (i)
		{
				/* Read FALSE, retry */
				if (I2C_Test_SDA())
						continue;
		}
		I2C_Start();
		if (I2C_Send(slaveAddr & 0xfe))
				continue;
		if (I2C_Send(dataAddr))
				continue;
		I2C_Start();
		if (I2C_Send(slaveAddr & 0x01))
				continue;
		*data = I2C_Receive(0);
		I2C_Stop();
		__enable_irq();
		return 0;
  }
	I2C_Stop();
	__enable_irq();
	return 1;
}



int I2C_SendData(uint8_t slaveAddr,uint16_t dataAddr, uint8_t *dat, uint16_t len)
{
 int i;
    int j;
    uint16_t section_len;
    uint8_t *p;
    uint8_t *q;
    __disable_irq();
    /* If the write spreads across pages in the EEPROM, we need to split the write
       into sections. */
    q = dat;
    while (len > 0)
    {
        section_len = len;
        for (i = 0;  i < MAX_I2C_TRIES;  ++i)
        {
            if (i)
            {
                /* Write FALSE, retry */
                if (I2C_Test_SDA())
                    continue;
            }
						I2C_Start();
						if (I2C_Send(slaveAddr & 0xfe))
								continue;
						if (I2C_Send(dataAddr))
							continue;
            p = q;
            for (j = section_len;  j > 0;  j--)
            {
                if (I2C_Send(*p++))
                    break;
            }
            if (j == 0)
                break;
            I2C_Stop();
        }
        I2C_Stop();
        if (i >= MAX_I2C_TRIES)
        {
            __enable_irq();
            return 1;
        }
        len -= section_len;
        dataAddr += section_len;
        q += section_len;
    }
    __enable_irq();
    return 0;
	}
