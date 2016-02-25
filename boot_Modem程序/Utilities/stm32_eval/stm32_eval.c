/* Define to prevent recursive inclusion -------------------------------------*/
#include "stm32_eval.h"
#include "platform_config.h"
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
static void USART_PARA_CONFIGURE(u32 baudrate, uint16_t COM)//BaudRate_TypeDef
{
  USART_InitStructure.USART_BaudRate            = baudrate;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
//  USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);
  USART_Init(COM_USART[COM], &USART_InitStructure);
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
	
	USART_DeInit(COM_USART[COM]);

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
void STM_EVAL_COMInit(uint8_t com)
{
	u32 baud = 0;
	baud = (  *(u8 *)BaudRateReadAddr)*65536 \
					+(*((u8 *)(BaudRateReadAddr+3))*256)\
					+(*((u8 *)(BaudRateReadAddr+2)));

  USART_GPIO_CONFIGURE(com);                  //串口时钟和引脚配置
//  USART_NVIC_CONFIGURE(com);                  //串口中断设置  
	if(!IsUsartBaudRight(baud)){
		baud = 9600;
	}
  USART_PARA_CONFIGURE(baud,com);      //串口参数配置Baud_9600
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
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	/*抢占优先级数字越小优先级越高*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0; /*从占优先级号码越小优先级越高*/
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
	timecount *= 72;
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
** 输  入:  none
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
  USART_SendData(USART1, (uint8_t) ch); /*发送一个字符函数*/
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);/*等待发送完成*/
  return ch;
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
//  STM_EVAL_COMInit(USART_PORT);        //串口配置
  STM_EVAL_GPIOInit();       //GPIO配置，所有用到的都集中配置
//  STM_EVAL_TIMInit();        //定时器配置
  STM_EVAL_DELAYInit();      //延时函数初始化 
	
}

