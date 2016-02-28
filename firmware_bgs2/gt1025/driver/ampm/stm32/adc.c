#include "adc.h"
#include "hw_config.h"
#define ADC_CHANNEL	8
#define SAMPLE_TIME ADC_SampleTime_28Cycles5
#define RANK 1

__IO uint16_t ADC1ConvertedValue = 0;
uint32_t adcChannel8Value = 0;
uint32_t adc8Sum = 0;
uint16_t adc8SampleCnt = 0;


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (SingleChannelContinuous_src), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles ADC1 and ADC2 global interrupts requests.
  * @param  None
  * @retval None
  */
void ADC1_2_IRQHandler(void)
{
  uint8_t itmask = 0;
	uint32_t i;
	uint32_t temp;
  /* Get the ADC IT index */
  itmask = (uint8_t)(ADC_IT_EOC >> 8);
  /* Clear the selected ADC interrupt pending bits */
  ADC1->SR = ~(uint32_t)itmask;
  /* Get converted value of Channel14 converted by ADC1 */
  ADC1ConvertedValue = ADC1->DR;

	adc8Sum += ADC1ConvertedValue;
	adc8SampleCnt++;
	if(adc8SampleCnt >= ADC_SAMPLE_TIMES)
	{
		adc8SampleCnt = 0;
		adcChannel8Value = (adc8Sum / ADC_SAMPLE_TIMES);
		adc8Sum = 0;
	}
}

void StartADC(void)
{
	ADC1->CR2 |= CR2_EXTTRIG_SWSTART_Set;
}

void ADC_Init(void)
{
	uint32_t temp;
	uint8_t u8t;
	
	NVIC_SetPriority(ADC1_2_IRQn, ((0x01<<3)| ADC_PRIORITY));
	NVIC_EnableIRQ(ADC1_2_IRQn);
	
	ADC1->CR1 &= CR1_CLEAR_Mask;
	ADC1->CR1 |= ADC_Mode_Independent;
	/* Enable the selected ADC interrupts */
	u8t = ADC_IT_EOC;
  ADC1->CR1 |= u8t;
	
	ADC1->CR2 &= CR2_CLEAR_Mask;
	
	ADC1->CR2 |= ADC_DataAlign_Right 
						| ADC_ExternalTrigConv_None;
						//| (1 << 1); /*Continous conv mode*/
	
	ADC1->SQR1 = 0;//1 channel
	/* Clear the old channel sample time */
	temp = ADC1->SMPR2;
	temp &= ~(SMPR2_SMP_Set << (3 * ADC_CHANNEL));
	/* Calculate the mask to set */
	 /* Set the new channel sample time */
	temp |= (uint32_t)SAMPLE_TIME << (3 * ADC_CHANNEL);
	ADC1->SMPR2 = temp;
	 /* For Rank 1 to 6 */
	temp =  ADC1->SQR3;
	temp &= ~(SQR3_SQ_Set << (5 * (RANK - 1)));
	temp |=  (uint32_t)ADC_CHANNEL << (5 * (RANK - 1));
	ADC1->SQR3 = temp;
	/*Enable ADC1*/
	ADC1->CR2 |= CR2_ADON_Set;
	/* Enable ADC1 reset calibaration register */   
	ADC1->CR2 |= CR2_RSTCAL_Set;
	 /* Check the end of ADC1 reset calibration register */
	while(((ADC1->CR2 & CR2_RSTCAL_Set) != (uint32_t)RESET));
	 /* Start ADC1 calibaration */
	ADC1->CR2 |= CR2_CAL_Set;
	/* Check the end of ADC1 calibration */
	while(((ADC1->CR2 & CR2_CAL_Set) != (uint32_t)RESET));
	/* Start ADC1 Software Conversion */ 
	 ADC1->CR2 |= CR2_EXTTRIG_SWSTART_Set;
	 
	
}


