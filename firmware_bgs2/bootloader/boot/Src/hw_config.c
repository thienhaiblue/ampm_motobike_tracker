#include "stm32f10x.h"
#include "hw_config.h"
#include "system_config.h"

IOstatusType  ioStatus;
void IO_Init(void)
{
	uint8_t i;
	ioStatus.updateCnt = 0;
	for(i = 0;i < 4;i++)
	{
		ioStatus.din[i].bitNew = 0;
		ioStatus.din[i].bitOld = 0;
		ioStatus.din[i].highCnt = 0;
		ioStatus.din[i].lowCnt = 0;
	}
}
#define IO_FILLTER_CNT	1
void IO_Control(uint32_t time)
{
	uint8_t i;
	uint32_t val;
	ioStatus.updateCnt++;
	if((ioStatus.updateCnt % IO_FILLTER_CNT) == 0)
	{
		for(i = 0;i < 5;i++)
		{
			if((ioStatus.din[i].highCnt == IO_FILLTER_CNT) && (ioStatus.din[i].lowCnt == 0))
				ioStatus.din[i].bitNew = 1;
			else if((ioStatus.din[i].highCnt == 0) && (ioStatus.din[i].lowCnt == IO_FILLTER_CNT))
				ioStatus.din[i].bitNew = 0;
			ioStatus.din[i].highCnt = 0;
			ioStatus.din[i].lowCnt = 0;
			if(ioStatus.din[i].bitNew != ioStatus.din[i].bitOld)
			{
				ioStatus.din[i].newUpdate = 1;
				ioStatus.din[i].bitOld = ioStatus.din[i].bitNew;
			}
		}
	}
	if(GET_AVLIO1_PIN)
		ioStatus.din[0].highCnt++;
	else
		ioStatus.din[0].lowCnt++;
	if(GET_AVLIO2_PIN)
		ioStatus.din[1].highCnt++;
	else
		ioStatus.din[1].lowCnt++;
	if(GET_AVLIO3_PIN)
		ioStatus.din[2].highCnt++;
	else
		ioStatus.din[2].lowCnt++;
	if(GET_AVLIO4_PIN)
		ioStatus.din[3].highCnt++;
	else
		ioStatus.din[3].lowCnt++;
	if(GET_AVLIO5_PIN)
		ioStatus.din[4].highCnt++;
	else
		ioStatus.din[4].lowCnt++;
}


