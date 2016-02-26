#ifndef __STM32_EVAL_H
#define __STM32_EVAL_H

#ifdef __cplusplus
 extern "C" { 
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "platform_config.h"
#include "stm32_delay.h"
/****************************************************/

/** @defgroup STM32_EVAL_Exported_Types
  * @{
  */
	 
	 
#define usart2
	   
#ifdef usart1
	#define USART_PORT COM1
	#define USARTx USART1
#else 
	#define USART_PORT COM2
	#define USARTx USART2
#endif

typedef enum 
{
  COM1 = 0,
  COM2 = 1
}COM_TypeDef;
typedef enum
{
  Tim2 = 0,
  Tim3 = 1,
  Tim4 = 2
}TypeDef_TIM;
typedef enum
{
  Baud_9600 = 9600,
  Baud_115200 = 115200
}BaudRate_TypeDef;

typedef struct
{ 
 uint8_t front, 
         rear;
 uint8_t size,
         items[1024];
}QueueTypeDef;

#define IsUsartBaudRight(baud) (baud == 9600 || \
																baud == 38400 || \
																baud == 57600 || \
																baud == 115200)
/** @λ������,ʵ��51���Ƶ�GPIO���ƹ���
  * @{
  */

//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

#define STM_BELL_ON (PCout(4) = 1)//PC4
#define STM_BELL_OFF (PCout(4) = 0)//PC4

#define STM_LED1 PBout(4)//PC0
#define STM_LED2 PBout(5)//PC1	
#define STM_LED3 PBout(8)//PC2	
#define STM_LED4 PBout(9)//PC3

/**
  * @}
  */
	 

/**
  * @}
  */

#define COMn                             2
#define NUMn                             3

#define us100(n) (n)
#define ms(n) (us100(n)*10)
#define s(n) (ms(n)*1000)

#define EVAL_TIM2                        TIM2
#define EVAL_TIM2_IRQn                   TIM2_IRQn
#define EVAL_TIM2_CLK                    RCC_APB1Periph_TIM2

#define EVAL_TIM3                        TIM3
#define EVAL_TIM3_IRQn                   TIM3_IRQn
#define EVAL_TIM3_CLK                    RCC_APB1Periph_TIM3

#define EVAL_TIM4                        TIM4
#define EVAL_TIM4_IRQn                   TIM4_IRQn
#define EVAL_TIM4_CLK                    RCC_APB1Periph_TIM4





/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_TX_PIN                 GPIO_Pin_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_RX_PIN                 GPIO_Pin_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2
 */ 
#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_TX_PIN                 GPIO_Pin_2
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_RX_PIN                 GPIO_Pin_3
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_IRQn                   USART2_IRQn

/**
  * @}
  */ 

/**
  * @}
  */

static void USART_PARA_CONFIGURE(u32 baudrate, uint16_t COM);//BaudRate_TypeDef
static void USART_NVIC_CONFIGURE(uint16_t COM);
static void USART_GPIO_CONFIGURE(COM_TypeDef COM);
extern void STM_EVAL_COMInit(uint8_t com);
static void TIM_NVIC_CONFIGURE(TypeDef_TIM TIM_X);
static void TIM_PARA_CONFIGURE(TypeDef_TIM  TIM_X, uint16_t timecount);
static void STM_EVAL_TIMCONFIGURE(uint16_t TimX,uint8_t time);
static void STM_EVAL_TIMInit(void);
static void STM_EVAL_STMWORK_LED(void);
static void STM_EVAL_GPIOInit(void);
static void STM_EVAL_DELAYInit(void);
extern int  fputc(int ch, FILE *f);
extern void STM_EVAL_SYSTEMINIT(void);
#ifdef __cplusplus
}
#endif

#endif

/**
  * @}
  */  

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/




