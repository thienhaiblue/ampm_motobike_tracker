/**********************************************************************
Name: Hai Nguyen Van
Cellphone: (84) 97-8779-222
Mail:thienhaiblue@ampm.com.vn 
----------------------------------
AMPM ELECTRONICS EQUIPMENT TRADING COMPANY LIMITED.,
Add: 634/6 Phan Van Suu street , Ward 13, Tan Binh District, HCM City, VN

*********************************************************************/
#include "comm.h"


 
RINGBUF *commTxRingBuf = &USART1_TxRingBuff;
RINGBUF *commRxRingBuf = &USART1_RxRingBuff;
extern uint8_t modem_IsOnline; 
uint8_t pppOpenedBefore = 0;

void COMM_Putc(uint8_t c)
{
	USART1_PutChar(c);
}
void COMM_Puts(uint8_t *s)
{
	USART1_PutString(s);
}


uint8_t COMM_CarrierDetected(void)
{
	return MODEM_CarrierDetected();
}
uint8_t COMM_ModemIsOnline(void)
{
	return modem_IsOnline;
}

uint32_t COMM_ModemSetOffline(void)
{
	modem_IsOnline = 0;
}

void COMM_GoToCmdMode(void)
{
		MODEM_GoToCmdMode();
}

uint8_t COMM_pppIsOpened(void)
{
	return pppOpenedBefore;
}

void COMM_pppSetOpened(void)
{
	pppOpenedBefore = 1;
}

void COMM_pppClrOpened(void)
{
	pppOpenedBefore = 0;
}



