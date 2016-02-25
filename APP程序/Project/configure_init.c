#include "configure_init.h"


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
void ReportState(void)
{
	if (system_start && topdeath)
	{
			heart_time_count = 0;
			state_change(TopDead);           //更改当前状态位上层死机，通信出现故障 
		SendStatusReply(current_state);
		  urgstop = TRUE;
			topdeath = FALSE;
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
   /******连续10次的命令都是错误情况，则认为上层与下层通信错误，下层做紧急停止*******/
void ErrorCodeLimit(void)
{
	if (ErrPar.time_limit)                                                  //出现错误情况，规定时间到 ，如果错误次数还是超限，则认为确实超限
	{
		ErrPar.time_limit = FALSE;
		if (errcode_count > ERRORCODE_UP_LIMIT)
		{
			urgstop = TRUE;                                                    //紧急停止标志位打开，后续作调整
			state_change(ErrorCodeOverLimit);
			SendStatusReply(current_state);                             //返回当前状态值
			errcode_count = 0;	                                       //清零错误命令次数计数值
		}
		else errcode_count = 0;                                     //手动长时间间隔点击发送相同命令则无错误超限情况存在
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
     /******连续10次的命令都与上次相同，则认为上层没有收到反馈，下层做紧急停止*******/
void RepeatCodeLimit(void)
{
	if (RepPar.time_limit)                                                   //出现重复情况，规定时间到 ，如果重复次数还是超限，则认为确实超限
	{
		RepPar.time_limit = FALSE;
		if (rep_count >= ORDER_REPEAT_UP_LIMIT)  
		{
			urgstop = TRUE;					                                            //紧急停止标志位打开，后续作调整
			state_change(OrderRepeatOverLimit);
			SendStatusReply(current_state);
			rep_count = 0;                                             //清零重复命令次数计数值
		}
		else rep_count = 0;	                                      //手动长时间间隔点击发送相同命令则无重复超限情况存在
	}
}

/******************************************************************
** 函数名: HandleRecData
** 输  入: 无
** 描  述: 处理串口接收到的数据
**                                                                  
** 全局变量: 
** 调用模块: 
** 作  者:   zcs
** 日  期:   2015-04-21
** 修  改:
** 日  期:

** 版  本: 1.0
*******************************************************************/
void HandleRecData(void)
{
	if (WorkQueueData(&Queue,&get_whole_data)) //判定接收到的指令是否匹配正确，若正确则继续运行，否则退出
	{
		ExecuteCmd(PretreatBuffer);
	}
}
