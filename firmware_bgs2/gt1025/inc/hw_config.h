/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H__
#define __HW_CONFIG_H__

#include "stm32f10x.h"
#include "system_config.h"
#include <stdint.h>

#define PAGE_SIZE                         (0x400)    /* 2 Kbyte */
#define FLASH_SIZE                        (0x20000)  /* 256 KBytes */

#define ApplicationAddress    0x8003000
#define EEPROM_START_ADDRESS ((uint32_t)(0x8060000 - PAGE_SIZE*2)) /* EEPROM emulation start address: after 64KByte of used Flash memory */

#define CONFIG_AREA_SIZE		2048
#define CONFIG_AREA_START ((uint32_t)(0x08000000 + FLASH_SIZE - PAGE_SIZE*(CONFIG_AREA_SIZE/PAGE_SIZE)))		

#define CAR_ALARM_PRESS_3TIMES_IN_4SEC_FLAG				0x001
#define MICROWAVE_ALARM_FLAG 	0x002
#define POWER_ALARM_FLAG			0x004
#define BATTERY_ALARM_FLAG		0x008
#define SOS_ALARM_FLAG				0x010
#define TIME_REMIND_FLAG			0x020
#define ACC_OFF_3MIN_FLAG			0x040
#define MILEAGE_REMIND_FLAG		0x080
#define DAILY_ALARM_FLAG			0x100
#define SPEED_ALARM_FLAG			0x200
#define ACC_ON_OFF_FLAG				0x400
#define ACCLEROMETER_FLAG			0x800
#define CAR_ALARM_HOLD_IN_4SEC_FLAG				0x1000
#define CAR_ACCIDENT_FLAG		0x2000
#define CAR_BE_TOWED_AWAY_FLAG		0x4000

//ACCELEROMETER
#define ACCEL_PORT			GPIOB
#define ACCEL_PIN			GPIO_IDR_IDR10
#define ACCEL_PIN_SET_INPUT		ACCEL_PORT->CRH	&= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);\
																												ACCEL_PORT->CRH	|=  GPIO_CRH_CNF10_1;\
																												ACCEL_PORT->ODR	|=  ACCEL_PIN

//ACC
#define AVLIO1_PORT			GPIOB
#define AVLIO1_PIN			GPIO_IDR_IDR3
#define AVLIO1_PIN_SET_INPUT		AVLIO1_PORT->CRL	&= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);\
																												AVLIO1_PORT->CRL	|=  GPIO_CRL_CNF3_1;\
																												AVLIO1_PORT->ODR	|=  AVLIO1_PIN
#define GET_AVLIO1_PIN 	(AVLIO1_PORT->IDR & AVLIO1_PIN)
//TRIC
#define AVLIO2_PORT			GPIOB	
#define AVLIO2_PIN			GPIO_IDR_IDR4
#define GET_AVLIO2_PIN 	(AVLIO2_PORT->IDR & AVLIO2_PIN)
#define AVLIO2_PIN_SET_INPUT		AVLIO2_PORT->CRL	&= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);\
																												AVLIO2_PORT->CRL	|=  GPIO_CRL_CNF4_1;\
																												AVLIO2_PORT->ODR	|=  AVLIO2_PIN
//SOS
#define AVLIO3_PORT			GPIOB	
#define AVLIO3_PIN			GPIO_IDR_IDR5
#define GET_AVLIO3_PIN 	(AVLIO3_PORT->IDR & AVLIO3_PIN)
#define AVLIO3_PIN_SET_INPUT		AVLIO3_PORT->CRL	&= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);\
																												AVLIO3_PORT->CRL	|=  GPIO_CRL_CNF5_1;\
																												AVLIO3_PORT->ODR	|=  AVLIO3_PIN
//MICROWAVE SIGNAL
#define AVLIO4_PORT			GPIOB
#define AVLIO4_PIN			GPIO_IDR_IDR5
#define GET_AVLIO4_PIN 	(AVLIO4_PORT->IDR & AVLIO4_PIN)
#define AVLIO4_PIN_SET_INPUT		AVLIO4_PORT->CRL	&= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);\
																												AVLIO4_PORT->CRL	|=  GPIO_CRL_CNF5_1;\
																												AVLIO4_PORT->ODR	|=  AVLIO4_PIN

#define AVLIO5_PORT			GPIOC
#define AVLIO5_PIN			GPIO_IDR_IDR0
#define GET_AVLIO5_PIN 	(AVLIO5_PORT->IDR & AVLIO5_PIN)
#define AVLIO5_SET_INPUT		AVLIO5_PORT->CRL	&= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);AVLIO5_PORT->CRL	|=  GPIO_CRL_CNF0_0

//PWR_EN
#define PWR_EN_PORT	GPIOA
#define PWR_EN_PIN							GPIO_BSRR_BS8
#define PWR_EN_PIN_SET_OUTPUT		PWR_EN_PORT->CRH	&= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8); PWR_EN_PORT->CRH	|= GPIO_CRH_MODE8_0
#define PWR_EN_PIN_SET_INPUT		PWR_EN_PORT->CRH	&= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8);
#define PWR_EN_PIN_SET					PWR_EN_PORT->BSRR = PWR_EN_PIN
#define PWR_EN_PIN_CLR					PWR_EN_PORT->BRR = PWR_EN_PIN

//GPS_EN 
#define GPS_EN_PORT	GPIOB
#define GPS_EN_PIN							GPIO_BSRR_BS11
#define GET_GPS_EN_PIN 					(GPS_EN_PORT->IDR & GPS_EN_PIN)
#define GPS_EN_PIN_SET_OUTPUT		GPS_EN_PORT->CRH	&= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11); GPS_EN_PORT->CRH	|= GPIO_CRH_MODE11_0
#define GPS_EN_PIN_SET_INPUT		GPS_EN_PORT->CRH	&= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
#define GPS_EN_PIN_SET					GPS_EN_PORT->BSRR = GPS_EN_PIN
#define GPS_EN_PIN_CLR					GPS_EN_PORT->BRR = GPS_EN_PIN
#define GPS_EN_GET()						(GPIOB->IDR & GPIO_BSRR_BS11)


/*
	PRIORITY CONFIG
*/
#define UART5_PRIORITY				2
#define CAMERA_PRIORITY				1
#define GPS_PRIORITY					2
#define GSM_PRIORITY  				1
#define RTC_ALARM_PRIORITY		1
#define TICK_PRIORITY 				1
#define RTCALARM_PRIORITY 		4
#define MODEMRI_PRIORITY 			1
#define TMR2_PRIORITY					4
#define USB_WAKEUP_PRIORITY		4
#define ACCL_PRIORITY					4
#define AVLIO1_PRIORITY				1
#define AVLIO2_PRIORITY				1
#define AVLIO3_PRIORITY				1
#define AVLIO4_PRIORITY				1
#define ADC_PRIORITY					4
#define RF_PRIORITY						4
#define SPEED_PRIORITY				1
#define GPS_PULSE_PRIORITY		1
#define BUZZER_PROORITY				1
#define USB_IRQ_PRIORITY			4

#define ALARM_TIME		300//5min


#define ACC_IS_ON 	(!GET_AVLIO1_PIN)
#define ACC_GET_STATUS ioStatus.din[0].bitNew

#define MICROWAVE_IS_ON	(!ioStatus.din[3].bitNew)
#define MICROWAVE_GET_STATUS ioStatus.din[3].bitNew

#define TRIC_IS_ON	(!ioStatus.din[1].bitNew)
#define TRIC_GET_STATUS ioStatus.din[1].bitNew

//extern led_type	ledCtr;	

#define TIMER_PERIOD	1	//ms


/*	7	<= VCC_INPUT <= 36
			|
			\
			/		R2 = 100k
			\
			/
			|________ADC10
			|
			\
			/
			\		R1 = 10k
			/
			|
			|
		GND

*/

#define AN10_R1	10.0
#define AN10_R2	39.0

#define ADC_INPUT_CH10_SCALE	(3.3*(AN10_R1 + AN10_R2)/4096.0/AN10_R1)


/*	3.6	<= BATTERY <= 4.4
			|
			\
			/		R2 = 39k
			\
			/
			|________ADC11
			|
			\
			/
			\		R1 = 10k
			/
			|
			|
		GND

*/

#define AN11_R1	10.0
#define AN11_R2	39.0

#define ADC_INPUT_CH11_SCALE	(3.3*(AN11_R1 + AN11_R2)/4096.0/AN11_R1)


#define GSM_POWER_PORT	GPIOB
#define GSM_POWER_PIN		GPIO_BSRR_BS5
#define GSM_POWER_PIN_SET_OUTPUT	GSM_POWER_PORT->CRL	&= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5); GSM_POWER_PORT->CRL	|= (GPIO_CRL_MODE5_0) 
#define GSM_POWER_PIN_SET					GSM_POWER_PORT->BSRR = GSM_POWER_PIN
#define GSM_POWER_PIN_CLR					GSM_POWER_PORT->BRR = GSM_POWER_PIN

#define POWER_PORT	GPIOA
#define POWER_PIN		GPIO_BSRR_BS15
#define POWER_PIN_SET_OUTPUT	POWER_PORT->CRH	&= ~(GPIO_CRH_MODE15 | GPIO_CRH_CNF15); POWER_PORT->CRH	|= (GPIO_CRH_MODE15_0 | GPIO_CRH_CNF15_0) //open drain
#define POWER_PIN_SET					POWER_PORT->BSRR = POWER_PIN
#define POWER_PIN_CLR					POWER_PORT->BRR = POWER_PIN


#define RESET_PORT	GPIOB
#define RESET_PIN		GPIO_BSRR_BS12
#define RESET_PIN_SET_OUTPUT	RESET_PORT->CRH	&= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12); RESET_PORT->CRH	|= (GPIO_CRH_MODE12_0 | GPIO_CRH_CNF12_0) 
#define RESET_PIN_SET					RESET_PORT->BSRR = RESET_PIN
#define RESET_PIN_CLR					RESET_PORT->BRR = RESET_PIN


#define RTS_PORT	GPIOA
#define RTS_PIN		GPIO_BSRR_BS12
#define RTS_PIN_SET_OUTPUT	RTS_PORT->CRH	&= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12); RTS_PORT->CRH	|= (GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1 | GPIO_CRH_CNF12_1) 
#define RTS_PIN_SET				RTS_PORT->BSRR = RTS_PIN
#define RTS_PIN_CLR					RTS_PORT->BRR = RTS_PIN

#define CTS_PORT	GPIOA
#define CTS_PIN		GPIO_BSRR_BS11
#define CTS_PIN_SET_INPUT		CTS_PORT->CRH	&= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11); CTS_PORT->CRH	|=  GPIO_CRH_CNF11_0


#define DTR_PORT									GPIOB
#define DTR_PIN										GPIO_BSRR_BS14
#define DTR_PIN_SET_OUTPUT				DTR_PORT->CRH	&= ~(GPIO_CRH_MODE14 | GPIO_CRH_CNF14); DTR_PORT->CRH	|= (GPIO_CRH_MODE14_0) 
#define DTR_PIN_SET_INPUT					DTR_PORT->CRH	&= ~(GPIO_CRH_MODE14 | GPIO_CRH_CNF14); DTR_PORT->CRH	|=  GPIO_CRH_CNF14_0
#define DTR_PIN_SET								DTR_PORT->BSRR = DTR_PIN
#define DTR_PIN_CLR								DTR_PORT->BRR = DTR_PIN


#define DCD_PORT	GPIOB
#define DCD_PIN		GPIO_BSRR_BS13
#define DCD_PIN_SET_INPUT		DCD_PORT->CRH	&= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); DCD_PORT->CRH	|=  GPIO_CRH_CNF13_0
#define MODEM_DCD()								(DCD_PORT->IDR & DCD_PIN)


#define RI_PORT			GPIOB
#define RI_PIN			GPIO_IDR_IDR15
#define GET_RI_PIN 	(RI_PORT->IDR & RI_PIN)
#define RI_PIN_SET_INPUT		RI_PORT->CRH	&= ~(GPIO_CRH_MODE15 | GPIO_CRH_CNF15);\
														RI_PORT->CRH	|=  GPIO_CRl_CNF15_1;\
														RI_PORT->ODR	|=  RI_PIN

						
typedef struct {
	uint8_t bitOld;
	uint8_t bitNew;
	uint8_t highCnt;
	uint8_t lowCnt;
	uint8_t newUpdate;
}IOFilterType;

typedef struct {
	uint32_t updateCnt;
	IOFilterType din[5]; // {ACC,LOW_ACT,SOS,POWER_IN}
}IOstatusType;


extern IOstatusType  ioStatus;

typedef enum{
	SYS_INIT = 0xA5A5,
	SYS_DEINIT = 0x5A5A,
	SYS_OK = 0
}SYS_STATUS;


void GPS_Enable(void);
void GPS_Disable(void);
void IO_Control(uint32_t time);
void IO_Init(void);

#endif  /*__HW_CONFIG_H*/

