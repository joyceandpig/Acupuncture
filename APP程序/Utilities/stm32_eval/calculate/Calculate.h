/**
  ******************************************************************************
  * @file    USART/Interrupt/stm32f10x_it.h 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   This file contains the headers of the interrupt handlers.
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
#ifndef _CALCULATE_H_
#define _CALCULATE_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "platform_config.h"
#include "AD5933.h"
#include "configure_init.h"
#include "stm32_eval.h"
#include "AD5933.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

typedef  void (*pFunction)(void);
typedef struct
{
  int16_t   size;
	uint16_t front,rear;
	uint8_t  items[CAPACITY];
}SqQueue;


typedef struct
{
	uint16_t function_code,
    			 peroid,
    			 ss_peroid,
    			 scope,
    			 wave_count,
					 led_count;
	uint8_t  channel,
    			 channel_open,
    			 get_new_data,
    			 channel_stop_state,
    			 base_wave_stay,
    			 Iwave_count_flag,		   	
    			 Dwave_count_flag,
    			 time5s_count,
           complex_status,
					 led_sta,
					 factor
}ChannelParameter;


typedef struct
{
	uint8_t  TimeFlag,
			     StartFlag,
			     DelayEndFlag,
			     DelayLoseFlag;
	uint16_t TimeCount;
}Heart;


typedef struct
{
	uint8_t  time_start,
			     time_limit;
	uint16_t time_count;
}RepeatOrder;


typedef struct
{
	uint8_t  time_start,
			     time_limit;
	uint16_t time_count;
}ErrorCode;
typedef struct
{
  uint8_t  time_start,
           time_limit,
           in_span;
  uint16_t time_count;
}Impval;

enum Function
{
	Contious      = 0x0101,   //������������
	Interrupt     = 0x0102,   //������������
	Dilatational  = 0x0103,   //���ܲ�������
	Stop          = 0x0104,  	//����ͨ��ֹͣ�������
  Reset         = 0x0105,   //��λ����
	DownLoad_App  = 0x0106,   //download the new app
	Run_NewApp,               //run the download app
	
	HeartB        = 0x0301,   //������⹦����
	Statement     = 0x0302,   //״̬���湦����


  ImpClk        = 0x0201,   //ʱ��
  ImpConCH      = 0x0202,   //�ο�����ͨ��
  ImpInpSign    = 0x0203,   //�����ź�
  ImpPGA        = 0x0204,   //PGA
  ImpInitFre    = 0x0205,   //��ʼƵ��
  ImpAddFre     = 0x0206,   //Ƶ������
  ImpNumAddF    = 0x0207,   //Ƶ��������
  BuildTime     = 0x0208,   //����ʱ��

  ImpMeaStart   = 0x0210,   //��ʼ�迹����
  ImpMeaStop    = 0x0211,   //ֹͣ�迹����
  ImpMeaAddFre  = 0x0212,   //����Ƶ�ʲ���
  ImpMeaReFre   = 0x0213,   //�ظ�Ƶ�ʲ���
  ImpMeaTemrt   = 0x0214,   //�����¶�
  ImpPowerSave  = 0x0215,   //ʡ��ģʽ
  ImpIdleMode   = 0x0216,   //����ģʽ

  ImpReadImp    = 0x0221,   //��ȡ�迹ֵ��ʵ�����鲿��
  ImpReadTempr  = 0x0222,   //��ȡ�����¶�

};

typedef enum
{ 
  Normal,          	            //����״̬
	OrderRepeatOverLimit,         //�ظ���������		  
	ErrorCodeOverLimit,    	      //���������������	 
	TopDead,             	        //��λʱ���ڴ���δͨ�ųɹ�(�ϲ�����)
}AckState;

enum DataFrame
{
	/* ��������֡�ĸ����εĶ�ӦBITλ*/
	BIT0,
	BIT1,
	BIT2,
	BIT3,
	BIT4,
	BIT5,
	BIT6,
	BIT7,
	BIT8,
	BIT9,
	BIT10,
	BIT11,
	BIT12,
	BIT13,
	BIT14
};
typedef enum 
{
  Led1 = 0x0010,//4
  Led2 = 0x0020,//5
  Led3 = 0x0100,//8
  Led4 = 0x0200//9
}LED_TypeDef;

extern uint8_t	current_state,						 //��ǰ״̬ 
        rep_count,			  //�ظ�����������
	      errcode_count,              //����������
        state_report_flag,
		    urgstop;		//����ֹͣλ


static uint8_t order[DATA_SUM],//ͨ�������ظ���������
			         ch_num,				//ͨ���ű���
			         count;								 //ѭ����������
			   
extern uint16_t LED_COUNT1,
         LED_COUNT2,
         report_time5s,
         led_time_count,
         heart_time_count;
extern uint8_t system_start,topdeath;
extern uint8_t get_whole_data;

extern SqQueue Queue;
extern ChannelParameter CH[CH_SUM];
extern Heart HeartBeat;
extern RepeatOrder RepPar;
extern ErrorCode ErrPar;
extern Impval ImpPar;
extern uint8_t PretreatBuffer[PRETREATSIZE];

static int8_t kAckBuf[4][14];
void state_change(AckState state_now);
void LightLed(uint16_t *count,LED_TypeDef LED_PORT);
static uint8_t IsFull(SqQueue *Q);
static uint8_t IsEmpty(SqQueue *Q);
extern uint8_t EnQueue(SqQueue *Q, uint8_t data);
extern uint8_t DeQueue(SqQueue *Q);
uint8_t GetFrontDataFromQueue(SqQueue *Q);
uint8_t GetLengthOfQueue(SqQueue *Q);
uint8_t ShiftFrontOfQueue(SqQueue *Q);
uint8_t GetDataFromQueue(SqQueue *Q,uint8_t length_of_frame,uint8_t *data_of_frame);

static void ClosechannelOutput(void);
void CloseAllOutPut(void);
extern uint8_t RepeatOrderDeal(uint8_t Rlength,uint8_t *Rdata);
extern void ComputeChParameter(uint8_t Clength,uint8_t *Cdata);
uint16_t Arry_Get_CRC16(uint8_t *start_pos, uint8_t usDataLen);
uint8_t CheckOutAndCompute(uint8_t Clength, uint8_t *Cdata);
extern uint8_t WorkQueueData(SqQueue *Q, uint8_t *IsGetWholeData);
static void HeartBeatTimeOut(void);
static void GetNewDataCon(void);
static void wave_output(void);
void WaveWork(void);
extern void ExecuteCmd(uint8_t *EC_Data);//������յ���ָ�


#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
