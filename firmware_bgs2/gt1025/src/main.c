/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "stm32f10x_iwdg.h"
#include "i2c.h"
#include "lib/ringbuf.h"
#include "lib/tick.h"
#include "lib/sys_tick.h"
#include "at_send_cmd_task.h"
#include "at_gsm_task.h"
#include "at_cmd_call_parser.h"
#include "at_cmd_sms_parser.h"
#include "system_config.h"
#include "gps/gps.h"
#include "rtc.h"
#include "low_power_mode.h"
#include "sst25.h"
#include "tracker.h"
#include "lib/data_cmp.h"
#include "adc_task.h"
#include "sms_task.h"
#include "call_task.h"
#include "alarm_task.h"
#include "uart_config_task.h"
#include "accelerometer_task.h"
#include "mma7660.h"
#include "database_app.h"
#include "uip.h"
#include "resolv.h"
#include "led.h"
#include "resolver.h"
#include "reporter.h"
#include "firmware_task.h"
#include "tcp_ip_task.h"
#include "firmware_task.h"

#define MAIN_Info(...)  //DbgCfgPrintf(__VA_ARGS__)


#define SMS_TIME_CHECK 600



void mCentrelTest(void);
void AppRun(void);
uint8_t GSM_GPRS_Task(uint8_t *arg);
void TIMER2_Init(uint32_t pclk);
void SysSleep(void);
void GPS_TaskControl(void);
void SysInit(void);

uint8_t mainBuf[256] = "Công ty TNHH thương mại thiết bị điện tử AMPM";

uint32_t timer2Cnt;
uint16_t sleepCnt = 0;
uint32_t reportInterVal = 0;
uint32_t smsTimeCheck = 0;
uint8_t sendStatus = 0;
uint8_t statusSentFlag = 0;

Timeout_Type tFwDownload;
Timeout_Type tResetApp;
Timeout_Type tAppTimeout;

uint8_t intervalReportWakeFlag = 0;

enum{
	SYS_SLEEP_MODE,
	SYS_RUN_MODE
}sysStatus = SYS_RUN_MODE;


void HardFault_Handler(void)
{
	NVIC_SystemReset();
	while(1);
}


int main(void)
{
	#ifndef DEBUG_MODE
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	IWDG_SetReload(3000); // 1/(40khz/prescaler/reload) =  3000*1.6 ms
	IWDG_ReloadCounter();
	IWDG_Enable();
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	#endif
	SysInit();
	CFG_Load();
	TrackerInit();
	InitTimeout(&tFwDownload,SYSTICK_TIME_SEC(1));
	while(1)
	{
		AppRun();
	}
	return 0;
}


void SysInit(void)
{
	__enable_irq();
	SystemInit();
	TICK_Init(1);
	RCC->APB2ENR = (RCC_APB2ENR_AFIOEN |  /*enable clock for Alternate Function*/
								 RCC_APB2ENR_IOPAEN |   /* enable clock for GPIOA*/
								 RCC_APB2ENR_IOPBEN |	/*enable clock for GPIOB*/
								 RCC_APB2ENR_IOPCEN |/*enable clock for GPIOc*/ 									 
								 RCC_APB2ENR_IOPDEN	
								);   
	AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE;  

	RTC_Init();
	EXTI_Init();
	PWR_EN_PIN_SET_OUTPUT;
	PWR_EN_PIN_CLR;
	GPS_EN_PIN_SET_OUTPUT;
	GPS_Enable(); //enable gps

	IO_Init();
	AccelerometerTaskInit(); 
	ADC_TaskInit();
	SST25_Init();
	DB_FlashMemInit();
/*GSM Init*/
 	USART1_Init(SystemCoreClock,__USART1_BAUDRATE);
/*GPS Init*/
 	GPSInit();
 	USART2_Init(SystemCoreClock/2,__USART2_BAUDRATE);
//	USART3_Init(SystemCoreClock/2,__USART3_BAUDRATE);
	
//	USART3_PutString("thienhaiblue@ampm.com.vn\r\n");
//	USART3_PutString("130 Nguyen Minh Hoang\r\n");
	UartConfigTaskInit();
	LedInit();
	TIMER2_Init(SystemCoreClock);
}

void SysWakeUp(void)
{
	SystemInit();
	TICK_Init(1);
	ADC_TaskInit();

	GPS_EN_PIN_SET_OUTPUT;
	GPS_Enable(); //enable gps

	SST25_Init();
/*GSM Init*/
 	USART1_Init(SystemCoreClock,__USART1_BAUDRATE);
/*GPS Init*/
 	GPSInit();
 	USART2_Init(SystemCoreClock/2,__USART2_BAUDRATE);
//	USART3_Init(SystemCoreClock/2,__USART3_BAUDRATE);
	UartConfigTaskInit();
	TIMER2_Init(SystemCoreClock);
	RTC_SetAlarm(RTC_GetCounter() + 1);
	MODEM_Wakeup();

}

void SysDeInit(void)
{
	TICK_DeInit();
	
	RCC->APB1ENR &= ~(//RCC_APB1ENR_I2C1EN |
								RCC_APB1ENR_USART2EN | 
							 //RCC_APB1ENR_USART3EN |
								 RCC_APB1ENR_TIM2EN	|
								 RCC_APB1ENR_TIM3EN );
	RCC->APB2ENR &= ~(  /*disable clock for Alternate Function*/					 
								 RCC_APB2ENR_USART1EN | /* disable clock for USART1*/
								 RCC_APB2ENR_ADC1EN |	/* disable clock for ADC1*/
								 RCC_APB2ENR_SPI1EN
								);     
	//disable gps
  GPS_Disable(); 
	//GSM disable
	MODEM_Sleep();
}


void SysSleep(void)
{
	uint32_t sysSleepCnt = 0;
	uint8_t accOnCnt = 0;
	uint32_t time;
	sysStatus = SYS_SLEEP_MODE;
	MODEM_SendCommand(modemOk,modemError,2000,1,"ATH\r");
	sleepCnt = 0;
	while(1)
	{
		sysSleepCnt++;
		watchdogFeed[WTD_MAIN_LOOP] = 0; 
		SysDeInit();
#ifndef DEBUG_MODE
		PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFE);
#else
		time = rtcTimeSec;
		while(time == rtcTimeSec);
#endif
		if(ACC_IS_ON)
			accOnCnt++;
		else
			accOnCnt = 0;
		if((!ACC_IS_ON && (rtcTimeSec - reportInterVal >= sysCfg.offReportInterval))
			|| (accOnCnt >= 3)
		)
		{
			intervalReportWakeFlag = 1;
			reportInterVal = rtcTimeSec;
			TrackerGetNewData();
			break;
		}

		if(alarmFlag & ACCLEROMETER_FLAG && gSensorAlarmTimes)
		{
			break;
		}

		if(!SmsTask_IsNoMsg())
		{
			break;
		}
		//IO Process an fillter
		IO_Control(NULL);
		AlarmTask(rtcTimeSec);

		RING_Process();
		if((atCmdCallParser.ringingDetectFlag))
		{
			break;
		}
		if((rtcTimeSec - smsTimeCheck >= SMS_TIME_CHECK))
		{
			SystemInit();
			TICK_Init(1);
			SST25_Init();
			USART1_Init(SystemCoreClock,__USART1_BAUDRATE);
//			USART3_Init(SystemCoreClock/2,__USART3_BAUDRATE);
			//GSM disable
			MODEM_Sleep();
			smsTimeCheck = rtcTimeSec;
			MODEM_GetCSQ(); 
			SMS_Manage(mainBuf,sizeof(mainBuf));
			if(SmsTask_IsNoMsg())
				MODEM_Sleep();
			else
				break;
		}
		
		RequestTableTask(rtcTimeSec);
		//ADC Check
		ADC_TaskInit();
		while(!ADC_Updated)
		{
			ADC_Task(0);
		}
		
		if(sysSleepCnt % 30 == 0)
		{
				if(!acclError)
				{
					if(MMA7660_Read(MODE_REG) != 0x01)
						AccelerometerTaskInit();
					else
						MMA7660_ReadStatus();
				}
				else if(sysSleepCnt % 100 == 0)
					AccelerometerTaskInit();
		}
	}
	SysWakeUp();
	sysStatus = SYS_RUN_MODE;
}



//Application
void AppRun(void)
{
	uint8_t arg = 0;
	LoadFirmwareFile();
	SmsTask_Init();
	SMS_TaskInit();
	AlarmTaskInit();
	watchdogEnable[WTD_MAIN_LOOP] = 1;
	while(1)
	{
		if(CheckTimeout(&tResetApp) == SYSTICK_TIMEOUT)
			InitTimeout(&tResetApp,SYSTICK_TIME_SEC(3600*24));
		if(GSM_GPRS_Task(&arg) || (!ACC_IS_ON))
		{
			if(!ACC_IS_ON)
				DB_U32Save(&powerOfTimeDelay,GPS_DIR_ADDR);
			SysSleep();
		}
	}
}

uint8_t GSM_GPRS_Task(uint8_t *arg)
{

		Timeout_Type tTrackerTask;
		GsmTask_Init();
		GsmTask_InitApn((uint8_t *)sysCfg.gprsApn,(uint8_t *)sysCfg.gprsUsr,(uint8_t *)sysCfg.gprsPwd);
		
		if(*arg)
		{
			if(MODEM_SendCommand("+CREG: 0,1",modemError,1000,3,"AT+CREG?\r") == 0)
				GsmTask_SetPhase(MODEM_SYS_COVERAGE_OK);
		}
		
		vTcpIpTaskInit();
		InitTimeout(&tAppTimeout,SYSTICK_TIME_SEC(120));
		while(CheckTimeout(&tAppTimeout) != SYSTICK_TIMEOUT && CheckTimeout(&tResetApp) != SYSTICK_TIMEOUT)  
		{
				watchdogFeed[WTD_MAIN_LOOP] = 0; 
			//Alarm Tasks
				AlarmTask(rtcTimeSec);
				//RequestTableTask
				RequestTableTask(rtcTimeSec);
			//GSM tasks
				Gsm_Task();
				if(modemCmdMode)
				{
					Sms_Task();
					VoiceCall_Task();
					ModemCmd_Task();
					CallTask(rtcTimeSec);
				}
				
				GsmCommon_Task();
				
				if(SmsCheckMessage_IsEmpty() 
				&& 	(modemIsBusy == 0)
				&& VoiceCallTask_GetPhase() == CALL_IDLE
				)
				{
					vTcpIpTask();
				}
				
				FIRMWARE_Task();
				//Config and debug task
				UartConfigTask();
				//Read ADC Value
				ADC_Task(rtcTimeSec);
				//CallTask
				CallTask(rtcTimeSec);
				//get IMEI
				if(flagGotIMEI && strstr((char *)sysCfg.imei,(char *)gsmIMEIBuf) == NULL)
				{
					flagGotIMEI = 0;
					newIMEI_IsUpdate = 1;
					strcpy((char *)sysCfg.imei,(char *)gsmIMEIBuf);
				}
			
			//SMS check
				if((GsmTask_GetPhase() == MODEM_SYS_COVERAGE_OK) && (modemIsBusy == 0))
				{
					if(modemCmdMode)
					{
						if((smsNewMessageFlag || (rtcTimeSec - smsTimeCheck >= SMS_TIME_CHECK)))
						{
							smsTimeCheck = rtcTimeSec;
							MODEM_GetCSQ(); 
							SMS_Manage(mainBuf,sizeof(mainBuf));
							
							if(smsNewMessageFlag)
							{
								smsNewMessageFlag = 0;
							}
						}
						else if(SmsCheckMessage_IsEmpty() 
						&& VoiceCallTask_GetPhase() == CALL_IDLE
						&& (!Modem_IsRinging())
						&& (logSendPt != NULL 
								|| ACC_IS_ON 
								|| gotAddrFlag 
								|| getAddrFlag
								|| FirmwareTask_IsBusy()
								|| (CheckTimeout(&tFwDownload) != SYSTICK_TIMEOUT)
								)
						)
						{
							if(GsmCommonTask_IsIdle())
							{
								if(!COMM_ModemIsOnline())
									modemDialFlag = 1;				
								else
									MODEM_GoToDataMode();
							}
						}
					}
					else if(smsNewMessageFlag || Modem_IsRinging())
					{
						COMM_GoToCmdMode();
					}
					else if(CheckTimeout(&tDataIsSending) == SYSTICK_TIMEOUT 
					&& gotAddrFlag == 0 
					&& getAddrFlag == 0
					&& (!FirmwareTask_IsBusy())
					&& (CheckTimeout(&tFwDownload) == SYSTICK_TIMEOUT)
					)
					{ 
						if((rtcTimeSec - smsTimeCheck >= SMS_TIME_CHECK) 
						|| (!SmsCheckMessage_IsEmpty())
						|| ((logSendPt == NULL) && (!ACC_IS_ON))
						)
						{
							COMM_GoToCmdMode();
						}
					}
				}
				if(modemCmdMode)
					logSource = GSM_AT_CMD_LOG;
				else
					logSource = TCP_IP_LOG;
				if((ACC_IS_ON && (rtcTimeSec - reportInterVal >= sysCfg.onReportInterval))
				|| (!ACC_IS_ON && (rtcTimeSec - reportInterVal >= sysCfg.offReportInterval))
				)
				{
					reportInterVal = rtcTimeSec;
					TrackerGetNewData();
				}
				
				if(CheckTimeout(&tTrackerTask) == SYSTICK_TIMEOUT)
				{
					InitTimeout(&tTrackerTask,SYSTICK_TIME_SEC(1));
					RING_Process();
					TrackerTask();
					//IO Process an fillter
					IO_Control(NULL);
				}
				
				if( ioStatus.din[0].newUpdate	)
				{
					ioStatus.din[0].newUpdate = 0;
					TrackerGetNewData();
					sendStatus = 1;
					statusSentFlag = 0;
				}

				if(FirmwareTask_IsBusy() 
				|| (VoiceCallTask_GetPhase() != CALL_IDLE)
				)
				{
					InitTimeout(&tAppTimeout,SYSTICK_TIME_SEC(120));
				}
				//Get Log
				GetDataFromLog();
				
				if(!ACC_IS_ON)
					DB_U32Save(&powerOfTimeDelay,GPS_DIR_ADDR);
				if((!ACC_IS_ON) 
				&& (!trackerSaveLog) 
				&& ((logSendPt == NULL))
				&& SmsCheckMessage_IsEmpty() 
				&& VoiceCallTask_GetPhase() == CALL_IDLE
				&& (!FirmwareTask_IsBusy())
				&& gotAddrFlag == 0 
				&& getAddrFlag == 0
				&& (!Modem_IsRinging())
				&& (!smsNewMessageFlag)
				&& modemCmdMode
				&& (modemIsBusy == 0)
				&&  (accelerometerAlarmCnt == 0)
				&& (CheckTimeout(&tFwDownload) == SYSTICK_TIMEOUT)
				)
				{
					if(statusSentFlag || (sendStatus == 0 &&  statusSentFlag == 0))
					{
						statusSentFlag = 0;
						if(GsmTask_GetPhase() == MODEM_SYS_COVERAGE_OK)
								*arg = 1;
							return 1;
					}
				}		
		}
		if(GsmTask_GetPhase() == MODEM_SYS_COVERAGE_OK)
			*arg = 1;
		return 0;
}



void mCentrelTest(void)
{
		uint8_t c;
		GsmTask_Init();
		GsmTask_InitApn((uint8_t *)sysCfg.gprsApn,(uint8_t *)sysCfg.gprsUsr,(uint8_t *)sysCfg.gprsPwd);
		SmsTask_Init();
		while(1)
		{
			Gsm_Task();
			Sms_Task();
			ModemCmd_Task();
			if(GsmTask_GetPhase() == MODEM_SYS_COVERAGE_OK)
			{
				//GPIOA->CRH 	 &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9 | GPIO_CRH_MODE10 | GPIO_CRH_CNF10);  					// Clear PA9, PA10               
				while(1){
					if(RINGBUF_Get(&USART1_RxRingBuff,&c) == 0)
					{
						while (!(USART3->SR & USART_SR_TXE));
						SysTick_DelayMs(2);
						USART3->DR = c;
						SysTick_DelayMs(2);
					}
					if(RINGBUF_Get(&USART3_RxRingBuff,&c) == 0)
					{
						while (!(USART1->SR & USART_SR_TXE));
						USART1->DR = c;
					}
				}
			}
		}
}


void TIM2_IRQHandler(void)
{
	if(TIM2->SR & 1)
	{
		TIM2->SR = (uint16_t)~0x0001;
		timer2Cnt++;
		if((timer2Cnt % 1000) == 0)
		{

		}
		if(timer2Cnt % 50 == 0)
			CtrLed(timer2Cnt);
		if(timer2Cnt % 100 == 0)
		{
			Accelerometer_Task_MMA7660();
		}
	}
}

void TIMER2_Init(uint32_t pclk)
{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;                     // enable clock for TIM2
    TIM2->PSC = (uint16_t)(pclk/1000000) - 1;            // set prescaler
    TIM2->ARR = (uint16_t)(1000000*TIMER_PERIOD/1000 - 1);  //1ms          // set auto-reload
    TIM2->CR1 = 0;                                          // reset command register 1
    TIM2->CR2 = 0;                                          // reset command register 2
		TIM2->DIER = 1;                             
		NVIC_SetPriority (TIM2_IRQn,((0x01<<3) | TMR2_PRIORITY));
		NVIC_EnableIRQ(TIM2_IRQn);// enable interrupt
    TIM2->CR1 |= 1;                              // enable timer
}

