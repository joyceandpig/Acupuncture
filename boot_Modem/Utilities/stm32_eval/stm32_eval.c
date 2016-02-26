/* Define to prevent recursive inclusion -------------------------------------*/
#include "stm32_eval.h"
#include "platform_config.h"
/** @defgroup STM32 Private_Variables
  * @{
  */ 
USART_InitTypeDef USART_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
/**
  * @}
  */ 
USART_TypeDef* COM_USART[COMn]       = {EVAL_COM1, EVAL_COM2}; 

GPIO_TypeDef* COM_TX_PORT[COMn]      = {EVAL_COM1_TX_GPIO_PORT, EVAL_COM2_TX_GPIO_PORT};
 
GPIO_TypeDef* COM_RX_PORT[COMn]      = {EVAL_COM1_RX_GPIO_PORT, EVAL_COM2_RX_GPIO_PORT};

TIM_TypeDef * TIM_NUM[NUMn]          = {EVAL_TIM2, EVAL_TIM3, EVAL_TIM4};


const uint32_t COM_USART_CLK[COMn]   = {EVAL_COM1_CLK, EVAL_COM2_CLK};

const uint32_t COM_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK, EVAL_COM2_TX_GPIO_CLK};
 
const uint32_t COM_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK, EVAL_COM2_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[COMn]      = {EVAL_COM1_TX_PIN, EVAL_COM2_TX_PIN};

const uint16_t COM_RX_PIN[COMn]      = {EVAL_COM1_RX_PIN, EVAL_COM2_RX_PIN};
const uint16_t COM_IRQn[COMn]        = {EVAL_COM1_IRQn, EVAL_COM2_IRQn};


const uint16_t TIM_NUM_CLK[NUMn]     = {EVAL_TIM2_CLK, EVAL_TIM3_CLK, EVAL_TIM4_CLK};
const uint16_t TIM_NUM_IRQn[NUMn]    = {EVAL_TIM2_IRQn, EVAL_TIM3_IRQn, EVAL_TIM4_IRQn};



/******************************************************************
** ������: USART_PARA_CONFIGURE
** ��  ��: baud_115200,baud_9600
           baud_115200, ѡ������Ϊ115200
           baud_9600,ѡ������Ϊ9600
** ��  ��: ����ʹ�ô��ڵĲ���
** 
** ȫ�ֱ���: 
** ����ģ��: 
** ��  ��:   zcs
** ��  ��:   2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
static void USART_PARA_CONFIGURE(u32 baudrate, uint16_t COM)//BaudRate_TypeDef
{
  USART_InitStructure.USART_BaudRate            = baudrate;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
//  USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);
  USART_Init(COM_USART[COM], &USART_InitStructure);
  USART_Cmd(COM_USART[COM], ENABLE);
	while (USART_GetFlagStatus(COM_USART[COM], USART_FLAG_TC) == RESET);/*�ȴ� ���������*/
}
/******************************************************************
** ������:  USART_NVIC_CONFIGURE
** ��  ��:  COM  COM = COM1,��ʾѡ�񴮿�1
                 COM = COM2����ʾѡ�񴮿�2   
** ��  ��: ���ô��ڵķ��ͽ����ж�
** 
** ȫ�ֱ���: 
** ����ģ��:
** ��  ��:   zcs
** ��  ��:   2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
static void USART_NVIC_CONFIGURE(uint16_t COM)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	
  NVIC_InitStructure.NVIC_IRQChannel                   = COM_IRQn[COM];	
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;  /*��ռ���ȼ�����ԽС���ȼ�Խ��*/
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

   /*ʹ�ܴ��ڵķ��ͺͽ����ж�*/
//  USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);  
//  USART_ITConfig(COM_USART[COM], USART_IT_TXE, ENABLE);
}
/******************************************************************
** ������:  USART_GPIO_CONFIGURE
** ��  ��:  COM  COM = COM1,��ʾѡ�񴮿�1
                 COM = COM2����ʾѡ�񴮿�2   
** ��  ��: ���ô���ʹ�õ�GPIO�ţ�ʱ�Ӽ����ù���
** 
** ȫ�ֱ���: 
** ����ģ��:
** ��  ��:   zcs
** ��  ��:   2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
static void USART_GPIO_CONFIGURE(COM_TypeDef COM)
{
  GPIO_InitTypeDef GPIO_InitStructure;    
  /* Enable GPIO clock */	   
  RCC_APB2PeriphClockCmd( COM_TX_PORT_CLK[COM] | \
													COM_RX_PORT_CLK[COM] | \
													RCC_APB2Periph_AFIO, ENABLE);
	
  if (COM == COM1)/* Enable UART clock */
    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
  else
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
	
	USART_DeInit(COM_USART[COM]);

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;    //���ù��ܵ��������_AF_PP ����Ƭ�����蹦��
  GPIO_InitStructure.GPIO_Pin   = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin  = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);
}
/******************************************************************
** ������: STM_EVAL_COMInit
** ��  ��: none
** ��  ��: ���ô��ڹ��ܣ������������ü���������
** 
** ȫ�ֱ���: 
** ����ģ��: USART_PARA_CONFIGURE
             USART_GPIO_CONFIGURE
** ��  ��:   zcs
** ��  ��:   2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
void STM_EVAL_COMInit(uint8_t com)
{
	u32 baud = 0;
	baud = (  *(u8 *)BaudRateReadAddr)*65536 \
					+(*((u8 *)(BaudRateReadAddr+3))*256)\
					+(*((u8 *)(BaudRateReadAddr+2)));

  USART_GPIO_CONFIGURE(com);                  //����ʱ�Ӻ���������
//  USART_NVIC_CONFIGURE(com);                  //�����ж�����  
	if(!IsUsartBaudRight(baud)){
		baud = 9600;
	}
  USART_PARA_CONFIGURE(baud,com);      //���ڲ�������Baud_9600
}
/******************************************************************
** ������:  TIM_CONFIGURE
** ��  ��:  TIM_x  TIM_X = Tim2,��ʾѡ��ʱ��2
                 TIM_X = Tim3����ʾѡ��ʱ��3   
** ��  ��: ���ö�ʱ����ʱ�Ӽ��жϲ���
** 
** ȫ�ֱ���: 
** ����ģ��:
** ��  ��:   zcs
** ��  ��:   2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
static void TIM_NVIC_CONFIGURE(TypeDef_TIM TIM_X)
{
  NVIC_InitTypeDef NVIC_InitStructure;
    
  RCC_APB1PeriphClockCmd(TIM_NUM_CLK[TIM_X],ENABLE);        /*ʹ�ܻ�����ʱ��2ʱ��(TIM2)*/	
  NVIC_InitStructure.NVIC_IRQChannel                   = TIM_NUM_IRQn[TIM_X];
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	/*��ռ���ȼ�����ԽС���ȼ�Խ��*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0; /*��ռ���ȼ�����ԽС���ȼ�Խ��*/
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/******************************************************************
** ������:  TIM_PARA_INIT
** ��  ��:  TIM_x  TIM_X = Tim2,��ʾѡ��ʱ��2
                 TIM_X = Tim3����ʾѡ��ʱ��3   
** ��  ��: ���ö�ʱ���ĸ������
** 
** ȫ�ֱ���: 
** ����ģ��:
** ��  ��:   zcs
** ��  ��:   2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/

static void TIM_PARA_CONFIGURE(TypeDef_TIM  TIM_X, uint16_t timecount)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	timecount *= 72;
	/*  !<  To Set the Timer OutCount ,Step to the num ,Event take break   72MHZ value < ffff(65535)*/
	TIM_TimeBaseStructure.TIM_Period        = timecount;  //72 -> 1us 
	/* !< 72MHZ BUSCLOCK and the timerclock = busclock/TIM_Prescaler  72MHZ/36000 = 2khz --->  T = 0.5ms*/
	TIM_TimeBaseStructure.TIM_Prescaler     = 0;   //value < ffff	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;       /*  clock division [ʱ�ӷ�Ƶϵ��] */
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; /* <<  Set TIM_Countermode    up means add mode*/	 
	TIM_TimeBaseInit(TIM_NUM[TIM_X], &TIM_TimeBaseStructure);   /*  !!!  save those data to TIM register     */	
	TIM_ITConfig(TIM_NUM[TIM_X],TIM_IT_Update,ENABLE);    /* !~~  Enable  TIM interrupt function  */
	TIM_Cmd(TIM_NUM[TIM_X],ENABLE);         	/*   <<<   Enable TIM   */
}
/******************************************************************
** ������:  STM_EVAL_TIMCONFIGURE
** ��  ��:  none
** ��  ��:  ���ö�ʱ���Ĳ��������ö�ʱ�ж�
** 
** ȫ�ֱ���: none 
** ����ģ��: TIM_NVIC_CONFIGURE
              TIM_PARA_CONFIGURE
** ��  ��:   zcs
** ��  ��:   2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
static void STM_EVAL_TIMCONFIGURE(uint16_t TimX,uint8_t time)
{
  TIM_NVIC_CONFIGURE(TimX);
  TIM_PARA_CONFIGURE(TimX,time);//���붨ʱʱ�䣬��λΪus
}
/******************************************************************
** ������:  STM_EVAL_TIMInit
** ��  ��:  none
** ��  ��:  ���ö�ʱ��
** 
** ȫ�ֱ���: none 
** ����ģ��: STM_EVAL_TIMCONFIGURE
** ��  ��:   zcs
** ��  ��:   2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
static void STM_EVAL_TIMInit(void)
{ 
  STM_EVAL_TIMCONFIGURE(Tim2,100);//TIM_Cmd(TIM_NUM[Tim2],DISABLE);//���β���ר��ʱ�� ���붨ʱʱ�䣬��λΪus
}

/******************************************************************
** ������: STM_EVAL_STMLED_GPIO
** ��  ��: none
** ��  ��: ����stm32��ʹ�õ�LED�ƣ�PORT_B Pin_12��
** 
** ȫ�ֱ���: none 
** ����ģ��: clock gpio  
** ��  ��:   zcs
** ��  ��:   2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
static void STM_EVAL_STMWORK_LED(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);		//ʹ��PC�˿�ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  GPIO_InitStructure.GPIO_Pin   = /*LED1*/GPIO_Pin_4 | /*LED3*/GPIO_Pin_5 |\
															   /*LED3*/GPIO_Pin_8 | /*LED4*/GPIO_Pin_9;  //�˿�����
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; 		 		//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 		//IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//�����趨������ʼ��
	
  GPIO_SetBits(GPIOB,
	            GPIO_Pin_4 | GPIO_Pin_5 | \
							GPIO_Pin_8 | GPIO_Pin_9   \
						  );																					//PC0:PC3 �����
}

/******************************************************************
** ������: STM_EVAL_GPIOInit
** ��  ��: none
** ��  ��: GPIO���ã�����ʹ�õ�stm32���ŵģ��������Ｏ������
** 
** ȫ�ֱ���: none
** ����ģ��:  STM_EVAL_RELAY_GPIO();         
              STM_EVAL_STMLED_GPIO();        
              STM_EVAL_ZIGBEE_GPIO();        
              STM_EVAL_WIFI_GPIO();          
** ��  ��:   zcs
** ��  ��:  2015-03-11
** ��  ��:
** ��  ��:
** ��  ��: 1.0
*******************************************************************/
static void STM_EVAL_GPIOInit(void)
{
  STM_EVAL_STMWORK_LED();          //stm32����ָʾ����������
}
/******************************************************************
** ������: STM_EVAL_DELAYInit
** ��  ��: none
** ��  ��: ��ʱ������ʼ��
** 
** ȫ�ֱ���: none
** ����ģ��:  DelayInit
** ��  ��:  zcs
** ��  ��:  2015-03-11
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
static void STM_EVAL_DELAYInit(void)
{
  DelayInit();
}

/******************************************************************
** ������: fputc
** ��  ��: none
** ��  ��: ϵͳ������Ϣ��ӡ����
** 
** ȫ�ֱ���: none
** ����ģ��: USART_SendData 
** ��  ��:  zcs
** ��  ��:  2015-03-11
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch); /*����һ���ַ�����*/
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);/*�ȴ��������*/
  return ch;
}

/******************************************************************
** ������: STM_EVAL_SYSTEMInit
** ��  ��: none
** ��  ��: ϵͳ��ʼ������
** 
** ȫ�ֱ���: none
** ����ģ��:  STM_EVAL_COMInit
              STM_EVAL_EXTIInit
              STM_EVAL_GPIOInit
              STM_EVAL_TIMInit
** ��  ��:  zcs
** ��  ��:  2015-03-11
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
void STM_EVAL_SYSTEMINIT(void)
{
//  STM_EVAL_COMInit(USART_PORT);        //��������
  STM_EVAL_GPIOInit();       //GPIO���ã������õ��Ķ���������
//  STM_EVAL_TIMInit();        //��ʱ������
  STM_EVAL_DELAYInit();      //��ʱ������ʼ�� 
	
}

