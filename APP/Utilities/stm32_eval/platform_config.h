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
/* Exported con line corresponding to the STMicroelectronics evaluation board
   used to run the example */
#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL) &&  !defined (USE_STM3210C_EVAL) &&  !defined (USE_STM32100B_EVAL)
 #define USE_STM3210E_EVAL
#endif

typedef enum {FALSE = 0, TRUE = !FALSE,Failture = 0,Success = !Failture} bool,Result_Judge;
typedef enum{NoError,Error}Result_Error;
/**************ȫ���б���ʹ�ü���ض���******************/
extern u32 usart_baud;

#define usart2
//#define usart_baud 115200
#define SYSCLOCK_32M //sysclk is 72Mhz or 32Mhz


#define WaveHighTime     		2        //һ�����������ڣ��ߵ�ƽά�ֵ�ʱ�� ��λ 0.1ms/�� 0.05ms
#define CAPACITY      			200 	       //���廷�λ�������С

#define JumpBootAddress 		0x08000000    //��ת��ַ����ת��boot����
#define Vector_Offset 			0x5000   //�ж���ӳ�䣬�����FLASH_BASE��ַ��ƫ������0x08000000��
														 						 //ָʾ���򱣴�λ��
#define IsUpdata 						0x08004800 //�Ƿ���±�־
#define BaudRateReadAddr    0x08004802



/****************Calculate��������ض��弰˵��**********************/

#define ChannelOffset 					12               	  	//DAоƬͨ����ַƫ����
#define ERRORCODE_UP_LIMIT  		12              //��������������ޣ�ÿ��֡�ڶ�����
#define ErrTimeLit 							5000            //���������ж��޶�ʱ��  ��λ��0.1ms/��
#define ORDER_REPEAT_UP_LIMIT		10	  		  //���������ظ���������
#define RepTimeLit 							5000     		//�ظ������ж��޶�ʱ�䣬��λ��0.1ms/��
#define ImpValTimeLit 					10000     //�迹�ɼ��޶�ʱ�䣬��λ��0.1ms/��
#define BaudRate 						115200 			//���ô��ڲ�����

#define CH_SUM  								6	                //ͨ������
#define DATA_SUM 								6	                //�������ݵĸ���
#define DISDATA_SUM 						8	            //�ǲ�������λ�ĸ���
#define DATA_STA 								6	            //����������ʼλ
#define FRAME_SUM 							14                //����֡�������ܸ���

#define Peroid_2hz							4990             	    //2hzʱ��Ӧ������ֵ
#define Peroid_10hz 						1000              	    //10hzʱ��Ӧ������ֵ
#define PEROID_COMPUTE_VALUE 		10000    	    //1/f (s) = 1000/f (ms) -> 10*1000/f (ms)���ڶ�ʱ����0.1msһ�Σ���Ҫ����10�βŵ�1ms
#define CLEARVALUE 							0	              	    //����ֵ����Ҫ���һЩ����������ѭ�����

#define WAVE_PEROID_MULTIPLE 		5              //���ɲ����ܼ��������ڱ���

#define PASTER_STYLE 						4               //��Ƭģʽ��,���ڷ�ֵ��DA��ֵ�Ķ�Ӧ����ϵ��
#define NEEDLING_STYLE      		2               //���ģʽ�£����ڷ�ֵ��DA��ֵ�Ķ�Ӧ����ϵ��
 
#define DA_SCOPE_UP_LIMIT 			1000	            //DA��ֵ������ֵ
#define PEROID_UP_LIMIT 				5000              //2hz peroid	����ֵ������

#define LED_BASE_COUNT 					5000                //LED״̬ʱ�������(�����������Ƶ��) 500ms
#define LED_TWINKLE_SPEED 			2                //led��˸��������

    
#define Time5sCount 						50000  				    //��ʱ5s�ļ�������0.1ms/���� 
#define MAX_HEART_TIME 					3             //����������ȴ�ʱ��,��λ��5s/��   5*(x+1)

#define PRETREATSIZE 						20                  //Ԥ����buffer��С

#define QueueOffset 						1	            //���в�����ƫ����
#define CrcOffset 							8	           //CRCУ������λ������ƫ����
#define AckOffset 							1		                //����������ƫ����
#define ShiftOffset 						8

#define RValueMeasuregears      4           //�迹�ɼ���λ������


/*********************��������֮�궨�����***************/
#define GPIO_HIGH(x,y) 						 GPIO_SetBits(x,y)
#define GPIO_LOW(x,y)   					 GPIO_ResetBits(x,y)

/* Define the STM32F10x hardware depending on the used evaluation board */
/************************������ز�������*****************/
#ifdef usart1
  #define USARTx                   USART1
  #define USARTx_GPIO			         USART1_GPIO
  #define USARTx_CLK               USART1_CLK
  #define USARTx_GPIO_CLK          USART1_GPIO_CLK
  #define USARTx_RxPin             USART1_RxPin
  #define USARTx_TxPin             USART1_TxPin
  #define USARTx_IRQn              USART1_IRQn
  #define USARTx_IRQHandler        USART1_IRQHandler
	#define USART_PORT               COM1
#else
  #define USARTx                   USART2
  #define USARTx_GPIO			         USART2_GPIO
  #define USARTx_CLK               USART2_CLK
  #define USARTx_GPIO_CLK          USART2_GPIO_CLK
  #define USARTx_RxPin             USART2_RxPin
  #define USARTx_TxPin             USART2_TxPin
  #define USARTx_IRQn              USART2_IRQn
  #define USARTx_IRQHandler        USART2_IRQHandler
	#define USART_PORT               COM2
#endif


#define USART1_GPIO              	 GPIOA
#define USART1_CLK                 RCC_APB2Periph_USART1
#define USART1_GPIO_CLK            RCC_APB2Periph_GPIOA
#define USART1_RxPin               GPIO_Pin_10
#define USART1_TxPin               GPIO_Pin_9
#define USART1_IRQn                USART1_IRQn
#define USART1_IRQHandler          USART1_IRQHandler

#define USART2_GPIO              	 GPIOA
#define USART2_CLK               	 RCC_APB1Periph_USART2
#define USART2_GPIO_CLK          	 RCC_APB2Periph_GPIOA
#define USART2_RxPin             	 GPIO_Pin_3
#define USART2_TxPin             	 GPIO_Pin_2
#define USART2_IRQn              	 USART2_IRQn
#define USART2_IRQHandler        	 USART2_IRQHandler

//�װ�LEDָʾ������
#define RCC_GPIO_LED               RCC_APB2Periph_GPIOB    /*LEDʹ�õ�GPIOʱ��*/
#define GPIO_LED                   GPIOB                   /*����III��LED��ʹ�õ�GPIO��F*/

#define LED1                       GPIO_Pin_4              /*DS1ʹ�õ�GPIO�ܽ�6*/
#define LED2                       GPIO_Pin_5		      /*DS2ʹ�õ�GPIO�ܽ�7*/
#define LED3                       GPIO_Pin_8  			  /*DS3ʹ�õ�GPIO�ܽ�8*/
#define LED4                       GPIO_Pin_9		      /*DS4ʹ�õ�GPIO�ܽ�9*/

/*****************************SPI GPIO�˿���ض���********************************/

#define SPIx                     SPI2
#define SPIx_CLK                 SPI2_CLK
#define SPIx_GPIO_CLK            SPI2_GPIO_CLK
#define SPIx_GPIO				  		   SPI2_GPIO
#define SPIx_SCK				  		   SPI2_SCK
#define SPIx_MISO                SPI2_MOSI
#define SPIx_MOSI                SPI2_MOSI

#define SPI2_CLK           		   RCC_APB1Periph_SPI2
#define SPI2_GPIO_CLK            RCC_APB2Periph_GPIOB
#define SPI2_GPIO                GPIOB
#define SPI2_SCK				  			 GPIO_Pin_13
#define SPI2_MISO 			   	 		 GPIO_Pin_14
#define SPI2_MOSI 				  		 GPIO_Pin_15


/****************TLV5608 ���ƶ˿���ض���
	*��FSΪlowʱ����ʾTLV5608��ʼ����master���͵����ݡ�
	*��LDACΪlowʱ����ʾTLV5608���½��յ����ݡ�
	*��MODEΪ�ߣ�DSPMODE FSֻ�õ�20ns���ɡ�
	*��MODEΪ�ͣ�ucMDOE  ��������ʱ��ҪFSһֱΪ�ס�
*/								   
#define TLV5608_FS               GPIO_Pin_12
#define TLV5608_LDAC             GPIO_Pin_10
#define GPIO_TLV5608             GPIOB
#define RCC_GPIO_TLV5608         RCC_APB2Periph_GPIOB    /*TLV5608ʹ�õ�GPIOʱ��*/

/***********************TLV5608����֡��ض���****************************************/
#define TLV5608_Channel_A		  					((uint16_t)(0 << 12))
#define TLV5608_Channel_B		  					((uint16_t)(1 << 12))
#define TLV5608_Channel_C		  					((uint16_t)(2 << 12))
#define TLV5608_Channel_D		  					((uint16_t)(3 << 12))
#define TLV5608_Channel_E		  					((uint16_t)(4 << 12))
#define TLV5608_Channel_F		  					((uint16_t)(5 << 12))
#define TLV5608_Channel_G		  					((uint16_t)(6 << 12))
#define TLV5608_Channel_H		  					((uint16_t)(7 << 12))

#define TLV5608_Register_CTRL0	 			  ((uint16_t)(8 << 12))
#define TLV5608_Register_CTRL1	  			((uint16_t)(9 << 12))
#define TLV5608_Register_PRESET	  			((uint16_t)(0x0a << 12))


#define Channel_A                 			TLV5608_Channel_A
#define Channel_B                			  TLV5608_Channel_B
#define Channel_C                 			TLV5608_Channel_C
#define Channel_D                 			TLV5608_Channel_D
#define Channel_E                 			TLV5608_Channel_E
#define Channel_F                 			TLV5608_Channel_F
#define Channel_G                 			TLV5608_Channel_G
#define Channel_H                 			TLV5608_Channel_H		

#define Channel_CTRL0             			TLV5608_Register_CTRL0
#define Channel_CTRL1             			TLV5608_Register_CTRL1
#define Channel_PRESET            			TLV5608_Register_PRESET



/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
