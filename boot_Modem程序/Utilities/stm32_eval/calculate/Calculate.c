/**
  ******************************************************************************
  * @file    USART/Interrupt/Calculate.h 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    06/23/2014
  * @brief   Evaluation board specific configuration file.
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

#include "Calculate.h"
#include "AD5933.h"
#include "configure_init.h"
#include "stm32f10x_it.h"
#include "stm32_eval.h"
#include "string.h"



/********************************************CRC校验中使用到的变量**************************************************/
uint8_t	current_state = Normal,						 //当前状态 
        rep_count = 0,			  //重复次数计数器
	      errcode_count = 0,              //错误代码次数
        state_report_flag,			  //是否状态报告标志位
		    urgstop = 0;		//紧急停止位
uint8_t system_start = 0,topdeath = 0;
uint8_t get_whole_data = 0;

static uint8_t order[DATA_SUM] = {0}, //通道命令重复次数数组
			         ch_num,								//通道号变量
			         count;								  //循环计数变量
			   
uint16_t LED_COUNT1 = LED_BASE_COUNT,
         LED_COUNT2 = LED_BASE_COUNT * LED_TWINKLE_SPEED,
         report_time5s = 0,
         heart_time_count = 0;
uint8_t PretreatBuffer[PRETREATSIZE] = {0};
         /**
  *@@brief define a struct about the  CicularQueue contain front rear and data
  *@@role init paramer to zero */

/****************paramer  prototypes********************************************/

SqQueue Queue = {0};                   /*****环形缓冲队列的变量定义***********/
ChannelParameter CH[CH_SUM] = {0};     /*****为每一个通道定义一个结构体*******/
Heart HeartBeat = {0};     						 /*****心跳检测参数定义一个结构体*******/
RepeatOrder RepPar = {0};              /*****指令重复检测定义一个结构体*******/
ErrorCode ErrPar = {0};                /*****指令错误检测定义一个结构体*******/
Impval ImpPar = {0};                   /*****阻抗参数检测定义一个结构体*******/


uint16_t	led_time_count;		           //指示灯计时器
static int8_t kAckBuf[7][14]={
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x00,0xFF,0xFF,0xFF,0xFF,0x65,0xFD},//正常
{0x0E,0x00,0x02,0x00,0x01,0x01,0x05,0x01,0xFF,0xFF,0xFF,0xFF,0x12,0xC1},//重复超限
{0x0E,0x00,0x02,0x00,0x01,0x01,0x05,0x02,0xFF,0xFF,0xFF,0xFF,0x12,0x85},//错误命令
{0x0E,0x00,0x02,0x00,0x01,0x01,0x05,0x03,0xFF,0xFF,0xFF,0xFF,0xD2,0x88},//上层死机
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x21,0xFF,0xFF,0xFF,0xFF,0xD2,0x88},//返回阻抗实部
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x21,0xFF,0xFF,0xFF,0xFF,0xD2,0x88},//返回阻抗虚部
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x23,0xFF,0xFF,0xFF,0xFF,0xD2,0x88} //返回当前选择的档位
};		  
/******************************************************************
**
** 描  述: CRC16校验所使用的数据表
**
*******************************************************************/
static const uint8_t auchCRCLo[] = {
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
  0x40
};
static const uint8_t auchCRCHi[] = {
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
  0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
  0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
  0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
  0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
  0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
  0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
  0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
  0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
  0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
  0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
  0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
  0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
  0x40
};

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
/******************************判定队列是否为空队列**********************/
void state_change(AckState state_now)
{
  current_state = state_now;
  
  if (ErrorCodeOverLimit == state_now)             /*****************增加闪烁频率**********/
  {
    LED_COUNT1 = LED_BASE_COUNT * LED_TWINKLE_SPEED;
    LED_COUNT2 = LED_COUNT1 * LED_TWINKLE_SPEED;
    CloseAllOutPut();         //关闭所有通道输出
  }
  else if (OrderRepeatOverLimit == state_now || \
					TopDead == state_now)                    /******************减小闪烁频率**********/
  {
    LED_COUNT1 = LED_BASE_COUNT / LED_TWINKLE_SPEED;
    LED_COUNT2 = LED_COUNT1 * LED_TWINKLE_SPEED;
    CloseAllOutPut();         //关闭所有通道输出
  }  
  else                                             /*****************恢复正常闪烁频率*******/
  {
    LED_COUNT1 = LED_BASE_COUNT;
    LED_COUNT2 = LED_COUNT1 * LED_TWINKLE_SPEED; 
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
void LightLed(uint16_t *count,LED_TypeDef LED_PORT)
{
  if (*count < LED_COUNT1)GPIO_LOW(GPIO_LED,LED_PORT);
	else if ((LED_COUNT1 <= *count) && (*count < LED_COUNT2))GPIO_HIGH(GPIO_LED,LED_PORT);
	else *count = CLEARVALUE;
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
/****************************判定队列是否为满队列*************************/
static uint8_t IsFull(SqQueue *Q)
{
	return (Q->size >= CAPACITY)?TRUE:FALSE;
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
/******************************判定队列是否为空队列**********************/
static uint8_t IsEmpty(SqQueue *Q)
{
	return (Q->size <= CLEARVALUE)?TRUE:FALSE;
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
/*************************环形队列 入队列操作***********************/
uint8_t EnQueue(SqQueue *Q, uint8_t data)
{
	if (IsFull(Q))
	{
		return FALSE;
	}	
	Q->items[Q->rear] = data;
	Q->size++;
	Q->rear = (Q->rear + 1)%CAPACITY;
	return TRUE;
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
uint8_t DeQueue(SqQueue *Q)
{
	if (IsEmpty(Q))
	{
		return FALSE;
	}	
	Q->front = (Q->front + 1)%CAPACITY;
	Q->size--;
	return TRUE;
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
/***************************读取队头元素*****************************/
uint8_t GetFrontDataFromQueue(SqQueue *Q)
{
	if (IsEmpty(Q))
	{
		return FALSE;
	}
	return Q->items[Q->front];
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
/*********************获取环形队列中数据长度***************************/
uint8_t GetLengthOfQueue(SqQueue *Q)
{
	return (Q->rear-Q->front + CAPACITY)%CAPACITY;
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
/*************************队列的头元素向后移动一位，但是不获取元素,只移动front************/
uint8_t ShiftFrontOfQueue(SqQueue *Q)
{
	if (IsEmpty(Q))return FALSE;
	Q->size--;
	Q->front = (Q->front+QueueOffset)%CAPACITY;
	return TRUE;
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
/**********************根据得到的帧头长度获取队列中指定长度数据，但是不移动front**********/
uint8_t GetDataFromQueue(SqQueue *Q,uint8_t length_of_frame,uint8_t *data_of_frame)
{
   uint16_t point,i;
   point = Q->front;
   for (i = 0; i < length_of_frame; i++)
   {
   	 *data_of_frame = Q->items[point];
	   point=(point+1)%CAPACITY;
	   data_of_frame++;
   }
   return TRUE;
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
/**********************************************反馈状态函数*********************************/
void SendStatusReply(uint8_t type)
{
	uint8_t usart_send_byte_count = 0;

	for (; usart_send_byte_count < FRAME_SUM; usart_send_byte_count++)
	{
		USART_SendData(USARTx,kAckBuf[type][usart_send_byte_count]);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); //等待字符发送完毕
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
/************************************************反馈控制命令以及相关数据********************/
void SendCmdReply(uint8_t *Sdata, uint8_t Slength)
{
  while(Slength--)
  {
    USART_SendData(USARTx,*Sdata++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); //等待字符发送完毕
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
//阻抗实部、虚部放入反馈帧节的数据位。并计算帧节校验码
static void TransformValDat(uint8_t *dat,signed short int dat1,signed short int dat2)
{
  uint16_t tmp;
  
  *(dat + 8) = dat1 >> 8;
  *(dat + 9) = dat1;
  *(dat + 10) = dat2 >> 8;
  *(dat + 11) = dat2;
 
  tmp = Arry_Get_CRC16(dat,12);
  *(dat + 12) = tmp >> 8;
  *(dat + 13) = tmp;
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
static void TransformSpanDat(uint8_t *dat,uint8_t dat1,uint8_t dat2)
{
  uint16_t tmp;
  
  *(dat + 8) = dat1;
  *(dat + 9) = dat2;

  tmp = Arry_Get_CRC16(dat,12);
  *(dat + 12) = tmp >> 8;
  *(dat + 13) = tmp; 
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
//采集阻抗程序
static void ImpVal(uint16_t Ifunc_code,uint8_t *Idata)
{
  uint16_t tmp;
  switch(Ifunc_code)
  {
    case 0x0201:        //时钟
      if (*(Idata+11) == 2)                    //外部时钟控制位     
      {
        AD5933_Para.CONTROL_REG_BIT3          = 1;             //  1:外部时钟  0：内部时钟
        AD5933_Para.AD5933_CLK                = 16000000UL;   //时钟频率
      }
      else if(*(Idata+11) == 1)              //内部时钟控制位     
      {
        AD5933_Para.CONTROL_REG_BIT3          = 0;           // 1:外部时钟  0：内部时钟
        AD5933_Para.AD5933_CLK                = 16776000UL;   //时钟频率
      }
      break;
    case 0x0202:        //参考电阻通道
      AD5933_Para.ADG714_CHANNEL_NUM = *(Idata+11);
      break;
    case 0x0203:        //激励信号
      if (*(Idata+11) == 1)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 0;        //2vpp
      else if (*(Idata+11) == 2)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 3;   //1vpp
      else if (*(Idata+11) == 3)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 =  2;  //400mvpp
      else if (*(Idata+11) == 4)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 1;   //200mvpp
      break;
    case 0x0204:        //PGA增益
      if(*(Idata+11) == 1)AD5933_Para.CONTROL_REG_BIT8 = 1;           // x1
      else if(*(Idata+11) == 5)AD5933_Para.CONTROL_REG_BIT8 = 0;       // x5
      break;
    case 0x0205:        //起始频率
      AD5933_Para.START_FREQUENCY = (*(Idata+8))*16777216+(*(Idata+9))*65536+(*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.START_FREQUENCY > 100000000)
        AD5933_Para.START_FREQUENCY = 100000000;
      break;
    case 0x0206:        //增量频率
      AD5933_Para.INC_FREQUENCY = (*(Idata+8))*16777216+(*(Idata+9))*65536+(*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.INC_FREQUENCY > 100000000)
        AD5933_Para.INC_FREQUENCY = 100000000;
      break;
    case 0x0207:        //频率增量数
      AD5933_Para.FREQUENCY_INC_NUM = (*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.FREQUENCY_INC_NUM > 511)
        AD5933_Para.FREQUENCY_INC_NUM = 511;
      break;
    case 0x0208:        //建立时间周期
      AD5933_Para.BUILD_TIME_CYCLES = (*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.BUILD_TIME_CYCLES > 2044)
        AD5933_Para.BUILD_TIME_CYCLES = 2044;
      break;
    case 0x0210:        //开始阻抗测量
      Test();
      break;
    case 0x0211:        //停止阻抗测量
      break;
    case 0x0212:        //递增频率测量
      AD5933_Para.INC_AND_REP_FRE = 0;
      Test();
      break;
    case 0x0213:        //频率重复测量
      AD5933_Para.INC_AND_REP_FRE = 1;
      Test();
      break;
    case 0x0214:        //测量温度
      break;
    case 0x0215:        //省电模式
      break;
    case 0x0216:        //待机模式
      break;
    case 0x0221:        //读取阻抗测量值
        Test();
        if(ImpPar.in_span==1)//数据有效才发送
				{
        TransformValDat(kAckBuf[4],real_Val,imag_Val);
        TransformSpanDat(kAckBuf[6],ImpPar.in_span,ConsultVal);
        SendCmdReply(kAckBuf[6],14);
        SendCmdReply(kAckBuf[4],14);
				}
      break;
    case 0x0222:        //读取环境温度值
      break;
    default:
      break;
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
/****************************************************************************/
static void CloseChannelOutput(void)
{
  uint8_t channel_number = 0;
  urgstop = TRUE;
  ReadyToUpdata();    
  for (;channel_number < CH_SUM;channel_number++)
  {
		SPI_SendData(channel_number << ChannelOffset,CLEARVALUE);// 依次关闭各个通道的DA电压输出
		CH[channel_number].channel_open = FALSE;
	}
	UpdataDAOutput();	     // 更新DA电压输出值
}
void CloseAllOutPut(void)
{
  CloseChannelOutput();
}
/* Private functions ---------------------------------------------------------*/

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
/********************************************重复命令处理***********************************/
uint8_t RepeatOrderDeal(uint8_t Rlength,uint8_t *Rdata)
{
	uint8_t rep_i,rep_si = 0,order_num = 0;

	for (rep_i = 0; rep_i < Rlength - DISDATA_SUM; rep_i++)  //与上次指令进行比对
		if (order[rep_i] == *(Rdata+DATA_STA+rep_i))
			order_num++;


	if (order_num == DATA_SUM)
		rep_count++;     																	//数据全都相同，认为指令重复一次
	else 
		rep_count = 0;

  if ((order[0] == 0x02) && (order[1] == 0x21))rep_count = 0;

	if (rep_count > 0)
	{
		RepPar.time_start = TRUE;	//出现重复情况，则启动重复计时
		printf("指令重复\n");
	}
	
	/*******************把本次接收的命令放入缓冲区内以供判断下一次命令的重复性**********/
	for (rep_si = 0; rep_si < (Rlength - DISDATA_SUM); rep_si++)
		order[rep_si] = *(Rdata+DATA_SUM+rep_si);
	return rep_count;	
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
static void ResetChannelParamer(uint8_t channel_num)
{
	CH[channel_num].wave_count = 0;
	/***************如果接收到正确数据，则初始化断续、疏密波的标志位，使其重新开始*****************/
	CH[channel_num].Iwave_count_flag = FALSE;
	CH[channel_num].Dwave_count_flag = FALSE;
	/***************如果接收到正确数据，则初始化断续、疏密波的时间计数，使其重新开始***************/
	CH[channel_num].time5s_count = 0;
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
static void ConfigureChannelParamer(uint8_t channel_num,uint8_t *Pdata)
{
	static uint16_t channel_frequency = 0;
	
	CH[channel_num].channel_open       = TRUE;	
	CH[channel_num].channel_stop_state = FALSE;
	CH[channel_num].channel            = channel_num;
	if(CH[channel_num].function_code   == 0x102 || \
		 CH[channel_num].function_code   == 0x103)        //判定波形是否为复杂波形
		CH[channel_num].complex_status   = TRUE;
	else
		CH[channel_num].complex_status   = FALSE;
	
	channel_frequency         = (uint16_t)*(Pdata+BIT9) << ShiftOffset|(uint16_t)*(Pdata+BIT10);
	CH[channel_num].peroid    = PEROID_COMPUTE_VALUE / channel_frequency;	   
	CH[channel_num].ss_peroid = WAVE_PEROID_MULTIPLE * CH[channel_num].peroid;
	CH[channel_num].scope     = *(Pdata+BIT11) * TO_DA_SCOPE_FACTOR;
	
	if (CH[channel_num].scope > DA_SCOPE_UP_LIMIT)
		CH[channel_num].scope   = DA_SCOPE_UP_LIMIT;//对DA电压输出限幅（超过DA芯片的最大值1000后，DA将会没有输出
	if (CH[channel_num].peroid > PEROID_UP_LIMIT)
		CH[channel_num].peroid  = PEROID_UP_LIMIT;//对转换后的周期进行限制，防止发送的数据超过预定值后出错
}
/* Private functions ---------------------------------------------------------*/

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
void ComputeChParameter(uint8_t Clength,uint8_t *Cdata)
{
  static uint16_t func_code;
	
	ch_num = 6 - *(Cdata+BIT8);              //输出通道转置，1号变为6号，2号变为5号
	ResetChannelParamer(ch_num);             //重置对应通道的计数值	
	func_code = (uint16_t)(*(Cdata+BIT6)<<ShiftOffset)|(uint16_t)*(Cdata+BIT7);

	if (func_code == Stop)                     //停止波形
	{
		CH[ch_num].channel_open = FALSE;
	  CH[ch_num].channel_stop_state = TRUE;
	}
	else                                       //其他指令
	{
		if (func_code < 0x0105)   							//表示控制对象为波形
		{		
			CH[ch_num].function_code = func_code;     //赋值波形类型
			ConfigureChannelParamer(ch_num,Cdata);    //计算通道波形的参数
		}
		else if ((func_code < 0x0223) && (func_code > 0x0200))//表示阻抗相关指令
      ImpVal(func_code,Cdata);
	}	
}
/******************************************************************
** 函数名: Arry_Get_CRC16
** 输  入: start_pos:指定从数组的何地开始进行校验
           usDataLen：指定进行校验的数据长度
** 描  述: 把指定长度和指定起始位置的数组数据进行校验，并返回校验值 ，
** 
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
uint16_t Arry_Get_CRC16(uint8_t *start_pos, uint8_t usDataLen)
{
  uint8_t uchCRCHi = 0xFF ;// 高CRC字节初始化 
  uint8_t uchCRCLo = 0xFF ;// 低CRC字节初始化 
  uint8_t uIndex ;// CRC循环中的索引 
   
  while (usDataLen--)// 传输消息缓冲区 
  { 
    uIndex   = uchCRCLo ^ (*(start_pos++)) ;// 计算CRC 
    uchCRCLo = uchCRCHi ^ auchCRCLo[uIndex] ; 
    uchCRCHi = auchCRCHi[uIndex] ; 
  } 
  return (uint16_t)((uchCRCHi << 8) | uchCRCLo); 
}
/******************************************************************
** 函数名: IsQueueRepeat
** 输  入: 无
** 描  述: 判断指令是否重复，并返回重复次数（大于等于0）
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
uint8_t IsFrameLenRight(SqQueue *Q)
{
  if ((Q->items[Q->front] <= PRETREATSIZE) && (Q->items[Q->front] <= Q->size) && (Q->size > 0))
    return TRUE;
  else
    return FALSE;
}
/******************************************************************
** 函数名: IsCheckCodeRight
** 输  入: buf:指定进行校验的数组
** 描  述: 对指定数组进行校验，并与原来的校验位相比较，如果相等则返回TRUE（1），否则返回FALSE（0）
** 
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:
** 版  本: 1.0
*******************************************************************/
uint8_t IsCheckCodeRight(uint8_t *buf)
{
  uint32_t checkcode = 0;
  uint8_t len = *buf;
	checkcode = Arry_Get_CRC16(buf,len-2);
  if (checkcode == ((*(buf+len-2)) << 8 | *(buf + len -1)))
    return TRUE;
  else
    return FALSE;
}
/******************************************************************
** 函数名: RemoveFrameFromQueue
** 输  入: Queue
** 描  述: 将指定的队列，按照帧头指定的长度出队操作，剔除数据
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:

** 版  本: 1.0    
*******************************************************************/
void RemoveFrameFromQueue(SqQueue *Q)
{
  static uint8_t len = 0;
	len = Q->items[Q->front];

  while(len--)
  {
    DeQueue(Q);
  }
}
/******************************************************************
** 函数名: IsQueueRepeat
** 输  入: 无
** 描  述: 判断指令是否重复，并返回重复次数（大于等于0）
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
uint8_t WorkQueueData(SqQueue *Q, uint8_t *IsGetWholeData)
{
  if (*IsGetWholeData)                												 //是否接收到完整数据
  {
//		printf("接收到完整数据串\n");
//		printf("队列当前大小%d\n",Q->size);
		system_start = 1;                    												//表示系统开始运行指令
		if (IsFrameLenRight(Q))                                     //帧节的帧长数据不超限
		{
//			printf("帧头不超限\n");
//			printf("帧头为%d\n",Q->items[Q->front]);
			memset(PretreatBuffer,'\0',PRETREATSIZE);
			GetDataFromQueue(Q,(Q->items[Q->front]),PretreatBuffer);//将队列的数据放入指定数组，
			if (IsCheckCodeRight(PretreatBuffer))                   //校验码正确
			{
//				printf("校验码正确\n");
				RemoveFrameFromQueue(Q);                              //若校验正确，则把队列中该帧数据剔出来
				heart_time_count = 0;                                 //接收到正确数据后，心跳包计数清零
				if (Q->size ==0) *IsGetWholeData = 0;       						//等待队列数据全部处理完全
				return TRUE;
			}
			else                                                  //若校验错误，则说明帧头表示的一帧数据不正确，
			{
//				printf("校验码错误\n");
				ErrPar.time_start = TRUE;                           //启动错误计时命令
				errcode_count++;
				DeQueue(Q);                                        //若检验错误，则删除该帧头的数据，然后再依次进行校验等判定
				if (Q->size ==0) *IsGetWholeData = 0;       			//等待队列数据全部处理完全
			} 
		}
		else
		{
//			printf("帧头超限\n");
//			printf("帧头为%d\n",Q->items[Q->front]);
			DeQueue(Q);                                       		//若帧长超限，则去除帧长位，继续进行各项判定     
			if (Q->size ==0) *IsGetWholeData = 0;       						//等待队列数据全部处理完全
		}
	}
	return FALSE;
}
/******************************************************************
** 函数名: IsQueueRepeat
** 输  入: 无
** 描  述: 判断指令是否重复，并返回重复次数（大于等于0）
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
uint8_t IsQueueRepeat(void)
{
  return RepeatOrderDeal(15,PretreatBuffer);
}
/******************************************************************
** 函数名: IsQueueRepeat
** 输  入: 无
** 描  述: 判断指令是否重复，并返回重复次数（大于等于0）
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
void ResetCmd(uint8_t *Re_Data)//在急停状态下，可发送复位指令，使恢复到正常状态
{
	//如果是复位信号，则启动复位
	uint16_t fun_code = (uint16_t)(*(Re_Data+BIT6)<<ShiftOffset)|(uint16_t)*(Re_Data+BIT7); 
	if (fun_code == Reset)                    //如果命令为复位
	{
		state_change(Normal);
		urgstop = FALSE;
		GPIO_HIGH(GPIO_LED,LED4);
	}
}
/******************************************************************
** 函数名: IsQueueRepeat
** 输  入: 无
** 描  述: 判断指令是否重复，并返回重复次数（大于等于0）
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
void RunNormal(uint8_t *RN_Data)             //运行非急停状态
{
	if(!RepeatOrderDeal(*RN_Data,RN_Data))            //若与上一次指令不重复就进行指令转换，否则不转换，直接反馈
	{
		printf("指令不重复\n");
		ComputeChParameter(*RN_Data,RN_Data);            //转换接收指令
	}
  SendCmdReply(RN_Data,*RN_Data);
}
/******************************************************************
** 函数名: IsQueueRepeat
** 输  入: 无
** 描  述: 判断指令是否重复，并返回重复次数（大于等于0）
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
void RunAbnormal(uint8_t *RA_Data)//运行急停状态
{
 ResetCmd(RA_Data);
 SendStatusReply(current_state);
}
/******************************************************************
** 函数名: IsQueueRepeat
** 输  入: 无
** 描  述: 判断指令是否重复，并返回重复次数（大于等于0）
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
uint8_t DisUrgentState(void)//非急停状况（正常时）
{
	return !urgstop;             //返回当前状态是否处理急停
}
/******************************************************************
** 函数名: IsQueueRepeat
** 输  入: 无
** 描  述: 判断指令是否重复，并返回重复次数（大于等于0）
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-03-12
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
void ExecuteCmd(uint8_t *EC_Data)//处理接收到的指令 
{
	if (DisUrgentState())         //若设备处理正常状态时（非急停状态），就运行正常模式代码
		RunNormal(EC_Data);
	else                          //若设备处理急停状态，则运行急停模式代码
		RunAbnormal(EC_Data);
}
