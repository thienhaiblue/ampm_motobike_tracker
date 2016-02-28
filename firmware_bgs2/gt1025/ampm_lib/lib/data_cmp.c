#include <stdint.h>
#include <string.h>
#include "data_cmp.h"




void InitFindData(COMPARE_TYPE *cmpData,uint8_t *data)
{
	cmpData->c = '\0';
	cmpData->buff = data;
	cmpData->index = 0;
	cmpData->len = strlen((char *)data);
}
uint8_t FindData(COMPARE_TYPE *cmpData,uint8_t c)
{
	if(cmpData->buff[cmpData->index] == c) 
	{
		cmpData->c = c;
		cmpData->index++;
	}
	else if(cmpData->c == cmpData->buff[0])
	{
		cmpData->index = 1;
	}
	else 
		cmpData->index = 0;
	if(cmpData->index >= cmpData->len)
	{
		cmpData->c = '\0';
		return 0;
	}
	return 0xff;
}
