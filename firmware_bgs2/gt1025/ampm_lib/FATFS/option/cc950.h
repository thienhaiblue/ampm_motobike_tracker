
#ifndef __CC950_H__
#define __CC950_H__

#include <stdint.h>

uint16_t uni_convert (	/* Converted code, 0 means conversion error */
	uint16_t	chr,	/* Character code to be converted */
	uint32_t	dir,		/* 0: Unicode to OEMCP, 1: OEMCP to Unicode */
	uint32_t litel_endian
);

 uint16_t uni_wtoupper (	/* Upper converted character */
	uint16_t chr		/* Input character */
);

uint16_t unilen( uint16_t *uni);
uint16_t big2litel_endian(uint16_t *da,uint16_t len);

int16_t ConverBig5_Unicode(uint16_t *out	/* Pointer to pointer to the segment in the path string */,
		const uint8_t *path	/* Pointer to pointer to the segment in the path string */
);

int16_t ConverASCII_Unicode(uint8_t *out	/* Pointer to pointer to the segment in the path string */,
		const uint8_t *path	/* Pointer to pointer to the segment in the path string */
);

uint16_t *uni_srtsrt(uint16_t *s1,uint16_t *s2);

#endif


