#ifndef _FCS_H_
#define _FCS_H_
#include <stdint.h>
uint16_t FCS_Get(uint8_t *cp, uint32_t len);
uint16_t FCS_Calculate(uint8_t val, uint16_t fcs);

#endif
