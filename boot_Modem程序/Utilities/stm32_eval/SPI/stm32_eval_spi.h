#ifndef __STM32_EVAL_SPI_H
#define __STM32_EVAL_SPI_H

#ifdef __cplusplus
 extern "C" { 
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup Utilities
  * @{
  */


 
/*=====================================================================================*/
//3线SPI 端口定义 SCLK，SDA，SS(S\Y\N\C\)
//SCLK
#define SPI_SCLK_PORT             GPIOA
#define SPI_SCLK_CLK              RCC_APB2Periph_GPIOA 
#define SPI_SCLK_PIN              GPIO_Pin_5

//SDA
#define SPI_SDA_PORT              GPIOA
#define SPI_SDA_CLK               RCC_APB2Periph_GPIOA  
#define SPI_SDA_PIN               GPIO_Pin_7

//SS(S\Y\N\C\)
#define SPI_SS_PORT              GPIOA
#define SPI_SS_CLK               RCC_APB2Periph_GPIOA  
#define SPI_SS_PIN               GPIO_Pin_11

//(R\E\S\E\T\)
#define SPI_RST_PORT             GPIOA
#define SPI_RST_CLK              RCC_APB2Periph_GPIOA  
#define SPI_RST_PIN              GPIO_Pin_12



//IO操作函数	 
#define Set_SPI_SCLK  {GPIO_SetBits  ( SPI_SCLK_PORT , SPI_SCLK_PIN);}
#define Clr_SPI_SCLK  {GPIO_ResetBits( SPI_SCLK_PORT , SPI_SCLK_PIN);} 

#define Set_SPI_SDA   {GPIO_SetBits  ( SPI_SDA_PORT  , SPI_SDA_PIN );}
#define Clr_SPI_SDA   {GPIO_ResetBits( SPI_SDA_PORT  , SPI_SDA_PIN );} 

#define Set_SPI_SS    {GPIO_SetBits  ( SPI_SS_PORT   , SPI_SS_PIN  );}
#define Clr_SPI_SS    {GPIO_ResetBits( SPI_SS_PORT   , SPI_SS_PIN  );} 	 

#define Set_SPI_RST    {GPIO_SetBits  ( SPI_RST_PORT   , SPI_RST_PIN  );}
#define Clr_SPI_RST    {GPIO_ResetBits( SPI_RST_PORT   , SPI_RST_PIN  );} 	 
/*=====================================================================================*/

void ADG714_Init(void);
void ADG714_WriteData( uint16_t Data );


#ifdef __cplusplus
}
#endif

#endif

/**
  * @}
  */  

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/


                                                                