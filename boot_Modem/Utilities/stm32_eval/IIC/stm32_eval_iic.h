#ifndef __STM32_EVAL_IIC_H
#define __STM32_EVAL_IIC_H

#ifdef __cplusplus
 extern "C" { 
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup Utilities
  * @{
  */
  
/*=====================================================================================*/
#define IIC_SCL_PORT              GPIOB
#define IIC_SCL_CLK               RCC_APB2Periph_GPIOB 
#define IIC_SCL_PIN               GPIO_Pin_6

#define IIC_SDA_PORT              GPIOB
#define IIC_SDA_CLK               RCC_APB2Periph_GPIOB  
#define IIC_SDA_PIN               GPIO_Pin_7
 	   		   
//IO方向设置
#define SDA_IN()  {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x40000000;}
#define SDA_OUT() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=0x10000000;}


//IO操作函数	 
//#define IIC_SCL    PB6 //SCL
#define Set_IIC_SCL  {GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN);}
#define Clr_IIC_SCL  {GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN);} 
//#define IIC_SDA    PB7 //SDA
#define Set_IIC_SDA  {GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN);}
#define Clr_IIC_SDA  {GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN);} 
//#define READ_SDA   PB7 //输入SDA 
#define READ_SDA    (GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA_PIN))
	 
/*=====================================================================================*/



#define sEE_FLAG_TIMEOUT         ((uint32_t)0x10000)
#define sEE_LONG_TIMEOUT         ((uint32_t)(10 * sEE_FLAG_TIMEOUT))

#define BLOCK_WRITE               0xA0        /*块写命令*/
#define BLOCK_READ                0xA1        /*块读取*/
#define ADDR_PTR                  0xB0        /*地址指针*/

#define sEE_OK                    0

void sEE_Init(void);
uint32_t sEE_TIMEOUT_UserCallback(void);
//void sEE_EnterCriticalSection_UserCallback(void);
//void sEE_ExitCriticalSection_UserCallback(void);
  
void sEE_Init(void);
uint32_t sEE_WriteData(uint16_t WriteAddr,uint16_t Data );
//uint32_t sEE_WriteBlock(uint16_t *pBuffer ,uint16_t WriteAddr,uint16_t* NumByteToWrite);
uint32_t sEE_ReadData(uint16_t ReadAddr,uint16_t *pBuffer );
//uint32_t sEE_ReadBlock(uint16_t *pBuffer ,uint16_t ReadAddr,uint16_t* NumByteToRead);
#ifdef __cplusplus
}
#endif

#endif

/**
  * @}
  */  

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

