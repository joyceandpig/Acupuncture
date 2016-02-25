/**
  ******************************************************************************
  * @file    User/platform_config.h 
  * @author  Chaosong.zhang
  * @version V2.0
  * @date    08/20/2014
  * @brief   To Define public paramer
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32_eval.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL) &&  !defined (USE_STM3210C_EVAL) &&  !defined (USE_STM32100B_EVAL)
 #define USE_STM3210E_EVAL
#endif

typedef enum {FALSE = 0, TRUE = !FALSE} bool;

/**************ȫ���б���ʹ�ü���ض���******************/

#define Download_ApplicationAddress  0x08005000   //�����ļ����ص�ַ
#define Vector_Offset 0x0000   //�ж���ӳ�䣬�����FLASH_BASE��ַ��ƫ������0x08000000��
#define BackupStartAddress 0x0800A000      //���ݵ�ַ
#define IsUpdataAddr 0x08004800   //�Ƿ���±�־λ
#define BaudRateReadAddr 0x08004802  //�����ʶ�ȡ��ַ


#define FileSize_Limit  0x5000   //�����ļ���С���� 20k

/*********************��������֮�궨�����***************/
#define GPIO_HIGH(x,y) GPIO_SetBits(x,y)
#define GPIO_LOW(x,y)  GPIO_ResetBits(x,y)



/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
