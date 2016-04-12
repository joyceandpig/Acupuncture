/**
  ******************************************************************************
  * @file    stm32_eval.c
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 
  * 
  * 
  * 
  * 
  * 
  *
  * 
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#include "stm32_eval.h"
#include "AD5933.h"
#include "dma.h"
u32 usart_baud = BaudRate;
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


/**
  ******************************************************************************
  * @file    stm32_eval.c
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention

  ******************************************************************************/ 


void STM_CHN_LED_ON(u8 lednum)
{
	switch(lednum)
	{
		case 0:STM_CHN_LED6 = 1;break;
		case 1:STM_CHN_LED5 = 1;break;
		case 2:STM_CHN_LED4 = 1;break;
		case 3:STM_CHN_LED3 = 1;break;
		case 4:STM_CHN_LED2 = 1;break;
		case 5:STM_CHN_LED1 = 1;break;
		default:break;
	}
}
void STM_CHN_LED_OFF(u8 lednum)
{
	switch(lednum)
	{
		case 0:STM_CHN_LED6 = 0;break;
		case 1:STM_CHN_LED5 = 0;break;
		case 2:STM_CHN_LED4 = 0;break;
		case 3:STM_CHN_LED3 = 0;break;
		case 4:STM_CHN_LED2 = 0;break;
		case 5:STM_CHN_LED1 = 0;break;
		default:break;
	}
}

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
static void USART_PARA_CONFIGURE(BaudRate_TypeDef baudrate, uint16_t COM)
{
  USART_InitStructure.USART_BaudRate            = baudrate;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
//	USART_Init(COM_USART[COM], &USART_InitStructure);
//  USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);//ʹ�ܽ����ж�
//  USART_Cmd(COM_USART[COM], ENABLE);
//	while (USART_GetFlagStatus(COM_USART[COM], USART_FLAG_TC) == RESET);/*�ȴ� ���������*/
	
  DMA_init();
  USART_Init(COM_USART[COM], &USART_InitStructure);
  USART_ITConfig(COM_USART[COM], USART_IT_IDLE, ENABLE);//ʹ�ܽ����ж�
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
	
	USART_DeInit(USARTx);

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
void STM_EVAL_COMInit(COM_TypeDef com)
{
  USART_GPIO_CONFIGURE(com);                  //����ʱ�Ӻ���������
  USART_NVIC_CONFIGURE(com);                  //�����ж�����  
  USART_PARA_CONFIGURE(usart_baud,com);      //���ڲ�������
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
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	/*��ռ���ȼ�����ԽС���ȼ�Խ��*/
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2; /*��ռ���ȼ�����ԽС���ȼ�Խ��*/
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
#ifdef SYSCLOCK_72M
	timecount *= 72;//72
#else
	timecount *= 32;
#endif
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
** ��  ��:  none``11111
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
  STM_EVAL_TIMCONFIGURE(Tim3,100);//TIM_Cmd(TIM_NUM[Tim3],DISABLE);//���ֳ����ʱר��ʱ�� ���붨ʱʱ�䣬��λΪus
//  STM_EVAL_TIMCONFIGURE(Tim4,100);TIM_Cmd(TIM_NUM[Tim4],DISABLE);  //���ڽ�������ר�ö�ʱ�� ���붨ʱʱ�䣬��λΪus
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
static void STM_EVAL_CHANNEL_LED(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//ʹ��PC�˿�ʱ��
  GPIO_InitStructure.GPIO_Pin = /*LED1*/GPIO_Pin_0 | /*LED2*/GPIO_Pin_1 |\
															  /*LED3*/GPIO_Pin_3 ;  //�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 		//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 		//IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//�����趨������ʼ��
	
  GPIO_ResetBits(GPIOB,
	            GPIO_Pin_0 | GPIO_Pin_1 | \
							GPIO_Pin_3 
						  );																					//PC0:PC3 �����
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//ʹ��PC�˿�ʱ��
  GPIO_InitStructure.GPIO_Pin = /*LED1*/GPIO_Pin_4 | /*LED2*/GPIO_Pin_6 |\
															  /*LED3*/GPIO_Pin_15 ;  //�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 		//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 		//IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 				//�����趨������ʼ��
	
  GPIO_ResetBits(GPIOA,
	            GPIO_Pin_4 | GPIO_Pin_6 | \
							GPIO_Pin_15 
						  );
}
static void STM_EVAL_BELL(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//ʹ��PC�˿�ʱ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  //�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 		//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 		//IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//�����趨������ʼ��
	
  GPIO_ResetBits(GPIOB,GPIO_Pin_11);	
//	GPIO_SetBits(GPIOC,GPIO_Pin_4);	
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
  STM_EVAL_CHANNEL_LED();					 //ͨ�����ָʾ��
	STM_EVAL_BELL();//����������
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
  USART_SendData(COM_USART[USART_PORT], (uint8_t) ch); /*����һ���ַ�����*/
  while (USART_GetFlagStatus(COM_USART[USART_PORT], USART_FLAG_TC) == RESET);/*�ȴ��������*/
  return ch;
}
/******************************************************************
** ������: STM_SPI_CLK_PORT
** ��  ��: none
** ��  ��: ����SPiʹ�õ�ʱ�ӺͶ�Ӧ�˿�
** 
** ȫ�ֱ���: none
** ����ģ��: 
** ��  ��:  zcs
** ��  ��:  2015-03-11
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
static void STM_SPI_CLK_PORT(void)
{
  /*ʹ��SPI1ʹ�õ�GPIOʱ�� 1*/
  RCC_APB1PeriphClockCmd(SPIx_CLK,ENABLE);
  RCC_APB2PeriphClockCmd(SPIx_GPIO_CLK,ENABLE);

   /*����SPI1ʹ��PORT�� */
  GPIO_InitStructure.GPIO_Pin   = SPIx_SCK | SPIx_MOSI; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPIx_GPIO, &GPIO_InitStructure);
}
/******************************************************************
** ������: STM_SPI_MODE_PARA
** ��  ��: none
** ��  ��: SPI������������
** 
** ȫ�ֱ���: none
** ����ģ��: 
** ��  ��:  zcs
** ��  ��:  2015-03-11
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
static void STM_SPI_MODE_PARA(void)
{
	SPI_InitTypeDef SPI_InitStructure;
  
	SPI_Cmd(SPIx,DISABLE);                                              /*  <<<  must disable spi2 before change the mode at first  */	
	SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;   /* Configuration SPI Mode is one Line and only Tx */      	
	SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;          /*  Configuration SPI  is Master   */            
	SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;  
	SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;            /*  SCK ��ʼ״̬���߻��ߵͣ�ʱ�ӵ�ƽ��ʼ��ƽ���� (��һ������Ϊ��λ���㴫��λ) */
	SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;           /* 0 ����SCK��ʼ���ظı䣬����һ���ز���    1 �� ��SCK��ʼ���ز�������һ���ظı� */   
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;             /* choice soft NSS  */   	 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; 
	SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;         /* transfromer first bit is MSB or LSB  */       	
	SPI_InitStructure.SPI_CRCPolynomial     = 7;   
	SPI_Init(SPIx,&SPI_InitStructure);
	SPI_Cmd(SPIx,ENABLE);
}
/******************************************************************
** ������: STM_EVAL_SPIInit
** ��  ��: none
** ��  ��: spi��ʼ�����ã�����ʱ�Ӷ˿ں͹�������
** 
** ȫ�ֱ���: none
** ����ģ��: STM_SPI_CLK_PORT
						 STM_SPI_MODE_PARA
** ��  ��:  zcs
** ��  ��:  2015-03-11
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
static void STM_EVAL_SPIInit(void)
{
  STM_SPI_CLK_PORT();   //����spi��ʱ�ӺͶ˿�
  STM_SPI_MODE_PARA();	//����spi����ģʽ�Ĳ���
}
/******************************************************************
** ������: SPI_SendData
** ��  ��: cmd_object����Ҫ���Ĳ�����ָ�����
					 Data�����͵�����
** ��  ��: SPI�������ݺ���������ָ����������ֵ
** 
** ȫ�ֱ���: none
** ����ģ��: 
** ��  ��:  zcs
** ��  ��:  2015-04-20
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
extern void SPI_SendData(uint16_t cmd_object, uint16_t Data)
{
	if (cmd_object == Channel_CTRL0 || cmd_object == Channel_CTRL1)
		Data = cmd_object | Data;  			 //��ʼ��оƬ����ʱ����Ҫ������λ	
	else
	 	Data = cmd_object | (Data << 2); //DA�Ĵ����ĵ�λ��2BITûʹ�ã��涨Ҫ������λ
	
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);  /* Wait for SPI1 Tx buffer empty */
	GPIO_ResetBits(GPIO_TLV5608,TLV5608_FS);	/* Set FS to low , start to transmit data */
	SPI_I2S_SendData(SPIx, Data);    /* Send SPI1 data  */
	GPIO_SetBits(GPIO_TLV5608,TLV5608_FS);/* Set FS to high , stop to transmit data */
}
/******************************************************************
** ������: DAC_CLK_PORT
** ��  ��: none
** ��  ��: DAоƬ��ʱ�ӺͶ˿�����
** 
** ȫ�ֱ���: none
** ����ģ��: 
** ��  ��:  zcs
** ��  ��:  2015-04-20
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
static void DAC_CLK_PORT(void)
{
  /*TLV5608��������ʹ�õ�GPIOʱ��*/
  RCC_APB2PeriphClockCmd(RCC_GPIO_TLV5608, ENABLE);

  /*����TLV5608��������ʹ�õ�GPIO�ܽ�ģʽ*/
  GPIO_InitStructure.GPIO_Pin   = TLV5608_FS|TLV5608_LDAC; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_TLV5608, &GPIO_InitStructure);
}
/******************************************************************
** ������: ReadyToUpdata
** ��  ��: none
** ��  ��: Ϊ����DA���������׼��
** 
** ȫ�ֱ���: none
** ����ģ��: GPIO_HIGH 
** ��  ��:  zcs
** ��  ��:  2015-04-20
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
void ReadyToUpdata(void)
{
	GPIO_HIGH(GPIO_TLV5608,TLV5608_LDAC);			      //���ߣ�ʹ��LDAC���б�Ͷ���������
}
/******************************************************************
** ������: UpdataDAOutput
** ��  ��: none
** ��  ��: ����DAоƬ�����
** 
** ȫ�ֱ���: none
** ����ģ��: GPIO_LOW
** ��  ��:  zcs
** ��  ��:  2015-04-20 
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
void UpdataDAOutput(void)
{
	GPIO_LOW(GPIO_TLV5608,TLV5608_LDAC);			      //ʹDA���ָ����ѹ����  ����DA
}
/******************************************************************
** ������: DAC_MODE_PARA
** ��  ��: none
** ��  ��: DAоƬ���в���������
** 
** ȫ�ֱ���: none
** ����ģ��: SPI_SendData 
						 ReadyToUpdata
						 UpdataDAOutput
** ��  ��:  zcs
** ��  ��:  2015-03-11
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
static void DAC_MODE_PARA(void)
{
	/**  MODE pin   1:uc Mode  	 
     *				0:Dsp Mode 	 
	 	
	 *  		D11 D10 ....D7     D6     D5    D4     D3     D2      D1     D0 */
	 /*  CTRL0 	 x	 x						    PD	   DO					 IM	   PD:full device powerdown  0:normal 1:PD 
	 																			   DO:diagil out  0:disable 1:enable
																				   IM:������or����	  0:������ 1:����
	 *   CTRL1   x   x 		Pgh	   Pef	 Pcd  	Pab	   Sgh    Sef     Scd    Sab
	 																			   Pxy:DACxy Powerdown	  0:normal  1:PD
																				   Sxy:DACxy Speed 0:slow  1:fast
	 */
  static uint8_t da_chn_count_i;
//	SPI_SendData(Channel_CTRL0,0x0010);						//����powerdown ģʽ
	SPI_SendData(Channel_CTRL1,0x000f);							//ʹ����ͨ�����ڿ���ģʽ��,����ѹ����
  ReadyToUpdata();
  for(da_chn_count_i = 0; da_chn_count_i < 6;da_chn_count_i++)
		SPI_SendData((uint16_t)da_chn_count_i << ChannelOffset,0);
	UpdataDAOutput();//
}
/******************************************************************
** ������: STM_EVAL_TLV5608Init
** ��  ��: none
** ��  ��: DAоƬ���Ƶĳ�ʼ���������˿����ú�оƬ�����в�������
** 
** ȫ�ֱ���: none
** ����ģ��: DAC_CLK_PORT
						 DAC_MODE_PARA
** ��  ��:  zcs
** ��  ��:  2015-04-20
** ��  ��:
** ��  ��: 
** ��  ��: 1.0
*******************************************************************/
static void STM_EVAL_TLV5608Init(void)					  
{
	DAC_CLK_PORT();//
	DAC_MODE_PARA();//
}
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	  USART_DeInit(USART2);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	  NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	 USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	 USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

}
void STM_EVAL_MCOCLK_ENABLE(void)
{
	RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);
}
void STM_EVAL_MCOCLK_DISABLE(void)
{
	RCC_MCOConfig(RCC_MCO_NoClock);
}
void STM_EVAL_MCOCLK_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	STM_EVAL_MCOCLK_DISABLE();
	STM_EVAL_MCOCLK_ENABLE();
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
  STM_EVAL_COMInit(USART_PORT);        //��������
  STM_EVAL_GPIOInit();       //GPIO���ã������õ��Ķ���������
  STM_EVAL_TIMInit();        //��ʱ������
  STM_EVAL_DELAYInit();      //��ʱ������ʼ�� 
	STM_EVAL_SPIInit();        //SPI�ӿڳ�ʼ��
	STM_EVAL_TLV5608Init();    //Tlv5608оƬ���Ƴ�ʼ��

	
	AD5933_Para_Init();     //��ʼ���迹�ɼ�����
	delay_ms(1000);
	STM_EVAL_MCOCLK_OUT();//�迹�ɼ�ʱ��Դ
	
//	STM_EVAL_TIM1_PA8_PWM_Init(4,0);
//	TIM_SetCompare1(TIM1,2);
}

