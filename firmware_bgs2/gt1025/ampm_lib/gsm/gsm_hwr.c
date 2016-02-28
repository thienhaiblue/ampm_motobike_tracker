/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "stm32f10x.h"
#include "gsm_hwr.h"
#include "uart1.h"
#include "hw_config.h"
#include "lib/sys_tick.h"
#include "led.h"

void Delay(uint32_t i)
{
	while(i--);
}

uint8_t MODEM_PowerOn(void)
{
	LedSetStatus(&led1Ctr,50,50,LED_TURN_ON,0xffffffff);	
	GSM_POWER_PIN_SET_OUTPUT;
	GSM_POWER_PIN_CLR;
	SysTick_DelayMs(2000);
	GSM_POWER_PIN_SET;

	POWER_PIN_SET_OUTPUT;
	RESET_PIN_SET_OUTPUT;
	DTR_PIN_SET_OUTPUT;
	DTR_PIN_CLR;

	RESET_PIN_CLR; 
	SysTick_DelayMs(100);    
	RESET_PIN_SET;
	POWER_PIN_CLR;
	SysTick_DelayMs(100); 
	POWER_PIN_SET; 
	Delay(350);     
	POWER_PIN_CLR;
	SysTick_DelayMs(1000);
	
	return 0;
}

void MODEM_Sleep(void)
{
	//DTR_PIN_SET;
}
void MODEM_Wakeup(void)
{
	//DTR_PIN_CLR;
}
void MODEM_UartInit(uint32_t baudrate)
{
	USART1_Init(SystemCoreClock,baudrate);
}


