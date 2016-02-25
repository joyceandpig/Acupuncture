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


extern signed short int real_Val;//ʵ��ֵ
extern signed short int imag_Val;//�鲿ֵ
extern unsigned char ConsultVal;//�ο�����ֵ

extern float  GainFactor;//����ϵ��

typedef struct
{
  unsigned char     ADG714_CHANNEL_NUM;         //�ο�����ͨ����   1--8
  unsigned char     USE_PGAx5;                  //����5��PGA       1:����  0:������
  unsigned char     USE_EXTERNAL_CLOCK;         //�����ⲿʱ��     1:����  0:������

  unsigned char     CONTROL_REG_BIT10_TO_BIT9;  //�����źŵĿ���λ    0-3  0->2vpp  1->0.2vpp  2->0.4vpp  3->1vpp

  unsigned char     CONTROL_REG_BIT3;           //�ⲿʱ�ӿ���λ     1:�ⲿʱ��  0���ڲ�ʱ��
  unsigned char     CONTROL_REG_BIT8;           //PGA����λ          1��������5��PGA  0������5��PGA

  unsigned char     INC_AND_REP_FRE;            //����ʱƵ�ʵ������ظ�   0������   1���ظ�

  unsigned long int AD5933_CLK;                 //ʱ��Ƶ��
  unsigned long int START_FREQUENCY;            //��ʼƵ��
  unsigned long int INC_FREQUENCY;              //Ƶ������
  unsigned long int BUILD_TIME_CYCLES;          //��������ʱ��
  unsigned long int FREQUENCY_INC_NUM;          //Ƶ��������
  unsigned long int OUTPUT_VOLTAGE;             //�����ź�


}Impe;
extern Impe AD5933_Para;



//#define AUTO                                //ѡ���Զ����Ĳο�����

//#define USE_PGAx5                       		/*ʹ��5�����棬PGAx5*/

/*
	�����ѹ����ֵ��
	ȷ�������е�һ����ѡ�񣡣���
*/        
//#define OUTPUT_VOLTAGE2000           				 	/*�����ѹ����ֵ��2.0Vp-p*/
//#define OUTPUT_VOLTAGE1000            				/*�����ѹ����ֵ��1.0Vp-p*/         
//#define OUTPUT_VOLTAGE400           				 	/*�����ѹ����ֵ��400mVp-p*/           
//#define OUTPUT_VOLTAGE200           				 	/*�����ѹ����ֵ��200mVp-p*/ 

//#if (defined(OUTPUT_VOLTAGE2000) + defined(OUTPUT_VOLTAGE1000) + \
//     defined(OUTPUT_VOLTAGE400)  + defined(OUTPUT_VOLTAGE200) != 1 )
//  #error "Please Right Choise AD5933 OUTPUT_VOLTAGE!!!"
//#endif 
   
//#if defined(USE_EXTERNAL_CLOCK)
//	#define AD5933_CLK	12000000UL							/*MCLKʱ��Ƶ��,��λHZ.*/
//#else
//	#define AD5933_CLK	16776000UL						 	/*�ڲ�ʱ��Ƶ��,��λHZ.*/
//#endif


//#define START_FREQUENCY							50000UL		/*�����ʼƵ��,��λHz.*/
#define START_FREQUENCY_CODE(x,y)		(unsigned long int)((x)*((1<<27)/((y>>2)*1.0)))
																							/*������ʼƵ���룬�����������ʼƵ�ʣ���λHz*/

//#define INC_FREQUENCY								2UL			 /*�������Ƶ��,��λHz.*/
#define FREQUENCY_INC_CODE(x,y)			(unsigned long int)((x)*((1<<27)/((y>>2)*1.0)))
																						  /*����Ƶ�������룬�������������Ƶ�ʣ���λHz*/

//#define BUILD_TIME_CYCLES           15       	/*����ʱ�������������ֵ2044*/

#define BUILD_TIME_CYCLES_MAX       2044      /*����ʱ�����������ֵ*/

//#if	(BUILD_TIME_CYCLES > BUILD_TIME_CYCLES_MAX)
//	#error	"BUILD_TIME_CYCLES More than the numbe 2044. "
//#endif

//#define FREQUENCY_INC_NUM						100				/*Ƶ�������������ֵ511*/

#define FREQUENCY_INC_NUM_MAX				511				/*Ƶ�����������ֵ*/

//#if	(FREQUENCY_INC_NUM > FREQUENCY_INC_NUM_MAX)
//	#error	"FREQUENCY_INC_NUM More than the numbe 511. "
//#endif


/** @����AD5933�Ĵ���
  * @{
  */

//Control Register
#define CONTROL_H_REG								0x80			/*���ƼĴ���D15-D8 */
#define CONTROL_L_REG								0x81			/*���ƼĴ���D7 -D0 */
											
//Start Frequency (24 Bits)
#define START_FREQUENCY_H_REG				0x82			/*��ʼƵ�ʼĴ���D23-D16*/
#define START_FREQUENCY_M_REG				0x83			/*��ʼƵ�ʼĴ���D15-D8 */
#define START_FREQUENCY_L_REG				0x84			/*��ʼƵ�ʼĴ���D7 -D0 */

//Frequency Increment Word (24 Bits)
#define FREQUENCY_INC_H_REG					0x85			/*Ƶ�������Ĵ���D23-D16*/
#define FREQUENCY_INC_M_REG					0x86			/*Ƶ�������Ĵ���D15-D8 */
#define FREQUENCY_INC_L_REG					0x87			/*Ƶ�������Ĵ���D7 -D0 */

//No. of frequency increments(9Bits)
//Bits D15-D9 = Don��t care 
//Bits D8-D0= number of frequency increments.
#define FREQUENCY_INC_NUM_H_REG			0x88			/*Ƶ���������Ĵ���D15-D8 */
#define FREQUENCY_INC_NUM_L_REG			0x89			/*Ƶ���������Ĵ���D7 -D0 */

//Settling time Cycles (16 Bits) 
//D15 �C D11= Don��t care 	 D10 �CD9 = 2 bit decode 	D8-D0 = number of cycles 
//D10 	D9 				description
//0 		0					Default 
//0 		1 				number of cycles x2 
//1 		0 				reserved 
//1 		1	 				number of cycles x4
#define BUILD_TIME_CYCLES_H_REG			0x8A			/*����ʱ���������Ĵ���D15-D8 */
#define BUILD_TIME_CYCLES_L_REG		  0x8B			/*����ʱ���������Ĵ���D7 -D0 */

//Leakage limit for test
#define TEST_A_REG									0x8C			/*���ԼĴ���A*/ 
#define TEST_B_REG									0x8D			/*���ԼĴ���A*/ 
#define TEST_C_REG									0x8E			/*���ԼĴ���A*/ 

//Status Register 
#define STATUS_REG									0x8F			/*״̬�Ĵ���*/ 

//Reserve
#define Reserve0_REG								0x90			/*����0*/
#define Reserve1_REG								0x91			/*����1*/

//Temperature Data Register
#define TEMP_DATA_H_REG							0x92			/*�¶����ݼĴ���D15-D8 */
#define TEMP_DATA_L_REG							0x93			/*�¶����ݼĴ���D7 -D0 */

//Real Data 
#define REAL_DATA_H_REG							0x94			/*ʵ�����ݼĴ���D15-D8 */
#define REAL_DATA_L_REG							0x95			/*ʵ�����ݼĴ���D7 -D0 */

//Imaginary Data
#define IMAG_DATA_H_REG							0x96			/*�鲿���ݼĴ���D15-D8 */
#define IMAG_DATA_L_REG							0x97			/*�鲿���ݼĴ���D7 -D0 */

//Checksum
#define CHECK_SUM_REG								0x98			/*У������ݼĴ���D7-D0 */

/**
  * @}
  */ 

/** @���ƼĴ�����ض���
  * @{
  */

/** @���ƼĴ�����������ָ���
  * @{
  */

#define NO_OPERAT0												0					/*�޲���*/
#define COMM_START_FREQUENCY_INIT        	1					/*����ʼƵ�ʳ�ʼ��*/
#define COMM_START_FREQUENCY_SCAN        	2					/*����Ƶ��ɨ��*/
#define COMM_INC_FREQUENCY               	3					/*����Ƶ��*/
#define COMM_REPEAT_FREQUENCY            	4					/*�ظ�Ƶ��*/
#define NO_OPERAT1												8					/*�޲���*/
#define COMM_TEMP_MEAS                   	9					/*�����¶�*/
#define COMM_POWER_SAVE                 	10				/*ʡ��ģʽ*/ 
#define COMM_STANDBY_MODE                	11				/*����ģʽ*/
#define NO_OPERAT2												12				/*�޲���*/
#define NO_OPERAT3												13				/*�޲���*/

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
//  #define CONTROL_REG_BIT3          	1				 /*�ⲿʱ��*/           
//#else
//  #define CONTROL_REG_BIT3          	0				 /*�ڲ�ʱ��*/ 
//#endif

//#if defined(USE_PGAx5)
//  #define CONTROL_REG_BIT8            0				 /*5�����棬PGAx5*/
//#else
//  #define CONTROL_REG_BIT8            1				 /*1�����棬PGAx1*/
//#endif  

//�ϳɿ��ƼĴ������ֽ�
#define CONTROL_H_REG_MAKE(Comm,x,y)				((Comm<<4)  |  (x<<1) | y)
#define Compl_TO_TrueForm(NUM)					(NUM & 0x8000) ? (((~NUM) + 1) * (-1)) : (NUM)//����תԭ��																	                                																					 
/**
  * @}
  */

typedef enum                                        //�ο�����ͨ��ѡ��ֵ����
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


void AD5933_Init(void);                 				    //��ʼ��
static void AD5933_Reset(void);                     //��λ��Ƶ��ɨ������
static void AD5933_StartFrequency_Init(void);       //��ʼƵ�ʳ�ʼ��
static void AD5933_SetBuildTimeCycles(void);        //���ý���ʱ������
static void AD5933_StartFrequency_Scan(void);       //��ʼƵ��ɨ��
static void AD5933_StandbyMode(void);               //�������ģʽ

void AD5933_TempMeas(void);                         //�¶Ȳ���
void AD5933_PowerSave(void);                        //ʡ��ģʽ��������ɺ����
void AD5933_IncFrequency(void);                     //����Ƶ����������в���
void AD5933_RepeatFrequency(void);                  //�ظ���ǰƵ�ʽ��в���

void AD5933_StartImpMeasure(void);                  //�����迹Ƶ��ɨ�����

unsigned int AD5933_GetStatus(void);    			    	//��ȡ����״̬
unsigned int AD5933_GetTemperatureMeasureVal(void); //��ȡ�¶Ȳ���ֵ
signed short int AD5933_GetImpMeasure_ImagVal(void);//��ȡ�迹�鲿����ֵ
signed short int AD5933_GetImpMeasure_RealVal(void);//��ȡ�迹ʵ������ֵ

void AD5933_ConsultRChoice(ChannelType Ch);                //�ο��迹ͨ��ѡ����

unsigned long int Test(void);   


#ifdef __cplusplus
}
#endif

#endif

/**
  * @}
  */  

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

