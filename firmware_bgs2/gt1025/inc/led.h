#ifndef __LED__H__
#define __LED__H__


#include <stdint.h>
#include "hw_config.h"





typedef struct _ledDEF
{
    uint32_t ledOnTime;
		uint32_t ledOffTime;
		uint32_t ledCounter;
		uint8_t ledStatus;
		uint8_t ledEnable;
		uint32_t ledTimes;
} LED_TYPE;


extern LED_TYPE	led1Ctr,led2Ctr;	

#define TIMER_PERIOD	1	//ms


#define LED_ON_TIME_DFG	(500 / TIMER_PERIOD) /*1s */
#define LED_OFF_TIME_DFG	(500 / TIMER_PERIOD) /*1s */
#define LED_TURN_ON	1
#define LED_TURN_OFF 0


#define LED_PORT	GPIOC
#define LED_PIN	GPIO_BSRR_BS13
#define LED_SET_OUTPUT		LED_PORT->CRH	&= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); LED_PORT->CRH	|= (GPIO_CRH_MODE13_0)
#define LED_SET_INPUT		LED_PORT->CRH	&= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); LED_PORT->CRH	|= (GPIO_CRH_CNF13_0)
#define LED_PIN_SET					LED_PORT->BSRR = LED_PIN
#define LED_PIN_CLR					LED_PORT->BRR = LED_PIN



#define LED_ON	1
#define LED_OFF 0

void LedCtr(LED_TYPE *ledCtr,uint32_t times);
void LedSetStatus(LED_TYPE *ledCtr,uint32_t onTime,uint32_t offTime,uint32_t ledEnable,uint32_t times);
void LedInit(void);
void CtrLed(uint32_t time);

#endif

