#ifndef _MODEM_H_
#define _MODEM_H_

#include <stdint.h>
#include "gsm_hwr.h"
#include "hw_config.h"

#define MODEM_DTR_Assert()			DTR_PIN_SET
#define MODEM_DTR_DeAssert()		DTR_PIN_CLR

extern const uint8_t *modemOk;
extern const uint8_t *modemError;
extern uint8_t modemCmdMode;
extern uint8_t modemDialFlag;
extern uint8_t modemToCommandModeFlag;
extern uint8_t modemToDataModeFlag;
extern uint8_t modem_IsOnline;
extern uint8_t *modemApn;
	
extern float csqValue;
extern uint8_t gsmSignal;
	
uint8_t MODEM_CommandMode(void);
uint8_t MODEM_Dial(void);
uint8_t MODEM_Init (void);
uint8_t MODEM_PowerOff(void);
uint8_t VoiceSetup(void);
uint8_t MODEM_CheckResponse(uint8_t *str,uint32_t t);
uint8_t MODEM_SendCommand(const uint8_t *resOk,const uint8_t *resFails,uint32_t timeout,uint8_t tryAgainNum,const uint8_t *format, ...);
void MODEM_FlushBuffer(void);
uint8_t MODEM_PowerOff(void);
uint8_t MODEM_GetCSQ(void);
void MODEM_GoToCmdMode(void);
void MODEM_GoToDataMode(void);
void MODEM_IsOfline(void);
void MODEM_SetApn(uint8_t *apn);

#endif




