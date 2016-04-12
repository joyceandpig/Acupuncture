/**
  ******************************************************************************
  * @file    USART/Interrupt/main.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "platform_config.h"
#include "stm32_eval.h"
#include "stdio.h"
#include "common.h"

#include "common.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32_eval.h"
#include "stm32f10x_bkp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern pFunction Jump_To_Application;
extern u32 JumpAddress;
extern u32 FlashDestination;


/******************************************************************
** 函数名: main
** 输  入: none
** 描  述: main program。主函数入口，系统运行起始点                                           
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-04-21
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
int main(int argc, char *argv[])
{   

	u32 timelimit = 4000000;//1s -200,0000
	u8 key =(*(u32 *)IsUpdataAddr);
  STM_EVAL_SYSTEMINIT();  	//初始化系统配置
	
	STM_LED3 = 0;

//	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);//等待上层数据来引导启动
//	USART_ClearFlag(USART1,USART_FLAG_RXNE);
	
	if(key == 0x01){
		STM_EVAL_COMInit(USART_PORT);        //串口配置
		FLASH_Unlock();
		FLASH_ErasePage(IsUpdataAddr);
		FLASH_Lock();
		UpDataFile();//文件更新
		key = 0;
	}
	
 
//	while (timelimit-- > 0){
//    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
//			{  //等待信号量   
//			STM_LED4 = 0;
//				UpDataFile();//文件更新
//			
//  	}
//	}
	RunApp();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
