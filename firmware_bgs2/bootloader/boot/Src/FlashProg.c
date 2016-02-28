
/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "flashprog.h"
#include "hw_config.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/



void myFMC_Erase(uint32_t u32addr)
{
	uint16_t  FlashStatus;
	FlashStatus = FLASH_ErasePage(u32addr);
	if(FLASH_COMPLETE != FlashStatus) 
	{
		NVIC_SystemReset();
	}
}

void FMC_ProgramPage(uint32_t u32startAddr, uint32_t * u32buff)
{
		uint16_t  FlashStatus;
    uint32_t i;
    for (i = 0; i < PAGE_SIZE/4; i++)
    {
			FlashStatus = FLASH_ProgramWord(u32startAddr + i*4, u32buff[i]);
			if(FLASH_COMPLETE != FlashStatus) 
			{
				NVIC_SystemReset();
			}
			if(u32buff[i] != *(uint32_t*)(u32startAddr + i*4)) //check wrote data
			{
				NVIC_SystemReset();
			}
    }    
}


