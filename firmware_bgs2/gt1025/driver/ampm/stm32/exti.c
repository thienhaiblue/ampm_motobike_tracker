/**
* \file
*         exti driver
* \author
*         Nguyen Van Hai <blue@ambo.com.vn>
*/

#include "exti.h"
#include "low_power_mode.h"
#include "system_config.h"
#include "rtc.h"
#include "gps/gps.h"
#include "accelerometer_task.h"

extern uint32_t alarmFlag;
extern uint8_t sendStatus;
extern uint8_t statusSentFlag;
extern uint32_t RingCallProcess(uint16_t cnt,uint8_t c); 

void EXTI_Init(void)
{
//		//RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
		/*Congig use for EXTI0*/
		AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;	//Disable JTAG funtion, keep SWJ funtion for debug(only user when PB3,PB4,PB5,PA15 user for another funtion)
		AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0;   // clear used pin
		AFIO->EXTICR[1] &= ~AFIO_EXTICR1_EXTI1;   // clear used pin
		AFIO->EXTICR[2] &= ~AFIO_EXTICR1_EXTI2;   // clear used pin
		AFIO->EXTICR[3] &= ~AFIO_EXTICR1_EXTI3;    // clear used pin
	
	
		/*Use PB3 is AVLIO1_PRIORITY*/
		EXTI->PR = EXTI_Line3;
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI3_PB;           // set pin to use
		EXTI->IMR       &= ~EXTI_Line3;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line3;             // mask event
    EXTI->RTSR      &= ~EXTI_Line3;            // clear rising edge
		EXTI->IMR       |= EXTI_Line3;             // mask interrupt
		EXTI->EMR       |= EXTI_Line3;             // mask event
		EXTI->RTSR      &= ~EXTI_Line3;            // clear rising edge
    EXTI->FTSR      |= EXTI_Line3;            // set falling edge
		/* preemption = MODEMRI_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(EXTI3_IRQn, ((0x01<<3)| MODEMRI_PRIORITY));
		NVIC_EnableIRQ(EXTI3_IRQn);
		
		
		/*Use PB4 is AVLIO2_PRIORITY*/
		EXTI->PR = EXTI_Line4;
    AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI4_PB;           // set pin to use
		EXTI->IMR       &= ~EXTI_Line4;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line4;             // mask event
    EXTI->RTSR      &= ~EXTI_Line4;            // clear rising edge
		EXTI->IMR       |= EXTI_Line4;             // mask interrupt
		EXTI->EMR       |= EXTI_Line4;             // mask event

    EXTI->FTSR      |= EXTI_Line4;            // set falling edge
		/* preemption = ACCL_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(EXTI4_IRQn, ((0x01<<3)| AVLIO2_PRIORITY));
		NVIC_EnableIRQ(EXTI4_IRQn);		
		
				/*Use PB0 is ACCEL*/ 
		ACCEL_PIN_SET_INPUT; //set ACCELEROMETER is input with internal resistor pull-up
		EXTI->PR = EXTI_Line10;
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI10_PB;           // set pin to use
		EXTI->IMR       &= ~EXTI_Line10;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line10;             // mask event
    EXTI->RTSR      &= ~EXTI_Line10;            // clear rising edge
		EXTI->IMR       |= EXTI_Line10;             // mask interrupt
		EXTI->EMR       |= EXTI_Line10;             // mask event
    EXTI->FTSR      |= EXTI_Line10;            // set falling edge
		/* preemption = ACCL_PRIORITY, sub-priority = 1 */
		
		/*Use PB15 is MODEMRI_PRIORITY*/
		EXTI->PR = EXTI_Line15;
    AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI15_PB;           // set pin to use
		EXTI->IMR       &= ~EXTI_Line15;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line15;             // mask event
    EXTI->RTSR      &= ~EXTI_Line15;            // clear rising edge
		EXTI->IMR       |= EXTI_Line15;             // mask interrupt
		EXTI->EMR       |= EXTI_Line15;             // mask event
    EXTI->FTSR      |= EXTI_Line15;            // set falling edge

		/* preemption = MODEMRI_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(EXTI15_10_IRQn, ((0x01<<3)| MODEMRI_PRIORITY));
		NVIC_EnableIRQ(EXTI15_10_IRQn);
		// 		/*Congig use for EXTI4*/
// 		/*Use RTC Wakeup*/
		EXTI->PR = EXTI_Line17;
		EXTI->IMR       &= ~EXTI_Line17;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line17;             // mask event
		EXTI->IMR       |= EXTI_Line17;             // mask interrupt
		EXTI->EMR       |= EXTI_Line17;             // mask event
    EXTI->RTSR      |= EXTI_Line17;            // clear rising edge

}


void EXTI3_IRQHandler(void)
{
		EXTI->PR = EXTI_Line3;
		
}

void EXTI4_IRQHandler(void)
{
		EXTI->PR = EXTI_Line4;
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & EXTI_Line10)
	{
		EXTI->PR = EXTI_Line10;
		alarmFlag |= ACCLEROMETER_FLAG;
		if(accelerometerAlarmCnt == 0)
			accelerometerAlarmCnt = 10;
	}
	if(EXTI->PR & EXTI_Line15)
	{
		EXTI->PR = EXTI_Line15;
		//RingCallProcess(NULL,NULL);
	}
}




void USBWakeUp_IRQHandler(void)
{
	EXTI->PR = EXTI_Line18;
}



