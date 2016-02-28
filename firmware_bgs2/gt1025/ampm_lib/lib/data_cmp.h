

#ifndef __DATA_CMP_H__
#define __DATA_CMP_H__

#include <stdint.h>

typedef struct {
	uint8_t c;
	uint8_t *buff;
	uint8_t index;
	uint8_t len;
}COMPARE_TYPE;

void InitFindData(COMPARE_TYPE *cmpData,uint8_t *data);
uint8_t FindData(COMPARE_TYPE *cmpData,uint8_t c);



#endif

