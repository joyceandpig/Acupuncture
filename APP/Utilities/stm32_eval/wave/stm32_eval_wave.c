#include "stm32_eval_wave.h"


/* Private functions ---------------------------------------------------------*/

/******************************************************************
** 函数名: wave_base
** 输  入: channel：波形通道号
**				 Per：波形的周期值
**				 Scope：波形的幅度值
** 描  述: 产生基础波形，即是0.2ms的高电平，其他时间都为低电平                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-04-21
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
/*****************************基础波形*****************************************/
static void wave_base(uint8_t channel,uint16_t Per,uint16_t Scope)		 
{	
	if (CH[channel].wave_count <= WaveHighTime)/*   维持200us的固定高电平时间，需要定时器进入两次，两次之内电平必须为高  */
	{	
		if (CH[channel].base_wave_stay == FALSE) /*   DA具有自保持功能，进入一次后，不需要再次发送控制字节以节约时间  */
		{
			SPI_SendData((uint16_t)channel << ChannelOffset,Scope); 	/*   通过SPI发送字节到DA芯片来控制输出幅度  */
			CH[channel].base_wave_stay = TRUE;
			GPIO_LOW(GPIO_LED,LED1);
			
		}
	}		
	else if (CH[channel].wave_count < Per) 	/* 计数值小于整个周期值时，产生低电平，同样只需要执行一次即可   */
	{
		if (CH[channel].base_wave_stay)
		{
			SPI_SendData((uint16_t)channel << ChannelOffset,0);
			CH[channel].base_wave_stay = FALSE;
			GPIO_HIGH(GPIO_LED,LED1);
		}
	}		
	else                                   /*  完成一个周期波形，对计数值和控制标志位清零  */
	{
		CH[channel].base_wave_stay = FALSE;
		CH[channel].wave_count = 0;
	}
}
/******************************************************************
** 函数名: Continuous_Wave
** 输  入: channel：波形通道号
**				 Per：波形的周期值
**				 Scope：波形的幅度值
** 描  述: 产生连续波形，波形参数由输入参数决定                                     
** 全局变量: 
** 调用模块: 
** 作  者:   zcs\

** 日  期:   2015-04-21
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
static void Continuous_Wave(uint8_t channel, uint16_t peroid, uint16_t scope)		   //产生连续波
{	
	wave_base(channel,peroid,scope);  /*  传入波形的  通道号  周期  幅度值  */
}
/******************************************************************
** 函数名: Interrupted_Wave
** 输  入: channel：波形通道号
**				 Per：波形的周期值
**				 Scope：波形的幅度值
** 描  述: 产生断续波，波形参数由输入参数决定，断续波类型为：15s输出，5s不输出。依次延伸                                       
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-04-21
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
static void Interrupted_Wave(uint8_t channel, uint16_t peroid, uint16_t scope)		//产生断续波
{
	if (CH[channel].time5s_count < 3)	//由于time5s是5s加一次，所以15s需要3次	 15s时间内，输出波形  
	{	
		if (CH[channel].Iwave_count_flag == FALSE)/*  在连续和断开切换时，要分别开始计数  */
		{
			CH[channel].Iwave_count_flag = TRUE;
			CH[channel].wave_count = CLEARVALUE;
		}
	 	wave_base(channel,peroid,scope);
	}
	else					                            /*    5s时间内，不输出波形  */
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
** 函数名: Dilatational_Wave
** 输  入: channel：波形通道号
**				 Per：波形的周期值
**				 Scope：波形的幅度值
** 描  述: 产生疏密波，波形参数由输入参数决定，疏密波类型为：10s密集波，5s疏松波，依次延伸                                         
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-04-21
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
/****************************************产生疏密波******************************************/

static void Dilatational_Wave(uint8_t channel, uint16_t peroid, uint16_t scope) //产生疏密波	频率之比  疏松 ：密集 = 1 ：5
{	
	if (CH[channel].time5s_count < 2)	//由于time5s是5s加一次，所以10s需要2次  在10s时间内，产生密集波  	   
	{
		if (CH[channel].Dwave_count_flag == FALSE)        	/*  在密集和疏松切换时，要分别开始计数  */
		{
			CH[channel].Dwave_count_flag = TRUE;
			CH[channel].wave_count = CLEARVALUE;
		}
		wave_base(channel,peroid,scope);
	}
	else				                                           /*   在5s时间内，产生疏松波    */
	{
		if (CH[channel].Dwave_count_flag )
		{
			CH[channel].Dwave_count_flag = FALSE;
			CH[channel].wave_count = CLEARVALUE;
		}	

		if (peroid > Peroid_10hz)              /*  如果输入的密波频率小于10hz，则疏波的频率=f密波/5;疏波最小为2hz  */
		{
			wave_base(channel,Peroid_2hz,scope);
		}
		else 
		{
			wave_base(channel,CH[channel].ss_peroid,scope); //	
		}		

		if (CH[channel].time5s_count > 2)       /*  5s时间的疏松波产生完成，即进入下一周期  */
		{
			CH[channel].time5s_count = CLEARVALUE;
		}
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
void wave_output(void)
{	
	ReadyToUpdata();//拉高，使得LDAC能有变低动作来更改
	for(count = 0; count < 6; count++)	/*    轮询通道的开启停止状态位   */
	{			 
		if (CH[count].channel_open){	        /*  相应通道打开，则产生相应波形  */
			if(CH[count].led_sta){
				STM_CHN_LED_ON(CH[count].channel);
				CH[count].led_sta = 0;
			}
			switch(CH[count].function_code){        //选择波形 
				case Contious: Continuous_Wave(count,CH[count].peroid,CH[count].scope);break;
				case Interrupt:	Interrupted_Wave(count,CH[count].peroid,CH[count].scope);break;
				case Dilatational:Dilatational_Wave(count,CH[count].peroid,CH[count].scope);break;	
				default :break;
			}				
		}else{
			if (CH[count].channel_stop_state){	 //通道的停止状态，标志位做法是保证只会进入一次，以节约时间
				SPI_SendData((uint16_t)count << ChannelOffset,0);
				CH[count].channel_stop_state = 0;
				STM_CHN_LED_OFF(CH[count].channel);
			}
		}
	}	
	UpdataDAOutput();			      //使DA输出指定电压数据  更新DA
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
void WaveWork(void)
{	
	if (!urgstop){    /******未出现上层重发次数超限的情况则继续产生波形，若出现超限，则不进入波形发生*******/
		wave_output();
		LightLed(&led_time_count,Led2);
	}else{                          //紧急停止情况，LED闪烁
		LightLed(&led_time_count,Led4);
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
void WaveExport(void)
{
	static uint32_t TIM_timeOld = 0;
	if (TIMx_timecount != TIM_timeOld){
		TIM_timeOld = TIMx_timecount;
		WaveWork();//产生波形的函数
	}
}
