#ifndef __FREQ_CAPTURE_H__
#define __FREQ_CAPTURE_H__
#include <stdio.h>

extern uint8_t  captureDoneFlag;
extern uint32_t  captureValue;
extern double speedFreq,speedFreqOld;
extern uint32_t capturePulseCnt;
extern void InitCapture(uint32_t pclk);

#endif

