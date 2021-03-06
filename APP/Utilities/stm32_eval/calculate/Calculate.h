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
	Contious      = 0x0101,   //连续波功能码
	Interrupt     = 0x0102,   //断续波功能码
	Dilatational  = 0x0103,   //疏密波功能码
	Stop          = 0x0104,  	//单个通道停止命令功能码
  Reset         = 0x0105,   //复位命令
	DownLoad_App  = 0x0106,   //download the new app
	Run_NewApp,               //run the download app
	
	HeartB        = 0x0301,   //心跳检测功能码
	Statement     = 0x0302,   //状态报告功能码


  ImpClk        = 0x0201,   //时钟
  ImpConCH      = 0x0202,   //参考电阻通道
  ImpInpSign    = 0x0203,   //激励信号
  ImpPGA        = 0x0204,   //PGA
  ImpInitFre    = 0x0205,   //起始频率
  ImpAddFre     = 0x0206,   //频率增量
  ImpNumAddF    = 0x0207,   //频率增量数
  BuildTime     = 0x0208,   //建立时间

  ImpMeaStart   = 0x0210,   //开始阻抗测量
  ImpMeaStop    = 0x0211,   //停止阻抗测量
  ImpMeaAddFre  = 0x0212,   //递增频率测量
  ImpMeaReFre   = 0x0213,   //重复频率测量
  ImpMeaTemrt   = 0x0214,   //测量温度
  ImpPowerSave  = 0x0215,   //省电模式
  ImpIdleMode   = 0x0216,   //待机模式

  ImpReadImp    = 0x0221,   //读取阻抗值（实部和虚部）
  ImpReadTempr  = 0x0222,   //读取环境温度

};

typedef enum
{ 
  Normal,          	            //正常状态
	OrderRepeatOverLimit,         //重复次数超限		  
	ErrorCodeOverLimit,    	      //错误命令次数超限	 
	TopDead,             	        //单位时间内串口未通信成功(上层死机)
}AckState;

enum DataFrame
{
	/* 接收数据帧的各个段的对应BIT位*/
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

extern uint8_t	current_state,						 //当前状态 
        rep_count,			  //重复次数计数器
	      errcode_count,              //错误代码次数
        state_report_flag,
		    urgstop;		//紧急停止位


static uint8_t order[DATA_SUM],//通道命令重复次数数组
			         ch_num,				//通道号变量
			         count;								 //循环计数变量
			   
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
 void wave_output(void);
void WaveWork(void);
extern void ExecuteCmd(uint8_t *EC_Data);//处理接收到的指�


#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
