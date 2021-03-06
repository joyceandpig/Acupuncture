#include "dma.h"
#include "platform_config.h"
#include "calculate.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK miniSTM32开发板
//DMA 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/8
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

DMA_InitTypeDef DMA_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void spi_dma_init(void);
typedef struct{
	void (*dma)();
	void (*dma1)();
}DMA;
typedef struct{
	DMA *_dma;
	void (*clk)();
	void (*paramer)();
}SPI;

DMA Dma={
	(void *)spi_dma_init,
};
SPI _spi= 
{
	&Dma,

};

u16 SpiSendBuff;
int rec_num = 200;
u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长�
static u8 USART1_RxBuffer[200] = {0};


void DMA_init(void)
{
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
	DMA_DeInit(DMA1_Channel6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USARTx->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Queue.items[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = CAPACITY;//sizeof(USART1_RxBuffer);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Channel6, ENABLE);
	
//	DMA_DeInit(DMA1_Channel4);
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART1_RxBuffer[0];
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//	DMA_InitStructure.DMA_BufferSize = sizeof(USART1_RxBuffer);
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
//	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
//  DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
	
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
//	USART_Cmd(USART1, ENABLE);
}

void DMA1_Channel6_IRQHandler(void)
{
	if ( DMA_GetITStatus(DMA1_IT_TC5) )
	{
		DMA_ClearITPendingBit(DMA1_IT_TC5);
		GPIO_SetBits(GPIO_TLV5608,TLV5608_FS);/* Set FS to high , stop to transmit data */
	}
}
//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Cmd(DMA_Streamx, DISABLE);                      //??DMA?? 
	while (DMA_GetFlagStatus(DMA1_FLAG_TC5)!=RESET){} //??DMA?????  
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //?????  
	DMA_Cmd(DMA_Streamx, ENABLE);                      //??DMA?? 
}
void spi_dma_init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	  /*使能SPI1使用的GPIO时钟 1*/
  RCC_APB1PeriphClockCmd(SPIx_CLK,ENABLE);
  RCC_APB2PeriphClockCmd(SPIx_GPIO_CLK,ENABLE);

   /*配置SPI1使用PORT口 */
  GPIO_InitStructure.GPIO_Pin   = SPIx_SCK | SPIx_MOSI; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPIx_GPIO, &GPIO_InitStructure);
	
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); //PB13??? SPI2
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2); //PB14??? SPI2
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2); //PB15??? SPI2
	
	SPI_Cmd(SPIx,DISABLE);                                              /*  <<<  must disable spi2 before change the mode at first  */	
	SPI_InitStructure.SPI_Direction         = SPI_Direction_1Line_Tx;   /* Configuration SPI Mode is one Line and only Tx */      	
	SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;          /*  Configuration SPI  is Master   */            
	SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;  
	SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;            /*  SCK 起始状态（高或者低）时钟电平起始电平设置 (以一个周期为单位计算传送位) */
	SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;           /* 0 ：在SCK起始边沿改变，在下一边沿捕获    1 ： 在SCK起始边沿捕获，在下一边沿改变 */   
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;             /* choice soft NSS  */   	 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //4
	SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;         /* transfromer first bit is MSB or LSB  */       	
	SPI_InitStructure.SPI_CRCPolynomial     = 7;   
	SPI_Init(SPIx,&SPI_InitStructure);
//	SPI_Cmd(SPIx,ENABLE);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(SPIx->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&SpiSendBuff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = sizeof(SpiSendBuff);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);
	
	SPI_I2S_DMACmd(SPIx,SPI_I2S_DMAReq_Tx,ENABLE);       //??DMA??
	SPI_Cmd(SPIx,ENABLE);
	
//	DMA_Cmd(DMA1_Channel5, ENABLE);
}
 void SPI_DMA_SendData(uint16_t cmd_object, uint16_t Data)
{
	if (cmd_object == Channel_CTRL0 || cmd_object == Channel_CTRL1)
		Data = cmd_object | Data;  			 //初始化芯片参数时不需要进行移位	
	else
	 	Data = cmd_object | (Data << 2); //DA寄存器的低位的2BIT没使用，规定要左移两位
	SpiSendBuff = Data;
//	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);  /* Wait for SPI1 Tx buffer empty */
	GPIO_ResetBits(GPIO_TLV5608,TLV5608_FS);	/* Set FS to low , start to transmit data */
	MYDMA_Enable(DMA1_Channel5,1);
//	GPIO_SetBits(GPIO_TLV5608,TLV5608_FS);/* Set FS to high , stop to transmit data */
}

























