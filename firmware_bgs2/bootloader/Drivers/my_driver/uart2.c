/**
* \file
*         uart2 driver
* \author
*         Nguyen Van Hai <hainv@ivx.vn>
*/

#include "uart2.h"
#include "gps.h"

/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART2 
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART2_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

uint8_t USART2_RxBuff[1024] = {0};
RINGBUF USART2_RxRingBuff;


void USART2_Init(uint32_t pclk1)
{
 //   RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
    AFIO->MAPR   &= ~(1 << 2);                              // clear USART2 remap
	
		//RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN;                  // enable clock for GPIOA
// 		GPIOA->CRL   &= ~(0xFFUL  << 8);                      // Clear PA2, PA3
// 		GPIOA->CRL   |=  (0x0BUL  << 8);                      // USART2 Tx (PA2)  alternate output push-pull
// 		GPIOA->CRL   |=  (0x04UL  << 12);                     // USART2 Rx (PA3)  input floating

    //RCC->APB1ENR |= RCC_APB1ENR_USART2EN;                   // enable clock for USART2
        
    USART2->BRR  = __USART2_BRR(pclk1, __USART2_BAUDRATE); // set baudrate
    USART2->CR1  = __USART2_DATABITS;                       // set Data bits
    USART2->CR2  = __USART2_STOPBITS;                       // set Stop bits
    USART2->CR1 |= __USART2_PARITY;                         // set Parity
    USART2->CR3  = __USART2_FLOWCTRL;                       // Set Flow Control

    USART2->CR1 |= (USART_CR1_RE | USART_CR1_TE);           // RX, TX enable
		//Ringbuf init
		RINGBUF_Init(&USART2_RxRingBuff,USART2_RxBuff,sizeof(USART2_RxBuff));
		// interrupts used
		USART2->CR1 |= USART_CR1_RXNEIE;
		
		/* preemption = GPS_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(USART2_IRQn, ((0x01<<3)| GPS_PRIORITY));
		NVIC_EnableIRQ(USART2_IRQn);
    USART2->CR1 |= USART_CR1_UE;                            // USART enable
}


uint8_t USART2_PutChar (uint8_t ch) 
{
   while (!(USART2->SR & USART_SR_TXE));
   return USART2->DR = ch;
}


uint8_t USART2_GetChar (void) 
{
  while (!(USART2->SR & USART_SR_RXNE));
  return ((uint8_t)(USART2->DR & 0x1FF));
}

void USART2_IRQHandler(void) 
{
		uint8_t c;
	if(USART2->SR & USART_SR_RXNE)
	{	  
			c = (uint8_t)(USART2->DR & 0x1FF);
			RINGBUF_Put(&USART2_RxRingBuff,c);
	}
}





/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/




