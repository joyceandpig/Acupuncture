#include "AD5933.h"
#include "math.h"
#include "stm32_delay.h"
#include "Calculate.h"
#include "stm32_eval.h"

static int8_t AckBuf[2][14]={
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x21,0x01,0x01,0x01,0x01,0xFF,0xFF},//�����迹
{0x0E,0x00,0x02,0x00,0x01,0x01,0x02,0x22,0x01,0x01,0x01,0x01,0xFF,0xFF}, //�����¶�
};	


/** @����AD5933˽�б���
  * @{
  */
#define C_H(x) pow(2,x)         //2^x �η�����
#define EXTERNAL_CLOCK								    /*ʹ���ⲿʱ����ΪAD5933��ʱ��Դ*/
//																					 	/*û���ⲿʱ��ʱ��ע�͵������*/

unsigned char  START_FREQUENCY_CODE_H,
               START_FREQUENCY_CODE_M,
               START_FREQUENCY_CODE_L,
               FREQUENCY_INC_CODE_H,
               FREQUENCY_INC_CODE_M,
               FREQUENCY_INC_CODE_L;
                                                                                                                                                                                        
Impe AD5933_Para;

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
  while(Slength--){
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
static void TransformSpanDat(uint8_t *dat, _FeedBack dat1)
{
  uint16_t tmp;
  
	*(dat + 8) = dat1.collect_val_imag>>8;
  *(dat + 9) = dat1.collect_val_imag;
	
  *(dat + 10) = dat1.collect_val_real>>8;
  *(dat + 11) = dat1.collect_val_real;

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
void ImpVal(uint16_t Ifunc_code,uint8_t *Idata)
{
  uint16_t tmp;

  switch(Ifunc_code)
  {
    case ImpClk:        //ʱ��
      if (*(Idata+11) == 2)                    //�ⲿʱ�ӿ���λ     
      {  
				AD5933_Para.CONTROL_REG_BIT3          = 1;            //  1:�ⲿʱ��  0���ڲ�ʱ��
        AD5933_Para.AD5933_CLK                = 16000000UL;   //ʱ��Ƶ��
      }
			else if(*(Idata+11) == 1)              //�ڲ�ʱ�ӿ���λ     
      { 
				AD5933_Para.CONTROL_REG_BIT3          = 0;           // 1:�ⲿʱ��  0���ڲ�ʱ��
        AD5933_Para.AD5933_CLK                = 16776000UL;   //ʱ��Ƶ��
      }
      break;
    case ImpConCH:        //�ο�����ͨ��
      AD5933_Para.ADG714_CHANNEL_NUM = *(Idata+11);
      break;
    case ImpInpSign:        //�����ź�
      if (*(Idata+11) == 1)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 0;        //2vpp
      else if (*(Idata+11) == 2)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 3;   //1vpp
      else if (*(Idata+11) == 3)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 =  2;  //400mvpp
      else if (*(Idata+11) == 4)AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 1;   //200mvpp
      break;
    case ImpPGA:        //PGA����
      if(*(Idata+11) == 1)AD5933_Para.CONTROL_REG_BIT8 = 1;           // x1
      else if(*(Idata+11) == 5)AD5933_Para.CONTROL_REG_BIT8 = 0;       // x5
      break;
    case ImpInitFre:        //��ʼƵ��
      AD5933_Para.START_FREQUENCY = (*(Idata+8))*16777216+(*(Idata+9))*65536+(*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.START_FREQUENCY > 100000000)
        AD5933_Para.START_FREQUENCY = 100000000;
      break;
    case ImpAddFre:        //����Ƶ��
      AD5933_Para.INC_FREQUENCY = (*(Idata+8))*16777216+(*(Idata+9))*65536+(*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.INC_FREQUENCY > 100000000)
        AD5933_Para.INC_FREQUENCY = 100000000;
      break;
    case ImpNumAddF:        //Ƶ��������
      AD5933_Para.FREQUENCY_INC_NUM = (*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.FREQUENCY_INC_NUM > 511)
        AD5933_Para.FREQUENCY_INC_NUM = 511;
      break;
    case BuildTime:        //����ʱ������
      AD5933_Para.BUILD_TIME_CYCLES = (*(Idata+10))*256+(*(Idata+11));
      if (AD5933_Para.BUILD_TIME_CYCLES > 2044)
        AD5933_Para.BUILD_TIME_CYCLES = 2044;
      break;
    case ImpMeaStart:        //��ʼ�迹����
			
 			GPIO_LOW(GPIO_LED,LED3);
			STM_EVAL_MCOCLK_ENABLE();//open ad5933 clk,The CLk From the PORTA8_MCO_OUTPUT
			switch(*(Idata+8))
			{
				case 1:AD5933_Para.ADG714_CHANNEL_NUM = C_H(1);break;//0x02  100
				case 2:AD5933_Para.ADG714_CHANNEL_NUM = C_H(2);break;//0x04  1k
				case 3:AD5933_Para.ADG714_CHANNEL_NUM = C_H(3);break;//0x08  10k
				case 4:AD5933_Para.ADG714_CHANNEL_NUM = C_H(5);break;//0x20  100k
				case 5:AD5933_Para.ADG714_CHANNEL_NUM = C_H(6);break;//0x40  1M
				default:break;
			}
			AD5933_Init();
			Test();
			STM_EVAL_MCOCLK_DISABLE();//close AD5933 CLK
			GPIO_HIGH(GPIO_LED,LED3);
      break;
    case ImpMeaStop:        //ֹͣ�迹����
			STM_EVAL_MCOCLK_DISABLE();
      break;
    case ImpMeaReFre:        //����Ƶ�ʲ���
      AD5933_Para.INC_AND_REP_FRE = 0;
      Test();
      break;
    case ImpMeaAddFre:        //Ƶ���ظ�����
      AD5933_Para.INC_AND_REP_FRE = 1;
      Test();
      break;
    case ImpMeaTemrt:        //�����¶�
      break;
    case ImpPowerSave:        //ʡ��ģʽ
      break;
    case ImpIdleMode:        //����ģʽ
      break;
    case ImpReadImp:        //��ȡ�迹����ֵ
			GPIO_LOW(GPIO_LED,LED4);
			TransformSpanDat(AckBuf[0],AD5933_Para.FeedBackVal);
			SendCmdReply(AckBuf[0],14);
			GPIO_HIGH(GPIO_LED,LED4);
      break;
    case ImpReadTempr:        //��ȡ�����¶�ֵ
			tmp = AD5933_GetTemperatureMeasureVal();
		  AckBuf[1][8] = tmp;
		  AckBuf[1][9] = tmp >> 8;
			AckBuf[1][10] = tmp>>16;
		  AckBuf[1][11] = tmp >> 24;
			SendCmdReply(AckBuf[1],14);
		
      break;
    default:
      break;
  } 
}
void AD5933_Para_Init(void)           //AD5933�ĳ�ʼ���ĸ��ֲ�������
{
  AD5933_Para.ADG714_CHANNEL_NUM        = 1;   //�ο�����ͨ����   1--8

  AD5933_Para.USE_PGAx5                 = 1;   //5��PGA       5:����  1:������
  AD5933_Para.CONTROL_REG_BIT8          = 1;   //PGA����λ          1��������5��PGA  0������5��PGA

  #ifdef EXTERNAL_CLOCK
  AD5933_Para.USE_EXTERNAL_CLOCK        = 1;   				//�ⲿʱ��   1-2   1:����  2:������
  AD5933_Para.CONTROL_REG_BIT3          = 1;   				//�ⲿʱ�ӿ���λ     1:�ⲿʱ��  0���ڲ�ʱ��
  AD5933_Para.AD5933_CLK                = 16000000LU; //ʱ��Ƶ�� �ڲ���16776000UL  �ⲿ��16000000UL
  #else
  AD5933_Para.USE_EXTERNAL_CLOCK        = 2;   //�ⲿʱ��   1-2   1:����  2:������
  AD5933_Para.CONTROL_REG_BIT3          = 0;   //�ⲿʱ�ӿ���λ     1:�ⲿʱ��  0���ڲ�ʱ��
  AD5933_Para.AD5933_CLK                = 16000000UL;   //ʱ��Ƶ�� �ڲ���16776000UL  �ⲿ��12000000UL
  #endif
  
  AD5933_Para.OUTPUT_VOLTAGE            = 1;   //�����ź�  1-4 1->2vpp 2->1vpp 3->0.4vpp 4->0.2vpp
  AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 0;   //�����źŵĿ���λ    0-3  0->2vpp  1->0.2vpp  2->0.4vpp  3->1vpp 
   
  AD5933_Para.START_FREQUENCY           = 50000UL;       //��ʼƵ�� 
  AD5933_Para.INC_FREQUENCY             = 5000UL;       //Ƶ������
  AD5933_Para.BUILD_TIME_CYCLES         = 100;           //��������ʱ��
  AD5933_Para.FREQUENCY_INC_NUM         = 100;          //Ƶ��������
	
}                                                 
                                                  
void AD5933FreInit(void)
{
  //Start Frequency (24 Bits)
  START_FREQUENCY_CODE_H = (unsigned char)((START_FREQUENCY_CODE(AD5933_Para.START_FREQUENCY,AD5933_Para.AD5933_CLK)&0x00FF0000)>>16);		
  																				/*��ʼƵ����:bit24~bit16*/
  START_FREQUENCY_CODE_M = (unsigned char)((START_FREQUENCY_CODE(AD5933_Para.START_FREQUENCY,AD5933_Para.AD5933_CLK)&0x0000FF00)>>8);
  																				/*��ʼƵ����:bit15~bit8 */
   START_FREQUENCY_CODE_L = (unsigned char)(START_FREQUENCY_CODE(AD5933_Para.START_FREQUENCY,AD5933_Para.AD5933_CLK)&0x000000FF);
  																				/*��ʼƵ����:bit7 ~bit0 */
  //Frequency Increment Word (24 Bits)
  FREQUENCY_INC_CODE_H = (unsigned char)((FREQUENCY_INC_CODE(AD5933_Para.INC_FREQUENCY,AD5933_Para.AD5933_CLK)&0x00FF0000)>>16);
  																				/*����Ƶ����:bit24~bit16*/
  FREQUENCY_INC_CODE_M = (unsigned char)((FREQUENCY_INC_CODE(AD5933_Para.INC_FREQUENCY,AD5933_Para.AD5933_CLK)&0x0000FF00)>>8);
  																				/*����Ƶ����:bit15~bit8 */
  FREQUENCY_INC_CODE_L = (unsigned char)( FREQUENCY_INC_CODE(AD5933_Para.INC_FREQUENCY,AD5933_Para.AD5933_CLK)&0x000000FF);																				/*����Ƶ����:bit7 ~bit0 */
}
/**
  * @}
  */  

//Init AD5933 
void AD5933_Init(void)
{
	sEE_Init();
  ADG714_Init();
  AD5933FreInit();
}

//Reset AD5933
void AD5933_Reset(void)
{
	unsigned int tmp;
	tmp = (AD5933_Para.CONTROL_REG_BIT3<<3)|(1<<4);
	sEE_WriteData(CONTROL_L_REG,tmp);
}

//Start Frequency Init
void AD5933_StartFrequency_Init(void)
{
	sEE_WriteData(CONTROL_H_REG,CONTROL_H_REG_MAKE(COMM_START_FREQUENCY_INIT,AD5933_Para.CONTROL_REG_BIT10_TO_BIT9,AD5933_Para.CONTROL_REG_BIT8));  
}

//Start Frequency Scan
void AD5933_StartFrequency_Scan(void)
{
	sEE_WriteData(CONTROL_H_REG,CONTROL_H_REG_MAKE(COMM_START_FREQUENCY_SCAN,AD5933_Para.CONTROL_REG_BIT10_TO_BIT9,AD5933_Para.CONTROL_REG_BIT8));  
}

//Incremental Frequency
void AD5933_IncFrequency(void)
{
	sEE_WriteData(CONTROL_H_REG,CONTROL_H_REG_MAKE(COMM_INC_FREQUENCY,AD5933_Para.CONTROL_REG_BIT10_TO_BIT9,AD5933_Para.CONTROL_REG_BIT8));  
}

//TEMP MEAS
void AD5933_TempMeas(void)
{
	sEE_WriteData(CONTROL_H_REG,CONTROL_H_REG_MAKE(COMM_TEMP_MEAS,AD5933_Para.CONTROL_REG_BIT10_TO_BIT9,AD5933_Para.CONTROL_REG_BIT8));  
}

//Power Save
void AD5933_PowerSave(void)
{
	sEE_WriteData(CONTROL_H_REG,CONTROL_H_REG_MAKE(COMM_POWER_SAVE,AD5933_Para.CONTROL_REG_BIT10_TO_BIT9,AD5933_Para.CONTROL_REG_BIT8));  
}

//Standby Mode
void AD5933_StandbyMode(void)
{
	sEE_WriteData(CONTROL_H_REG,CONTROL_H_REG_MAKE(COMM_STANDBY_MODE,AD5933_Para.CONTROL_REG_BIT10_TO_BIT9,AD5933_Para.CONTROL_REG_BIT8));	  
}



//Repeat Frequency
void AD5933_RepeatFrequency(void)
{
	sEE_WriteData(CONTROL_H_REG,CONTROL_H_REG_MAKE(COMM_REPEAT_FREQUENCY,AD5933_Para.CONTROL_REG_BIT10_TO_BIT9,AD5933_Para.CONTROL_REG_BIT8));  
}
//Set Build Time Cycles Register
void AD5933_SetBuildTimeCycles(void)
{    
  unsigned int tmp;
  if(AD5933_Para.BUILD_TIME_CYCLES <= 511)
  {
  	sEE_WriteData(BUILD_TIME_CYCLES_H_REG,(AD5933_Para.BUILD_TIME_CYCLES>>8) & 0x01); 
  	sEE_WriteData(BUILD_TIME_CYCLES_L_REG,AD5933_Para.BUILD_TIME_CYCLES&0x00FF);      
  }
  else if(AD5933_Para.BUILD_TIME_CYCLES >511 && AD5933_Para.BUILD_TIME_CYCLES <=1022)
  {    
    tmp = AD5933_Para.BUILD_TIME_CYCLES >> 1 ;
  	sEE_WriteData(BUILD_TIME_CYCLES_H_REG,((tmp>>8) & 0x01) | 0x02); //������x2
  	sEE_WriteData(BUILD_TIME_CYCLES_L_REG,tmp&0x00FF);     
  }
  else
  { 
    tmp = AD5933_Para.BUILD_TIME_CYCLES >> 2 ;
  	sEE_WriteData(BUILD_TIME_CYCLES_H_REG,((tmp>>8) & 0x01) | 0x06); //������x4
  	sEE_WriteData(BUILD_TIME_CYCLES_L_REG,tmp&0x00FF);    
  } 
}
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*���AD5933״̬
**���أ�
**	 1 - �¶Ȳ�����Ч
**   2 - ʵֵ/��ֵ��Ч
**   3 - Ƶ��ɨ�����
**   0 - ת��δ���
*/
unsigned int AD5933_GetStatus(void)
{
  unsigned short int tmp;
  unsigned int return_val;
  sEE_ReadData(STATUS_REG,&tmp);  //��Ҫ����һ����ʱ���ƣ���ֹiic���߶�����
  if(tmp & 0x04)
    return_val = 3;
  else if(tmp & 0x02)
    return_val = 2;
  else if(tmp & 0x01)
    return_val = 1;
  else
    return_val = 0;
  return (return_val);
}

//AD5933 ��ȡ�¶Ȳ���ֵ
unsigned int AD5933_GetTemperatureMeasureVal(void)
{
	  unsigned short int tmp;
  unsigned short int temp_meas_val;
	 
  //�������ģʽ  
  AD5933_StandbyMode(); 
   
  //��λ
  AD5933_Reset();
	
	AD5933_TempMeas();

  sEE_ReadData(TEMP_DATA_H_REG,&tmp);
  temp_meas_val = tmp<<8;
  sEE_ReadData(TEMP_DATA_L_REG,&tmp);
  temp_meas_val += tmp;
  return (temp_meas_val);  
}	

//AD5933 ��ȡ�迹�鲿����ֵ
signed short int AD5933_GetImpMeasure_ImagVal(void)
{
  unsigned short int tmp;
  unsigned short int imag_val;
	signed 	 short int return_val;
  sEE_ReadData(IMAG_DATA_H_REG,&tmp);
  imag_val = tmp<<8;
  sEE_ReadData(IMAG_DATA_L_REG,&tmp);
  imag_val += tmp;
	return_val = Compl_TO_TrueForm(imag_val);//����תԭ��
  return (return_val);
}

//AD5933 ��ȡ�迹ʵ������ֵ
signed short int AD5933_GetImpMeasure_RealVal(void)
{
  unsigned short int tmp;
  unsigned short int real_val;
	signed 	 short int return_val;
  sEE_ReadData(REAL_DATA_H_REG,&tmp);
  real_val = tmp<<8;
  sEE_ReadData(REAL_DATA_L_REG,&tmp);
  real_val += tmp;
	return_val = Compl_TO_TrueForm(real_val);//����תԭ��
  return (return_val);  
}
void delays(unsigned int n)
{
	unsigned int i,j;
	for(i=n;i>0;i--)
	for(j=20;j>0;j--);
}
//AD5933 �����迹ɨ�����
void AD5933_StartImpMeasure(void)
{
	//Start Frequency
  sEE_WriteData(START_FREQUENCY_H_REG,START_FREQUENCY_CODE_H);
  sEE_WriteData(START_FREQUENCY_M_REG,START_FREQUENCY_CODE_M);
  sEE_WriteData(START_FREQUENCY_L_REG,START_FREQUENCY_CODE_L);
  
	//No. of frequency increments(9Bits)
  sEE_WriteData(FREQUENCY_INC_NUM_H_REG,(AD5933_Para.FREQUENCY_INC_NUM & 0xFF00)>>8);
  sEE_WriteData(FREQUENCY_INC_NUM_L_REG,(AD5933_Para.FREQUENCY_INC_NUM & 0x00FF));
  
	//Increment Frequency
  sEE_WriteData(FREQUENCY_INC_H_REG,FREQUENCY_INC_CODE_H);
  sEE_WriteData(FREQUENCY_INC_M_REG,FREQUENCY_INC_CODE_M);
  sEE_WriteData(FREQUENCY_INC_L_REG,FREQUENCY_INC_CODE_L);

  //�������ģʽ  
  AD5933_StandbyMode(); 
   
  //��λ
  AD5933_Reset();
 
  AD5933_SetBuildTimeCycles();     
  AD5933_StartFrequency_Init();
    
  AD5933_StartFrequency_Scan();

}

void AD5933_ConsultRChoice(ChannelType Ch)
{
  ADG714_WriteData(Ch);
}
/*----------------------------------------------------------------------*/
/*                               TEST                                   */
/*----------------------------------------------------------------------*/



//��һ���Բ�õĶ��ֵ���дӴ�С������  ð�ݷ�����
void TodoRank(signed short int *data,unsigned char num)   
{
  static unsigned char rank_i,rank_j;
  static signed short int tmp = 0;
  
  for(rank_i = 0; rank_i < num; rank_i++){
    for(rank_j = rank_i+1; rank_j < num; rank_j++){
      if (*(data + rank_i) < *(data + rank_j)){
        tmp = *(data+rank_i);
        *(data + rank_i) = *(data + rank_j);
        *(data + rank_j) = tmp;
      }
    }
  }
}

//���������ֵ��ƽ��ֵ
signed short int TodoAverage(signed short int *Data,unsigned char num) 
{
  uint8_t aver_i;
  signed short int average = 0;
  signed long int sum = 0;
	static uint8_t lost_val = 1;//����ֵ
  
  if (num > 2)//ȥ�������Сֵ������ƽ��ֵ
  {
    for (aver_i = lost_val; aver_i < num-lost_val; aver_i++)
    {
      sum += *(Data + aver_i);
    } 
    average = (signed short int)(sum / num - 2 * lost_val);
  }
  else 
  {
    average = (*Data + *(Data+1))/num;
  }
  return average;
}
#define MEAS_TIME	10	 //��������
//�ɼ��迹
unsigned long int Test(void)
{
  unsigned char get_val = 0,get_val_count = 0;
  unsigned short int tmp = 0;
  
  double Range = 0.0;           //����ֵ

	unsigned char t_i,t_j,val; 
  signed short int real_tmp[MEAS_TIME] = {0};//ʵ��ֵ����
	signed short int imag_tmp[MEAS_TIME] = {0};//�鲿ֵ����
	signed short int T_imag_Val = 0;
	signed short int T_real_Val = 0;

		ImpPar.time_start = TRUE;
		AD5933_ConsultRChoice(AD5933_Para.ADG714_CHANNEL_NUM);   //ѡ��ο�����ͨ�� 
    AD5933_StartImpMeasure();         //��ʼ�迹�ɼ�    �������ģʽ,���ü���Ƶ�ʲ�����׼���迹�ɼ���

    do
    {
    	for(t_i = 0; t_i < MEAS_TIME; t_i++)//MEAS_TIME�β���ȡƽ��
    	{
//        delay_us(8000);
    		tmp = AD5933_GetStatus();
//				delay_us(8000);
        if(tmp == 2)        //�������,��ֵת���ɹ�
        {
          get_val = 1;
          
          T_imag_Val = AD5933_GetImpMeasure_ImagVal();//�鲿,ԭ��洢
          T_real_Val = AD5933_GetImpMeasure_RealVal();//ʵ��,ԭ��洢

    		  real_tmp[get_val_count] = T_real_Val;		   		  //���浽����
    		  imag_tmp[get_val_count] = T_imag_Val;
          get_val_count++;
//					delay_us(3000);
  
          AD5933_RepeatFrequency();//�ظ�Ƶ��
        } 
        if (ImpPar.time_limit)
        {
          ImpPar.time_limit = FALSE;
          return FALSE; 
        }
    	}
    }while(get_val != 1);  //ȫ���������
//		delay_us(8000);
  
    TodoRank(imag_tmp,get_val_count);       //�鲿����
    TodoRank(real_tmp,get_val_count);       //ʵ������
  
    AD5933_Para.FeedBackVal.collect_val_imag = TodoAverage(imag_tmp,get_val_count);    //�鲿��ƽ��ֵ
    AD5933_Para.FeedBackVal.collect_val_real = TodoAverage(real_tmp,get_val_count);    //ʵ����ƽ��ֵ
		
    get_val_count = 0;
    AD5933_PowerSave();//ʡ��ģʽ
		
  ImpPar.time_limit = FALSE; //ȫ����λ���ⶨ��ɻ��߲ɼ�����Ч���ݣ���ر��迹�ɼ���ʱ
  ImpPar.time_start = FALSE;
  ImpPar.time_count = 0;
  return TRUE;
}


