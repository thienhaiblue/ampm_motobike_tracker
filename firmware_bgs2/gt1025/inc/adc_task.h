

#ifndef __ADC_TASK_H__
#define __ADC_TASK_H__
#include <stm32f10x.h>


#define ADC_BUF_LEN       40

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define Avg_Slope	0.0043 // V/°C
#define V25			1.43

extern __IO int16_t ADC1ConvertedValue[ADC_BUF_LEN];
extern int32_t ADC1ConvertedValueTotal[3];
extern float ADC1_10_Value;
extern float ADC1_11_Value;
extern float temperature;
extern float ADC1_Temperature;
extern uint8_t ADC_Updated;
void ADC_TaskInit(void);
void ADC_TaskDeInit(void);
void ADC_Task(uint32_t rtcTime);

#endif


