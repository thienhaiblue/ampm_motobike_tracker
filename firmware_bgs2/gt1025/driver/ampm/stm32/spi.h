#ifndef __SPI_H__
#define __SPI_H__

#include "stm32f10x.h"


#define SPI_TIMEOUT 100000

//----------------------------------------------------------------------------------
//   Common Macros
//----------------------------------------------------------------------------------
////#define st(x)      do { x } while (__LINE__ == -1)
//#define HAL_SPI2_CS_PORT			GPIOB
//#define HAL_SPI2_CS_PIN				GPIO_BSRR_BS12
//#define HAL_SPI2_SOMI_PORT		GPIOB
//#define HAL_SPI2_SOMI_PIN			GPIO_IDR_IDR14
//#define HAL_SPI2_CS_DEASSERT  HAL_SPI2_CS_PORT->BSRR |= HAL_SPI2_CS_PIN
//#define HAL_SPI2_CS_ASSERT    HAL_SPI2_CS_PORT->BRR |= HAL_SPI2_CS_PIN
//#define HAL_SPI2_SOMI_VAL     (HAL_SPI2_SOMI_PORT->IDR & HAL_SPI2_SOMI_PIN)
//#define HAL_SPI2_BEGIN(x)     HAL_SPI2_CS_ASSERT; while(HAL_SPI2_SOMI_VAL){if(x-- == 0) return 0xff;} 
//#define HAL_SPI2_END(x)       HAL_SPI2_CS_DEASSERT;        



#define HAL_SPI1_CS_PORT			GPIOA
#define HAL_SPI1_CS_PIN				GPIO_BSRR_BS4
#define HAL_SPI1_CS_DEASSERT  HAL_SPI1_CS_PORT->BSRR |= HAL_SPI1_CS_PIN
#define HAL_SPI1_CS_ASSERT    HAL_SPI1_CS_PORT->BRR |= HAL_SPI1_CS_PIN;HAL_SPI1_ACCEL_CS_DEASSERT
#define HAL_SPI1_CS_SET_OUTPUT	HAL_SPI1_CS_PORT->CRL	&= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4); HAL_SPI1_CS_PORT->CRL	|= (GPIO_CRL_MODE4_0 | GPIO_CRL_MODE4_1)
#define HAL_SPI1_CS_SET_INPUT		HAL_SPI1_CS_PORT->CRL	&= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4)

#define HAL_SPI1_ACCEL_CS_PORT			GPIOC
#define HAL_SPI1_ACCEL_CS_PIN				GPIO_BSRR_BS13
#define HAL_SPI1_ACCEL_CS_DEASSERT  HAL_SPI1_ACCEL_CS_PORT->BSRR |= HAL_SPI1_ACCEL_CS_PIN
#define HAL_SPI1_ACCEL_CS_ASSERT    HAL_SPI1_ACCEL_CS_PORT->BRR |= HAL_SPI1_ACCEL_CS_PIN;HAL_SPI1_CS_DEASSERT
#define HAL_SPI1_ACCEL_CS_SET_OUTPUT	HAL_SPI1_ACCEL_CS_PORT->CRH	&= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); HAL_SPI1_ACCEL_CS_PORT->CRH	|= (GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1)
#define HAL_SPI1_ACCEL_CS_SET_INPUT		HAL_SPI1_ACCEL_CS_PORT->CRH	&= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13)

/** @defgroup SPI_Private_Defines
  * @{
  */

/* SPI SPE mask */
#define CR1_SPE_Set          ((uint16_t)0x0040)
#define CR1_SPE_Reset        ((uint16_t)0xFFBF)

/* I2S I2SE mask */
#define I2SCFGR_I2SE_Set     ((uint16_t)0x0400)
#define I2SCFGR_I2SE_Reset   ((uint16_t)0xFBFF)

/* SPI CRCNext mask */
#define CR1_CRCNext_Set      ((uint16_t)0x1000)

/* SPI CRCEN mask */
#define CR1_CRCEN_Set        ((uint16_t)0x2000)
#define CR1_CRCEN_Reset      ((uint16_t)0xDFFF)

/* SPI SSOE mask */
#define CR2_SSOE_Set         ((uint16_t)0x0004)
#define CR2_SSOE_Reset       ((uint16_t)0xFFFB)

/* SPI registers Masks */
#define SPI_CR1_CLEAR_Mask       ((uint16_t)0x3040)
#define I2SCFGR_CLEAR_Mask   ((uint16_t)0xF040)

/* SPI or I2S mode selection masks */
#define SPI_Mode_Select      ((uint16_t)0xF7FF)
#define I2S_Mode_Select      ((uint16_t)0x0800) 

/**
  * @}
  */
	
	
uint8_t halSpiWriteByte(SPI_TypeDef* SPIx,uint8_t data);
void halSpiWriteMutilByte(const unsigned char* buf,uint32_t len);
void halSpiReadMutilByte(unsigned char* buf,uint32_t len);

void SPI_InitHighFreq(uint8_t channel);

void SPI_InitLowFreq(uint8_t channel);

void SPI_DeInit(void);

#endif

