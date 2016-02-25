/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes 
------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "platform_config.h"
#include "Calculate.h"
#include "stm32f10x_tim.h"
#include "stm32_eval.h"

extern void ChannelLed(void);

static uint8_t ch_count = 0;  
__IO uint32_t TIMx_timecount = 0,
				 TIMy_timecount = 0,
				 TIMz_timecount = 0;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/



/******************************************************************
** 函数名: TIMy_IRQHandler
** 输  入: none
** 描  述: 该定时器用作检测和LED控制计时使用                                            
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-04-21
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
void TIMx_IRQHandler(void)
{
	/******作为计时使用，为波形变换时间提供依据,0.1ms进入一次*****/
 	if (TIM_GetITStatus(TIMx,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIMx, TIM_FLAG_Update);
		TIMx_timecount++;
		if (TIMx_timecount > Time5sCount)		          //定时一个5s时间
		{
			TIMx_timecount = 0;                    
			for (ch_count = 0; ch_count < 6; ch_count++)//判定通道是否开启复杂波形，如果是，则对相应通道进行计数
			{
				if(CH[ch_count].complex_status)
					CH[ch_count].time5s_count++;     
			}   
		}
		for (ch_count = 0; ch_count < 6; ch_count++)//如果通道开启，则对通道的基本波形时间计数
		{
			if(CH[ch_count].channel_open)
				CH[ch_count].wave_count++;
		}
		WaveWork();//产生波形的函数
	}
}
/******************************************************************
** 函数名: TIMy_IRQHandler
** 输  入: none
** 描  述: 该定时器用作检测和LED控制计时使用                                            
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-04-21
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
void TIMy_IRQHandler(void)
{
	/******作为计时使用，并为定时状态报告和心跳检测提供时间,0.1ms进入一次*****/
 	if (TIM_GetITStatus(TIMy,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIMy, TIM_FLAG_Update);
    TIMy_timecount++;
		if (TIMy_timecount > Time5sCount)		 //定时一个5s时间
		{
      TIMy_timecount = 0; 	
				
			heart_time_count++;
      if(heart_time_count > MAX_HEART_TIME) //心跳检测最大等待时间
      {
        heart_time_count = 0;
				if (system_start)
				{
					topdeath = TRUE;
				}
      }
		}		
	if (RepPar.time_start) //命令重复次数检测，规定时间内重复次数超过设定数值,则认为重复超限，急停设备
	{
		 RepPar.time_count++;
		 if (RepPar.time_count > RepTimeLit){ //到达重复命令预定时间
		 	RepPar.time_limit = TRUE;
			RepPar.time_count = 0;
			RepPar.time_start = FALSE;
		 }
	}
	if (ErrPar.time_start) //命令错误次数检测，规定时间内错误次数超过设定数值,则认为错误超限，急停设备
	{
		 ErrPar.time_count++;

		 if (ErrPar.time_count > ErrTimeLit){ //到达错误命令预定时间
		 	ErrPar.time_limit = TRUE;
			ErrPar.time_count = 0;
			ErrPar.time_start = FALSE;
		 }	
	}
  if (ImpPar.time_start)  //阻抗采集限制时间，超过一定时间没有得到数据返回，则跳过该次采集
  {
    ImpPar.time_count++;
    if (ImpPar.time_count > ImpValTimeLit){
      ImpPar.time_limit = TRUE;
      ImpPar.time_count = 0;
      ImpPar.time_start = FALSE;
    }
  }
	led_time_count++;				//指示灯计时器	
	}
}

/******************************************************************
** 函数名: TIMz_IRQHandler
** 输  入: none
** 描  述: 使用定时器4作为串口2的接收计时，每接收到一byte数据，就开启定时器3，
**         如果其计数值超过4个单元计数，则认为已经接收到一次完整数据 ，则否就置零
**         重新计时
**                                                                  
** 全局变量: 串口2完整接收数据标志位zigbeeusart_getwholedat
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-31
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
void TIMz_IRQHandler(void)
{
	/******作为计时使用，0.1ms进入一次*****/
  if (TIM_GetITStatus(TIMz,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIMz, TIM_FLAG_Update);
#if(usart_baud == Baud_115200)
    if ((TIMz_timecount++) > us100(4))//4 old is :us100(4)  ms(30)
#else
		if ((TIMz_timecount++) > ms(30))
#endif
    {
        TIM_Cmd(TIMz,DISABLE);
        TIMz_timecount = 0;
				get_whole_data = 1;
		}			
	}
}
/******************************************************************
** 函数名: TIMy_IRQHandler
** 输  入: none
** 描  述: 该定时器用作检测和LED控制计时使用                                            
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-04-21
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
void USARTx_IRQHandler(void)
{	
	if (USART_GetITStatus(USARTx,USART_IT_RXNE) != RESET)
	{
		EnQueue(&Queue,USART_ReceiveData(USARTx));           	/*  把串口接收的数据放入环形缓冲区   */
		USART_ClearITPendingBit(USARTx,USART_IT_RXNE); 
		TIM_Cmd(TIMz,DISABLE);
		TIM_SetCounter(TIMz,0);
		TIM_Cmd(TIMz,ENABLE);
	}	
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
