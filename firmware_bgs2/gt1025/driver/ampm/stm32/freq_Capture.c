/*---------------------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "freq_Capture.h"
#include "exti.h"
#include "lib/sys_tick.h"
uint8_t  captureDoneFlag = 0;
uint32_t  captureValue = 0,captureValueOld = 0;
double speedFreq = 0;
double speedFreqOld = 0;
uint32_t capturePulseCnt = 0;
uint32_t pluseStatusClearCnt = 0;

void InitCapture(uint32_t pclk)
{
//     TIM3->PSC = (uint16_t)(pclk/1000000) - 1;            // set prescaler
//     TIM3->ARR = 0xFFFF;  //1ms          // set auto-reload
//     TIM3->CR1 = 0;                                         // reset command register 1
//     TIM3->CR2 = 0;                                          // reset command register 2
// // 		TIM3->DIER = 1;                             
// // 		NVIC_SetPriority (TIM3_IRQn,((0x01<<3) | TICK_PRIORITY));
// // 		NVIC_EnableIRQ(TIM3_IRQn);// enable interrupt
//     TIM3->CR1 |= 1;                              // enable timer
}

void EXTI4_IRQHandler(void)
{
	uint32_t u32Temp;
	EXTI->PR = EXTI_Line4;
	if(!captureDoneFlag)
	{
		capturePulseCnt++;
		pluseStatusClearCnt = 0;
		u32Temp = SysTick_Get()*1000 + SysTick->VAL/47;
		captureValue = u32Temp - captureValueOld;
		captureValueOld = u32Temp;
		captureDoneFlag = 1;
	}
}
