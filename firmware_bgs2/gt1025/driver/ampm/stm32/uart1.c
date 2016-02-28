
/**
* \file
*         uart1 driver
* \author
*         Nguyen Van Hai <blue@ambo.com.vn>
*/
#include "uart1.h"
/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART1 
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART1_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))



uint8_t USART1_RxBuff[1024] = {0};
RINGBUF USART1_RxRingBuff;

extern uint8_t modemCmdMode;
extern void AT_ComnandParser(char c);
extern RINGBUF configTaskRingBuf;

#ifdef USE_TX_RINGBUFF
uint8_t USART1_TxBuff[1024] = {0};
RINGBUF USART1_TxRingBuff;
#endif

void USART1_Init(uint32_t pclk2,uint32_t baudrate)
{
    //AFIO->MAPR   &= ~(1 << 2);                              // clear USART1 remap
		//RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN; /* enable clock for USART1*/
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN; /* enable clock for USART1*/
		//UART1 Pin
		GPIOA->CRH 	 &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9 | GPIO_CRH_MODE10 | GPIO_CRH_CNF10);  					// Clear PA9, PA10               
		GPIOA->CRH   |=    GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1; // USART1 Tx (PA9)   Alternate function output Open-drain
		GPIOA->CRH   |=  (GPIO_CRH_CNF10_0);                      // USART1 Rx (PA10) input floating
	
    USART1->BRR  = __USART1_BRR(pclk2, baudrate); // set baudrate
    USART1->CR1  = __USART1_DATABITS;                       // set Data bits
    USART1->CR2  = __USART1_STOPBITS;                       // set Stop bits
    USART1->CR1 |= __USART1_PARITY;                         // set Parity
    USART1->CR3  = __USART1_FLOWCTRL;                       // Set Flow Control

    USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE);           // RX, TX enable
		
		//CTS
		#ifdef UART1_USE_CTS
			USART1->CR3 |= USART_CR3_CTSE | USART_CR3_CTSIE;
			GPIOA->CRH 	 &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11 );  					// PA11  
			GPIOA->CRH   |=  (GPIO_CRH_CNF11_0);                      // USART1 CTS (PA11) input floating
		#endif
		#ifdef UART1_USE_RTS
			GPIOA->CRH 	 &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);  					// PA12  
			GPIOA->CRH   |=     GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1 | GPIO_CRH_CNF12_0; // 
			GPIOA->BRR = GPIO_BSRR_BS12;
		//	USART1->CR3 |= USART_CR3_RTSE;
		#endif
			
 
		// interrupts used
		USART1->CR1 |= USART_CR1_RXNEIE;
		RINGBUF_Init(&USART1_RxRingBuff,USART1_RxBuff,sizeof(USART1_RxBuff));
		
		#ifdef USE_TX_RINGBUFF
		USART1->CR1 |= USART_CR1_TXEIE;
		RINGBUF_Init(&USART1_TxRingBuff,USART1_TxBuff,sizeof(USART1_TxBuff));
		#endif
		
		/* preemption = GSM_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(USART1_IRQn, ((0x01<<3)| 1));
		NVIC_EnableIRQ(USART1_IRQn);
		
    USART1->CR1 |= USART_CR1_UE;                            // USART enable
}



void USART1_DeInit(void)
{
	RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN; /* enable clock for USART1*/
	//UART1 Pin
	GPIOA->CRH 	 &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9 | GPIO_CRH_MODE10 | GPIO_CRH_CNF10);  					// Clear PA9, PA10               
}

void USART1_PutString (uint8_t *s) 
{
   while(*s)
	{
		USART1_PutChar(*s++);
	}
}
   

uint8_t USART1_PutChar (uint8_t ch) 
{
	uint32_t timeOut = 100000000;
#ifndef USE_TX_RINGBUFF
  while (!(USART1->SR & USART_SR_TXE));
	USART1->DR = ch;
#else
	while((RINGBUF_GetFill(&USART1_TxRingBuff) >= USART1_TxRingBuff.size) && timeOut--){}
	RINGBUF_Put(&USART1_TxRingBuff,ch);
	if((USART1->CR1 & USART_CR1_TXEIE) != USART_CR1_TXEIE)
		USART1->CR1 |= USART_CR1_TXEIE;
#endif
	return ch;
}


uint8_t USART1_GetChar (void) 
{
  while (!(USART1->SR & USART_SR_RXNE));
  return ((uint8_t)(USART1->DR & 0x1FF));
}


void USART1_IRQHandler(void) 
{
	uint8_t c = 0xff;
	if(USART1->SR & (USART_SR_RXNE | USART_SR_ORE))
	{
			c = (uint8_t)(USART1->DR & 0x1FF);
			if(RINGBUF_Put(&USART1_RxRingBuff,c) == 0){};
			if(RINGBUF_GetFill(&USART1_RxRingBuff) >= USART1_RxRingBuff.size - 2)
				{}//GPIOA->BSRR = GPIO_BSRR_BS12;
			if(modemCmdMode)
				AT_ComnandParser(c);
	}
//	else
//	{
//		c = (uint8_t)(USART1->DR & 0x1FF); 
//	}
	

	#ifdef USE_TX_RINGBUFF
	#ifdef UART1_USE_CTS
	if(USART1->SR & USART_SR_CTS)
	{
			USART1->SR &= ~USART_SR_CTS;
			if(GPIOA->IDR & GPIO_BSRR_BS11)
			{
				if(USART1->SR & USART_SR_TXE); //clear tx flag
			
				USART1->CR1 &= (~USART_CR1_TXEIE);
			}
			else if(RINGBUF_Get(&USART1_TxRingBuff,&c) == 0)
			{
				if((USART1->CR1 & USART_CR1_TXEIE) != USART_CR1_TXEIE)
					USART1->CR1 |= USART_CR1_TXEIE;
				USART1->DR = c;
			}
	}
	else if(USART1->SR & USART_SR_TXE)
	{
		if(RINGBUF_Get(&USART1_TxRingBuff,&c) == 0)
		{
			USART1->DR = c;
		}
		else
			USART1->CR1 &= (~USART_CR1_TXEIE);
	}
	#else
	if(USART1->SR & USART_SR_TXE)
	{
		if(RINGBUF_Get(&USART1_TxRingBuff,&c) == 0)
		{
			USART1->DR = c;
		}
		else
			USART1->CR1 &= (~USART_CR1_TXEIE);
	}
	#endif
	#endif
}





/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/




