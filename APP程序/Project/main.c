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
#include "Calculate.h"
#include "AD5933.h"
#include "configure_init.h"
#include "stm32_eval.h"
#include "stm32_eval_wave.h"
#include "stdio.h"

/******************************************************************
** ������: InitOutput
** ��  ��: none
** ��  ��: ���豸���������ָ���Ĳ���                                         
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
void InitOutput(void)
{
	uint8_t i;
  for (i = 0; i < 6; i++)
  {
    CH[i].channel_open = 1;
    CH[i].function_code = 0x101;                             
    CH[i].peroid = 200;//peroid_max:4999 peroid_min:100
    CH[i].ss_peroid = 5 * (CH[i].peroid);
    CH[i].scope = 890;//scope_max:890 scope_min:0
    CH[i].complex_status = 0;
		CH[i].led_sta = 1;
  }
}
void BellOnceTime(void)
{
	STM_BELL_ON;
	delay_ms(50);
	STM_BELL_OFF;
}
/******************************************************************
** ������: main
** ��  ��: none
** ��  ��: main program����������ڣ�ϵͳ������ʼ��                                           
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
int main(int argc, char *argv[])
{ 
//	RCC_ClocksTypeDef  RCC_Clocks; 
  STM_EVAL_SYSTEMINIT();  	//��ʼ��ϵͳ����

	BellOnceTime();   //����������
//	InitOutput(); //�������������
	
//	RCC_GetClocksFreq(&RCC_Clocks);
//	printf("\nHCLK___Frequency = %dMhz,\n\
//PCLK1__Frequency = %dMhz,\n\
//PCLK2__Frequency = %dMhz,\n\
//SYSCLK_Frequency = %dMhz\n",\
//					RCC_Clocks.HCLK_Frequency/1000000,	\
//					RCC_Clocks.PCLK1_Frequency /1000000,	\
//					RCC_Clocks.PCLK2_Frequency/1000000,	\
//					RCC_Clocks.SYSCLK_Frequency/1000000	\
//	);
//	do{
//	
//	}while();
  while (1)
  {  
  	HandleRecData();			//������ն�������		
    ReportState(); 				//�������
    ErrorCodeLimit();			//��������
    RepeatCodeLimit(); 		//�ظ�����
  }       
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
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
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
