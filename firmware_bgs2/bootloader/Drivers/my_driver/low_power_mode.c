#include "low_power_mode.h"

void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
  uint32_t tmpreg = 0;
  /* Select the regulator state in STOP mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDS bits */
  tmpreg &= CR_DS_Mask;
  /* Set LPDS bit according to PWR_Regulator value */
  tmpreg |= PWR_Regulator;
  /* Store the new value */
  PWR->CR = tmpreg;
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  *(__IO uint32_t *) SCB_SysCtrl |= SysCtrl_SLEEPDEEP_Set;
  
  /* Select STOP mode entry --------------------------------------------------*/
  if(PWR_STOPEntry == PWR_STOPEntry_WFI)
  {   
    /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
    /* Request Wait For Event */
    __WFE();
  }
}

/**
  * @brief  Enters STANDBY mode.
  * @param  None
  * @retval : None
  */
void PWR_EnterSTANDBYMode(void)
{
  /* Clear Wake-up flag */
  PWR->CR |= CR_CWUF_Set;
  /* Select STANDBY mode */
  PWR->CR |= CR_PDDS_Set;
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  *(__IO uint32_t *) SCB_SysCtrl |= SysCtrl_SLEEPDEEP_Set;
/* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM   )
  __force_stores();
#endif
  /* Request Wait For Interrupt */
  __WFI();
}



