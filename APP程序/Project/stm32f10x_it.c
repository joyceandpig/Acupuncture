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
** ������: TIMy_IRQHandler
** ��  ��: none
** ��  ��: �ö�ʱ����������LED���Ƽ�ʱʹ��                                            
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
void TIMx_IRQHandler(void)
{
	/******��Ϊ��ʱʹ�ã�Ϊ���α任ʱ���ṩ����,0.1ms����һ��*****/
 	if (TIM_GetITStatus(TIMx,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIMx, TIM_FLAG_Update);
		TIMx_timecount++;
		if (TIMx_timecount > Time5sCount)		          //��ʱһ��5sʱ��
		{
			TIMx_timecount = 0;                    
			for (ch_count = 0; ch_count < 6; ch_count++)//�ж�ͨ���Ƿ������Ӳ��Σ�����ǣ������Ӧͨ�����м���
			{
				if(CH[ch_count].complex_status)
					CH[ch_count].time5s_count++;     
			}   
		}
		for (ch_count = 0; ch_count < 6; ch_count++)//���ͨ�����������ͨ���Ļ�������ʱ�����
		{
			if(CH[ch_count].channel_open)
				CH[ch_count].wave_count++;
		}
		WaveWork();//�������εĺ���
	}
}
/******************************************************************
** ������: TIMy_IRQHandler
** ��  ��: none
** ��  ��: �ö�ʱ����������LED���Ƽ�ʱʹ��                                            
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
void TIMy_IRQHandler(void)
{
	/******��Ϊ��ʱʹ�ã���Ϊ��ʱ״̬�������������ṩʱ��,0.1ms����һ��*****/
 	if (TIM_GetITStatus(TIMy,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIMy, TIM_FLAG_Update);
    TIMy_timecount++;
		if (TIMy_timecount > Time5sCount)		 //��ʱһ��5sʱ��
		{
      TIMy_timecount = 0; 	
				
			heart_time_count++;
      if(heart_time_count > MAX_HEART_TIME) //����������ȴ�ʱ��
      {
        heart_time_count = 0;
				if (system_start)
				{
					topdeath = TRUE;
				}
      }
		}		
	if (RepPar.time_start) //�����ظ�������⣬�涨ʱ�����ظ����������趨��ֵ,����Ϊ�ظ����ޣ���ͣ�豸
	{
		 RepPar.time_count++;
		 if (RepPar.time_count > RepTimeLit){ //�����ظ�����Ԥ��ʱ��
		 	RepPar.time_limit = TRUE;
			RepPar.time_count = 0;
			RepPar.time_start = FALSE;
		 }
	}
	if (ErrPar.time_start) //������������⣬�涨ʱ���ڴ�����������趨��ֵ,����Ϊ�����ޣ���ͣ�豸
	{
		 ErrPar.time_count++;

		 if (ErrPar.time_count > ErrTimeLit){ //�����������Ԥ��ʱ��
		 	ErrPar.time_limit = TRUE;
			ErrPar.time_count = 0;
			ErrPar.time_start = FALSE;
		 }	
	}
  if (ImpPar.time_start)  //�迹�ɼ�����ʱ�䣬����һ��ʱ��û�еõ����ݷ��أ��������ôβɼ�
  {
    ImpPar.time_count++;
    if (ImpPar.time_count > ImpValTimeLit){
      ImpPar.time_limit = TRUE;
      ImpPar.time_count = 0;
      ImpPar.time_start = FALSE;
    }
  }
	led_time_count++;				//ָʾ�Ƽ�ʱ��	
	}
}

/******************************************************************
** ������: TIMz_IRQHandler
** ��  ��: none
** ��  ��: ʹ�ö�ʱ��4��Ϊ����2�Ľ��ռ�ʱ��ÿ���յ�һbyte���ݣ��Ϳ�����ʱ��3��
**         ��������ֵ����4����Ԫ����������Ϊ�Ѿ����յ�һ���������� ����������
**         ���¼�ʱ
**                                                                  
** ȫ�ֱ���: ����2�����������ݱ�־λzigbeeusart_getwholedat
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-31
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
void TIMz_IRQHandler(void)
{
	/******��Ϊ��ʱʹ�ã�0.1ms����һ��*****/
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
** ������: TIMy_IRQHandler
** ��  ��: none
** ��  ��: �ö�ʱ����������LED���Ƽ�ʱʹ��                                            
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
void USARTx_IRQHandler(void)
{	
	if (USART_GetITStatus(USARTx,USART_IT_RXNE) != RESET)
	{
		EnQueue(&Queue,USART_ReceiveData(USARTx));           	/*  �Ѵ��ڽ��յ����ݷ��뻷�λ�����   */
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
