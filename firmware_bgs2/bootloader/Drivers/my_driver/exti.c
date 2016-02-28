/**
* \file
*         exti driver
* \author
*         Nguyen Van Hai <hainv@ivx.vn>
*/

#include "exti.h"
#include "low_power_mode.h"
#include "accelerometer.h"
#include "system_config.h"
#include "rtc.h"


uint32_t sysEventFlag __attribute__((zero_init));
uint32_t accelerometerFlagCnt = 0;

extern void SysInit(void);
extern uint8_t EventCheckSum(void);

void EXTI0_IRQHandler(void)
{
	EXTI->PR = EXTI_Line0;
	sysEventFlag |=  ACC_FLAG;	
}

void EXTI1_IRQHandler(void)
{
	EXTI->PR = EXTI_Line1;
	sysEventFlag |=  POWER_FAILS_FLAG;	
}

void EXTI2_IRQHandler(void)
{
	EXTI->PR = EXTI_Line2;
	sysEventFlag |=  ACCELEROMETER_FLAG;
}

void EXTI4_IRQHandler(void)
{
	EXTI->PR = EXTI_Line4;
	sysEventFlag |= SOS_FLAG;
}

void EXTI9_5_IRQHandler(void)
{
	EXTI->PR = EXTI_Line8;
	sysEventFlag |=  MODEM_RI_FLAG;	
}


void USBWakeUp_IRQHandler(void)
{
	EXTI->PR = EXTI_Line18;
	sysEventFlag |=  USB_WAKEUP_FLAG;
}

void EXTI_Init(void)
{
		//RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
		/*Congig use for EXTI0*/
		AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;	//Disable JTAG funtion, keep SWJ funtion for debug(only user when PB3,PB4,PB5,PA15 user for another funtion)
		AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0;   // clear used pin
		AFIO->EXTICR[1] &= ~AFIO_EXTICR1_EXTI1;   // clear used pin
		AFIO->EXTICR[2] &= ~AFIO_EXTICR1_EXTI2;   // clear used pin
		AFIO->EXTICR[3] &= ~AFIO_EXTICR1_EXTI3;    // clear used pin
		/*Use PA0 is ACC*/ 
		EXTI->PR = EXTI_Line0;
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;           // set pin to use
		EXTI->IMR       &= ~EXTI_Line0;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line0;             // mask event
    EXTI->RTSR      &= ~EXTI_Line0;            // clear rising edge
		EXTI->IMR       |= EXTI_Line0;             // mask interrupt
		EXTI->EMR       |= EXTI_Line0;             // mask event
    EXTI->FTSR      |= EXTI_Line0;            // set falling edge
		/* preemption = ACCL_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(EXTI0_IRQn, ((0x01<<3)| ACC_PRIORITY));
		NVIC_EnableIRQ(EXTI0_IRQn);
	
	/*Use PA2 is ACCLEROMETER*/
		EXTI->PR = EXTI_Line2;
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;           // set pin to use
		EXTI->IMR       &= ~EXTI_Line2;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line2;             // mask event
    EXTI->RTSR      &= ~EXTI_Line2;            // clear rising edge
		EXTI->IMR       |= EXTI_Line2;             // mask interrupt
		EXTI->EMR       |= EXTI_Line2;             // mask event
    EXTI->FTSR      |= EXTI_Line2;            // set falling edge
		/* preemption = ACCL_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(EXTI2_IRQn, ((0x01<<3)| ACCL_PRIORITY));
		NVIC_EnableIRQ(EXTI2_IRQn);
	
	/*Congig use for EXTI5*/
		/*Use PA8 is Modem RI*/
		EXTI->PR = EXTI_Line8;
    AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI5_PA;           // set pin to use
		EXTI->IMR       &= ~EXTI_Line8;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line8;             // mask event
		EXTI->IMR       |= 	EXTI_Line8;             // mask interrupt
		EXTI->EMR       |= EXTI_Line8;             // mask event
    EXTI->RTSR      &= ~EXTI_Line8;            // clear rising edge
    EXTI->FTSR      |= EXTI_Line8;            // set falling edge
		/* preemption = MODEMRI_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(EXTI9_5_IRQn, ((0x01<<3)| MODEMRI_PRIORITY));
		NVIC_EnableIRQ(EXTI9_5_IRQn);
		
		
		
		/*Congig use for EXTI1*/
		/*Use PB1*/
		EXTI->PR = EXTI_Line1;
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PB;           // set pin to use
		EXTI->IMR       &= ~EXTI_Line1;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line1;             // mask event
		EXTI->IMR       |= EXTI_Line1;             // mask interrupt
		EXTI->EMR       |= EXTI_Line1;             // mask event
    EXTI->RTSR      &= ~EXTI_Line1;            // clear rising edge
    EXTI->FTSR      |= EXTI_Line1;            // set falling edge
		/* preemption = SOS_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(EXTI1_IRQn, ((0x01<<3)| SOS_PRIORITY));
		NVIC_EnableIRQ(EXTI1_IRQn);
		
		/*Congig use for EXTI4*/
		/*Use PB4*/
		EXTI->PR = EXTI_Line4;
    AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI4_PB;           // set pin to use
		EXTI->IMR       &= ~EXTI_Line4;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line4;             // mask event
		EXTI->IMR       |= EXTI_Line4;             // mask interrupt
		EXTI->EMR       |= EXTI_Line4;             // mask event
    EXTI->RTSR      &= ~EXTI_Line4;            // clear rising edge
    EXTI->FTSR      |= EXTI_Line4;            // set falling edge
		/* preemption = SOS_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(EXTI4_IRQn, ((0x01<<3)| SOS_PRIORITY));
		NVIC_EnableIRQ(EXTI4_IRQn);
		
		
		// 		/*Congig use for EXTI4*/
// 		/*Use RTC Wakeup*/
		EXTI->PR = EXTI_Line17;
		EXTI->IMR       &= ~EXTI_Line17;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line17;             // mask event
		EXTI->IMR       |= EXTI_Line17;             // mask interrupt
		EXTI->EMR       |= EXTI_Line17;             // mask event
    EXTI->RTSR      |= EXTI_Line17;            // clear rising edge
    //EXTI->FTSR      |= EXTI_Line17;            // set falling edge
		//NVIC_EnableIRQ(RTCAlarm_IRQn);
// 		/*Congig use for EXTI4*/
// 		/*Use USB Wakeup*/
		EXTI->PR = EXTI_Line18;
		EXTI->IMR       &= ~EXTI_Line18;             // mask interrupt
    EXTI->EMR       &= ~EXTI_Line18;             // mask event
		EXTI->IMR       |= EXTI_Line18;             // mask interrupt
		EXTI->EMR       |= EXTI_Line18;             // mask event
    EXTI->RTSR      |= EXTI_Line18;            // clear rising edge
    EXTI->FTSR      |= EXTI_Line18;            // set falling edge
		/* preemption = USB_WAKEUP_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(EXTI4_IRQn, ((0x01<<3)| USB_WAKEUP_PRIORITY));
		NVIC_EnableIRQ(USBWakeUp_IRQn);
}



