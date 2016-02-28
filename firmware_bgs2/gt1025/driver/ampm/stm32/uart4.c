/**
* \file
*         uart2 driver
* \author
*         Nguyen Van Hai <blue@ambo.com.vn>
*/

#include "uart3.h"
#include "camera/rs485_camera.h"
extern RINGBUF comRingBuf;
extern uint8_t logEnable;
extern void RFID_Input(char c);
/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART4 
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART4_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

// uint8_t USART4_RxBuff[1024] = {0};
// RINGBUF USART4_RxRingBuff;


void USART4_Init(uint32_t pclk1,uint32_t baudrate)
{
    AFIO->MAPR   &= ~(1 << 2);                              // clear USART4 remap
	
    USART4->BRR  = __USART4_BRR(pclk1, baudrate); // set baudrate
    USART4->CR1  = __USART4_DATABITS;                       // set Data bits
    USART4->CR2  = __USART4_STOPBITS;                       // set Stop bits
    USART4->CR1 |= __USART4_PARITY;                         // set Parity
    USART4->CR3  = __USART4_FLOWCTRL;                       // Set Flow Control

    USART4->CR1 |= (USART_CR1_RE | USART_CR1_TE);           // RX, TX enable
		// interrupts used
		USART4->CR1 |= USART_CR1_RXNEIE;
		
		/* preemption = GPS_PRIORITY, sub-priority = 1 */
		NVIC_SetPriority(USART4_IRQn, ((0x01<<3)| CAMERA_PRIORITY));
		NVIC_EnableIRQ(USART4_IRQn);
    USART4->CR1 |= USART_CR1_UE;                            // USART enable
}



uint8_t USART4_PutChar (uint8_t ch) 
{
   while (!(USART4->SR & USART_SR_TXE));
   return USART4->DR = ch;
}

void USART4_PutString (char *s) 
{
   while(*s)
	{
		USART4_PutChar((uint8_t)*s++);
	}
}

uint8_t USART4_GetChar (void) 
{
  while (!(USART4->SR & USART_SR_RXNE));
  return ((uint8_t)(USART4->DR & 0x1FF));
}

void USART4_IRQHandler(void) 
{
		uint8_t c;
	if(USART4->SR & (USART_SR_RXNE | USART_SR_ORE))
	{	  
			c = (uint8_t)(USART4->DR & 0x1FF);
	}
	else
	{
		c = (uint8_t)(USART4->DR & 0x1FF); 
	}
	CAM_Input(c);
}





/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/




