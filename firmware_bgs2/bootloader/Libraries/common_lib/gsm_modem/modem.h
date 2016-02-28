#ifndef _MODEM_H_
#define _MODEM_H_

#include <string.h>
#include <stdio.h>
#include "tick.h"
#include "uart1.h"
#include "ringbuf.h"
#include "dbg_cfg_app.h"
#include "system_config.h"
#include "sys_time.h"



#define POWER_PORT	GPIOB	
#define POWER_PIN		GPIO_BSRR_BS11
#define POWER_PIN_SET_OUTPUT	POWER_PORT->CRH	&= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11); POWER_PORT->CRH	|= (GPIO_CRH_MODE11_0 | GPIO_CRH_CNF11_0) 
#define POWER_PIN_SET_INPUT		POWER_PORT->CRH	&= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11); POWER_PORT->CRH	|=  GPIO_CRH_CNF11_0
#define POWER_PIN_SET					POWER_PORT->BSRR = POWER_PIN
#define POWER_PIN_CLR					POWER_PORT->BRR = POWER_PIN


#define RESET_PORT	GPIOB	
#define RESET_PIN		GPIO_BSRR_BS10
#define RESET_PIN_SET_OUTPUT	RESET_PORT->CRH	&= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10); RESET_PORT->CRH	|= (GPIO_CRH_MODE10_0 | GPIO_CRH_CNF10_0) 
#define RESET_PIN_SET_INPUT		RESET_PORT->CRH	&= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10); RESET_PORT->CRH	|=  GPIO_CRH_CNF10_0
#define RESET_PIN_SET					RESET_PORT->BSRR = RESET_PIN
#define RESET_PIN_CLR					RESET_PORT->BRR = RESET_PIN

#define DTR_PORT	GPIOB	
#define DTR_PIN		GPIO_BSRR_BS12
#define DTR_PIN_SET_OUTPUT	DTR_PORT->CRL	&= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12); RESET_PORT->CRH	|= (GPIO_CRH_MODE12_0 | GPIO_CRH_CNF12_0) 
#define DTR_PIN_SET_INPUT		DTR_PORT->CRL	&= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12); RESET_PORT->CRH	|=  GPIO_CRH_CNF12_0
#define DTR_PIN_SET					DTR_PORT->BSRR = DTR_PIN
#define DTR_PIN_CLR					DTR_PORT->BRR = DTR_PIN

#define RTS_PORT	GPIOB	
#define RTS_PIN		GPIO_BSRR_BS15
#define RTS_PIN_SET_OUTPUT	DTR_PORT->CRL	&= ~(GPIO_CRH_MODE15 | GPIO_CRH_CNF15); RESET_PORT->CRH	|= (GPIO_CRH_MODE15_0 | GPIO_CRH_CNF15_0) 
#define RTS_PIN_SET_INPUT		DTR_PORT->CRL	&= ~(GPIO_CRH_MODE15 | GPIO_CRH_CNF15); RESET_PORT->CRH	|=  GPIO_CRH_CNF15_0
#define RTS_PIN_SET					DTR_PORT->BSRR = RTS_PIN
#define RTS_PIN_CLR					DTR_PORT->BRR = RTS_PIN


#define CTS_PORT			GPIOB	
#define CTS_PIN				GPIO_IDR_IDR14
#define GET_CTS_PIN 	(STATUS_PORT->IDR & STATUS_PIN)

#define STATUS_PORT			GPIOB	
#define STATUS_PIN			GPIO_IDR_IDR2
#define GET_STATUS_PIN 	(STATUS_PORT->IDR & STATUS_PIN)

#define RI_PORT			GPIOA	
#define RI_PIN			GPIO_IDR_IDR8
#define GET_RI_PIN 	(RI_PORT->IDR & RI_PIN)


#define MODEM_POWER_ON	1		
#define MODEM_POWER_OFF	0


#define MODEM_Info(...)		DbgCfgPrintf(__VA_ARGS__)
#define MODEM_Debug(...) //DbgCfgPrintf(__VA_ARGS__)


/**
 *CELL LOCATE
 */
typedef struct _cellLocate
{
    DATE_TIME time;       /**< UTC of position */
		double  lat;        /**< Latitude in NDEG - [degree][min].[sec/60] */
		double  lon;        /**< Longitude in NDEG - [degree][min].[sec/60] */
    double  speed;      /**< Speed over the ground in knots */
    double  dir;  /**< Track angle in degrees True */
    double  uncertainty; 
		double alt;
		char ew;
		char ns;
} cellLocateType;

extern cellLocateType cellLocate;


extern uint8_t gsmDebugBuff[256];
extern volatile uint8_t modemRinging;
extern uint8_t csqBuff[16];
extern float csqValue;
uint8_t MODEM_Init(void);
void MODEM_SetApn(uint8_t *apn);
void MODEM_GetIMEI(uint8_t *imei);
uint8_t MODEM_Dial(void);
uint8_t MODEM_Ringing(void);
uint8_t MODEM_CarrierDetected(void);
void MODEM_DataMode(void);
void MODEM_CommandMode(void);
void MODEM_MonitorInput(void);
void MODEM_EndRing(void);
void MODEM_Hangup(void);
uint8_t MODEM_CheckResponse(uint8_t *str,uint32_t t);
uint8_t MODEM_Wait(const uint8_t *res, uint32_t time);
uint8_t MODEM_Gets(uint8_t *buf, uint16_t bufSize, uint8_t match);
uint32_t MODEM_CheckAccount(void);
uint8_t MODEM_GetOperatorName(uint8_t *buf, uint16_t bufSize);
uint8_t MODEM_SendSMS(const uint8_t *recipient, uint8_t *data);
uint8_t SendSMS(const uint8_t *recipient, const uint8_t *data);
uint8_t MODEM_GetCellId(uint32_t *lac, uint32_t *ci);
uint8_t MODEM_GetCSQ(void);
uint8_t InternetSetProfiles(uint8_t *apn,uint8_t *usr,uint8_t *pwd);
uint8_t MODEM_ConnectSocket(uint8_t socket,uint8_t *ip,uint16_t port,uint8_t useIp);
uint8_t MODEM_CloseSocket(uint8_t socket);
uint8_t MODEM_EnableGPRS(void);
uint8_t MODEM_DisableGPRS(void);
uint8_t MODEM_CreateSocket(uint8_t socket);
uint16_t MODEM_CheckGPRSDataOut(uint8_t socket);
uint8_t MODEM_GprsSendData(uint8_t socket,uint8_t *data,uint32_t length);
uint8_t MODEM_Started(void);
uint8_t GetCellLocate(uint8_t *buff);
uint8_t CallingProcess(uint8_t callAction);
uint8_t VoiceSetup(void);


#endif
