
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lib/encoding.h"

uint16_t unilen( uint16_t *uni)
{
	uint16_t di;
	 di = 0;
	for (;;) {
		if(*uni == 0)
			break;
		uni++;
		di++;
		if(di >= 128)
			return 0;
	}
	return di;
}

uint16_t big2litel_endian(uint16_t *da,uint16_t len)
{
	uint16_t i;
	uint8_t *pt0,*pt1,c;
	for(i = 0;i < len;i++)
	{
		pt0 = (uint8_t *)&da[i];
		pt1 = (pt0 + 1);
		c = *pt0;
		*pt0 = *pt1;
		*pt1 = c;
	}
	return 0;
}

int utf8s_to_ucs2s (int16_t *out,uint8_t *in)
{
	uint16_t len = 0;
	uint8_t *next;
	*out = 0;
	for(;;)
	{
		*out = utf8_to_ucs2(in,&next);
		in = next;
		if(*out == -1)
		{
			*out = 0;	
			break;
		}
		else
		{
			out++;
			len++;
		}
	}
	return len;
}
/* Convert a UTF-8 encoded character in "input" into a number. This
   function returns the unicode value of the UTF-8 character if
   successful, and -1 if not successful. "end_ptr" is set to the next
   character after the read character on success. "end_ptr" is set to
   the start of input on failure. "end_ptr" may not be null. */

int utf8_to_ucs2 (uint8_t *input,uint8_t **end_ptr)
{
    *end_ptr = input;
    if (input[0] == 0)
        return -1;
    if (input[0] < 0x80) {
        *end_ptr = input + 1;
        return input[0];
    }
    if ((input[0] & 0xE0) == 0xE0) {
        if (input[1] == 0 || input[2] == 0)
            return -1;
        *end_ptr = input + 3;
        return
            (input[0] & 0x0F)<<12 |
            (input[1] & 0x3F)<<6  |
            (input[2] & 0x3F);
    }
    if ((input[0] & 0xC0) == 0xC0) {
        if (input[1] == 0)
            return -1;
        *end_ptr = input + 2;
        return
            (input[0] & 0x1F)<<6  |
            (input[1] & 0x3F);
    }
    return -1;
}

/* Input: a Unicode code point, "ucs2". 

   Output: UTF-8 characters in buffer "utf8". 

   Return value: the number of bytes written into "utf8", or -1 if
   there was an error.

   This adds a zero byte to the end of the string. It assumes that the
   buffer "utf8" has at least four bytes of space to write to. */

int ucs2_to_utf8 (int ucs2, uint8_t * utf8)
{
    if (ucs2 < 0x80) {
        utf8[0] = ucs2;
        utf8[1] = '\0';
        return 1;
    }
    if (ucs2 >= 0x80  && ucs2 < 0x800) {
        utf8[0] = (ucs2 >> 6)   | 0xC0;
        utf8[1] = (ucs2 & 0x3F) | 0x80;
        utf8[2] = '\0';
        return 2;
    }
    if (ucs2 >= 0x800 && ucs2 < 0xFFFF) {
			if (ucs2 >= 0xD800 && ucs2 <= 0xDFFF) {
					/* Ill-formed. */
					return 0;
			}
			utf8[0] = ((ucs2 >> 12)       ) | 0xE0;
			utf8[1] = ((ucs2 >> 6 ) & 0x3F) | 0x80;
			utf8[2] = ((ucs2      ) & 0x3F) | 0x80;
			utf8[3] = '\0';
			return 3;
    }
    if (ucs2 >= 0x10000 && ucs2 < 0x10FFFF) {
			/* http://tidy.sourceforge.net/cgi-bin/lxr/source/src/utf8.c#L380 */
			utf8[0] = 0xF0 | (ucs2 >> 18);
			utf8[1] = 0x80 | ((ucs2 >> 12) & 0x3F);
			utf8[2] = 0x80 | ((ucs2 >> 6) & 0x3F);
			utf8[3] = 0x80 | ((ucs2 & 0x3F));
			utf8[4] = '\0';
			return 4;
		}
    return -1;
}

uint16_t *uni_srtsrt(uint16_t *s1,uint16_t *s2)
{
	uint16_t *u16pt0 = s2,*res = 0;
	for(;;)
	{
		if(*s1 == *u16pt0)
		{
				if(res == 0)
						res = s1;
				u16pt0++;
				if(*u16pt0 == 0)
					break;
		}
		else
		{
			res = 0;
			u16pt0 = s2;
		}
		s1++;
		if(*s1 == 0) 
			break;
	}
	return res;
}




