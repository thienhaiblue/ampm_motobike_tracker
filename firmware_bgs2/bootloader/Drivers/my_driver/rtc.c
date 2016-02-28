
/**
* \file
*         rtc driver
* \author
*         Nguyen Van Hai <hainv@ivx.vn>
*/
#include "rtc.h"
#include "tick.h"
#define CRL_CNF_Set      ((uint16_t)0x0010)      /* Configuration Flag Enable Mask */
#define CRL_CNF_Reset    ((uint16_t)0xFFEF)      /* Configuration Flag Disable Mask */
#define RTC_LSB_Mask     ((uint32_t)0x0000FFFF)  /* RTC LSB Mask */
#define PRLH_MSB_Mask    ((uint32_t)0x000F0000)  /* RTC Prescaler MSB Mask */


#define RTC_FLAG_RTOFF       ((uint16_t)0x0020)  /* RTC Operation OFF flag */
#define RTC_FLAG_RSF         ((uint16_t)0x0008)  /* Registers Synchronized flag */
#define RTC_FLAG_OW          ((uint16_t)0x0004)  /* Overflow flag */
#define RTC_FLAG_ALR         ((uint16_t)0x0002)  /* Alarm flag */
#define RTC_FLAG_SEC         ((uint16_t)0x0001)  /* Second flag */

uint32_t sleepTimeOld = 0;
uint32_t timepowerOffDelay = 0;
Timeout_Type 	MCU_WakeupTimeout;
// void RTC_IRQHandler(void)
// {
// 	RTC->CRL &= (uint16_t)RTC_FLAG_SEC;
// 	RTC->CRL &= (uint16_t)RTC_FLAG_ALR;
// 	sec++;
// }

void RTCAlarm_IRQHandler(void)
{	
	uint32_t RTC_counter;
	RTC->CRL &= (uint16_t)RTC_FLAG_ALR;
	EXTI->PR = EXTI_Line17;
	RTC_counter = RTC_GetCounter();
	if(RTC_counter >= (sysCfg.sleepTime + sleepTimeOld))
	{
		sleepTimeOld = RTC_counter;
		sysEventFlag |=  MCU_WAKEUP_FLAG;	
		InitTimeout(&MCU_WakeupTimeout,TIME_SEC(60));
	}
	
	
	if(GET_ACC_PIN == ACC_ON)
	{
			POWER_EN_SET_OUTPUT;
			POWER_EN_PIN_SET;
			timepowerOffDelay = RTC_counter;
	}
	else if(RTC_counter >= (sysCfg.powerOffDelayTime + timepowerOffDelay))
	{
			POWER_EN_SET_INPUT;
			timepowerOffDelay = RTC_counter;
	}
	RTC_SetAlarm((RTC_counter + ALARM_TIME));
	sysEventFlag |=  RTC_ALARM_FLAG;	
}

void RTC_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;                            // enable clock for Power interface
	PWR->CR      |= PWR_CR_DBP;                                   // enable access to RTC, BDC registers

	RCC->CSR |= RCC_CSR_LSION;                                  // enable LSI
	while ((RCC->CSR & RCC_CSR_LSIRDY) == 0);                   // Wait for LSERDY = 1 (LSE is ready)

	RCC->BDCR |= (RCC_BDCR_RTCSEL_1 | RCC_BDCR_RTCEN);             // set RTC clock source, enable RTC clock
	//  RTC->CRL   &= ~(1<<3);                                        // reset Registers Synchronized Flag
	//  while ((RTC->CRL & (1<<3)) == 0);                             // wait until registers are synchronized
	RTC->CRL  |=  RTC_CRL_CNF;                                    // set configuration mode
  RTC->PRLH  = 0;   // set prescaler load register high
  RTC->PRLL  = 32767;//32768-1;   // set prescaler load register low
  RTC->CNTH  = 0;                      // set counter high
  RTC->CNTL  = 0;                      // set counter low
  RTC->ALRH  = 0;                      // set alarm high
  RTC->ALRL  = 0;                      // set alarm low
	
	RTC->CRH = RTC_CRH_ALRIE;                                       // enable Alarm RTC interrupts
  NVIC->ISER[1] |= (1 << (RTCAlarm_IRQn & 0x1F));            		// enable interrupt
	//NVIC->ISER[0] |= (1 << (RTC_IRQn & 0x1F));            		// enable interrupt
	
	RTC->CRL  &= ~RTC_CRL_CNF;                                    // reset configuration mode
  while ((RTC->CRL & RTC_CRL_RTOFF) == 0);                      // wait until write is finished
	
  //PWR->CR   &= ~PWR_CR_DBP;                                     // disable access to RTC registers
	InitTimeout(&MCU_WakeupTimeout,TIME_SEC(60));
}	

uint32_t RTC_GetCounter(void)
{
	uint16_t tmp = 0;
  tmp = RTC->CNTL;
  return (((uint32_t)RTC->CNTH << 16 ) | tmp) ;
}

/**
  * @brief  Sets the RTC alarm value.
  * @param AlarmValue: RTC alarm new value.
  * @retval : None
  */
void RTC_SetAlarm(uint32_t AlarmValue)
{  
 /* Set the CNF flag to enter in the Configuration Mode */
  RTC->CRL |= CRL_CNF_Set;
  /* Set the ALARM MSB word */
  RTC->ALRH = AlarmValue >> 16;
  /* Set the ALARM LSB word */
  RTC->ALRL = (AlarmValue & 0x0000FFFF);
  /* Reset the CNF flag to exit from the Configuration Mode */
  RTC->CRL &= CRL_CNF_Reset;
}



