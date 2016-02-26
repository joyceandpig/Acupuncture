#include "stm32_eval_wave.h"


/* Private functions ---------------------------------------------------------*/

/******************************************************************
** ������: wave_base
** ��  ��: channel������ͨ����
**				 Per�����ε�����ֵ
**				 Scope�����εķ���ֵ
** ��  ��: �����������Σ�����0.2ms�ĸߵ�ƽ������ʱ�䶼Ϊ�͵�ƽ                                  
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
/*****************************��������*****************************************/
static void wave_base(uint8_t channel,uint16_t Per,uint16_t Scope)		 
{	
	if (CH[channel].wave_count <= WaveHighTime)/*   ά��200us�Ĺ̶��ߵ�ƽʱ�䣬��Ҫ��ʱ���������Σ�����֮�ڵ�ƽ����Ϊ��  */
	{	
		if (CH[channel].base_wave_stay == FALSE) /*   DA�����Ա��ֹ��ܣ�����һ�κ󣬲���Ҫ�ٴη��Ϳ����ֽ��Խ�Լʱ��  */
		{
			SPI_SendData((uint16_t)channel << ChannelOffset,Scope); 	/*   ͨ��SPI�����ֽڵ�DAоƬ�������������  */
			CH[channel].base_wave_stay = TRUE;
			GPIO_LOW(GPIO_LED,LED1);
			
		}
	}		
	else if (CH[channel].wave_count < Per) 	/* ����ֵС����������ֵʱ�������͵�ƽ��ͬ��ֻ��Ҫִ��һ�μ���   */
	{
		if (CH[channel].base_wave_stay)
		{
			SPI_SendData((uint16_t)channel << ChannelOffset,0);
			CH[channel].base_wave_stay = FALSE;
			GPIO_HIGH(GPIO_LED,LED1);
		}
	}		
	else                                   /*  ���һ�����ڲ��Σ��Լ���ֵ�Ϳ��Ʊ�־λ����  */
	{
		CH[channel].base_wave_stay = FALSE;
		CH[channel].wave_count = 0;
	}
}
/******************************************************************
** ������: Continuous_Wave
** ��  ��: channel������ͨ����
**				 Per�����ε�����ֵ
**				 Scope�����εķ���ֵ
** ��  ��: �����������Σ����β����������������                                     
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs\

** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
static void Continuous_Wave(uint8_t channel, uint16_t peroid, uint16_t scope)		   //����������
{	
	wave_base(channel,peroid,scope);  /*  ���벨�ε�  ͨ����  ����  ����ֵ  */
}
/******************************************************************
** ������: Interrupted_Wave
** ��  ��: channel������ͨ����
**				 Per�����ε�����ֵ
**				 Scope�����εķ���ֵ
** ��  ��: ���������������β����������������������������Ϊ��15s�����5s���������������                                       
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
static void Interrupted_Wave(uint8_t channel, uint16_t peroid, uint16_t scope)		//����������
{
	if (CH[channel].time5s_count < 3)	//����time5s��5s��һ�Σ�����15s��Ҫ3��	 15sʱ���ڣ��������  
	{	
		if (CH[channel].Iwave_count_flag == FALSE)/*  �������ͶϿ��л�ʱ��Ҫ�ֱ�ʼ����  */
		{
			CH[channel].Iwave_count_flag = TRUE;
			CH[channel].wave_count = CLEARVALUE;
		}
	 	wave_base(channel,peroid,scope);
	}
	else					                            /*    5sʱ���ڣ����������  */
	{
	   if (CH[channel].Iwave_count_flag)
	   {
	   	CH[channel].Iwave_count_flag = FALSE;
			CH[channel].wave_count = CLEARVALUE;
      SPI_SendData((uint16_t)channel << ChannelOffset,0);
	   }
  }  
	if (CH[channel].time5s_count > 3)
	{
		CH[channel].time5s_count = 0;
	}
}
/* Private functions ---------------------------------------------------------*/

/******************************************************************
** ������: Dilatational_Wave
** ��  ��: channel������ͨ����
**				 Per�����ε�����ֵ
**				 Scope�����εķ���ֵ
** ��  ��: �������ܲ������β���������������������ܲ�����Ϊ��10s�ܼ�����5s���ɲ�����������                                         
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-04-21
** ��  ��:
** ��  ��:

** ��  ��: 1.0
*******************************************************************/
/****************************************�������ܲ�******************************************/

static void Dilatational_Wave(uint8_t channel, uint16_t peroid, uint16_t scope) //�������ܲ�	Ƶ��֮��  ���� ���ܼ� = 1 ��5
{	
	if (CH[channel].time5s_count < 2)	//����time5s��5s��һ�Σ�����10s��Ҫ2��  ��10sʱ���ڣ������ܼ���  	   
	{
		if (CH[channel].Dwave_count_flag == FALSE)        	/*  ���ܼ��������л�ʱ��Ҫ�ֱ�ʼ����  */
		{
			CH[channel].Dwave_count_flag = TRUE;
			CH[channel].wave_count = CLEARVALUE;
		}
		wave_base(channel,peroid,scope);
	}
	else				                                           /*   ��5sʱ���ڣ��������ɲ�    */
	{
		if (CH[channel].Dwave_count_flag )
		{
			CH[channel].Dwave_count_flag = FALSE;
			CH[channel].wave_count = CLEARVALUE;
		}	

		if (peroid > Peroid_10hz)              /*  ���������ܲ�Ƶ��С��10hz�����貨��Ƶ��=f�ܲ�/5;�貨��СΪ2hz  */
		{
			wave_base(channel,Peroid_2hz,scope);
		}
		else 
		{
			wave_base(channel,CH[channel].ss_peroid,scope); //	
		}		

		if (CH[channel].time5s_count > 2)       /*  5sʱ������ɲ�������ɣ���������һ����  */
		{
			CH[channel].time5s_count = CLEARVALUE;
		}
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
void wave_output(void)
{	
	ReadyToUpdata();//���ߣ�ʹ��LDAC���б�Ͷ���������
	for(count = 0; count < 6; count++)	/*    ��ѯͨ���Ŀ���ֹͣ״̬λ   */
	{			 
		if (CH[count].channel_open){	        /*  ��Ӧͨ���򿪣��������Ӧ����  */
			if(CH[count].led_sta){
				STM_CHN_LED_ON(CH[count].channel);
				CH[count].led_sta = 0;
			}
			switch(CH[count].function_code){        //ѡ���� 
				case Contious: Continuous_Wave(count,CH[count].peroid,CH[count].scope);break;
				case Interrupt:	Interrupted_Wave(count,CH[count].peroid,CH[count].scope);break;
				case Dilatational:Dilatational_Wave(count,CH[count].peroid,CH[count].scope);break;	
				default :break;
			}				
		}else{
			if (CH[count].channel_stop_state){	 //ͨ����ֹͣ״̬����־λ�����Ǳ�ֻ֤�����һ�Σ��Խ�Լʱ��
				SPI_SendData((uint16_t)count << ChannelOffset,0);
				CH[count].channel_stop_state = 0;
				STM_CHN_LED_OFF(CH[count].channel);
			}
		}
	}	
	UpdataDAOutput();			      //ʹDA���ָ����ѹ����  ����DA
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
void WaveWork(void)
{	
	if (!urgstop){    /******δ�����ϲ��ط��������޵����������������Σ������ֳ��ޣ��򲻽��벨�η���*******/
		wave_output();
		LightLed(&led_time_count,Led2);
	}else{                          //����ֹͣ�����LED��˸
		LightLed(&led_time_count,Led4);
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
void WaveExport(void)
{
	static uint32_t TIM_timeOld = 0;
	if (TIMx_timecount != TIM_timeOld){
		TIM_timeOld = TIMx_timecount;
		WaveWork();//�������εĺ���
	}
}
