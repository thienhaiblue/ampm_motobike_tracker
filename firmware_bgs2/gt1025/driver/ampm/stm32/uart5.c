/**
* \file
*         uart2 driver
* \author
*         Nguyen Van Hai <blue@ambo.com.vn>
*/

#include "uart5.h"


extern RINGBUF comRingBuf;
extern uint8_t logEnable;

uint8_t UART5_TxBuff[256];
RINGBUF UART5_TxRingBuff;
uint8_t UART5_RxBuff[256];
RINGBUF UART5_RxRingBuff;

/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for UART5 
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __UART5_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

// uint8_t UART5_RxBuff[1024] = {0};
// RINGBUF UART5_RxRingBuff;


void UART5_Init(uint32_t pclk1,uint32_t baudrate)
{

		RCC->APB1ENR |= RCC_APB1ENR_UART5EN; /* enable clock for UART5*/
		//UART1 Pin
		GPIOD->CRL 	 &= ~(GPIO_CRL_MODE2 | GPIO_CRL_CNF2);  					// Clear PD2          
		GPIOD->CRL   |=  (GPIO_CRL_CNF2_0);                      // USART5  Rx (PD2) input floating	
		GPIOC->CRH 	 &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);  					// Clear PC12        
		GPIOC->CRH   |=   GPIO_CRH_CNF12_1 | GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1; // USART5 Tx (PC12)   Alternate function output Open-drain
		
	
    UART5->BRR  = __UART5_BRR(pclk1, baudrate); // set baudrate
    UART5->CR1  = __UART5_DATABITS;                       // set Data bits
    UART5->CR2  = __UART5_STOPBITS;                       // set Stop bits
    UART5->CR1 |= __UART5_PARITY;                         // set Parity
    UART5->CR3  = __UART5_FLOWCTRL;                       // Set Flow Control

	  UART5->CR1 |= (USART_CR1_RE | USART_CR1_TE);           // RX, TX enable
 
		// interrupts used
		UART5->CR1 |= USART_CR1_RXNEIE;
		RINGBUF_Init(&UART5_RxRingBuff,UART5_RxBuff,sizeof(UART5_RxBuff));
		
		#ifdef USE_TX_RINGBUFF
		UART5->CR1 |= USART_CR1_TXEIE;
		RINGBUF_Init(&UART5_TxRingBuff,UART5_TxBuff,sizeof(UART5_TxBuff));
		#endif
		
		/* preemption = GPS_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(UART5_IRQn, ((0x01<<3)| UART5_PRIORITY)); 
		NVIC_EnableIRQ(UART5_IRQn);
    UART5->CR1 |= USART_CR1_UE;                            // USART enable
}



uint8_t UART5_PutChar (uint8_t ch) 
{
	uint32_t timeOut = 100000000;
#ifndef USE_TX_RINGBUFF
  while (!(UART5->SR & USART_SR_TXE));
	UART5->DR = ch;
#else
	while((RINGBUF_GetFill(&UART5_TxRingBuff) >= UART5_TxRingBuff.size) && timeOut--){}
	RINGBUF_Put(&UART5_TxRingBuff,ch);
	if((UART5->CR1 & USART_CR1_TXEIE) != USART_CR1_TXEIE)
		UART5->CR1 |= USART_CR1_TXEIE;
#endif
	return ch;
}

void UART5_PutString (char *s) 
{
   while(*s)
	{
		UART5_PutChar((uint8_t)*s++);
	}
}

uint8_t UART5_GetChar (void) 
{
  while (!(UART5->SR & USART_SR_RXNE));
  return ((uint8_t)(UART5->DR & 0x1FF));
}

void UART5_IRQHandler(void) 
{
		uint8_t c;
	if(UART5->SR & (USART_SR_RXNE | USART_SR_ORE))
	{	  
			c = (uint8_t)(UART5->DR & 0x1FF);
			RINGBUF_Put(&UART5_RxRingBuff,c);
	}
	else
	{
		c = (uint8_t)(UART5->DR & 0x1FF); 
	}
	
	if(UART5->SR & USART_SR_TXE)
	{
		if(RINGBUF_Get(&UART5_TxRingBuff,&c) == 0)
		{
			UART5->DR = c;
		}
		else
			UART5->CR1 &= (~USART_CR1_TXEIE);
	}

}





/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/




