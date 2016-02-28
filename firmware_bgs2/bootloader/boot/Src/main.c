
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include "sst25.h"
#include "stm32f10x_flash.h"
#include "flashprog.h"
#include "uart3.h"
#include "proto_parser.h"
#include "tick.h"


#define FLASH_BUF_SIZE 2048
#define USER_FLASH_START		0x08002800

#define USB_rxBuff	HID_receivedBuf



uint8_t USB_rxBuff[2048];


uint8_t buff[4096];
uint8_t flashBuff[2048];
uint32_t fileSize;
uint32_t fileCrc;
uint32_t packetNo;
uint32_t firmwareFileOffSet;
uint32_t firmwareFileSize;
uint8_t USB_txBuff[2048];
uint8_t usbRecv = 0;
uint32_t memAddr = 0;
uint32_t flashCheckSum;

CFG_PROTOCOL_TYPE UART_ProtoRecv;
PARSER_PACKET_TYPE UART_parserPacket;

Timeout_Type bootLoginTimeout;
Timeout_Type bootDisTimeout;
Timeout_Type bootExitTimeout;

uint8_t bootMode = 0,phase = 0;

void SysDeInit(void);
void HardFault_Handler(void)
{
		NVIC_SystemReset();
}

void SetOutReport (void)
{
	usbRecv = 1;
}

void GetInReport  (void)
{

}

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

void execute_user_code(void)
{
	USART3_PutString("\r\nGO TO APPLICATION\r\n");
	SysDeInit();
	if (((*(vu32*)USER_FLASH_START) & 0x2FFF0000 ) == 0x20000000)
    { /* Jump to user application */
			//SCB->VTOR = USER_FLASH_START;
      JumpAddress = *(vu32*) (USER_FLASH_START + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(vu32*) USER_FLASH_START);
      Jump_To_Application();
    }    
}

void SysInit(void)
{
	//__disable_irq();
	SystemInit();
	TICK_Init(1);
	RCC->APB2ENR = (RCC_APB2ENR_AFIOEN |  /*enable clock for Alternate Function*/
								 RCC_APB2ENR_IOPAEN |   /* enable clock for GPIOA*/
								 RCC_APB2ENR_IOPBEN |	/*enable clock for GPIOB*/
								 RCC_APB2ENR_IOPCEN | /*enable clock for GPIOc*/ 									 
								 RCC_APB2ENR_SPI1EN	/*enable clock for SPI1*/
								);     
	RCC->CFGR |= RCC_CFGR_USBPRE; // precale is 1 for usb clock
	RCC->APB1ENR |= RCC_APB1ENR_USBEN;/*enable clock for USB*/
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;	//Disable JTAG funtion, keep SWJ funtion for debug(only user when PB3,PB4,PB5,PA15 user for another funtion)
	USART3_Init(SystemCoreClock,115200);
	__enable_irq();
}

void SysDeInit(void)
{
	//default reset
	__disable_irq();
	NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	
	GPIOA->CRH = 0x88844444;
	GPIOA->CRL = 0x44444444;
	
	GPIOB->CRH = 0x44444444;
	GPIOB->CRL = 0x44444444;
	
	GPIOC->CRH = 0x44444444;
	GPIOC->CRL = 0x44444444;
	
	RCC->APB1ENR = 0;
	RCC->APB2ENR = 0;

	__enable_irq();
}

void Delay(uint32_t i)
{
	while(i--);
}

int main(void)
{
	uint32_t *u32Pt = (uint32_t *)buff;
	uint8_t *u8pt,u8temp;
	int32_t i = 10000000,j = 0;
	uint32_t u32temp;
	uint16_t u16temp;
	uint32_t res;
	uint8_t c;
	uint32_t timeOut = 3000000;
	uint8_t tryNum,exit;
	SysInit();
	
//	RS232_PIN_SET_OUTPUT;
//	RS232_PIN_CLR;
	USART3_PutString("\r\n************************AMPM*********************\r\n");
	USART3_PutString("\r\nHello!\r\n");
	USART3_PutString("\r\nI'm GSM_STM32_BOOTLOADER\r\n");
	USART3_PutString("\r\nMy Ver:V1.0\r\n");
	USART3_PutString("\r\nhttp://ampm.com.vn\r\n");
	USART3_PutString("\r\n************************START MAIN***************\r\n");
	DelayMs(1000);
	
	InitTimeout(&bootDisTimeout,TIME_SEC(3));
	InitTimeout(&bootLoginTimeout,TIME_SEC(1));
	InitTimeout(&bootExitTimeout,TIME_SEC(10));
	UART_ProtoRecv.dataPt = &USB_rxBuff[4];
	UART_parserPacket.state = CFG_CMD_WAITING_SATRT_CODE;
	UART_parserPacket.lenMax = UART_PACKET_SIZE;
	FLASH_Unlock();
	while((CheckTimeout(&bootDisTimeout) != TIMEOUT || bootMode)
		&& CheckTimeout(&bootExitTimeout) != TIMEOUT
	)
//	while(0)
	{
			if(bootMode == 0 && CheckTimeout(&bootLoginTimeout) == TIMEOUT)
			{
				USB_txBuff[0] = 0xCA;
				USB_txBuff[1] = 4; //length
				USB_txBuff[3] = 0x88; //opcode
				USB_txBuff[4] = 0xA5;
				USB_txBuff[5] = 0xA5;
				USB_txBuff[6] = 0xA5;
				USB_txBuff[7] = 0xA5;
				USB_txBuff[USB_txBuff[1] + 4] = CfgCalcCheckSum((uint8_t *)&USB_txBuff[4],USB_txBuff[1]);
				for(i = 0;i < USB_txBuff[1] + 4 + 1; i++)
				{
					USART3_PutChar(USB_txBuff[i]);
				}
				USB_rxBuff[0] = 0;
				InitTimeout(&bootLoginTimeout,TIME_SEC(2));
			}
			while(RINGBUF_Get(&USART3_RxRingBuff,&c) == 0)
			{
				if(CfgParserPacket(&UART_parserPacket,&UART_ProtoRecv,c) == 0)
				{
					USB_rxBuff[0]  = 0xCA;
					USB_rxBuff[1]  = ((uint8_t *)&UART_ProtoRecv.length)[0];
					USB_rxBuff[2]  = ((uint8_t *)&UART_ProtoRecv.length)[1];
					USB_rxBuff[3]  = UART_ProtoRecv.opcode;
					
					UART_ProtoRecv.dataPt = &USB_rxBuff[4];
					UART_parserPacket.state = CFG_CMD_WAITING_SATRT_CODE;
					UART_parserPacket.lenMax = UART_PACKET_SIZE;
					break;
				}
			}
			if(USB_rxBuff[0] == 0xCA)
			{
					USB_txBuff[1] = 0;
					USB_txBuff[2] = 0;
					USB_txBuff[3] = 0;
					switch(USB_rxBuff[3])
					{
						case 0x12:
							InitTimeout(&bootExitTimeout,TIME_SEC(60));
							u32Pt = (uint32_t *)&USB_rxBuff[4];
							packetNo = *u32Pt;
							if(packetNo == 0xA5A5A5A5)
							{
								firmwareFileOffSet = 0;
								u32Pt = (uint32_t *)&USB_rxBuff[8];
								firmwareFileSize = *u32Pt;
								USB_txBuff[1] = 12; //length
								USB_txBuff[3] = 0x12; //opcode
								
								USB_txBuff[4] = USB_rxBuff[4];
								USB_txBuff[5] = USB_rxBuff[5];
								USB_txBuff[6] = USB_rxBuff[6];
								USB_txBuff[7] = USB_rxBuff[7];
								//Max file size
								USB_txBuff[8] = USB_rxBuff[8];
								USB_txBuff[9] = USB_rxBuff[9];
								USB_txBuff[10] = USB_rxBuff[10];
								USB_txBuff[11] = USB_rxBuff[11];
								//Max packet size
								u16temp = UART_PACKET_SIZE - 4;
								USB_txBuff[12] = ((uint8_t *)&u16temp)[0]; //max packet size
								USB_txBuff[13] = ((uint8_t *)&u16temp)[1]; //max packet size
								USB_txBuff[14] = 0; //max packet size
								USB_txBuff[15] = 0; //max packet size
							}
							else
							{
								InitTimeout(&bootExitTimeout,TIME_SEC(10));
								if(packetNo == firmwareFileOffSet)
								{
									firmwareFileOffSet += (UART_ProtoRecv.length - 4);
									u32temp = firmwareFileOffSet % FLASH_BUF_SIZE;
									for(i = 0;i < (UART_ProtoRecv.length - 4);i++)
									{
										flashBuff[(packetNo % FLASH_BUF_SIZE) + i] = USB_rxBuff[i + 8];
									}						
									if(u32temp == 0)
									{
										u32temp = USER_FLASH_START + firmwareFileOffSet - FLASH_BUF_SIZE;
										phase = 5;
										u32Pt = (uint32_t *)flashBuff;
										__disable_irq();
										myFMC_Erase(u32temp);
										FMC_ProgramPage(u32temp,u32Pt);
										__enable_irq();
										phase = 6;
										for(i = 0;i < FLASH_BUF_SIZE; i++)
											flashBuff[i] = 0xff;
									}
									else if(firmwareFileOffSet >= firmwareFileSize)
									{
										i = (firmwareFileOffSet / FLASH_BUF_SIZE)*FLASH_BUF_SIZE;
										u32temp = USER_FLASH_START + i;
										u32Pt = (uint32_t *)flashBuff;
										__disable_irq();
										myFMC_Erase(u32temp);
										FMC_ProgramPage(u32temp,u32Pt);
										__enable_irq();
									}
								}
								packetNo = firmwareFileOffSet;
								if(firmwareFileOffSet >= firmwareFileSize)
								{						
									firmwareFileOffSet = 0;
									packetNo = 0x5A5A5A5A;
									InitTimeout(&bootExitTimeout,TIME_SEC(1));
								}
								u8pt = (uint8_t *)&packetNo;
								USB_txBuff[1] = 4; //length
								USB_txBuff[3] = 0x12; //opcode
								USB_txBuff[4] = u8pt[0];
								USB_txBuff[5] = u8pt[1];
								USB_txBuff[6] = u8pt[2];
								USB_txBuff[7] = u8pt[3];
							}
						break;
						case 0xA5:
							FLASH_Lock();
							SysDeInit();
							execute_user_code();
						break;
						case 0x88:
							bootMode = 1;
						break;
						default:
						USB_txBuff[1] = 4; //length
						USB_txBuff[3] = 0x33; //opcode
						USB_txBuff[4] = 0xA5;
						USB_txBuff[5] = 0xA5;
						USB_txBuff[6] = 0xA5;
						USB_txBuff[7] = 0xA5;
						break;
				}
				if(USB_txBuff[3])
				{
					USB_txBuff[0] = 0xCA;
					USB_txBuff[USB_txBuff[1] + 4] = CfgCalcCheckSum((uint8_t *)&USB_txBuff[4],USB_txBuff[1]);
					for(i = 0;i < USB_txBuff[1] + 4 + 1; i++)
					{
						USART3_PutChar(USB_txBuff[i]);
					}
				}
				USB_rxBuff[0] = 0;
				UART_ProtoRecv.opcode = 0;
		}
	}
	
	u32Pt = (uint32_t *)buff;
	SST25_Init();
	SST25_Read(FIRMWARE_INFO_ADDR + FIRMWARE_STATUS_OFFSET*4,buff,4);
	if(*u32Pt != 0x5A5A5A5A)	
	{
		execute_user_code();
	}
	USART3_PutString("\r\nPROGRAM NEW FIRMWARE*\r\n");
	tryNum = 10;
	exit = 0;
	while(!exit && tryNum--)
	{
		SST25_Read(FIRMWARE_INFO_ADDR + FIRMWARE_CRC_OFFSET*4,buff,4);
		fileCrc = 	*u32Pt;
		SST25_Read(FIRMWARE_INFO_ADDR + FIRMWARE_FILE_SIZE_OFFSET*4,buff,4);
		fileSize = 	*u32Pt;
		if(fileSize < FIRMWARE_MAX_SIZE)
		{
			flashCheckSum = 0;
			for(i = 0; i < fileSize;i += PAGE_SIZE)
			{
				USART3_PutChar('*');
				SST25_Read(i + FIRMWARE_BASE_ADDR,buff, PAGE_SIZE);
				for(j = 0 ; j < PAGE_SIZE;j++)
				{
					if(i + j < fileSize)
						flashCheckSum += buff[j];
					else
						break;
				}
				myFMC_Erase(USER_FLASH_START + i);
				FMC_ProgramPage(USER_FLASH_START + i,u32Pt);
				SST25_Read(i + FIRMWARE_BASE_ADDR,buff, PAGE_SIZE);
				memAddr = USER_FLASH_START + i;
				if(memcmp(buff, (void*)memAddr , PAGE_SIZE) != NULL)
					break;
			}
			if(flashCheckSum == fileCrc)
			{
					USART3_PutString("\r\nFINISHED!\r\n");
					
					SST25_Read(BASE_FIRMWARE_INFO_ADDR + FIRMWARE_STATUS_OFFSET*4,buff,4);
					if(*u32Pt != 0x5A5A5A5A)	
					{
						tryNum = 10;
						while(tryNum--)
						{
							flashCheckSum = 0;
							for(i = 0; i < fileSize;i += 256)
							{
								SST25_Read(i + FIRMWARE_BASE_ADDR,buff, 256);
								SST25_Write(i + BASE_FIRMWARE_BASE_ADDR,buff, 256);
								SST25_Read(i + BASE_FIRMWARE_BASE_ADDR,flashBuff, 256);
								if(memcmp(buff,flashBuff,256) != NULL)
								{
										break;
								}
							}
							if(i >= fileSize)
								for(i = 0; i < fileSize;i += 256)
								{
										SST25_Read(i + BASE_FIRMWARE_BASE_ADDR,buff, 256);
										for(j = 0 ; j < 256;j++)
										{
											if(i + j < fileSize)
												flashCheckSum += buff[j];
											else
												break;
										}
								}
							if(flashCheckSum == fileCrc)
							{
								SST25_Read(FIRMWARE_INFO_ADDR,buff, 256);
								SST25_Write(BASE_FIRMWARE_INFO_ADDR,buff, 256);
								SST25_Read(BASE_FIRMWARE_INFO_ADDR,flashBuff, 256);
								if(memcmp(buff,flashBuff,256) == NULL)
								{
										break;
								}
							}
						}
					}
					exit = 1;
			}
		}
 }
	if(exit == 0 && tryNum == 0xff)
	{
			USART3_PutString("\r\nFAIL!\r\n");
			USART3_PutString("\r\nDOWNLOAD BACKUP FIRMWARE*\r\n");
			tryNum = 10;
			exit = 0;
			while(!exit && tryNum--)
			{
				SST25_Read(BASE_FIRMWARE_INFO_ADDR + FIRMWARE_CRC_OFFSET*4,buff,4);
				fileCrc = 	*u32Pt;
				SST25_Read(BASE_FIRMWARE_INFO_ADDR + FIRMWARE_FILE_SIZE_OFFSET*4,buff,4);
				fileSize = 	*u32Pt;
				if(fileSize < FIRMWARE_MAX_SIZE)
				{
					flashCheckSum = 0;
					for(i = 0; i < fileSize;i += PAGE_SIZE)
					{
						USART3_PutChar('*');
						SST25_Read(i + BASE_FIRMWARE_BASE_ADDR,buff, PAGE_SIZE);
						for(j = 0 ; j < PAGE_SIZE;j++)
						{
								if(i + j < fileSize)
									flashCheckSum += buff[j];
								else
									break;
						}
						myFMC_Erase(USER_FLASH_START + i);
						FMC_ProgramPage(USER_FLASH_START + i,u32Pt);
						SST25_Read(i + BASE_FIRMWARE_BASE_ADDR,buff, PAGE_SIZE);
						memAddr = USER_FLASH_START + i;
						if(memcmp(buff, (void*)memAddr , PAGE_SIZE) != NULL)
							break;
					}
					if(flashCheckSum == fileCrc)
					{
							exit = 1;
							USART3_PutString("\r\nDOWNLOAD BACKUP FINISHED!\r\n");
					}
				}
		 }
	}	

	SST25_Erase(FIRMWARE_INFO_ADDR,block4k);
	execute_user_code();
	while(1)
	{
	}
}

