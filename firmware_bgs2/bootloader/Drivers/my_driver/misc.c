/* Includes ------------------------------------------------------------------*/
#include "misc.h"

#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)

/**
  * @brief  Configures the priority grouping: pre-emption priority and subpriority.
  * @param  NVIC_PriorityGroup: specifies the priority grouping bits length. 
  *   This parameter can be one of the following values:
  *     @arg NVIC_PriorityGroup_0: 0 bits for pre-emption priority
  *                                4 bits for subpriority
  *     @arg NVIC_PriorityGroup_1: 1 bits for pre-emption priority
  *                                3 bits for subpriority
  *     @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority
  *                                2 bits for subpriority
  *     @arg NVIC_PriorityGroup_3: 3 bits for pre-emption priority
  *                                1 bits for subpriority
  *     @arg NVIC_PriorityGroup_4: 4 bits for pre-emption priority
  *                                0 bits for subpriority
  * @retval None
  */
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
  /* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
  SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup;
}

void NVIC_PriorityInit(uint32_t NVIC_IRQChannel,uint32_t NVIC_IRQChannelSubPriority,uint32_t NVIC_IRQChannelPreemptionPriority)
{
  uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
    /* Compute the Corresponding IRQ Priority --------------------------------*/    
    tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;
    tmppriority = NVIC_IRQChannelPreemptionPriority << tmppre;
    tmppriority |=  NVIC_IRQChannelSubPriority & tmpsub;
    tmppriority = tmppriority << 0x04; 
    NVIC->IP[NVIC_IRQChannel] = tmppriority;
}

void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{ 
  SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}

void NVIC_SystemLPConfig(uint8_t LowPowerMode, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    SCB->SCR |= LowPowerMode;
  }
  else
  {
    SCB->SCR &= (uint32_t)(~(uint32_t)LowPowerMode);
  }
}
