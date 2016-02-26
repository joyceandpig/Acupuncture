#include "stm32_eval_SPI.h"

//��ʼ��SPI
void ADG714_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//��ʹ������IO PORTAʱ�� 
	RCC_APB2PeriphClockCmd(	SPI_SCLK_CLK | SPI_SDA_CLK | SPI_SS_CLK | SPI_RST_CLK, ENABLE );	

	GPIO_InitStructure.GPIO_Pin = SPI_SCLK_PIN;         //SCLK����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_SCLK_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_SDA_PIN;          //SDA����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_SDA_PORT, &GPIO_InitStructure);	

 	GPIO_InitStructure.GPIO_Pin = SPI_SS_PIN;           //Ƭѡ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_SS_PORT, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = SPI_RST_PIN;            //��λ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;     //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_RST_PORT, &GPIO_InitStructure);

  Clr_SPI_RST;Set_SPI_RST;                              //��λ��ȫ��
  
}


void ADG714_WriteData( uint16_t Data )
{
  uint16_t Tmp,i;

  Tmp = Data;

  Set_SPI_SS;//SYNC �ø�
  Clr_SPI_SS;//SYNC �õͣ���ʼ���ݴ���

  for(i = 8; i>0; i--)
  {  
    Set_SPI_SCLK;//1   //�½��ز���
    if((Tmp & 0x80) == 0x80)
    {
      Set_SPI_SDA;
    }
    else
    {
      Clr_SPI_SDA;
    } 
    Clr_SPI_SCLK; //0
    Tmp <<= 1;    //����1λ
  }
  Set_SPI_SS;     //SYNC �øߣ��������ݴ���,���¿���ѡ��
}
