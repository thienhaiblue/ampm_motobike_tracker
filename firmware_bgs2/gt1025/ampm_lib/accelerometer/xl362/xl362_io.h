/*----------------------------------------------------------------------
  File Name          : 
  Author             : MPD Application Team
  Version            : V0.0.1
  Date               : 11/06/2008
  Description        : 
  File ID            : $Id: xl362_io.h,v 1.1.1.1 2012/01/10 15:18:18 jlee11 Exp $

  Analog Devices ADXL 345 digital output accellerometer 
  with advanced digital features.

  (c) 2008 Analog Devices application support team.
  xxx@analog.com

  ----------------------------------------------------------------------

  The present firmware which is for guidance only aims at providing
  customers with coding information regarding their products in order
  for them to save time.  As a result, Analog Devices shall not be
  held liable for any direct, indirect or consequential damages with
  respect to any claims arising from the content of such firmware and/or
  the use made by customers of the coding information contained herein
  in connection with their products.

----------------------------------------------------------------------*/
#ifndef __XL362_IO_H
#define __XL362_IO_H
#include "xl362/XL362.h"
#include <stdint.h>

#if defined ( __ICCARM__ )
#pragma pack(1) 
typedef struct {
	int8_t x;
	int8_t y;
	int8_t z;
	uint8_t status;
	uint16_t fifo_samples;
	int16_t x16;
	int16_t y16;
	int16_t z16;
	int16_t temp;
	int16_t adc;
} XL362_Type;

#pragma pack()
#elif defined (__CC_ARM)
typedef struct __attribute__((packed)){
	int8_t x;
	int8_t y;
	int8_t z;
	uint8_t status;
	uint16_t fifo_samples;
	int16_t x16;
	int16_t y16;
	int16_t z16;
	int16_t temp;
	int16_t adc;
} XL362_Type;

#endif




void xl362Init(void);
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
void xl362Read(uint8_t count, uint8_t regaddr, uint8_t *buf);
/*
  The fifo read function takes a byte count as an int and a
  pointer to the buffer where to return the data.  When the read
  function runs, it goes through the following sequence:

    1) CS_N Low
    2) Send the correct command
    4) Read each byte
    5) CS_N high
*/
void xl362FifoRead(uint16_t count, uint8_t *buf);
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
void xl362Write(uint8_t count, uint8_t regaddr, uint8_t *buf);
#endif
