#include "adc.h"

#define ADC_CHANNEL	9
#define SAMPLE_TIME ADC_SampleTime_28Cycles5
#define RANK 1

void ADC_Init(void)
{
	uint32_t temp;
	ADC1->CR1 &= CR1_CLEAR_Mask;
	ADC1->CR1 |= ADC_Mode_Independent;
	
	ADC1->CR2 &= CR2_CLEAR_Mask;
	
	ADC1->CR2 |= ADC_DataAlign_Right 
						| ADC_ExternalTrigConv_None 
						| (1 << 1); /*Continous conv mode*/
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


uint32_t ADC_ReadChannel(uint8_t channel)
{
 return 0;
}

