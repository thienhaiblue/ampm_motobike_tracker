/**
* \file
*         uart2 driver
* \author
*         Nguyen Van Hai <blue@ambo.com.vn>
*/

#include "uart3.h"
extern RINGBUF comRingBuf;
extern uint8_t logEnable;
extern void RFID_Input(char c);
/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART3 
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART3_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

 uint8_t USART3_RxBuff[1024] = {0};
 RINGBUF USART3_RxRingBuff;


void USART3_Init(uint32_t pclk1,uint32_t baudrate)
{
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN;                     // enable clock for Alternate Function
		//AFIO->MAPR   &= ~(3 << 4);                              // clear USART2 remap
		//UART3 Pin
		GPIOB->CRH 	 &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10 | GPIO_CRH_MODE11 | GPIO_CRH_CNF11);  					// Clear PA11, PA10               
		GPIOB->CRH   |=   GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1; // USART1 Tx (PA10)   Alternate function output Open-drain
		GPIOB->CRH   |=  (GPIO_CRH_CNF11_0);                      // USART1 Rx (PA11) input floating
	
	
    USART3->BRR  = __USART3_BRR(pclk1, baudrate); // set baudrate
    USART3->CR1  = __USART3_DATABITS;                       // set Data bits
    USART3->CR2  = __USART3_STOPBITS;                       // set Stop bits
    USART3->CR1 |= __USART3_PARITY;                         // set Parity
    USART3->CR3  = __USART3_FLOWCTRL;                       // Set Flow Control
	
    USART3->CR1 |= (USART_CR1_RE | USART_CR1_TE);           // RX, TX enable
		// interrupts used
		USART3->CR1 |= USART_CR1_RXNEIE;
			
		RINGBUF_Init(&USART3_RxRingBuff,USART3_RxBuff,sizeof(USART3_RxBuff));
	
		/* preemption = GPS_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(USART3_IRQn, ((0x01<<3)| 1));
		NVIC_EnableIRQ(USART3_IRQn);
    USART3->CR1 |= USART_CR1_UE;                            // USART enable
}

void USART3_DeInit(void)
{
	RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN;                     // enable clock for Alternate Function
	GPIOA->CRL   &= ~( GPIO_CRL_CNF3 | GPIO_CRL_MODE3);           // Clear PA3
}


uint8_t USART3_PutChar (uint8_t ch) 
{
   while (!(USART3->SR & USART_SR_TXE));
   return USART3->DR = ch;
}

void USART3_PutString (char *s) 
{
   while(*s)
	{
		USART3_PutChar((uint8_t)*s++);
	}
}

uint8_t USART3_GetChar (void) 
{
  while (!(USART3->SR & USART_SR_RXNE));
  return ((uint8_t)(USART3->DR & 0x1FF));
}

void USART3_IRQHandler(void) 
{
		uint8_t c;
	if(USART3->SR & (USART_SR_RXNE | USART_SR_ORE))
	{	  
			c = (uint8_t)(USART3->DR & 0x1FF);
			RINGBUF_Put(&USART3_RxRingBuff,c);
	}
	else
	{
		c = (uint8_t)(USART3->DR & 0x1FF); 
	}
	//CAM_Input(c);
}





/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/




