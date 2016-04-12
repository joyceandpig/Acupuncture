/**
  ******************************************************************************
  * @file    stm32_eval.c
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 
  * 
  * 
  * 
  * 
  * 
  *
  * 
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#include "stm32_eval.h"
#include "AD5933.h"
#include "dma.h"
u32 usart_baud = BaudRate;
/** @defgroup STM32 Private_Variables
  * @{
  */ 
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
/**
  * @}
  */ 
USART_TypeDef* COM_USART[COMn]       = {EVAL_COM1, EVAL_COM2}; 

GPIO_TypeDef* COM_TX_PORT[COMn]      = {EVAL_COM1_TX_GPIO_PORT, EVAL_COM2_TX_GPIO_PORT};
 
GPIO_TypeDef* COM_RX_PORT[COMn]      = {EVAL_COM1_RX_GPIO_PORT, EVAL_COM2_RX_GPIO_PORT};

TIM_TypeDef * TIM_NUM[NUMn]          = {EVAL_TIM2, EVAL_TIM3, EVAL_TIM4};


const uint32_t COM_USART_CLK[COMn]   = {EVAL_COM1_CLK, EVAL_COM2_CLK};

const uint32_t COM_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK, EVAL_COM2_TX_GPIO_CLK};
 
const uint32_t COM_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK, EVAL_COM2_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[COMn]      = {EVAL_COM1_TX_PIN, EVAL_COM2_TX_PIN};

const uint16_t COM_RX_PIN[COMn]      = {EVAL_COM1_RX_PIN, EVAL_COM2_RX_PIN};
const uint16_t COM_IRQn[COMn]        = {EVAL_COM1_IRQn, EVAL_COM2_IRQn};


const uint16_t TIM_NUM_CLK[NUMn]     = {EVAL_TIM2_CLK, EVAL_TIM3_CLK, EVAL_TIM4_CLK};
const uint16_t TIM_NUM_IRQn[NUMn]    = {EVAL_TIM2_IRQn, EVAL_TIM3_IRQn, EVAL_TIM4_IRQn};


/**
  ******************************************************************************
  * @file    stm32_eval.c
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention

  ******************************************************************************/ 


void STM_CHN_LED_ON(u8 lednum)
{
	switch(lednum)
	{
		case 0:STM_CHN_LED6 = 1;break;
		case 1:STM_CHN_LED5 = 1;break;
		case 2:STM_CHN_LED4 = 1;break;
		case 3:STM_CHN_LED3 = 1;break;
		case 4:STM_CHN_LED2 = 1;break;
		case 5:STM_CHN_LED1 = 1;break;
		default:break;
	}
}
void STM_CHN_LED_OFF(u8 lednum)
{
	switch(lednum)
	{
		case 0:STM_CHN_LED6 = 0;break;
		case 1:STM_CHN_LED5 = 0;break;
		case 2:STM_CHN_LED4 = 0;break;
		case 3:STM_CHN_LED3 = 0;break;
		case 4:STM_CHN_LED2 = 0;break;
		case 5:STM_CHN_LED1 = 0;break;
		default:break;
	}
}

/******************************************************************
** 函数名: USART_PARA_CONFIGURE
** 输  入: baud_115200,baud_9600
           baud_115200, 选择波特率为115200
           baud_9600,选择波特率为9600
** 描  述: 设置使用串口的参数
** 
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
static void USART_PARA_CONFIGURE(BaudRate_TypeDef baudrate, uint16_t COM)
{
  USART_InitStructure.USART_BaudRate            = baudrate;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
//	USART_Init(COM_USART[COM], &USART_InitStructure);
//  USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);//使能接收中断
//  USART_Cmd(COM_USART[COM], ENABLE);
//	while (USART_GetFlagStatus(COM_USART[COM], USART_FLAG_TC) == RESET);/*等待 发送区清空*/
	
  DMA_init();
  USART_Init(COM_USART[COM], &USART_InitStructure);
  USART_ITConfig(COM_USART[COM], USART_IT_IDLE, ENABLE);//使能接收中断
  USART_Cmd(COM_USART[COM], ENABLE);
	while (USART_GetFlagStatus(COM_USART[COM], USART_FLAG_TC) == RESET);/*等待 发送区清空*/
}
/******************************************************************
** 函数名:  USART_NVIC_CONFIGURE
** 输  入:  COM  COM = COM1,表示选择串口1
                 COM = COM2，表示选择串口2   
** 描  述: 设置串口的发送接收中断
** 
** 全局变量: 
** 调用模块:
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
static void USART_NVIC_CONFIGURE(uint16_t COM)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	
  NVIC_InitStructure.NVIC_IRQChannel                   = COM_IRQn[COM];	
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;  /*从占优先级号码越小优先级越高*/
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	
	
   /*使能串口的发送和接收中断*/
//  USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);  
//  USART_ITConfig(COM_USART[COM], USART_IT_TXE, ENABLE);
	
}
/******************************************************************
** 函数名:  USART_GPIO_CONFIGURE
** 输  入:  COM  COM = COM1,表示选择串口1
                 COM = COM2，表示选择串口2   
** 描  述: 设置串口使用的GPIO脚，时钟及复用功能
** 
** 全局变量: 
** 调用模块:
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
static void USART_GPIO_CONFIGURE(COM_TypeDef COM)
{
  GPIO_InitTypeDef GPIO_InitStructure;    
  /* Enable GPIO clock */	   
  RCC_APB2PeriphClockCmd( COM_TX_PORT_CLK[COM] | \
													COM_RX_PORT_CLK[COM] | \
													RCC_APB2Periph_AFIO, ENABLE);
	
  if (COM == COM1)/* Enable UART clock */
    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
  else
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
	
	USART_DeInit(USARTx);

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;    //复用功能的推挽输出_AF_PP ——片内外设功能
  GPIO_InitStructure.GPIO_Pin   = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin  = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);
}
/******************************************************************
** 函数名: STM_EVAL_COMInit
** 输  入: none
** 描  述: 配置串口功能，包括参数设置及引脚配置
** 
** 全局变量: 
** 调用模块: USART_PARA_CONFIGURE
             USART_GPIO_CONFIGURE
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
void STM_EVAL_COMInit(COM_TypeDef com)
{
  USART_GPIO_CONFIGURE(com);                  //串口时钟和引脚配置
  USART_NVIC_CONFIGURE(com);                  //串口中断设置  
  USART_PARA_CONFIGURE(usart_baud,com);      //串口参数配置
}
/******************************************************************
** 函数名:  TIM_CONFIGURE
** 输  入:  TIM_x  TIM_X = Tim2,表示选择定时器2
                 TIM_X = Tim3，表示选择定时器3   
** 描  述: 设置定时器的时钟及中断参数
** 
** 全局变量: 
** 调用模块:
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
static void TIM_NVIC_CONFIGURE(TypeDef_TIM TIM_X)
{
  NVIC_InitTypeDef NVIC_InitStructure;
    
  RCC_APB1PeriphClockCmd(TIM_NUM_CLK[TIM_X],ENABLE);        /*使能基本定时器2时钟(TIM2)*/	
  NVIC_InitStructure.NVIC_IRQChannel                   = TIM_NUM_IRQn[TIM_X];
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	/*抢占优先级数字越小优先级越高*/
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2; /*从占优先级号码越小优先级越高*/
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/******************************************************************
** 函数名:  TIM_PARA_INIT
** 输  入:  TIM_x  TIM_X = Tim2,表示选择定时器2
                 TIM_X = Tim3，表示选择定时器3   
** 描  述: 设置定时器的各项参数
** 
** 全局变量: 
** 调用模块:
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/

static void TIM_PARA_CONFIGURE(TypeDef_TIM  TIM_X, uint16_t timecount)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
#ifdef SYSCLOCK_72M
	timecount *= 72;//72
#else
	timecount *= 32;
#endif
	/*  !<  To Set the Timer OutCount ,Step to the num ,Event take break   72MHZ value < ffff(65535)*/
	TIM_TimeBaseStructure.TIM_Period        = timecount;  //72 -> 1us 
	/* !< 72MHZ BUSCLOCK and the timerclock = busclock/TIM_Prescaler  72MHZ/36000 = 2khz --->  T = 0.5ms*/
	TIM_TimeBaseStructure.TIM_Prescaler     = 0;   //value < ffff	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;       /*  clock division [时钟分频系数] */
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; /* <<  Set TIM_Countermode    up means add mode*/	 
	TIM_TimeBaseInit(TIM_NUM[TIM_X], &TIM_TimeBaseStructure);   /*  !!!  save those data to TIM register     */	
	TIM_ITConfig(TIM_NUM[TIM_X],TIM_IT_Update,ENABLE);    /* !~~  Enable  TIM interrupt function  */
	TIM_Cmd(TIM_NUM[TIM_X],ENABLE);         	/*   <<<   Enable TIM   */
}
/******************************************************************
** 函数名:  STM_EVAL_TIMCONFIGURE
** 输  入:  none
** 描  述:  设置定时器的参数及启用定时中断
** 
** 全局变量: none 
** 调用模块: TIM_NVIC_CONFIGURE
              TIM_PARA_CONFIGURE
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
static void STM_EVAL_TIMCONFIGURE(uint16_t TimX,uint8_t time)
{
  TIM_NVIC_CONFIGURE(TimX);
  TIM_PARA_CONFIGURE(TimX,time);//输入定时时间，单位为us
}
/******************************************************************
** 函数名:  STM_EVAL_TIMInit
** 输  入:  none``11111
** 描  述:  设置定时器
** 
** 全局变量: none 
** 调用模块: STM_EVAL_TIMCONFIGURE
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
static void STM_EVAL_TIMInit(void)
{ 
  STM_EVAL_TIMCONFIGURE(Tim2,100);//TIM_Cmd(TIM_NUM[Tim2],DISABLE);//波形产生专用时钟 输入定时时间，单位为us
  STM_EVAL_TIMCONFIGURE(Tim3,100);//TIM_Cmd(TIM_NUM[Tim3],DISABLE);//各种常规计时专用时钟 输入定时时间，单位为us
//  STM_EVAL_TIMCONFIGURE(Tim4,100);TIM_Cmd(TIM_NUM[Tim4],DISABLE);  //串口接收数据专用定时器 输入定时时间，单位为us
}

/******************************************************************
** 函数名: STM_EVAL_STMLED_GPIO
** 输  入: none
** 描  述: 配置stm32上使用的LED灯（PORT_B Pin_12）
** 
** 全局变量: none 
** 调用模块: clock gpio  
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
static void STM_EVAL_STMWORK_LED(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);		//使能PC端口时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  GPIO_InitStructure.GPIO_Pin   = /*LED1*/GPIO_Pin_4 | /*LED3*/GPIO_Pin_5 |\
															   /*LED3*/GPIO_Pin_8 | /*LED4*/GPIO_Pin_9;  //端口配置
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 		 		//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 		//IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//根据设定参数初始化
	
  GPIO_SetBits(GPIOB,
	            GPIO_Pin_4 | GPIO_Pin_5 | \
							GPIO_Pin_8 | GPIO_Pin_9   \
						  );																					//PC0:PC3 输出高
}
/******************************************************************
** 函数名: STM_EVAL_STMLED_GPIO
** 输  入: none
** 描  述: 配置stm32上使用的LED灯（PORT_B Pin_12）
** 
** 全局变量: none 
** 调用模块: clock gpio  
** 作  者:   zcs
** 日  期:   2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
static void STM_EVAL_CHANNEL_LED(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//使能PC端口时钟
  GPIO_InitStructure.GPIO_Pin = /*LED1*/GPIO_Pin_0 | /*LED2*/GPIO_Pin_1 |\
															  /*LED3*/GPIO_Pin_3 ;  //端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 		//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 		//IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//根据设定参数初始化
	
  GPIO_ResetBits(GPIOB,
	            GPIO_Pin_0 | GPIO_Pin_1 | \
							GPIO_Pin_3 
						  );																					//PC0:PC3 输出高
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//使能PC端口时钟
  GPIO_InitStructure.GPIO_Pin = /*LED1*/GPIO_Pin_4 | /*LED2*/GPIO_Pin_6 |\
															  /*LED3*/GPIO_Pin_15 ;  //端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 		//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 		//IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 				//根据设定参数初始化
	
  GPIO_ResetBits(GPIOA,
	            GPIO_Pin_4 | GPIO_Pin_6 | \
							GPIO_Pin_15 
						  );
}
static void STM_EVAL_BELL(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//使能PC端口时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  //端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 		//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 		//IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//根据设定参数初始化
	
  GPIO_ResetBits(GPIOB,GPIO_Pin_11);	
//	GPIO_SetBits(GPIOC,GPIO_Pin_4);	
}
/******************************************************************
** 函数名: STM_EVAL_GPIOInit
** 输  入: none
** 描  述: GPIO配置，所有使用到stm32引脚的，都在这里集中配置
** 
** 全局变量: none
** 调用模块:  STM_EVAL_RELAY_GPIO();         
              STM_EVAL_STMLED_GPIO();        
              STM_EVAL_ZIGBEE_GPIO();        
              STM_EVAL_WIFI_GPIO();          
** 作  者:   zcs
** 日  期:  2015-03-11
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
static void STM_EVAL_GPIOInit(void)
{
  STM_EVAL_STMWORK_LED();          //stm32工作指示灯引脚配置
  STM_EVAL_CHANNEL_LED();					 //通道输出指示灯
	STM_EVAL_BELL();//蜂鸣器配置
}
/******************************************************************
** 函数名: STM_EVAL_DELAYInit
** 输  入: none
** 描  述: 延时函数初始化
** 
** 全局变量: none
** 调用模块:  DelayInit
** 作  者:  zcs
** 日  期:  2015-03-11
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
static void STM_EVAL_DELAYInit(void)
{
  DelayInit();
}

/******************************************************************
** 函数名: fputc
** 输  入: none
** 描  述: 系统运行信息打印函数
** 
** 全局变量: none
** 调用模块: USART_SendData 
** 作  者:  zcs
** 日  期:  2015-03-11
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(COM_USART[USART_PORT], (uint8_t) ch); /*发送一个字符函数*/
  while (USART_GetFlagStatus(COM_USART[USART_PORT], USART_FLAG_TC) == RESET);/*等待发送完成*/
  return ch;
}
/******************************************************************
** 函数名: STM_SPI_CLK_PORT
** 输  入: none
** 描  述: 配置SPi使用的时钟和对应端口
** 
** 全局变量: none
** 调用模块: 
** 作  者:  zcs
** 日  期:  2015-03-11
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
static void STM_SPI_CLK_PORT(void)
{
  /*使能SPI1使用的GPIO时钟 1*/
  RCC_APB1PeriphClockCmd(SPIx_CLK,ENABLE);
  RCC_APB2PeriphClockCmd(SPIx_GPIO_CLK,ENABLE);

   /*配置SPI1使用PORT口 */
  GPIO_InitStructure.GPIO_Pin   = SPIx_SCK | SPIx_MOSI; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPIx_GPIO, &GPIO_InitStructure);
}
/******************************************************************
** 函数名: STM_SPI_MODE_PARA
** 输  入: none
** 描  述: SPI工作参数设置
** 
** 全局变量: none
** 调用模块: 
** 作  者:  zcs
** 日  期:  2015-03-11
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
static void STM_SPI_MODE_PARA(void)
{
	SPI_InitTypeDef SPI_InitStructure;
  
	SPI_Cmd(SPIx,DISABLE);                                              /*  <<<  must disable spi2 before change the mode at first  */	
	SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;   /* Configuration SPI Mode is one Line and only Tx */      	
	SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;          /*  Configuration SPI  is Master   */            
	SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;  
	SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;            /*  SCK 起始状态（高或者低）时钟电平起始电平设置 (以一个周期为单位计算传送位) */
	SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;           /* 0 ：在SCK起始边沿改变，在下一边沿捕获    1 ： 在SCK起始边沿捕获，在下一边沿改变 */   
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;             /* choice soft NSS  */   	 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; 
	SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;         /* transfromer first bit is MSB or LSB  */       	
	SPI_InitStructure.SPI_CRCPolynomial     = 7;   
	SPI_Init(SPIx,&SPI_InitStructure);
	SPI_Cmd(SPIx,ENABLE);
}
/******************************************************************
** 函数名: STM_EVAL_SPIInit
** 输  入: none
** 描  述: spi初始化配置，包括时钟端口和工作参数
** 
** 全局变量: none
** 调用模块: STM_SPI_CLK_PORT
						 STM_SPI_MODE_PARA
** 作  者:  zcs
** 日  期:  2015-03-11
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
static void STM_EVAL_SPIInit(void)
{
  STM_SPI_CLK_PORT();   //配置spi的时钟和端口
  STM_SPI_MODE_PARA();	//配置spi工作模式的参数
}
/******************************************************************
** 函数名: SPI_SendData
** 输  入: cmd_object：需要更改参数的指令对象
					 Data：发送的数据
** 描  述: SPI发送数据函数，包括指令对象和数据值
** 
** 全局变量: none
** 调用模块: 
** 作  者:  zcs
** 日  期:  2015-04-20
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
extern void SPI_SendData(uint16_t cmd_object, uint16_t Data)
{
	if (cmd_object == Channel_CTRL0 || cmd_object == Channel_CTRL1)
		Data = cmd_object | Data;  			 //初始化芯片参数时不需要进行移位	
	else
	 	Data = cmd_object | (Data << 2); //DA寄存器的低位的2BIT没使用，规定要左移两位
	
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);  /* Wait for SPI1 Tx buffer empty */
	GPIO_ResetBits(GPIO_TLV5608,TLV5608_FS);	/* Set FS to low , start to transmit data */
	SPI_I2S_SendData(SPIx, Data);    /* Send SPI1 data  */
	GPIO_SetBits(GPIO_TLV5608,TLV5608_FS);/* Set FS to high , stop to transmit data */
}
/******************************************************************
** 函数名: DAC_CLK_PORT
** 输  入: none
** 描  述: DA芯片的时钟和端口配置
** 
** 全局变量: none
** 调用模块: 
** 作  者:  zcs
** 日  期:  2015-04-20
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
static void DAC_CLK_PORT(void)
{
  /*TLV5608控制引脚使用的GPIO时钟*/
  RCC_APB2PeriphClockCmd(RCC_GPIO_TLV5608, ENABLE);

  /*配置TLV5608控制引脚使用的GPIO管脚模式*/
  GPIO_InitStructure.GPIO_Pin   = TLV5608_FS|TLV5608_LDAC; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_TLV5608, &GPIO_InitStructure);
}
/******************************************************************
** 函数名: ReadyToUpdata
** 输  入: none
** 描  述: 为更新DA输出作配置准备
** 
** 全局变量: none
** 调用模块: GPIO_HIGH 
** 作  者:  zcs
** 日  期:  2015-04-20
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
void ReadyToUpdata(void)
{
	GPIO_HIGH(GPIO_TLV5608,TLV5608_LDAC);			      //拉高，使得LDAC能有变低动作来更新
}
/******************************************************************
** 函数名: UpdataDAOutput
** 输  入: none
** 描  述: 更新DA芯片的输出
** 
** 全局变量: none
** 调用模块: GPIO_LOW
** 作  者:  zcs
** 日  期:  2015-04-20 
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
void UpdataDAOutput(void)
{
	GPIO_LOW(GPIO_TLV5608,TLV5608_LDAC);			      //使DA输出指定电压数据  更新DA
}
/******************************************************************
** 函数名: DAC_MODE_PARA
** 输  入: none
** 描  述: DA芯片运行参数的配置
** 
** 全局变量: none
** 调用模块: SPI_SendData 
						 ReadyToUpdata
						 UpdataDAOutput
** 作  者:  zcs
** 日  期:  2015-03-11
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
static void DAC_MODE_PARA(void)
{
	/**  MODE pin   1:uc Mode  	 
     *				0:Dsp Mode 	 
	 	
	 *  		D11 D10 ....D7     D6     D5    D4     D3     D2      D1     D0 */
	 /*  CTRL0 	 x	 x						    PD	   DO					 IM	   PD:full device powerdown  0:normal 1:PD 
	 																			   DO:diagil out  0:disable 1:enable
																				   IM:二进制or补码	  0:二进制 1:补码
	 *   CTRL1   x   x 		Pgh	   Pef	 Pcd  	Pab	   Sgh    Sef     Scd    Sab
	 																			   Pxy:DACxy Powerdown	  0:normal  1:PD
																				   Sxy:DACxy Speed 0:slow  1:fast
	 */
  static uint8_t da_chn_count_i;
//	SPI_SendData(Channel_CTRL0,0x0010);						//开启powerdown 模式
	SPI_SendData(Channel_CTRL1,0x000f);							//使各个通道处于快速模式下,增大压摆率
  ReadyToUpdata();
  for(da_chn_count_i = 0; da_chn_count_i < 6;da_chn_count_i++)
		SPI_SendData((uint16_t)da_chn_count_i << ChannelOffset,0);
	UpdataDAOutput();//
}
/******************************************************************
** 函数名: STM_EVAL_TLV5608Init
** 输  入: none
** 描  述: DA芯片控制的初始化，包括端口配置和芯片的运行参数控制
** 
** 全局变量: none
** 调用模块: DAC_CLK_PORT
						 DAC_MODE_PARA
** 作  者:  zcs
** 日  期:  2015-04-20
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
static void STM_EVAL_TLV5608Init(void)					  
{
	DAC_CLK_PORT();//
	DAC_MODE_PARA();//
}
void uart_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	  USART_DeInit(USART2);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	  NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	 USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	 USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART2, ENABLE);                    //使能串口 

}
void STM_EVAL_MCOCLK_ENABLE(void)
{
	RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);
}
void STM_EVAL_MCOCLK_DISABLE(void)
{
	RCC_MCOConfig(RCC_MCO_NoClock);
}
void STM_EVAL_MCOCLK_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	STM_EVAL_MCOCLK_DISABLE();
	STM_EVAL_MCOCLK_ENABLE();
}
/******************************************************************
** 函数名: STM_EVAL_SYSTEMInit
** 输  入: none
** 描  述: 系统初始化设置
** 
** 全局变量: none
** 调用模块:  STM_EVAL_COMInit
              STM_EVAL_EXTIInit
              STM_EVAL_GPIOInit
              STM_EVAL_TIMInit
** 作  者:  zcs
** 日  期:  2015-03-11
** 修  改:
** 日  期: 
** 版  本: 1.0
*******************************************************************/
void STM_EVAL_SYSTEMINIT(void)
{
  STM_EVAL_COMInit(USART_PORT);        //串口配置
  STM_EVAL_GPIOInit();       //GPIO配置，所有用到的都集中配置
  STM_EVAL_TIMInit();        //定时器配置
  STM_EVAL_DELAYInit();      //延时函数初始化 
	STM_EVAL_SPIInit();        //SPI接口初始化
	STM_EVAL_TLV5608Init();    //Tlv5608芯片控制初始化

	
	AD5933_Para_Init();     //初始化阻抗采集参数
	delay_ms(1000);
	STM_EVAL_MCOCLK_OUT();//阻抗采集时钟源
	
//	STM_EVAL_TIM1_PA8_PWM_Init(4,0);
//	TIM_SetCompare1(TIM1,2);
}

