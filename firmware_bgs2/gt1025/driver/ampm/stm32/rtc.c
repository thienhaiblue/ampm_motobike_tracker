
/**
* \file
*         rtc driver
* \author
*         Nguyen Van Hai <blue@ambo.com.vn>
*/
#include "rtc.h"
#include "lib/sys_tick.h"
#include "stm32f10x_iwdg.h"
#include "lib/sys_time.h"
#include "database_app.h"
#include "uart3.h"


#define CRL_CNF_Set      ((uint16_t)0x0010)      /* Configuration Flag Enable Mask */
#define CRL_CNF_Reset    ((uint16_t)0xFFEF)      /* Configuration Flag Disable Mask */
#define RTC_LSB_Mask     ((uint32_t)0x0000FFFF)  /* RTC LSB Mask */
#define PRLH_MSB_Mask    ((uint32_t)0x000F0000)  /* RTC Prescaler MSB Mask */


#define RTC_FLAG_RTOFF       ((uint16_t)0x0020)  /* RTC Operation OFF flag */
#define RTC_FLAG_RSF         ((uint16_t)0x0008)  /* Registers Synchronized flag */
#define RTC_FLAG_OW          ((uint16_t)0x0004)  /* Overflow flag */
#define RTC_FLAG_ALR         ((uint16_t)0x0002)  /* Alarm flag */
#define RTC_FLAG_SEC         ((uint16_t)0x0001)  /* Second flag */

#define RTC_LSB_MASK     ((uint32_t)0x0000FFFF)  /*!< RTC LSB Mask */
#define PRLH_MSB_MASK    ((uint32_t)0x000F0000)  /*!< RTC Prescaler MSB Mask */

#define MAIN_LOOP_TIME_MAX 30 //120sec
#define TIMER0_LOOP_TIME_MAX 60 //60sec
#define TIMER1_LOOP_TIME_MAX 60 //60sec

#define RTC_USE_LSI


uint32_t rtcCnt = 0;
uint32_t sec;


uint8_t resetCnt = 0;

uint32_t RTC_SanityCheck(void);
extern void TrackerGetNewData(void);

extern uint32_t deviceTurnOffTime;
extern uint32_t alarmFlag;
extern uint8_t accelerometerAlarmCnt;

uint32_t watchdogEnable[WATCHDOG_NUM] = {0};
volatile uint32_t watchdogCnt[WATCHDOG_NUM] = {0};
volatile uint32_t watchdogFeed[WATCHDOG_NUM] = {0};
const uint32_t watchdogCntValueMax[WATCHDOG_NUM] = {MAIN_LOOP_TIME_MAX};




void RTC_IRQHandler(void)
{
	RTC->CRL &= (uint16_t)RTC_FLAG_SEC;
	RTC->CRL &= (uint16_t)RTC_FLAG_ALR;
	sec++;
}



void RTCAlarm_IRQHandler(void)
{
	uint32_t RTC_counter,temp;
	RTC->CRL &= (uint16_t)RTC_FLAG_ALR;
	EXTI->PR = EXTI_Line17;
	RTC_counter = RTC_GetCounter();
	if(RTC_counter < 1400401742) //utc time : 18/5/2014->8:29:2
	{
		if(rtcTimeSec < 1400401742)
			rtcTimeSec = 1400401742;
		RTC_SetCounter(rtcTimeSec);
	}
	else
	{
		rtcTimeSec = RTC_GetCounter();
		if(rtcUpdateFlag)
		{
			rtcUpdateFlag = 0;
			rtcTimeSec = rtcUpdateCnt;
			RTC_SetCounter(rtcTimeSec);
		}
		else
		{
			rtcTimeSec = RTC_GetCounter();
			RTC_SetAlarm((rtcTimeSec + RTC_FREQUENCY));
		}
	}
	TIME_FromSec(&sysTime,rtcTimeSec);	
	
	
	temp = rtcTimeSec % WATCHDOG_NUM;
	if(watchdogEnable[temp])
	{
		//check watchdog value for funtion
		watchdogCnt[temp] += WATCHDOG_NUM;
		if(watchdogCnt[temp] >= watchdogCntValueMax[temp]) 
		{
			NVIC_SystemReset();
		}
		if(watchdogFeed[temp] == 0)
		{
			watchdogCnt[temp] = 0;
			watchdogFeed[temp] = 1;
		}
	}
	else	
	{
		watchdogCnt[temp] = 0;
	}
	
	IWDG_ReloadCounter();
	rtcCnt++;
	//1 Sec interval
	if(accelerometerAlarmCnt)
	{
			accelerometerAlarmCnt--;
	}
	
	if(sysResetMcuFlag == 1)
	{
		if(resetCnt++ >= 30)
			NVIC_SystemReset();
	}
	else if(sysResetMcuFlag == 2)
	{
		if(resetCnt++ >= 10)
			NVIC_SystemReset();
	}
	else
		resetCnt  = 0;
	if(rtcCnt % 600) //sync config each 10 min
		CFG_ReLoad();

	//PWR CONTROL
	if(ACC_IS_ON) 
	{	
		powerOfTimeDelay.value = rtcTimeSec;//172800;//48 hour
		PWR_EN_PIN_SET;
	}
	else if(rtcTimeSec >= powerOfTimeDelay.value + 172800/*= 48 hour*/)
	{
		PWR_EN_PIN_CLR;
	}
	else
		PWR_EN_PIN_SET;
}


void RTC_Init(void)
{
	uint32_t tout = 10000000;
	uint32_t u32T;
	
//		GPIOC->CRH 	 &= ~(GPIO_CRH_MODE14 | GPIO_CRH_CNF14 | GPIO_CRH_MODE15 | GPIO_CRH_CNF15);  					// Clear PC12        
//		GPIOC->CRH   |=   GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1 | GPIO_CRH_MODE14_0 | GPIO_CRH_MODE15_0;
	
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;                            // enable clock for Power interface
	PWR->CR      |= PWR_CR_DBP;                                   // enable access to RTC, BDC registers

	if(RTC_SanityCheck() != 0)
	{
			rtcTimeSec = RTC_GetCounter();
	}
	
#ifdef RTC_USE_LSI
	RCC->CSR |= RCC_CSR_LSION;                                  // enable LSI
	while ((RCC->CSR & RCC_CSR_LSIRDY) == 0);                   // Wait for LSERDY = 1 (LSI is ready)
	RCC->BDCR |= (RCC_BDCR_RTCSEL_1 | RCC_BDCR_RTCEN);             // set RTC clock source, enable RTC clock
#else
 	RCC->BDCR |= RCC_BDCR_LSEON;
 	while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0); 
	RCC->BDCR |= (RCC_BDCR_RTCSEL_0 | RCC_BDCR_RTCEN);             // set RTC clock source, enable RTC clock
#endif
	RTC->CRL  |=  RTC_CRL_CNF;                                    // set configuration mode
  RTC->PRLH  = 0;   // set prescaler load register high
#ifdef RTC_USE_LSI
  RTC->PRLL  = 32768/RTC_FREQUENCY - 1;// set prescaler load register low
#else
	RTC->PRLL  = 32768/RTC_FREQUENCY - 1;// set prescaler load register low
#endif
  RTC->CNTH  = 0;                      // set counter high
  RTC->CNTL  = 0;                      // set counter low
  RTC->ALRH  = 0;                      // set alarm high
  RTC->ALRL  = 0;                      // set alarm low

	
	//interrupt enable
  NVIC->ISER[1] |= (1 << (RTCAlarm_IRQn & 0x1F));            		// enable interrupt
	
	RTC->CRH =  RTC_CRH_ALRIE;
	NVIC->ISER[0] |= (1 << (RTC_IRQn & 0x1F));            		// enable interrupt
	
	RTC->CRL  &= ~RTC_CRL_CNF;                                    // reset configuration mode
  while ((RTC->CRL & RTC_CRL_RTOFF) == 0)                      // wait until write is finished
	{
		if(tout-- == 0)
		{
//				USART3_Init(SystemCoreClock/2,__USART3_BAUDRATE);
//				USART3_PutString("\r\nRTC Crystal 32k problem!\r\n");
//				USART3_PutString("\r\nRTC Crystal 32k problem!\r\n");
//				USART3_PutString("\r\nRTC Crystal 32k problem!\r\n");
//				USART3_PutString("\r\nRTC Crystal 32k problem!\r\n");
//				USART3_PutString("\r\nRTC Crystal 32k problem!\r\n");
//				break;
		}
	}
	//RTC_SetAlarm((60));
  //PWR->CR   &= ~PWR_CR_DBP;                                     // disable access to RTC registers
}	


uint32_t RTC_SanityCheck(void)
{
        uint32_t bdcr;

        bdcr = RCC->BDCR;

        if (!(bdcr & RCC_BDCR_LSERDY)) {
                /*LSE not ready*/
                return 0;
        }

        if (!(bdcr & RCC_BDCR_LSEON)) {
                /*LSE disabled*/
                return 0;
        }

        if (!(bdcr & RCC_BDCR_RTCEN)) {
                /*RTC disabled*/
                return 0;
        }

        if ((bdcr & RCC_BDCR_RTCSEL) != RCC_BDCR_RTCSEL_0) {
                /*RTC oscillator != LSE*/
                return 0;
        }

        return 1;
}


/**
  * @brief  Enters the RTC configuration mode.
  * @param  None
  * @retval None
  */
void RTC_EnterConfigMode(void)
{
  /* Set the CNF flag to enter in the Configuration Mode */
  RTC->CRL |= RTC_CRL_CNF;
}

/**
  * @brief  Exits from the RTC configuration mode.
  * @param  None
  * @retval None
  */
void RTC_ExitConfigMode(void)
{
  /* Reset the CNF flag to exit from the Configuration Mode */
  RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF); 
}

/**
  * @brief  Gets the RTC counter value.
  * @param  None
  * @retval RTC counter value.
  */
uint32_t RTC_GetCounter(void)
{
  uint16_t tmp = 0;
  tmp = RTC->CNTL;
  return (((uint32_t)RTC->CNTH << 16 ) | tmp) ;
}

/**
  * @brief  Sets the RTC counter value.
  * @param  CounterValue: RTC counter new value.
  * @retval None
  */
void RTC_SetCounterX(uint32_t CounterValue)
{ 
  RTC_EnterConfigMode();
  /* Set RTC COUNTER MSB word */
  RTC->CNTH = CounterValue >> 16;
  /* Set RTC COUNTER LSB word */
  RTC->CNTL = (CounterValue & RTC_LSB_MASK);
  RTC_ExitConfigMode();
}

uint8_t RTC_SetCounter(uint32_t CounterValue)
{ 
	uint32_t u32T,AlarmValue = CounterValue + RTC_FREQUENCY;
	RTC_EnterConfigMode();
	/* Set RTC COUNTER MSB word */
	RTC->CNTH = CounterValue >> 16;
	/* Set RTC COUNTER LSB word */
	RTC->CNTL = (CounterValue & RTC_LSB_MASK);
	/* Set the ALARM MSB word */
	RTC->ALRH = AlarmValue >> 16;
	/* Set the ALARM LSB word */
	RTC->ALRL = (AlarmValue & RTC_LSB_MASK);
	RTC_ExitConfigMode();
		return 0;
}


/**
  * @brief  Sets the RTC alarm value.
  * @param AlarmValue: RTC alarm new value.
  * @retval : None
  */
void RTC_SetAlarm(uint32_t AlarmValue)
{  
 /* Set the CNF flag to enter in the Configuration Mode */
  RTC_EnterConfigMode();
  /* Set the ALARM MSB word */
  RTC->ALRH = AlarmValue >> 16;
  /* Set the ALARM LSB word */
  RTC->ALRL = (AlarmValue & RTC_LSB_MASK);
  /* Reset the CNF flag to exit from the Configuration Mode */
  RTC_ExitConfigMode();
}



