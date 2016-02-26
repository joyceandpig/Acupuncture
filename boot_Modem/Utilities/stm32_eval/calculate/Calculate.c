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



/********************************************CRCУ����ʹ�õ��ı���**************************************************/
uint8_t	current_state = Normal,						 //��ǰ״̬ 
        rep_count = 0,			  //�ظ�����������
	      errcode_count = 0,              //����������
        state_report_flag,			  //�Ƿ�״̬�����־λ
		    urgstop = 0;		//����ֹͣλ
uint8_t system_start = 0,topdeath = 0;
uint8_t get_whole_data = 0;

static uint8_t order[DATA_SUM] = {0}, //ͨ�������ظ���������
			         ch_num,								//ͨ���ű���
			         count;								  //ѭ����������
			   
uint16_t LED_COUNT1 = LED_BASE_COUNT,
         LED_COUNT2 = LED_BASE_COUNT * LED_TWINKLE_SPEED,
         report_time5s = 0,
         heart_time_count = 0;
uint8_t PretreatBuffer[PRETREATSIZE] = {0};
         /**
  *@@brief define a struct about the  CicularQueue contain front rear and data
  *@@role init paramer to zero */

/****************paramer  prototypes********************************************/

SqQueue Queue = {0};                   /*****���λ�����еı�������***********/
ChannelParameter CH[CH_SUM] = {0};     /*****Ϊÿһ��ͨ������һ���ṹ��*******/
Heart HeartBeat = {0};     						 /*****��������������һ���ṹ��*******/
RepeatOrder RepPar = {0};              /*****ָ���ظ���ⶨ��һ���ṹ��*******/
ErrorCode ErrPar = {0};                /*****ָ������ⶨ��һ���ṹ��*******/
Impval ImpPar = {0};                   /*****�迹������ⶨ��һ���ṹ��*******/


uint16_t	led_time_count;		           //ָʾ�Ƽ�ʱ��
static int8_t kAckBuf[7][14]={
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x00,0xFF,0xFF,0xFF,0xFF,0x65,0xFD},//����
{0x0E,0x00,0x02,0x00,0x01,0x01,0x05,0x01,0xFF,0xFF,0xFF,0xFF,0x12,0xC1},//�ظ�����
{0x0E,0x00,0x02,0x00,0x01,0x01,0x05,0x02,0xFF,0xFF,0xFF,0xFF,0x12,0x85},//��������
{0x0E,0x00,0x02,0x00,0x01,0x01,0x05,0x03,0xFF,0xFF,0xFF,0xFF,0xD2,0x88},//�ϲ�����
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x21,0xFF,0xFF,0xFF,0xFF,0xD2,0x88},//�����迹ʵ��
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x21,0xFF,0xFF,0xFF,0xFF,0xD2,0x88},//�����迹�鲿
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x23,0xFF,0xFF,0xFF,0xFF,0xD2,0x88} //���ص�ǰѡ��ĵ�λ
};		  
/******************************************************************
**
** ��  ��: CRC16У����ʹ�õ����ݱ�
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
/******************************�ж������Ƿ�Ϊ�ն���**********************/
void state_change(AckState state_now)
{
  current_state = state_now;
  
  if (ErrorCodeOverLimit == state_now)             /*****************������˸Ƶ��**********/
  {
    LED_COUNT1 = LED_BASE_COUNT * LED_TWINKLE_SPEED;
    LED_COUNT2 = LED_COUNT1 * LED_TWINKLE_SPEED;
    CloseAllOutPut();         //�ر�����ͨ�����
  }
  else if (OrderRepeatOverLimit == state_now || \
					TopDead == state_now)                    /******************��С��˸Ƶ��**********/
  {
    LED_COUNT1 = LED_BASE_COUNT / LED_TWINKLE_SPEED;
    LED_COUNT2 = LED_COUNT1 * LED_TWINKLE_SPEED;
    CloseAllOutPut();         //�ر�����ͨ�����
  }  
  else                                             /*****************�ָ�������˸Ƶ��*******/
  {
    LED_COUNT1 = LED_BASE_COUNT;
    LED_COUNT2 = LED_COUNT1 * LED_TWINKLE_SPEED; 
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
void LightLed(uint16_t *count,LED_TypeDef LED_PORT)
{
  if (*count < LED_COUNT1)GPIO_LOW(GPIO_LED,LED_PORT);
	else if ((LED_COUNT1 <= *count) && (*count < LED_COUNT2))GPIO_HIGH(GPIO_LED,LED_PORT);
	else *count = CLEARVALUE;
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
/****************************�ж������Ƿ�Ϊ������*************************/
static uint8_t IsFull(SqQueue *Q)
{
	return (Q->size >= CAPACITY)?TRUE:FALSE;
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
/******************************�ж������Ƿ�Ϊ�ն���**********************/
static uint8_t IsEmpty(SqQueue *Q)
{
	return (Q->size <= CLEARVALUE)?TRUE:FALSE;
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
/*************************���ζ��� ����в���***********************/
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
/***************************��ȡ��ͷԪ��*****************************/
uint8_t GetFrontDataFromQueue(SqQueue *Q)
{
	if (IsEmpty(Q))
	{
		return FALSE;
	}
	return Q->items[Q->front];
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
/*********************��ȡ���ζ��������ݳ���***************************/
uint8_t GetLengthOfQueue(SqQueue *Q)
{
	return (Q->rear-Q->front + CAPACITY)%CAPACITY;
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
/*************************���е�ͷԪ������ƶ�һλ�����ǲ���ȡԪ��,ֻ�ƶ�front************/
uint8_t ShiftFrontOfQueue(SqQueue *Q)
{
	if (IsEmpty(Q))return FALSE;
	Q->size--;
	Q->front = (Q->front+QueueOffset)%CAPACITY;
	return TRUE;
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
/**********************���ݵõ���֡ͷ���Ȼ�ȡ������ָ���������ݣ����ǲ��ƶ�front**********/
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
/**********************************************����״̬����*********************************/
void SendStatusReply(uint8_t type)
{
	uint8_t usart_send_byte_count = 0;

	for (; usart_send_byte_count < FRAME_SUM; usart_send_byte_count++)
	{
		USART_SendData(USARTx,kAckBuf[type][usart_send_byte_count]);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); //�ȴ��ַ��������
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
/************************************************�������������Լ��������********************/
void SendCmdReply(uint8_t *Sdata, uint8_t Slength)
{
  while(Slength--)
  {
    USART_SendData(USARTx,*Sdata++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); //�ȴ��ַ��������
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
//�迹ʵ�����鲿���뷴��֡�ڵ�����λ��������֡��У����
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
//�ɼ��迹����
static void ImpVal(uint16_t Ifunc_code,uint8_t *Idata)
{
  uint16_t tmp;
  switch(Ifunc_code)
  {
    case 0x0201:        //ʱ��
      if (*(Idata+11) == 2)                    //�ⲿʱ�ӿ���λ     
      {
        AD5933_Para.CONTROL_REG_BIT3          = 1;             //  1:�ⲿʱ��  0���ڲ�ʱ��
        AD5933_Para.AD5933_CLK                = 16000000UL;   //ʱ��Ƶ��
      }
      else if(*(Idata+11) == 1)              //�ڲ�ʱ�ӿ���λ     
      {
        AD5933_Para.CONTROL_REG_BIT3          = 0;           // 1:�ⲿʱ��  0���ڲ�ʱ��
        AD5933_Para.AD5933_CLK                = 16776000UL;   //ʱ��Ƶ��
      }
      break;
    case 0x0202:        //�ο�����ͨ��
      AD5933_Para.ADG714_CHANNEL_NUM = *(Idata+11);
      break;
    case 0x0203:        //�����ź�
      if (*(Idata+11) == 1)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 0;        //2vpp
      else if (*(Idata+11) == 2)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 3;   //1vpp
      else if (*(Idata+11) == 3)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 =  2;  //400mvpp
      else if (*(Idata+11) == 4)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 1;   //200mvpp
      break;
    case 0x0204:        //PGA����
      if(*(Idata+11) == 1)AD5933_Para.CONTROL_REG_BIT8 = 1;           // x1
      else if(*(Idata+11) == 5)AD5933_Para.CONTROL_REG_BIT8 = 0;       // x5
      break;
    case 0x0205:        //��ʼƵ��
      AD5933_Para.START_FREQUENCY = (*(Idata+8))*16777216+(*(Idata+9))*65536+(*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.START_FREQUENCY > 100000000)
        AD5933_Para.START_FREQUENCY = 100000000;
      break;
    case 0x0206:        //����Ƶ��
      AD5933_Para.INC_FREQUENCY = (*(Idata+8))*16777216+(*(Idata+9))*65536+(*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.INC_FREQUENCY > 100000000)
        AD5933_Para.INC_FREQUENCY = 100000000;
      break;
    case 0x0207:        //Ƶ��������
      AD5933_Para.FREQUENCY_INC_NUM = (*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.FREQUENCY_INC_NUM > 511)
        AD5933_Para.FREQUENCY_INC_NUM = 511;
      break;
    case 0x0208:        //����ʱ������
      AD5933_Para.BUILD_TIME_CYCLES = (*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.BUILD_TIME_CYCLES > 2044)
        AD5933_Para.BUILD_TIME_CYCLES = 2044;
      break;
    case 0x0210:        //��ʼ�迹����
      Test();
      break;
    case 0x0211:        //ֹͣ�迹����
      break;
    case 0x0212:        //����Ƶ�ʲ���
      AD5933_Para.INC_AND_REP_FRE = 0;
      Test();
      break;
    case 0x0213:        //Ƶ���ظ�����
      AD5933_Para.INC_AND_REP_FRE = 1;
      Test();
      break;
    case 0x0214:        //�����¶�
      break;
    case 0x0215:        //ʡ��ģʽ
      break;
    case 0x0216:        //����ģʽ
      break;
    case 0x0221:        //��ȡ�迹����ֵ
        Test();
        if(ImpPar.in_span==1)//������Ч�ŷ���
				{
        TransformValDat(kAckBuf[4],real_Val,imag_Val);
        TransformSpanDat(kAckBuf[6],ImpPar.in_span,ConsultVal);
        SendCmdReply(kAckBuf[6],14);
        SendCmdReply(kAckBuf[4],14);
				}
      break;
    case 0x0222:        //��ȡ�����¶�ֵ
      break;
    default:
      break;
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
/****************************************************************************/
static void CloseChannelOutput(void)
{
  uint8_t channel_number = 0;
  urgstop = TRUE;
  ReadyToUpdata();    
  for (;channel_number < CH_SUM;channel_number++)
  {
		SPI_SendData(channel_number << ChannelOffset,CLEARVALUE);// ���ιرո���ͨ����DA��ѹ���
		CH[channel_number].channel_open = FALSE;
	}
	UpdataDAOutput();	     // ����DA��ѹ���ֵ
}
void CloseAllOutPut(void)
{
  CloseChannelOutput();
}
/* Private functions ---------------------------------------------------------*/

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
/********************************************�ظ������***********************************/
uint8_t RepeatOrderDeal(uint8_t Rlength,uint8_t *Rdata)
{
	uint8_t rep_i,rep_si = 0,order_num = 0;

	for (rep_i = 0; rep_i < Rlength - DISDATA_SUM; rep_i++)  //���ϴ�ָ����бȶ�
		if (order[rep_i] == *(Rdata+DATA_STA+rep_i))
			order_num++;


	if (order_num == DATA_SUM)
		rep_count++;     																	//����ȫ����ͬ����Ϊָ���ظ�һ��
	else 
		rep_count = 0;

  if ((order[0] == 0x02) && (order[1] == 0x21))rep_count = 0;

	if (rep_count > 0)
	{
		RepPar.time_start = TRUE;	//�����ظ�������������ظ���ʱ
		printf("ָ���ظ�\n");
	}
	
	/*******************�ѱ��ν��յ�������뻺�������Թ��ж���һ��������ظ���**********/
	for (rep_si = 0; rep_si < (Rlength - DISDATA_SUM); rep_si++)
		order[rep_si] = *(Rdata+DATA_SUM+rep_si);
	return rep_count;	
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
static void ResetChannelParamer(uint8_t channel_num)
{
	CH[channel_num].wave_count = 0;
	/***************������յ���ȷ���ݣ����ʼ�����������ܲ��ı�־λ��ʹ�����¿�ʼ*****************/
	CH[channel_num].Iwave_count_flag = FALSE;
	CH[channel_num].Dwave_count_flag = FALSE;
	/***************������յ���ȷ���ݣ����ʼ�����������ܲ���ʱ�������ʹ�����¿�ʼ***************/
	CH[channel_num].time5s_count = 0;
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
static void ConfigureChannelParamer(uint8_t channel_num,uint8_t *Pdata)
{
	static uint16_t channel_frequency = 0;
	
	CH[channel_num].channel_open       = TRUE;	
	CH[channel_num].channel_stop_state = FALSE;
	CH[channel_num].channel            = channel_num;
	if(CH[channel_num].function_code   == 0x102 || \
		 CH[channel_num].function_code   == 0x103)        //�ж������Ƿ�Ϊ���Ӳ���
		CH[channel_num].complex_status   = TRUE;
	else
		CH[channel_num].complex_status   = FALSE;
	
	channel_frequency         = (uint16_t)*(Pdata+BIT9) << ShiftOffset|(uint16_t)*(Pdata+BIT10);
	CH[channel_num].peroid    = PEROID_COMPUTE_VALUE / channel_frequency;	   
	CH[channel_num].ss_peroid = WAVE_PEROID_MULTIPLE * CH[channel_num].peroid;
	CH[channel_num].scope     = *(Pdata+BIT11) * TO_DA_SCOPE_FACTOR;
	
	if (CH[channel_num].scope > DA_SCOPE_UP_LIMIT)
		CH[channel_num].scope   = DA_SCOPE_UP_LIMIT;//��DA��ѹ����޷�������DAоƬ�����ֵ1000��DA����û�����
	if (CH[channel_num].peroid > PEROID_UP_LIMIT)
		CH[channel_num].peroid  = PEROID_UP_LIMIT;//��ת��������ڽ������ƣ���ֹ���͵����ݳ���Ԥ��ֵ�����
}
/* Private functions ---------------------------------------------------------*/

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
void ComputeChParameter(uint8_t Clength,uint8_t *Cdata)
{
  static uint16_t func_code;
	
	ch_num = 6 - *(Cdata+BIT8);              //���ͨ��ת�ã�1�ű�Ϊ6�ţ�2�ű�Ϊ5��
	ResetChannelParamer(ch_num);             //���ö�Ӧͨ���ļ���ֵ	
	func_code = (uint16_t)(*(Cdata+BIT6)<<ShiftOffset)|(uint16_t)*(Cdata+BIT7);

	if (func_code == Stop)                     //ֹͣ����
	{
		CH[ch_num].channel_open = FALSE;
	  CH[ch_num].channel_stop_state = TRUE;
	}
	else                                       //����ָ��
	{
		if (func_code < 0x0105)   							//��ʾ���ƶ���Ϊ����
		{		
			CH[ch_num].function_code = func_code;     //��ֵ��������
			ConfigureChannelParamer(ch_num,Cdata);    //����ͨ�����εĲ���
		}
		else if ((func_code < 0x0223) && (func_code > 0x0200))//��ʾ�迹���ָ��
      ImpVal(func_code,Cdata);
	}	
}
/******************************************************************
** ������: Arry_Get_CRC16
** ��  ��: start_pos:ָ��������ĺεؿ�ʼ����У��
           usDataLen��ָ������У������ݳ���
** ��  ��: ��ָ�����Ⱥ�ָ����ʼλ�õ��������ݽ���У�飬������У��ֵ ��
** 
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
uint16_t Arry_Get_CRC16(uint8_t *start_pos, uint8_t usDataLen)
{
  uint8_t uchCRCHi = 0xFF ;// ��CRC�ֽڳ�ʼ�� 
  uint8_t uchCRCLo = 0xFF ;// ��CRC�ֽڳ�ʼ�� 
  uint8_t uIndex ;// CRCѭ���е����� 
   
  while (usDataLen--)// ������Ϣ������ 
  { 
    uIndex   = uchCRCLo ^ (*(start_pos++)) ;// ����CRC 
    uchCRCLo = uchCRCHi ^ auchCRCLo[uIndex] ; 
    uchCRCHi = auchCRCHi[uIndex] ; 
  } 
  return (uint16_t)((uchCRCHi << 8) | uchCRCLo); 
}
/******************************************************************
** ������: IsQueueRepeat
** ��  ��: ��
** ��  ��: �ж�ָ���Ƿ��ظ����������ظ����������ڵ���0��
**                                                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
uint8_t IsFrameLenRight(SqQueue *Q)
{
  if ((Q->items[Q->front] <= PRETREATSIZE) && (Q->items[Q->front] <= Q->size) && (Q->size > 0))
    return TRUE;
  else
    return FALSE;
}
/******************************************************************
** ������: IsCheckCodeRight
** ��  ��: buf:ָ������У�������
** ��  ��: ��ָ���������У�飬����ԭ����У��λ��Ƚϣ��������򷵻�TRUE��1�������򷵻�FALSE��0��
** 
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:
** ��  ��: 1.0
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
** ������: RemoveFrameFromQueue
** ��  ��: Queue
** ��  ��: ��ָ���Ķ��У�����֡ͷָ���ĳ��ȳ��Ӳ������޳�����
**                                                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:

** ��  ��: 1.0    
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
** ������: IsQueueRepeat
** ��  ��: ��
** ��  ��: �ж�ָ���Ƿ��ظ����������ظ����������ڵ���0��
**                                                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
uint8_t WorkQueueData(SqQueue *Q, uint8_t *IsGetWholeData)
{
  if (*IsGetWholeData)                												 //�Ƿ���յ���������
  {
//		printf("���յ��������ݴ�\n");
//		printf("���е�ǰ��С%d\n",Q->size);
		system_start = 1;                    												//��ʾϵͳ��ʼ����ָ��
		if (IsFrameLenRight(Q))                                     //֡�ڵ�֡�����ݲ�����
		{
//			printf("֡ͷ������\n");
//			printf("֡ͷΪ%d\n",Q->items[Q->front]);
			memset(PretreatBuffer,'\0',PRETREATSIZE);
			GetDataFromQueue(Q,(Q->items[Q->front]),PretreatBuffer);//�����е����ݷ���ָ�����飬
			if (IsCheckCodeRight(PretreatBuffer))                   //У������ȷ
			{
//				printf("У������ȷ\n");
				RemoveFrameFromQueue(Q);                              //��У����ȷ����Ѷ����и�֡�����޳���
				heart_time_count = 0;                                 //���յ���ȷ���ݺ���������������
				if (Q->size ==0) *IsGetWholeData = 0;       						//�ȴ���������ȫ��������ȫ
				return TRUE;
			}
			else                                                  //��У�������˵��֡ͷ��ʾ��һ֡���ݲ���ȷ��
			{
//				printf("У�������\n");
				ErrPar.time_start = TRUE;                           //���������ʱ����
				errcode_count++;
				DeQueue(Q);                                        //�����������ɾ����֡ͷ�����ݣ�Ȼ�������ν���У����ж�
				if (Q->size ==0) *IsGetWholeData = 0;       			//�ȴ���������ȫ��������ȫ
			} 
		}
		else
		{
//			printf("֡ͷ����\n");
//			printf("֡ͷΪ%d\n",Q->items[Q->front]);
			DeQueue(Q);                                       		//��֡�����ޣ���ȥ��֡��λ���������и����ж�     
			if (Q->size ==0) *IsGetWholeData = 0;       						//�ȴ���������ȫ��������ȫ
		}
	}
	return FALSE;
}
/******************************************************************
** ������: IsQueueRepeat
** ��  ��: ��
** ��  ��: �ж�ָ���Ƿ��ظ����������ظ����������ڵ���0��
**                                                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
uint8_t IsQueueRepeat(void)
{
  return RepeatOrderDeal(15,PretreatBuffer);
}
/******************************************************************
** ������: IsQueueRepeat
** ��  ��: ��
** ��  ��: �ж�ָ���Ƿ��ظ����������ظ����������ڵ���0��
**                                                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
void ResetCmd(uint8_t *Re_Data)//�ڼ�ͣ״̬�£��ɷ��͸�λָ�ʹ�ָ�������״̬
{
	//����Ǹ�λ�źţ���������λ
	uint16_t fun_code = (uint16_t)(*(Re_Data+BIT6)<<ShiftOffset)|(uint16_t)*(Re_Data+BIT7); 
	if (fun_code == Reset)                    //�������Ϊ��λ
	{
		state_change(Normal);
		urgstop = FALSE;
		GPIO_HIGH(GPIO_LED,LED4);
	}
}
/******************************************************************
** ������: IsQueueRepeat
** ��  ��: ��
** ��  ��: �ж�ָ���Ƿ��ظ����������ظ����������ڵ���0��
**                                                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
void RunNormal(uint8_t *RN_Data)             //���зǼ�ͣ״̬
{
	if(!RepeatOrderDeal(*RN_Data,RN_Data))            //������һ��ָ��ظ��ͽ���ָ��ת��������ת����ֱ�ӷ���
	{
		printf("ָ��ظ�\n");
		ComputeChParameter(*RN_Data,RN_Data);            //ת������ָ��
	}
  SendCmdReply(RN_Data,*RN_Data);
}
/******************************************************************
** ������: IsQueueRepeat
** ��  ��: ��
** ��  ��: �ж�ָ���Ƿ��ظ����������ظ����������ڵ���0��
**                                                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
void RunAbnormal(uint8_t *RA_Data)//���м�ͣ״̬
{
 ResetCmd(RA_Data);
 SendStatusReply(current_state);
}
/******************************************************************
** ������: IsQueueRepeat
** ��  ��: ��
** ��  ��: �ж�ָ���Ƿ��ظ����������ظ����������ڵ���0��
**                                                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
uint8_t DisUrgentState(void)//�Ǽ�ͣ״��������ʱ��
{
	return !urgstop;             //���ص�ǰ״̬�Ƿ���ͣ
}
/******************************************************************
** ������: IsQueueRepeat
** ��  ��: ��
** ��  ��: �ж�ָ���Ƿ��ظ����������ظ����������ڵ���0��
**                                                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-12
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
void ExecuteCmd(uint8_t *EC_Data)//������յ���ָ�� 
{
	if (DisUrgentState())         //���豸��������״̬ʱ���Ǽ�ͣ״̬��������������ģʽ����
		RunNormal(EC_Data);
	else                          //���豸����ͣ״̬�������м�ͣģʽ����
		RunAbnormal(EC_Data);
}
