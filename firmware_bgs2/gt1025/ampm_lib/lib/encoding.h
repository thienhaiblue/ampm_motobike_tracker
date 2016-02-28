

#ifndef __ENCODING_H__
#define __ENCODING_H__

int utf8_to_ucs2 (uint8_t *input,uint8_t **end_ptr);
int utf8s_to_ucs2s (int16_t *out,uint8_t *in);
uint16_t big2litel_endian(uint16_t *da,uint16_t len);
uint16_t unilen( uint16_t *uni);
uint16_t *uni_srtsrt(uint16_t *s1,uint16_t *s2);

#endif
