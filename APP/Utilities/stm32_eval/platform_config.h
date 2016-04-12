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
/**************全局中变量使用及相关定义******************/
extern u32 usart_baud;

#define usart2
//#define usart_baud 115200
#define SYSCLOCK_32M //sysclk is 72Mhz or 32Mhz


#define WaveHighTime     		2        //一个波形周期内，高电平维持的时间 单位 0.1ms/个 0.05ms
#define CAPACITY      			200 	       //定义环形缓冲区大小

#define JumpBootAddress 		0x08000000    //跳转地址，跳转至boot运行
#define Vector_Offset 			0x5000   //中断重映射，相对于FLASH_BASE地址的偏移量（0x08000000）
														 						 //指示程序保存位置
#define IsUpdata 						0x08004800 //是否更新标志
#define BaudRateReadAddr    0x08004802



/****************Calculate函数中相关定义及说明**********************/

#define ChannelOffset 					12               	  	//DA芯片通道地址偏移量
#define ERRORCODE_UP_LIMIT  		12              //错误命令次数上限，每串帧节都可能
#define ErrTimeLit 							5000            //错误命令判定限定时间  单位：0.1ms/个
#define ORDER_REPEAT_UP_LIMIT		10	  		  //串口命令重复次数上限
#define RepTimeLit 							5000     		//重复命令判定限定时间，单位：0.1ms/个
#define ImpValTimeLit 					10000     //阻抗采集限定时间，单位：0.1ms/个
#define BaudRate 						115200 			//设置串口波特率

#define CH_SUM  								6	                //通道个数
#define DATA_SUM 								6	                //参数数据的个数
#define DISDATA_SUM 						8	            //非参数数据位的个数
#define DATA_STA 								6	            //参数数据起始位
#define FRAME_SUM 							14                //数据帧的数据总个数

#define Peroid_2hz							4990             	    //2hz时对应的周期值
#define Peroid_10hz 						1000              	    //10hz时对应的周期值
#define PEROID_COMPUTE_VALUE 		10000    	    //1/f (s) = 1000/f (ms) -> 10*1000/f (ms)由于定时器是0.1ms一次，需要计数10次才到1ms
#define CLEARVALUE 							0	              	    //清零值，主要针对一些计数起点或者循环起点

#define WAVE_PEROID_MULTIPLE 		5              //疏松波和密集波的周期倍数

#define PASTER_STYLE 						4               //贴片模式下,串口幅值和DA幅值的对应调整系数
#define NEEDLING_STYLE      		2               //针刺模式下，串口幅值和DA幅值的对应调整系数
 
#define DA_SCOPE_UP_LIMIT 			1000	            //DA幅值的上限值
#define PEROID_UP_LIMIT 				5000              //2hz peroid	周期值的上限

#define LED_BASE_COUNT 					5000                //LED状态时间计数点(正常情况闪亮频率) 500ms
#define LED_TWINKLE_SPEED 			2                //led闪烁快慢调整

    
#define Time5sCount 						50000  				    //定时5s的计数器（0.1ms/个） 
#define MAX_HEART_TIME 					3             //心跳检测最大等待时间,单位是5s/个   5*(x+1)

#define PRETREATSIZE 						20                  //预处理buffer大小

#define QueueOffset 						1	            //队列操作的偏移量
#define CrcOffset 							8	           //CRC校验码移位操作的偏移量
#define AckOffset 							1		                //回馈操作的偏移量
#define ShiftOffset 						8

#define RValueMeasuregears      4           //阻抗采集档位分区数


/*********************内联函数之宏定义操作***************/
#define GPIO_HIGH(x,y) 						 GPIO_SetBits(x,y)
#define GPIO_LOW(x,y)   					 GPIO_ResetBits(x,y)

/* Define the STM32F10x hardware depending on the used evaluation board */
/************************串口相关参数定义*****************/
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

//底板LED指示灯配置
#define RCC_GPIO_LED               RCC_APB2Periph_GPIOB    /*LED使用的GPIO时钟*/
#define GPIO_LED                   GPIOB                   /*神舟III号LED灯使用的GPIO组F*/

#define LED1                       GPIO_Pin_4              /*DS1使用的GPIO管脚6*/
#define LED2                       GPIO_Pin_5		      /*DS2使用的GPIO管脚7*/
#define LED3                       GPIO_Pin_8  			  /*DS3使用的GPIO管脚8*/
#define LED4                       GPIO_Pin_9		      /*DS4使用的GPIO管脚9*/

/*****************************SPI GPIO端口相关定义********************************/

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


/****************TLV5608 控制端口相关定义
	*【FS为low时，表示TLV5608开始接收master传送的数据】
	*【LDAC为low时，表示TLV5608更新接收的数据】
	*【MODE为高，DSPMODE FS只置低20ns即可】
	*【MODE为低，ucMDOE  传输数据时需要FS一直为底】
*/								   
#define TLV5608_FS               GPIO_Pin_12
#define TLV5608_LDAC             GPIO_Pin_10
#define GPIO_TLV5608             GPIOB
#define RCC_GPIO_TLV5608         RCC_APB2Periph_GPIOB    /*TLV5608使用的GPIO时钟*/

/***********************TLV5608数据帧相关定义****************************************/
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
