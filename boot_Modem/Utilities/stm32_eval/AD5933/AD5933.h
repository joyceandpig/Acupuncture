#ifndef __AD5933_H
#define __AD5933_H

#ifdef __cplusplus
 extern "C" { 
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval_iic.h"
#include "stm32_eval_SPI.h"
#include "Calculate.h"
#include "platform_config.h"


extern signed short int real_Val;//实部值
extern signed short int imag_Val;//虚部值
extern unsigned char ConsultVal;//参考电阻值

extern float  GainFactor;//增益系数

typedef struct
{
  unsigned char     ADG714_CHANNEL_NUM;         //参考电阻通道号   1--8
  unsigned char     USE_PGAx5;                  //启用5倍PGA       1:启用  0:不启用
  unsigned char     USE_EXTERNAL_CLOCK;         //启用外部时钟     1:启用  0:不启用

  unsigned char     CONTROL_REG_BIT10_TO_BIT9;  //激励信号的控制位    0-3  0->2vpp  1->0.2vpp  2->0.4vpp  3->1vpp

  unsigned char     CONTROL_REG_BIT3;           //外部时钟控制位     1:外部时钟  0：内部时钟
  unsigned char     CONTROL_REG_BIT8;           //PGA控制位          1：不启用5倍PGA  0：启用5倍PGA

  unsigned char     INC_AND_REP_FRE;            //测量时频率递增或重复   0：递增   1：重复

  unsigned long int AD5933_CLK;                 //时钟频率
  unsigned long int START_FREQUENCY;            //起始频率
  unsigned long int INC_FREQUENCY;              //频率增量
  unsigned long int BUILD_TIME_CYCLES;          //建立周期时间
  unsigned long int FREQUENCY_INC_NUM;          //频率增量数
  unsigned long int OUTPUT_VOLTAGE;             //激励信号


}Impe;
extern Impe AD5933_Para;



//#define AUTO                                //选择自动更改参考电阻

//#define USE_PGAx5                       		/*使用5倍增益，PGAx5*/

/*
	输出电压典型值，
	确保下面中的一个被选择！！！
*/        
//#define OUTPUT_VOLTAGE2000           				 	/*输出电压典型值，2.0Vp-p*/
//#define OUTPUT_VOLTAGE1000            				/*输出电压典型值，1.0Vp-p*/         
//#define OUTPUT_VOLTAGE400           				 	/*输出电压典型值，400mVp-p*/           
//#define OUTPUT_VOLTAGE200           				 	/*输出电压典型值，200mVp-p*/ 

//#if (defined(OUTPUT_VOLTAGE2000) + defined(OUTPUT_VOLTAGE1000) + \
//     defined(OUTPUT_VOLTAGE400)  + defined(OUTPUT_VOLTAGE200) != 1 )
//  #error "Please Right Choise AD5933 OUTPUT_VOLTAGE!!!"
//#endif 
   
//#if defined(USE_EXTERNAL_CLOCK)
//	#define AD5933_CLK	12000000UL							/*MCLK时钟频率,单位HZ.*/
//#else
//	#define AD5933_CLK	16776000UL						 	/*内部时钟频率,单位HZ.*/
//#endif


//#define START_FREQUENCY							50000UL		/*输出起始频率,单位Hz.*/
#define START_FREQUENCY_CODE(x,y)		(unsigned long int)((x)*((1<<27)/((y>>2)*1.0)))
																							/*计算起始频率码，输入参数：起始频率，单位Hz*/

//#define INC_FREQUENCY								2UL			 /*输出增量频率,单位Hz.*/
#define FREQUENCY_INC_CODE(x,y)			(unsigned long int)((x)*((1<<27)/((y>>2)*1.0)))
																						  /*计算频率增量码，输入参数：增量频率，单位Hz*/

//#define BUILD_TIME_CYCLES           15       	/*建立时间周期数，最大值2044*/

#define BUILD_TIME_CYCLES_MAX       2044      /*建立时间周期数最大值*/

//#if	(BUILD_TIME_CYCLES > BUILD_TIME_CYCLES_MAX)
//	#error	"BUILD_TIME_CYCLES More than the numbe 2044. "
//#endif

//#define FREQUENCY_INC_NUM						100				/*频率增量数，最大值511*/

#define FREQUENCY_INC_NUM_MAX				511				/*频率增量数最大值*/

//#if	(FREQUENCY_INC_NUM > FREQUENCY_INC_NUM_MAX)
//	#error	"FREQUENCY_INC_NUM More than the numbe 511. "
//#endif


/** @定义AD5933寄存器
  * @{
  */

//Control Register
#define CONTROL_H_REG								0x80			/*控制寄存器D15-D8 */
#define CONTROL_L_REG								0x81			/*控制寄存器D7 -D0 */
											
//Start Frequency (24 Bits)
#define START_FREQUENCY_H_REG				0x82			/*起始频率寄存器D23-D16*/
#define START_FREQUENCY_M_REG				0x83			/*起始频率寄存器D15-D8 */
#define START_FREQUENCY_L_REG				0x84			/*起始频率寄存器D7 -D0 */

//Frequency Increment Word (24 Bits)
#define FREQUENCY_INC_H_REG					0x85			/*频率增量寄存器D23-D16*/
#define FREQUENCY_INC_M_REG					0x86			/*频率增量寄存器D15-D8 */
#define FREQUENCY_INC_L_REG					0x87			/*频率增量寄存器D7 -D0 */

//No. of frequency increments(9Bits)
//Bits D15-D9 = Don’t care 
//Bits D8-D0= number of frequency increments.
#define FREQUENCY_INC_NUM_H_REG			0x88			/*频率增量数寄存器D15-D8 */
#define FREQUENCY_INC_NUM_L_REG			0x89			/*频率增量数寄存器D7 -D0 */

//Settling time Cycles (16 Bits) 
//D15 – D11= Don’t care 	 D10 –D9 = 2 bit decode 	D8-D0 = number of cycles 
//D10 	D9 				description
//0 		0					Default 
//0 		1 				number of cycles x2 
//1 		0 				reserved 
//1 		1	 				number of cycles x4
#define BUILD_TIME_CYCLES_H_REG			0x8A			/*建立时间周期数寄存器D15-D8 */
#define BUILD_TIME_CYCLES_L_REG		  0x8B			/*建立时间周期数寄存器D7 -D0 */

//Leakage limit for test
#define TEST_A_REG									0x8C			/*测试寄存器A*/ 
#define TEST_B_REG									0x8D			/*测试寄存器A*/ 
#define TEST_C_REG									0x8E			/*测试寄存器A*/ 

//Status Register 
#define STATUS_REG									0x8F			/*状态寄存器*/ 

//Reserve
#define Reserve0_REG								0x90			/*保留0*/
#define Reserve1_REG								0x91			/*保留1*/

//Temperature Data Register
#define TEMP_DATA_H_REG							0x92			/*温度数据寄存器D15-D8 */
#define TEMP_DATA_L_REG							0x93			/*温度数据寄存器D7 -D0 */

//Real Data 
#define REAL_DATA_H_REG							0x94			/*实部数据寄存器D15-D8 */
#define REAL_DATA_L_REG							0x95			/*实部数据寄存器D7 -D0 */

//Imaginary Data
#define IMAG_DATA_H_REG							0x96			/*虚部数据寄存器D15-D8 */
#define IMAG_DATA_L_REG							0x97			/*虚部数据寄存器D7 -D0 */

//Checksum
#define CHECK_SUM_REG								0x98			/*校验和数据寄存器D7-D0 */

/**
  * @}
  */ 

/** @控制寄存器相关定义
  * @{
  */

/** @控制寄存器，命令区指令宏
  * @{
  */

#define NO_OPERAT0												0					/*无操作*/
#define COMM_START_FREQUENCY_INIT        	1					/*以起始频率初始化*/
#define COMM_START_FREQUENCY_SCAN        	2					/*启动频率扫描*/
#define COMM_INC_FREQUENCY               	3					/*递增频率*/
#define COMM_REPEAT_FREQUENCY            	4					/*重复频率*/
#define NO_OPERAT1												8					/*无操作*/
#define COMM_TEMP_MEAS                   	9					/*测量温度*/
#define COMM_POWER_SAVE                 	10				/*省电模式*/ 
#define COMM_STANDBY_MODE                	11				/*待机模式*/
#define NO_OPERAT2												12				/*无操作*/
#define NO_OPERAT3												13				/*无操作*/

/**
  * @}
  */    
       
//#if defined(OUTPUT_VOLTAGE2000)
//  #define CONTROL_REG_BIT10_TO_BIT9 	0
//#elif defined(OUTPUT_VOLTAGE1000)
//  #define CONTROL_REG_BIT10_TO_BIT9 	3
//#elif defined(OUTPUT_VOLTAGE400)
//  #define CONTROL_REG_BIT10_TO_BIT9 	2
//#elif defined(OUTPUT_VOLTAGE200)
//  #define CONTROL_REG_BIT10_TO_BIT9 	1
//#endif

//#if defined(USE_EXTERNAL_CLOCK)
//  #define CONTROL_REG_BIT3          	1				 /*外部时钟*/           
//#else
//  #define CONTROL_REG_BIT3          	0				 /*内部时钟*/ 
//#endif

//#if defined(USE_PGAx5)
//  #define CONTROL_REG_BIT8            0				 /*5倍增益，PGAx5*/
//#else
//  #define CONTROL_REG_BIT8            1				 /*1倍增益，PGAx1*/
//#endif  

//合成控制寄存器高字节
#define CONTROL_H_REG_MAKE(Comm,x,y)				((Comm<<4)  |  (x<<1) | y)
#define Compl_TO_TrueForm(NUM)					(NUM & 0x8000) ? (((~NUM) + 1) * (-1)) : (NUM)//补码转原码																	                                																					 
/**
  * @}
  */

typedef enum                                        //参考电阻通道选择值定义
{
  Channel_1 = 0x01,                                 // 47R
  Channel_2 = 0x02,                                 // 470R
  Channel_3 = 0x04,                                 // 1K
  Channel_4 = 0x08,                                 // 10K
  Channel_5 = 0x10,                                 // 47K
  Channel_6 = 0x20,                                 // 100K
  Channel_7 = 0x40,                                 // 1M
  Channel_8 = 0x80                                  // 4M
} ChannelType;


void AD5933_Init(void);                 				    //初始化
static void AD5933_Reset(void);                     //复位，频率扫描必须的
static void AD5933_StartFrequency_Init(void);       //起始频率初始化
static void AD5933_SetBuildTimeCycles(void);        //设置建立时间周期
static void AD5933_StartFrequency_Scan(void);       //起始频率扫描
static void AD5933_StandbyMode(void);               //进入待机模式

void AD5933_TempMeas(void);                         //温度测量
void AD5933_PowerSave(void);                        //省电模式，测量完成后挂起
void AD5933_IncFrequency(void);                     //增量频率增量后进行测量
void AD5933_RepeatFrequency(void);                  //重复当前频率进行测量

void AD5933_StartImpMeasure(void);                  //启动阻抗频率扫描测量

unsigned int AD5933_GetStatus(void);    			    	//读取测量状态
unsigned int AD5933_GetTemperatureMeasureVal(void); //读取温度测量值
signed short int AD5933_GetImpMeasure_ImagVal(void);//读取阻抗虚部测量值
signed short int AD5933_GetImpMeasure_RealVal(void);//读取阻抗实部测量值

void AD5933_ConsultRChoice(ChannelType Ch);                //参考阻抗通道选择函数

unsigned long int Test(void);   


#ifdef __cplusplus
}
#endif

#endif

/**
  * @}
  */  

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

