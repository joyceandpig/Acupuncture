#include "stm32_eval_SPI.h"

//初始化SPI
void ADG714_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//先使能外设IO PORTA时钟 
	RCC_APB2PeriphClockCmd(	SPI_SCLK_CLK | SPI_SDA_CLK | SPI_SS_CLK | SPI_RST_CLK, ENABLE );	

	GPIO_InitStructure.GPIO_Pin = SPI_SCLK_PIN;         //SCLK引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_SCLK_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_SDA_PIN;          //SDA引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_SDA_PORT, &GPIO_InitStructure);	

 	GPIO_InitStructure.GPIO_Pin = SPI_SS_PIN;           //片选引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_SS_PORT, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = SPI_RST_PIN;            //复位引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;     //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_RST_PORT, &GPIO_InitStructure);

  Clr_SPI_RST;Set_SPI_RST;                              //复位，全开
  
}


void ADG714_WriteData( uint16_t Data )
{
  uint16_t Tmp,i;

  Tmp = Data;

  Set_SPI_SS;//SYNC 置高
  Clr_SPI_SS;//SYNC 置低，开始数据传送

  for(i = 8; i>0; i--)
  {  
    Set_SPI_SCLK;//1   //下降沿采样
    if((Tmp & 0x80) == 0x80)
    {
      Set_SPI_SDA;
    }
    else
    {
      Clr_SPI_SDA;
    } 
    Clr_SPI_SCLK; //0
    Tmp <<= 1;    //左移1位
  }
  Set_SPI_SS;     //SYNC 置高，结束数据传输,更新开关选择
}
