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

/**************全局中变量使用及相关定义******************/

#define Download_ApplicationAddress  0x08005000   //更新文件下载地址
#define Vector_Offset 0x0000   //中断重映射，相对于FLASH_BASE地址的偏移量（0x08000000）
#define BackupStartAddress 0x0800A000      //备份地址
#define IsUpdataAddr 0x08004800   //是否更新标志位
#define BaudRateReadAddr 0x08004802  //波特率读取地址


#define FileSize_Limit  0x5000   //更新文件大小限制 20k

/*********************内联函数之宏定义操作***************/
#define GPIO_HIGH(x,y) GPIO_SetBits(x,y)
#define GPIO_LOW(x,y)  GPIO_ResetBits(x,y)



/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
