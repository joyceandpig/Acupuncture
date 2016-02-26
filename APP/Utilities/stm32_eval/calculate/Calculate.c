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
#include "stm32f10x_bkp.h"



pFunction Jump_To_Application;

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

uint16_t	led_time_count;		           //ָʾ�Ƽ�ʱ��


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


static int8_t kAckBuf[4][14]={
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x00,0xFF,0xFF,0xFF,0xFF,0x65,0xFD},//����
{0x0E,0x00,0x02,0x00,0x01,0x01,0x05,0x01,0xFF,0xFF,0xFF,0xFF,0x12,0xC1},//�ظ�����
{0x0E,0x00,0x02,0x00,0x01,0x01,0x05,0x02,0xFF,0xFF,0xFF,0xFF,0x12,0x85},//��������
{0x0E,0x00,0x02,0x00,0x01,0x01,0x05,0x03,0xFF,0xFF,0xFF,0xFF,0xD2,0xB8},//ͨ���ж�
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
/**********************************************����״̬����*********************************/
void SendStatusReply(uint8_t type)
{
	uint8_t usart_send_byte_count = 0;

	for (; usart_send_byte_count < FRAME_SUM; usart_send_byte_count++){
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
/******************************�ж������Ƿ�Ϊ�ն���**********************/
void state_change(AckState state_now)
{
  current_state = state_now;
	CloseAllOutPut();         //�ر�����ͨ�����
	STM_LED2 = 1;
  
  if (ErrorCodeOverLimit == state_now)             /*****************��С��˸Ƶ��,��˸����**********/
  {
    LED_COUNT1 = LED_BASE_COUNT * LED_TWINKLE_SPEED;
    LED_COUNT2 = LED_COUNT1 * LED_TWINKLE_SPEED;
  }
  else if (/*OrderRepeatOverLimit == state_now*/TopDead == state_now)/******************������˸Ƶ�ʣ���˸���**********/
  {
    LED_COUNT1 = LED_BASE_COUNT / LED_TWINKLE_SPEED;
    LED_COUNT2 = LED_COUNT1 * LED_TWINKLE_SPEED;
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
	u8 i;
  if (*count < LED_COUNT1){		
		GPIO_LOW(GPIO_LED,LED_PORT);
		if(urgstop){
		STM_BELL_ON;
		}
	}else if ((LED_COUNT1 <= *count) && (*count < LED_COUNT2)){
		GPIO_HIGH(GPIO_LED,LED_PORT);
		if(urgstop){
		STM_BELL_OFF;
		}
	}
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
	Q->items[Q->rear] = 0;
	Q->rear = (Q->rear + 1)%CAPACITY;
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
	return Q->items[Q->rear];
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
   point = Q->rear;
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
/****************************************************************************/
void CloseChannelOutput(void)
{
  uint8_t CCO_cnt = 0;
  ReadyToUpdata();    //׼������DA
  for (;CCO_cnt < CH_SUM;CCO_cnt++)
  {
		SPI_SendData(CCO_cnt << ChannelOffset,0);// ���ιرո���ͨ����DA��ѹ���
		CH[CCO_cnt].channel_open = FALSE;					//�ر�ͨ������
		CH[CCO_cnt].led_sta = 0;
		STM_CHN_LED_OFF(CCO_cnt);                 //�رն�Ӧͨ��ָʾ��
		delay_ms(1);//��������ʱ����һ��mos�ܻᷢ�̣�ԭ�򣿣�����
	}
	UpdataDAOutput();	     // ����DA��ѹ���ֵ
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
void CloseAllOutPut(void)
{
  CloseChannelOutput();
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
/********************************************�ظ������***********************************/
uint8_t RepeatOrderDeal(uint8_t Rlength,uint8_t *Rdata)
{
	uint8_t rep_i,rep_si = 0,order_num = 0;

	for (rep_i = 0; rep_i < Rlength - DISDATA_SUM; rep_i++){  //���ϴ�ָ����бȶ�
		if (order[rep_i] == *(Rdata+DATA_STA+rep_i))order_num++;
	}

	if (order_num == Rlength - DISDATA_SUM){
		rep_count++;     																	//����ȫ����ͬ����Ϊָ���ظ�һ��
	}else {
		rep_count = 0;
	}

  if ((order[0] == 0x02) && (order[1] == 0x21))rep_count = 0;
	if ((order[0] == 0x03) && (order[1] == 0x01))rep_count = 0;

	if (rep_count > 0){
		RepPar.time_start = TRUE;	//�����ظ�������������ظ���ʱ
	}
	
	/*******************�ѱ��ν��յ�������뻺�������Թ��ж���һ��������ظ���**********/
		for (rep_si = 0; rep_si < (Rlength - DISDATA_SUM); rep_si++){
			order[rep_si] = *(Rdata+DATA_SUM+rep_si);
	}
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
void IsComplexStyle(uint16_t f_code)
{
	if(CH[ch_num].function_code   == 0x102 || CH[ch_num].function_code   == 0x103){//�ж������Ƿ�Ϊ���Ӳ���
		CH[ch_num].complex_status   = TRUE;
	}else{
		CH[ch_num].complex_status   = FALSE;
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
void DoOverLimit(uint8_t num)
{
	if (CH[num].scope > DA_SCOPE_UP_LIMIT)
		CH[num].scope   = DA_SCOPE_UP_LIMIT;//��DA��ѹ����޷�������DAоƬ�����ֵ1000��DA����û�����
	if (CH[num].peroid > PEROID_UP_LIMIT)
		CH[num].peroid  = PEROID_UP_LIMIT;//��ת��������ڽ������ƣ���ֹ���͵����ݳ���Ԥ��ֵ�����
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
void IsNeedleStyle(uint8_t *Idata)
{
	if (*(Idata+BIT9) == 1)//���ģʽ  �������Ӧ�ø�С
	{
		CH[ch_num].factor =NEEDLING_STYLE;
	}
	else
	{
		CH[ch_num].factor =PASTER_STYLE;
	}
}

/******************************************************************
** ������: TIMy_IRQHandler
** ��  ��: none
** ��  ��:��30������ʱ��ÿһ��λ��Ӧ�仯���Ƚϴ󣬳���30���󣬱仯�ʽ�С������������ת��ϵ����2.3�� 1.9
���е�128��ֵ����Ϊ�����л�������λ����ʱ�ĵ���
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:

** ��  ��: 1.0
j*******************************************************************/
void OutScopeCon(u8 ch_number ,u8 *Pdata)
{
	static uint8_t level_num = 0;
	level_num = *(Pdata+12);
	
	if(level_num==0){
		CH[ch_number].scope=0;                             //ѡ��0������Ӧ���Ϊ0
	}else if(level_num==100){
		CH[ch_number].scope=890*CH[ch_number].factor/4;//ѡ��100���������������ѡ��1000����500�����ֵ
	}else if(level_num<=10){												 //DA���376����Ӧ�������130V����
		CH[ch_number].scope = (100 + (CH[ch_number].factor * 6.9) * level_num) *CH[ch_number].factor/4;//��10������ʹ��һ���仯��
	}else {
		CH[ch_number].scope = (318 + (CH[ch_number].factor * 1.43) * level_num) *CH[ch_number].factor/4;//10-99��ʹ��һ���仯��
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
static void ConfigureChannelParamer(uint8_t channel_num,uint8_t *Pdata)
{
	static uint16_t channel_frequency = 0;
	
	CH[channel_num].led_sta = 1;
	CH[channel_num].channel_open       = TRUE;	
	CH[channel_num].channel_stop_state = FALSE;
	CH[channel_num].channel            = channel_num;
	IsComplexStyle(CH[channel_num].function_code);//�ж��Ƿ�Ϊ���Ӳ������ͣ����ܲ�����������
	IsNeedleStyle(Pdata);//ѡ���������

	channel_frequency         = (uint16_t)*(Pdata+BIT10) << ShiftOffset|(uint16_t)*(Pdata+BIT11);
	if(channel_frequency == 0){
		channel_frequency = 1;
	}
	CH[channel_num].peroid    = PEROID_COMPUTE_VALUE / channel_frequency;	   
	CH[channel_num].ss_peroid = WAVE_PEROID_MULTIPLE * CH[channel_num].peroid;
	
	OutScopeCon(channel_num,Pdata);//�����ѹ���ȵ���
	DoOverLimit(CH[channel_num].channel);//��Ƶ�ʺͷ��Ƚ����޷�����
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
static void UartClose(void)
{
    USART_Cmd(USART1, DISABLE);

    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);

    USART_ClearITPendingBit(USART1, USART_IT_TXE);
    USART_ClearITPendingBit(USART1, USART_IT_TC);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
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
static void TimClose(void)
{
    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM2, DISABLE);
	
	  TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM3, DISABLE);
	
	  TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM4, DISABLE);
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
static void Config_Close(void)
{
	UartClose();
	TimClose();
	CloseChannelOutput();
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
uint8_t Sector(uint16_t f_code)
{
	if(0x01 <= f_code && f_code < 0x0105){// ���ο���
		return 0;													
	}else if(0x200 < f_code && f_code < 0x223){// �迹����
		return 1;
	}else if(f_code == DownLoad_App){//�̼����¿���
		return 2;
	}else{
		return 3;//����ָ�����������
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
void DoUpdata(void)
{
		u32 JumpAddress;
	
		FLASH_Unlock();
		FLASH_ErasePage(IsUpdata);
		FLASH_ProgramHalfWord(IsUpdata,0x01); //��Ӹ���IAP��ʶ�� С��ģʽ����λ��ǰ
	
		FLASH_ProgramHalfWord(BaudRateReadAddr,(usart_baud>>16)&0xffff);
		FLASH_ProgramHalfWord(BaudRateReadAddr+2,usart_baud&0xffff);

	
		FLASH_Lock();
		Config_Close();
		
		JumpAddress = *(vu32*) (JumpBootAddress + 4);  //the address value	
		Jump_To_Application = (pFunction) JumpAddress;/* Jump to user application */		
		__set_MSP(*(vu32*) JumpBootAddress);/* Initialize user application's Stack Pointer */
		Jump_To_Application();
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
uint8_t DoWave(uint16_t f_code,uint8_t *Cdata)
{
	ch_num = *(Cdata+BIT8)-1;
	if(f_code == Stop)
	{
		CH[ch_num].channel_open = FALSE;
	  CH[ch_num].channel_stop_state = TRUE;
		CH[ch_num].led_count = 0;
		CH[ch_num].led_sta = 0;
		STM_CHN_LED_OFF(ch_num);
		return 0;
	}
	ResetChannelParamer(ch_num);             //���ö�Ӧͨ���ļ���ֵ
	if ((*Cdata == 0x0F) && (ch_num < 6)) //��Ⲩ�ο��Ƶ�֡���Ƿ���ȷ,ͨ�����Ƿ�Ϊ0-5��
	{                 
		CH[ch_num].function_code = f_code;     //��ֵ��������
		CH[ch_num].led_count = 0;
		ConfigureChannelParamer(ch_num,Cdata);    //����ͨ�����εĲ���
	}
	return 1;
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
void ComputeChParameter(uint8_t Clength,uint8_t *Cdata)
{
  static uint16_t func_code;

	func_code = (uint16_t)(*(Cdata+BIT6)<<ShiftOffset)|(uint16_t)*(Cdata+BIT7);
	switch(Sector(func_code))
	{
		case 0:DoWave(func_code,Cdata);break;
		case 1:ImpVal(func_code,Cdata);break;
		case 2:DoUpdata();break;
		case 3:break;
		default:break;
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
  if ((Q->items[Q->rear] >= 14)&&(Q->items[Q->rear] <= PRETREATSIZE)\
		&& (Q->items[Q->rear] <= Q->size) && (Q->size >= 14))
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
	len = Q->items[Q->rear];

  while(len--)
  {
    DeQueue(Q);
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
void JudgeComplete(SqQueue *Q,uint8_t *IsGetWholeData)
{
	if (Q->size ==0) 
	{
		*IsGetWholeData = 0;       						//�ȴ���������ȫ��������ȫ
		Q->front = Q->rear;
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
void DoCheckCodeRight(SqQueue *Q,  uint8_t *IsGetWholeData)
{
		RemoveFrameFromQueue(Q);                              //��У����ȷ����Ѷ����и�֡�����޳���
		system_start = 1;                    												//��ʾϵͳ��ʼ����ָ��                             
		JudgeComplete(Q,IsGetWholeData);
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
void DoCheckCodeERR(SqQueue *Q,  uint8_t *IsGetWholeData)
{
		ErrPar.time_start = TRUE;                           //���������ʱ����
		errcode_count++;
		DeQueue(Q);                                        //�����������ɾ����֡ͷ�����ݣ�Ȼ�������ν���У����ж�
		JudgeComplete(Q,IsGetWholeData);
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
uint8_t DoJudgeCMDInfo(SqQueue *Q,  uint8_t *IsGetWholeData)
{
		if (IsCheckCodeRight(PretreatBuffer))                   //У������ȷ
		{
			heart_time_count = 0;              //���յ���ȷ���ݺ���������������
			TIMy_timecount = 0;
			DoCheckCodeRight(Q,IsGetWholeData);
			return TRUE;
		}
		else                                                  //��У�������˵��֡ͷ��ʾ��һ֡���ݲ���ȷ��
		{
			DoCheckCodeERR(Q,IsGetWholeData);
			return FALSE;
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
  if (*IsGetWholeData){                												 //�Ƿ���յ���������
		Q->size = (Q->front - Q->rear+CAPACITY)%CAPACITY;
		if (IsFrameLenRight(Q)){                                     //֡�ڵ�֡�����ݲ�����	
			memset(PretreatBuffer,'\0',PRETREATSIZE);
			GetDataFromQueue(Q,(Q->items[Q->rear]),PretreatBuffer);//�����е����ݷ���ָ�����飬
 			if(DoJudgeCMDInfo(Q,IsGetWholeData)){
				return TRUE;
			}
		}
		else{
			DeQueue(Q);                                       		//��֡�����ޣ���ȥ��֡��λ���������и����ж�     
			JudgeComplete(Q,IsGetWholeData);
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
	static uint16_t fun_code;
	
	fun_code = (uint16_t)(*(Re_Data+BIT6)<<ShiftOffset)|(uint16_t)*(Re_Data+BIT7); 
//	if (fun_code == Reset)                    //�������Ϊ��λ
	{
		urgstop = FALSE;
		system_start = TRUE;
		state_change(Normal);
		STM_BELL_OFF;
		delay_ms(1);
		STM_LED4 = 1;	
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
//	SendCmdReply(RA_Data,*RA_Data);
// SendStatusReply(current_state);
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
//	if (DisUrgentState())         //���豸��������״̬ʱ���Ǽ�ͣ״̬��������������ģʽ����
//		RunNormal(EC_Data);
//	else                          //���豸����ͣ״̬�������м�ͣģʽ����
//	{
//		RunAbnormal(EC_Data);
//	  RunNormal(EC_Data);
//	}
	if(urgstop){
		RunAbnormal(EC_Data);
	}
	RunNormal(EC_Data);
	
}
