
/**
* \file
*         uart1 driver
* \author
*         Nguyen Van Hai <hainv@ivx.vn>
*/
#include "uart1.h"
#include "at_command_parser.h"
/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART1 
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART1_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))



uint8_t USART1_RxBuff[512] = {0};
RINGBUF USART1_RxRingBuff;

uint8_t USART1_USB_RxBuff[512] = {0};
RINGBUF USART1_USB_RxRingBuff;

#ifdef USE_TX_RINGBUFF
uint8_t USART1_TxBuff[512] = {0};;
RINGBUF USART1_TxRingBuff;
#endif

void USART1_Init(uint32_t pclk2)
{
//    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
    AFIO->MAPR   &= ~(1 << 2);                              // clear USART1 remap
	
// 		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;                   // enable clock for GPIOA
// 		GPIOA->CRH   &= ~(0xFFUL  << 4);                      // Clear PA9, PA10
// 		GPIOA->CRH   |=  (0x0BUL  << 4);                      // USART1 Tx (PA9)  alternate output push-pull
// 		GPIOA->CRH   |=  (0x04UL  << 8);                      // USART1 Rx (PA10) input floating

    //RCC->APB2ENR |= RCC_APB2ENR_USART1EN;                   // enable clock for USART1
        
    USART1->BRR  = __USART1_BRR(pclk2, __USART1_BAUDRATE); // set baudrate
    USART1->CR1  = __USART1_DATABITS;                       // set Data bits
    USART1->CR2  = __USART1_STOPBITS;                       // set Stop bits
    USART1->CR1 |= __USART1_PARITY;                         // set Parity
    USART1->CR3  = __USART1_FLOWCTRL;                       // Set Flow Control

    USART1->CR1 |= (USART_CR1_RE | USART_CR1_TE);           // RX, TX enable
 
		// interrupts used
		USART1->CR1 |= USART_CR1_RXNEIE;
		RINGBUF_Init(&USART1_RxRingBuff,USART1_RxBuff,sizeof(USART1_RxBuff));
		
		RINGBUF_Init(&USART1_USB_RxRingBuff,USART1_USB_RxBuff,sizeof(USART1_USB_RxBuff));
		
		#ifdef USE_TX_RINGBUFF
		USART1->CR1 |= USART_CR1_TXEIE;
		RINGBUF_Init(&USART1_TxRingBuff,USART1_TxBuff,sizeof(USART1_TxBuff));
		#endif
		
		/* preemption = GSM_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(USART1_IRQn, ((0x01<<3)| GSM_PRIORITY));
		NVIC_EnableIRQ(USART1_IRQn);
    
		
		
		
    USART1->CR1 |= USART_CR1_UE;                            // USART enable
}



void USART1_DeInit(void)
{

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
   while (!(USART1->SR & USART_SR_TXE));
   return USART1->DR = ch;
}


uint8_t USART1_GetChar (void) 
{
  while (!(USART1->SR & USART_SR_RXNE));
  return ((uint8_t)(USART1->DR & 0x1FF));
}

void USART1_IRQHandler(void) 
{
		uint8_t c = 0xff;
	if(USART1->SR & USART_SR_RXNE)
	{
			c = (uint8_t)(USART1->DR & 0x1FF);
			RINGBUF_Put(&USART1_RxRingBuff,c);
			RINGBUF_Put(&USART1_USB_RxRingBuff,c);
			AT_ComnandParser(c);
	}
	#ifdef USE_TX_RINGBUFF
	if(USART1->SR & USART_SR_TXE)
	{
		if(RINGBUF_Get(&USART1_TxRingBuff,&c) == 0)
			USART1->DR = c;
		else
			USART1->CR1 &= (~USART_CR1_TXEIE);
	}
	#endif
}





/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/




