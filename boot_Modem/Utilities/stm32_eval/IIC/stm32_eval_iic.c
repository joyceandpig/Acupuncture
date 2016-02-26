#include "stm32_eval_iic.h"
//
//#define sEE_I2C		I2C1
#define I2C_SLAVE_ADDRESS7 0x1A
//#define I2C_SPEED 	10000
//
//__IO uint16_t  sEEAddress = 0; 

__IO uint32_t  sEETimeout = sEE_LONG_TIMEOUT; 



///**
//  * @brief  Initializes peripherals used by the I2C EEPROM driver.
//  * @param  None
//  * @retval None
//  */
//void sEE_LowLevel_Init(void)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
////  NVIC_InitTypeDef NVIC_InitStructure;  
//    
//  /*!< sEE_I2C Periph clock enable */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
//  
//  /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//  
//  /* Reset sEE_I2C peripheral */
//  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
//  
//  /* Release reset signal of sEE_I2C IP */
//  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
//    
//  /*!< GPIO configuration */  
//  /*!< Configure sEE_I2C pins: SCL */   
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD ;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
//  GPIO_Init(GPIOB , &GPIO_InitStructure);
//
//  /*!< Configure sEE_I2C pins: SDA */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//  GPIO_Init(GPIOB , &GPIO_InitStructure);
//}
//
//
//void sEE_Init(void)
//{ 
//	I2C_InitTypeDef  I2C_InitStructure;
//	
//	sEE_LowLevel_Init();
//	RCC_APB2PeriphClockCmd(RCC_APB1Periph_I2C1 | RCC_APB2Periph_AFIO, ENABLE);//使能I2C1时钟
//	/* GPIOB Periph clock enable */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//
//
//	I2C_DeInit(I2C1); 	
//	/*!< I2C configuration */
//	/* sEE_I2C configuration */
//	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//	I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7;
//	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
//	
//	/* sEE_I2C Peripheral Enable */
//	I2C_Cmd(sEE_I2C, ENABLE);
//	/* Apply sEE_I2C configuration after enabling it */
//	I2C_Init(sEE_I2C, &I2C_InitStructure);
//	sEEAddress = I2C_SLAVE_ADDRESS7;//记录从机地址
////  /* Enable the sEE_I2C peripheral DMA requests */
////  I2C_DMACmd(sEE_I2C, ENABLE);  
//}
//
//uint32_t sEE_WriteData(uint16_t WriteAddr,uint16_t Data )
//{
//	sEETimeout = sEE_LONG_TIMEOUT;
//	/*!< While the bus is busy */
//	while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BUSY))
//	{
//	  if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//	}
//	
//	/*!< Send START condition */
//	I2C_GenerateSTART(sEE_I2C, ENABLE);
//	
//	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//	{//Wait ACK
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback(); 
//	}
//	
//	/*!< Send AD5933 address for write */
//	I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
//	
//	/*!< Test on EV6 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//	{//Wait ACK
//	//	if((sEETimeout--) == 0) 
//			//return sEE_TIMEOUT_UserCallback();   
//	}
//	/*!< Send WriteAddr */
//	I2C_SendData(sEE_I2C, WriteAddr);
//	
//	/*!< Test on EV8 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	
//	/*!< Send Write Data */
//  I2C_SendData(sEE_I2C, Data);	
//	/*!< Test on EV6 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	
//	/*!< Send STOP Condition */
//	I2C_GenerateSTOP(sEE_I2C, ENABLE);
//				
//	/*!< Test on EV8 and clear it */   
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) == RESET)
//	{
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//	}
//
//	/* If all operations OK, return sEE_OK (0) */
//	return sEE_OK ;      
//}
//
//uint32_t sEE_WriteBlock(uint16_t *pBuffer ,uint16_t WriteAddr,uint16_t* NumByteToWrite)
//{
//  uint16_t sEEWriteDataNum = *NumByteToWrite;
//  
//	sEETimeout = sEE_LONG_TIMEOUT;
//	/*!< While the bus is busy */
//	while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BUSY))
//	{
//	  if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//	}
//	
//	/*!< Send START condition */
//	I2C_GenerateSTART(sEE_I2C, ENABLE);
//	
//	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//	{//Wait ACK
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback(); 
//	}
//	
//	/*!< Send AD5933 address for write */
//	I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
//	
//	/*!< Test on EV6 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	/*!< Send WriteAddr */
//	I2C_SendData(sEE_I2C, WriteAddr);
//	
//	/*!< Test on EV8 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//     
//	/*!< Send Second START condition */
//	I2C_GenerateSTART(sEE_I2C, ENABLE);
//	
//	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//	{//Wait ACK
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback(); 
//	}
//	
//	/*!< Send AD5933 address for write */
//	I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
//	
//	/*!< Test on EV6 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	/*!< Send Write Block Comm */
//	I2C_SendData(sEE_I2C, BLOCK_WRITE);
//	
//	/*!< Test on EV8 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	
//	/*!< Send Number of Bytes */
//  I2C_SendData(sEE_I2C, sEEWriteDataNum);
//  	
//	/*!< Test on EV6 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	
//	/*!< Send Block Data */
//	while(sEEWriteDataNum--)
//	{
//    I2C_SendData(sEE_I2C, *pBuffer);	
//  	/*!< Test on EV6 and clear it */
//  	sEETimeout = sEE_FLAG_TIMEOUT;
//    while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//  	{//Wait ACK
//  		if((sEETimeout--) == 0) 
//  			return sEE_TIMEOUT_UserCallback();   
//  	}
//  	pBuffer++;	    
//	}
//	
//	/*!< Send STOP Condition */
//	I2C_GenerateSTOP(sEE_I2C, ENABLE);
//				
//	/*!< Test on EV8 and clear it */   
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF) == RESET)
//	{
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//	}
//
//	/* If all operations OK, return sEE_OK (0) */
//	return 	(*(pBuffer-1)) ;      
//}
//
//uint32_t sEE_ReadData(uint16_t ReadAddr,uint16_t *pBuffer )
//{
//	sEETimeout = sEE_LONG_TIMEOUT;
//	/*!< While the bus is busy */
//	while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BUSY))
//	{
//	  if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//	}
//	
//	/*!< Send START condition */
//	I2C_GenerateSTART(sEE_I2C, ENABLE);
//	
//	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//	{//Wait ACK
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback(); 
//	}
//	
//	/*!< Send AD5933 address for write */
//	I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
//	
//	/*!< Test on EV6 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	/*!< Send ReadAddr */
//	I2C_SendData(sEE_I2C, ReadAddr);
//	
//	/*!< Test on EV8 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//
//  /*!< Send STRAT condition a second time */ 
//	I2C_GenerateSTART(sEE_I2C, ENABLE);
//	
//	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//	{//Wait ACK
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback(); 
//	}
//	
//	/*!< Send AD5933 address for read */
//	I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Receiver);
//	
//  /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_ADDR) == RESET)
//  {
//    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//  }     
//  
//  /*!< Disable Acknowledgement */
//  I2C_AcknowledgeConfig(sEE_I2C, DISABLE);   
//  
//  /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
//  (void)sEE_I2C->SR2;
//  
//  /*!< Send STOP Condition */
//  I2C_GenerateSTOP(sEE_I2C, ENABLE);
//  
//  /* Wait for the byte to be received */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_RXNE) == RESET)
//  {
//    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//  }
//  
//  /*!< Read the byte received from the EEPROM */
//  *pBuffer = I2C_ReceiveData(sEE_I2C);    
//  
//  /* Wait to make sure that STOP control bit has been cleared */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while(sEE_I2C->CR1 & I2C_CR1_STOP)
//  {
//    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//  }  
//  
//  /*!< Re-Enable Acknowledgement to be ready for another reception */
//  I2C_AcknowledgeConfig(sEE_I2C, ENABLE);  
//
//  /* If all operations OK, return sEE_OK (0) */
//  return sEE_OK ;      
//}
//
//uint32_t sEE_ReadBlock(uint16_t *pBuffer ,uint16_t ReadAddr,uint16_t* NumByteToRead)
//{
//  uint16_t sEEReadDataNum = *NumByteToRead;
//	sEETimeout = sEE_LONG_TIMEOUT;
//	/*!< While the bus is busy */
//	while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BUSY))
//	{
//	  if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//	}
//	
//	/*!< Send  START condition */
//	I2C_GenerateSTART(sEE_I2C, ENABLE);
//	
//	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//	{//Wait ACK
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback(); 
//	}
//	
//	/*!< Send AD5933 address for Write */
//	I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
//	
//	/*!< Test on EV6 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	/*!< Send Pointer Comm */
//	I2C_SendData(sEE_I2C, ADDR_PTR );
//	
//	/*!< Test on EV8 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	
//	/*!< Send Pointer Addr */
//	I2C_SendData(sEE_I2C, ReadAddr );
//	
//	/*!< Test on EV8 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}	
//	
//	    
//	/*!< Send Second START condition */
//	I2C_GenerateSTART(sEE_I2C, ENABLE);
//	
//	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//	{//Wait ACK
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback(); 
//	}
//	
//	/*!< Send AD5933 address for Write */
//	I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);
//	
//	/*!< Test on EV6 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	/*!< Send Read Block Comm */
//	I2C_SendData(sEE_I2C, BLOCK_READ );
//	
//	/*!< Test on EV8 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	
//	/*!< Send Number of Bytes */
//  I2C_SendData(sEE_I2C, sEEReadDataNum);
//  	
//	/*!< Test on EV6 and clear it */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
//	{//Wait ACK
//		if((sEETimeout--) == 0) 
//			return sEE_TIMEOUT_UserCallback();   
//	}
//	
//  /*!< Send STRAT condition a 3 time */ 
//	I2C_GenerateSTART(sEE_I2C, ENABLE);
//	
//	/*!< Test on EV5 and clear it (cleared by reading SR1 then writing to DR) */
//	sEETimeout = sEE_FLAG_TIMEOUT;
//	while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
//	{//Wait ACK
//		if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback(); 
//	}
//	
//	/*!< Send AD5933 address for read */
//	I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Receiver);
//	
//  /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_ADDR) == RESET)
//  {
//    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//  } 
////////////////////////////////////////////////	 	
//	while(sEEReadDataNum--)
//	{
//    /* Wait for the byte to be received */
//    sEETimeout = sEE_FLAG_TIMEOUT;
//    while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_RXNE) == RESET)
//    {
//      if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//    }
//    
//    /*!< Read the byte received from the EEPROM */
//    *pBuffer = I2C_ReceiveData(sEE_I2C); 
//    /*!< Enable Acknowledgement */
//    I2C_AcknowledgeConfig(sEE_I2C, ENABLE);         
////
////    /* Wait on ADDR flag to be set (ADDR is still not cleared at this level */
////    sEETimeout = sEE_FLAG_TIMEOUT;
////    while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_ADDR) == RESET)
////    {
////      if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
////    }           
//    pBuffer++;     	  
//	}
//	
//  /*!< Disable Acknowledgement */
//  I2C_AcknowledgeConfig(sEE_I2C, DISABLE);   
//  
//  /* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
//  (void)sEE_I2C->SR2;
//  
//  /*!< Send STOP Condition */
//  I2C_GenerateSTOP(sEE_I2C, ENABLE);
//      
//  /* Wait to make sure that STOP control bit has been cleared */
//  sEETimeout = sEE_FLAG_TIMEOUT;
//  while(sEE_I2C->CR1 & I2C_CR1_STOP)
//  {
//    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
//  }  
//  
//  /*!< Re-Enable Acknowledgement to be ready for another reception */
//  I2C_AcknowledgeConfig(sEE_I2C, ENABLE);  
//  /* If all operations OK, return sEE_OK (0) */
//  return sEE_OK ;       
//}
//
/////////////////////////////////////////////////////////////////////////////////

//
//void sEE_EnterCriticalSection_UserCallback(void)
//{
//  __disable_irq();  
//}
//
//void sEE_ExitCriticalSection_UserCallback(void)
//{
//  __enable_irq();
//}
//


/*===================================================================================*/
static void IIC_delay_us(unsigned int n)
{
  unsigned int i;
  for(i = n;i>0;i--);
} 

//初始化IIC
void sEE_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//先使能外设IO PORTB时钟 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	Set_IIC_SCL;
	Set_IIC_SDA;						   
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);	

}

//产生IIC起始信号
static void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	Set_IIC_SDA;	  	  
	Set_IIC_SCL;
	IIC_delay_us(4);
 	Clr_IIC_SDA;//START:when CLK is high,DATA change form high to low 
	IIC_delay_us(4);
	Clr_IIC_SCL;//钳住I2C总线，准备发送或接收数据 
}
	  
//产生IIC停止信号
static void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	Clr_IIC_SCL;
	Clr_IIC_SDA;//STOP:when CLK is high DATA change form low to high
 	IIC_delay_us(4);
	Set_IIC_SCL; 
	Set_IIC_SDA;//发送I2C总线结束信号
	IIC_delay_us(4);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	Set_IIC_SDA;IIC_delay_us(1);	   
	Set_IIC_SCL;IIC_delay_us(1);
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}	
	Clr_IIC_SCL;//时钟输出0 	   
	return 0;  
}
 
//产生ACK应答
static void IIC_Ack(void)
{
	Clr_IIC_SCL;
	SDA_OUT();
	Clr_IIC_SDA;
	IIC_delay_us(2);
	Set_IIC_SCL;
	IIC_delay_us(2);
	Clr_IIC_SCL;
}

//不产生ACK应答		    
static void IIC_NAck(void)
{
	Clr_IIC_SCL;
	SDA_OUT();
	Set_IIC_SDA;
	IIC_delay_us(2);
	Set_IIC_SCL;
	IIC_delay_us(2);
	Clr_IIC_SCL;
}	
				 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
static void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    Clr_IIC_SCL;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
  		if(txd & 0x80) 	
				Set_IIC_SDA
  		else 
				Clr_IIC_SDA;
      txd <<= 1; 	  
  		IIC_delay_us(2);   //对TEA5767这三个延时都是必须的
  		Set_IIC_SCL;
  		IIC_delay_us(2); 
  		Clr_IIC_SCL;	
  		IIC_delay_us(2);
    }	 
} 	
    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
		Clr_IIC_SCL; 
		IIC_delay_us(2);
		Set_IIC_SCL;
		receive<<=1;
		if(READ_SDA)receive++;   
		IIC_delay_us(1); 
  }					 
  if (!ack)
      IIC_NAck();//发送nACK
  else
      IIC_Ack(); //发送ACK   
  return receive;
}

uint32_t sEE_WriteData(uint16_t WriteAddr,uint16_t Data )
{
 	IIC_Start();

  IIC_Send_Byte(I2C_SLAVE_ADDRESS7 | 0x00);//7位从机地址

  sEETimeout = sEE_FLAG_TIMEOUT;
  while(IIC_Wait_Ack() == 1)//等待应答
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }  
  
  IIC_Send_Byte(WriteAddr);

  sEETimeout = sEE_FLAG_TIMEOUT;
  while(IIC_Wait_Ack() == 1)//等待应答
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  } 
	  
  IIC_Send_Byte(Data);
		
  sEETimeout = sEE_FLAG_TIMEOUT;
  while(IIC_Wait_Ack() == 1)//等待应答
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
	   
  IIC_Stop();
	/* If all operations OK, return sEE_OK (0) */
	return sEE_OK ;      
}

uint32_t sEE_ReadData(uint16_t ReadAddr,uint16_t *pBuffer )
{
  IIC_Start();

  IIC_Send_Byte(I2C_SLAVE_ADDRESS7 | 0x00);//7位从机地址

  sEETimeout = sEE_FLAG_TIMEOUT;
  while(IIC_Wait_Ack() == 1)//等待应答
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }  
  IIC_Send_Byte(ADDR_PTR); 

  sEETimeout = sEE_FLAG_TIMEOUT;
  while(IIC_Wait_Ack() == 1)//等待应答
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }    
  IIC_Send_Byte(ReadAddr);	

  sEETimeout = sEE_FLAG_TIMEOUT;
  while(IIC_Wait_Ack() == 1)//等待应答
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
	  
  IIC_Stop();
  
  IIC_Start();

  IIC_Send_Byte(I2C_SLAVE_ADDRESS7 | 0x01);//7位从机地址

  sEETimeout = sEE_FLAG_TIMEOUT;
  while(IIC_Wait_Ack() == 1)//等待应答
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }  

  *pBuffer = IIC_Read_Byte(0); //读取数据，不应答

  IIC_Stop();
  /* If all operations OK, return sEE_OK (0) */
  return sEE_OK ;      
}

uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {   
  }
}

