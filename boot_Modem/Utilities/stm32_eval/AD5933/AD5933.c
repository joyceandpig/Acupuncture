#include "AD5933.h"
#include "math.h"
#include "stm32_delay.h"

/** @定义AD5933私有变量
  * @{
  */
#define PI 			3.141592			//定义Pi值
float  GainFactor = 1.3157e-5;//增益系数
#define C_H(x) pow(2,x)         //2^x 次方运算
#define EXTERNAL_CLOCK								    /*使用外部时钟作为AD5933的时钟源*/
//																					 	/*没有外部时钟时，注释掉该语句*/

unsigned char  START_FREQUENCY_CODE_H,
               START_FREQUENCY_CODE_M,
               START_FREQUENCY_CODE_L,
               FREQUENCY_INC_CODE_H,
               FREQUENCY_INC_CODE_M,
               FREQUENCY_INC_CODE_L;
signed short int real_Val = 0;//实部值
signed short int imag_Val = 0;//虚部值
unsigned char ConsultVal = 0;//参考电阻值

#ifdef EXTERNAL_CLOCK
//unsigned short int CH_Range[12] = {3900,110,978,463,972,100,980,220,9532,5955,5950,3703}; //959
unsigned short int CH_Range[12] = {3900,50,980,90,990,115};
#else
unsigned short int CH_Range[12] = {3900,50,980,90,990,115}; //
#endif                                                                                                                                                                                            
Impe AD5933_Para;    

void AD5933_Para_Init(void)           //AD5933的初始化的各种参数设置
{
  AD5933_Para.ADG714_CHANNEL_NUM        = 1;   //参考电阻通道号   1--8

  AD5933_Para.USE_PGAx5                 = 1;   //5倍PGA       5:启用  1:不启用
  AD5933_Para.CONTROL_REG_BIT8          = 1;   //PGA控制位          1：不启用5倍PGA  0：启用5倍PGA

  #ifdef EXTERNAL_CLOCK
  AD5933_Para.USE_EXTERNAL_CLOCK        = 1;   				//外部时钟   1-2   1:启用  2:不启用
  AD5933_Para.CONTROL_REG_BIT3          = 1;   				//外部时钟控制位     1:外部时钟  0：内部时钟
  AD5933_Para.AD5933_CLK                = 16000000LU; //时钟频率 内部：16776000UL  外部：16000000UL
  #else
  AD5933_Para.USE_EXTERNAL_CLOCK        = 2;   //外部时钟   1-2   1:启用  2:不启用
  AD5933_Para.CONTROL_REG_BIT3          = 0;   //外部时钟控制位     1:外部时钟  0：内部时钟
  AD5933_Para.AD5933_CLK                = 16000000UL;   //时钟频率 内部：16776000UL  外部：12000000UL
  #endif
  
  AD5933_Para.OUTPUT_VOLTAGE            = 4;   //激励信号  1-4 1->2vpp 2->1vpp 3->0.4vpp 4->0.2vpp
  AD5933_Para.CONTROL_REG_BIT10_TO_BIT9 = 1;   //激励信号的控制位    0-3  0->2vpp  1->0.2vpp  2->0.4vpp  3->1vpp 
   
  AD5933_Para.START_FREQUENCY           = 50000UL;       //起始频率 
  AD5933_Para.INC_FREQUENCY             = 5000UL;       //频率增量
  AD5933_Para.BUILD_TIME_CYCLES         = 100;           //建立周期时间
  AD5933_Para.FREQUENCY_INC_NUM         = 100;          //频率增量数
  
}                                                 
                                                  
void AD5933FreInit(void)
{
  //Start Frequency (24 Bits)
  START_FREQUENCY_CODE_H = (unsigned char)((START_FREQUENCY_CODE(AD5933_Para.START_FREQUENCY,AD5933_Para.AD5933_CLK)&0x00FF0000)>>16);		
  																				/*起始频率码:bit24~bit16*/
  START_FREQUENCY_CODE_M = (unsigned char)((START_FREQUENCY_CODE(AD5933_Para.START_FREQUENCY,AD5933_Para.AD5933_CLK)&0x0000FF00)>>8);
  																				/*起始频率码:bit15~bit8 */
   START_FREQUENCY_CODE_L = (unsigned char)(START_FREQUENCY_CODE(AD5933_Para.START_FREQUENCY,AD5933_Para.AD5933_CLK)&0x000000FF);
  																				/*起始频率码:bit7 ~bit0 */
  //Frequency Increment Word (24 Bits)
  FREQUENCY_INC_CODE_H = (unsigned char)((FREQUENCY_INC_CODE(AD5933_Para.INC_FREQUENCY,AD5933_Para.AD5933_CLK)&0x00FF0000)>>16);
  																				/*增量频率码:bit24~bit16*/
  FREQUENCY_INC_CODE_M = (unsigned char)((FREQUENCY_INC_CODE(AD5933_Para.INC_FREQUENCY,AD5933_Para.AD5933_CLK)&0x0000FF00)>>8);
  																				/*增量频率码:bit15~bit8 */
  FREQUENCY_INC_CODE_L = (unsigned char)( FREQUENCY_INC_CODE(AD5933_Para.INC_FREQUENCY,AD5933_Para.AD5933_CLK)&0x000000FF);																				/*增量频率码:bit7 ~bit0 */
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
  	sEE_WriteData(BUILD_TIME_CYCLES_H_REG,((tmp>>8) & 0x01) | 0x02); //周期数x2
  	sEE_WriteData(BUILD_TIME_CYCLES_L_REG,tmp&0x00FF);     
  }
  else
  { 
    tmp = AD5933_Para.BUILD_TIME_CYCLES >> 2 ;
  	sEE_WriteData(BUILD_TIME_CYCLES_H_REG,((tmp>>8) & 0x01) | 0x06); //周期数x4
  	sEE_WriteData(BUILD_TIME_CYCLES_L_REG,tmp&0x00FF);    
  } 
}
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/*检查AD5933状态
**返回：
**	 1 - 温度测量有效
**   2 - 实值/虚值有效
**   3 - 频率扫描完成
**   0 - 转换未完成
*/
unsigned int AD5933_GetStatus(void)
{
  unsigned short int tmp;
  unsigned int return_val;
  sEE_ReadData(STATUS_REG,&tmp);  
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

//AD5933 读取温度测量值
unsigned int AD5933_GetTemperatureMeasureVal(void)
{
  unsigned short int tmp;
  unsigned short int temp_meas_val;
  sEE_ReadData(TEMP_DATA_H_REG,&tmp);
  temp_meas_val = tmp<<8;
  sEE_ReadData(TEMP_DATA_L_REG,&tmp);
  temp_meas_val += tmp;
  return (temp_meas_val);  
}

//AD5933 读取阻抗虚部测量值
signed short int AD5933_GetImpMeasure_ImagVal(void)
{
  unsigned short int tmp;
  unsigned short int imag_val;
	signed 	 short int return_val;
  sEE_ReadData(IMAG_DATA_H_REG,&tmp);
  imag_val = tmp<<8;
  sEE_ReadData(IMAG_DATA_L_REG,&tmp);
  imag_val += tmp;
	return_val = Compl_TO_TrueForm(imag_val);//补码转原码
  return (return_val);
}

//AD5933 读取阻抗实部测量值
signed short int AD5933_GetImpMeasure_RealVal(void)
{
  unsigned short int tmp;
  unsigned short int real_val;
	signed 	 short int return_val;
  sEE_ReadData(REAL_DATA_H_REG,&tmp);
  real_val = tmp<<8;
  sEE_ReadData(REAL_DATA_L_REG,&tmp);
  real_val += tmp;
	return_val = Compl_TO_TrueForm(real_val);//补码转原码
  return (return_val);  
}
void delays(unsigned int n)
{
	unsigned int i,j;
	for(i=n;i>0;i--)
	for(j=20;j>0;j--);
}
//AD5933 启动阻抗扫描测量
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

  //进入待机模式  
  AD5933_StandbyMode(); 
   
  //复位
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

//相角
signed short int CalculateAngle(signed short int real,signed short int imag)
{
  double tmp1;
  signed int return_val;   
 
  tmp1 = atan(imag/(real*1.0)) * 180/PI; 
          
  if(((real & 0x8000) != 0x8000) && ((imag & 0x8000) != 0x8000))
  {//00
    return_val = tmp1; 
  }
  else if(((real & 0x8000) != 0x8000) && ((imag & 0x8000) == 0x8000))
  {//01
    return_val = tmp1 + 180;
  } 
  else if(((real & 0x8000) == 0x8000) && ((imag & 0x8000) != 0x8000))
  {//10
    return_val = tmp1 + 180;
  } 
  else if(((real & 0x8000) == 0x8000) && ((imag & 0x8000) == 0x8000))
  {//11
    return_val = tmp1 + 360;
  }      
  return (return_val); 
}

//阻抗计算
unsigned long int CalculateImpedance(signed short int real,signed short int imag)
{
  double range;
  unsigned long int return_val; 

  range = sqrt((imag * imag) + (real * real));//幅度
																								 
  return_val = (unsigned long int)(1.0/(GainFactor * range)) ;
  return (return_val); 
}

#define MEAS_TIME	10	 //测量次数

//把一次性测得的多个值进行从大到小的排序  冒泡法排序
void TodoRank(signed short int *data,unsigned char num)   
{
  unsigned char rank_i,rank_j;
  signed short int tmp = 0;
  
  for(rank_i = 0; rank_i < num; rank_i++)
  {
    for(rank_j = rank_i+1; rank_j < num; rank_j++)
    {
      if (*(data + rank_i) < *(data + rank_j))
      {
        tmp = *(data+rank_i);
        *(data + rank_i) = *(data + rank_j);
        *(data + rank_j) = tmp;
      }
    }
  }  
}

//把排序的中值求平均值
signed short int TodoAverage(signed short int *Data,unsigned char num) 
{
  uint8_t aver_i;
  signed short int average = 0;
  signed long int sum = 0;
  
  if (num > 2)
  {
    for (aver_i = 1; aver_i < num-1; aver_i++)
    {
      sum += *(Data + aver_i);
    } 
    num -= 2;
    average = (signed short int)(sum / num);
  }
  else 
  {
    average = (*Data + *(Data+1))/num;
  }
  return average;
}

//返回通道的参考电阻具体值
unsigned long int TodoConsult(ChannelType CHN)    
{
  unsigned long int Val = 0;
  switch(CHN)
  {
    case 0x01:
      Val = 47;          //47
      break;
    case 0x02:
      Val = 470;         //470
      break;
    case 0x04:
      Val = 1000;        //1k
      break;
    case 0x08:
      Val = 10000;       //10K
      break;
    case 0x10:           
      Val = 47000;       //47K
      break;
    case 0x20:
      Val = 100000;      //100K
      break;
    case 0x40:
      Val = 1000000;     //1M
      break;
    case 0x80:
      Val = 4000000;     //4M
      break;
    default:
      break;   
  }
  return Val;
}


//比较电阻幅度值是否处于通道范围内，来决定是否切换量程
unsigned char CompareCHRange(unsigned short int *CH,unsigned char CCR_i,unsigned short int range)
{
  if ((*(CH+CCR_i) > range) && (range > (*(CH+CCR_i+1))))  //在范围内 
  {
    ImpPar.in_span = 1;   //得到有效的档位阻抗值（在测量的上下限范围内）
    return 1; 
  }
  else       //不在预测的档位范围值内
  {
    if (CCR_i > 3)          //如果是最后一个档位，则跳出本次采集
    {
			ImpPar.in_span = 2;    //均不满足档位量程范围的阻抗值（超过量程），不进入计算环节
    }
    if (CCR_i < 1)             //如果阻抗小于10欧姆，则优化为0
    {
      if((5800 > range) && (range > 3900))
      {
        imag_Val = 40000;
        real_Val = 0;
        ImpPar.in_span = 1;
        return 1;
      }
    }
    return 0;
  }
}

//采集阻抗
unsigned long int Test(void)
{
  unsigned char get_val = 0,get_val_count = 0;
  unsigned short int tmp = 0;
  
  double Range = 0.0;           //幅度值

	unsigned char t_i,t_j,val; 
  signed short int real_tmp[MEAS_TIME] = {0};//实部值缓存
	signed short int imag_tmp[MEAS_TIME] = {0};//虚部值缓存

  ImpPar.time_start = TRUE;
  ImpPar.in_span = 0;      //采集到的数据是否有效，0||2：无效 1：有效
  for(t_j = 1; t_j < RValueMeasuregears; t_j++) //档位选择，规定电阻的档位有多少个，档位分区
  {
    ConsultVal = t_j;       		//表示选择的档位值
    if(t_j < 2)val = t_j - 1;
    else val = t_j;
		
    AD5933_ConsultRChoice(C_H(val));    //选择参考电阻通道
		printf("当前电阻选择 %d 号档位\n",t_j);
    ConsultVal = val;                 //查看选择的档位 1-6
    AD5933_StartImpMeasure();         //开始阻抗采集    进入待机模式,配置激励频率参数，准备阻抗采集。

    do
    {
    	for(t_i = 0; t_i < MEAS_TIME; t_i++)//MEAS_TIME次测量取平均
    	{
        delay_us(1000);
    		tmp = AD5933_GetStatus();
        delay_us(1000);
        if(tmp == 2)        //测量完成,数值转换成功
        {
          get_val = 1;
          
          imag_Val = AD5933_GetImpMeasure_ImagVal();//虚部,原码存储
          real_Val = AD5933_GetImpMeasure_RealVal();//实部,原码存储

    
    		  real_tmp[get_val_count] = real_Val;		   		  //储存到缓存
    		  imag_tmp[get_val_count] = imag_Val;
          get_val_count++;
  
          AD5933_RepeatFrequency();//重复频率
        } 
        if (ImpPar.time_limit)
        {
          ImpPar.time_limit = FALSE;
          ImpPar.in_span = 2;     //得到的阻抗值不进入计算环节
					printf("阻抗检测一直没有回复，跳出采集程序\n");
          return FALSE;
        }
    	}
    }while(get_val != 1);  //全部测量完成
    delay_us(1000);
  
    TodoRank(imag_tmp,get_val_count);       //虚部排序
    TodoRank(real_tmp,get_val_count);       //实部排序
  
    imag_Val = TodoAverage(imag_tmp,get_val_count);    //虚部求平均值
    if (imag_Val < 0)imag_Val = -imag_Val;
    real_Val = TodoAverage(real_tmp,get_val_count);    //实部求平均值
    if (real_Val < 0)real_Val = -real_Val;

    tmp = (unsigned short int)sqrt((imag_Val * imag_Val) + (real_Val * real_Val));//幅度

    if (CompareCHRange(CH_Range,(t_j-1)*2,tmp))
		{
			printf("该测试电阻属于当前档位\n");
			break; //查看结果是否属于指定量程范围内,若满足则跳出测量
		}
    get_val_count = 0;
    AD5933_PowerSave();//省电模式
  }
  ImpPar.time_limit = FALSE; //全部档位都测定完成或者采集到有效数据，则关闭阻抗采集计时
  ImpPar.time_start = FALSE;
  ImpPar.time_count = 0;
	printf("采集完成\n");
  return TRUE;
}


