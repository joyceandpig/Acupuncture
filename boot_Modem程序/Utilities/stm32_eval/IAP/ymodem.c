/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : ymodem.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : This file provides all the software functions related to
*                      the ymodem protocol.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "stm32f10x_flash.h"
#include "stm32_eval.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 file_name[FILE_NAME_LENGTH];
u32 FlashDestination = Download_ApplicationAddress; /* Flash user program offset */
u16 PageSize = 0x400;
u32 EraseCounter = 0x0;
u32 NbrOfPage = 0;
FLASH_Status FLASHStatus = FLASH_COMPLETE;
u32 RamSource;
extern u8 tab_1024[1024];
u32 file_start = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Receive_Byte
* Description    : Receive byte from sender
* Input          : - c: Character
*                  - timeout: Timeout
* Output         : None
* Return         : 0: Byte received
*                  -1: Timeout
*******************************************************************************/
static  s32 Receive_Byte (u8 *c, u32 timeout)
{
  while (timeout-- > 0)
  {
    if (SerialKeyPressed(c) == 1){
      return 0;
    }
  }
  return -1;
}

/*******************************************************************************
* Function Name  : Send_Byte
* Description    : Send a byte
* Input          : - c: Character
* Output         : None
* Return         : 0: Byte sent
*******************************************************************************/
static u32 Send_Byte (u8 c)
{
  SerialPutChar(c);
  return 0;
}

/*******************************************************************************
* Function Name  : Receive_Packet
* Description    : Receive a packet from sender
* Input 1        : - data
* Input 2        : - length
* Input 3        : - timeout
* Output         : *length:
*                  0: end of transmission
*                  -1: abort by sender
*                  >0: packet length
* Return         : 0: normally return
*                  -1: timeout or packet error
*                  1: abort by user
*******************************************************************************/
static s32 Receive_Packet (u8 *data, s32 *length, u32 timeout)
{
  u16 i, packet_size;
  u8 c;
  *length = 0;
  if (Receive_Byte(&c, timeout) != 0){//0�����յ����ݣ�-1��timeout
    return -1;
  }
  switch (c)    //�ж����յ���������ʲô
  {
  case SOH:
    packet_size = PACKET_SIZE;
    break;
  case STX:
    packet_size = PACKET_1K_SIZE;
    break;
  case EOT:  //��������
    return 0;
  case CA:   //������ֹ
    if ((Receive_Byte(&c, timeout) == 0) && (c == CA)){  //�ڽ��յ�������ֹ����timeoutʱ������Ȼ����
																												//������������Ϊ������ָֹ�� 
      *length = -1;        //˵����������ֹ���ͣ�abort by sender
      return 0;
    }else{
      return -1;             //timeout or packet error
    }
  case ABORT1:               // abort by user  A��a
  case ABORT2:
    return 1;
  default:
    return -1;               //timeout or packet error
  }
  
	*data = c;
  for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++)//֡ͷ3byte+У���2byte+���ݶ�pack_size
  {
    if (Receive_Byte(data + i, timeout) != 0){//�����յ������ݰ�����data�����У�����֡ͷ����λ+����+У�飩
																							//���յ������򷵻�0 ���򷵻�-1
      return -1;
    }
  }
  if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))//���ݰ���Ų������䷴��
  {
    return -1;   //����ȷ�ϲ���ȷ
  }
  *length = packet_size;
  return 0;
}
void Erase_BackupSpace(void)
{
	u32 FileSize = FileSize_Limit;
	
	NbrOfPage = FLASH_PagesMask(FileSize);
	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++){
		FLASHStatus = FLASH_ErasePage(BackupStartAddress + (PageSize * EraseCounter));
	}
}
void LoadAppCodeToBackup(void)
{
	uint8_t j;
	u32 FileSize = FileSize_Limit,Destination_Address = BackupStartAddress, APP_Address = Download_ApplicationAddress;
	
	for (j = 0;(j < FileSize) && (Destination_Address <  BackupStartAddress + FileSize);j += 4){
		FLASH_ProgramWord(Destination_Address, *(u32*)APP_Address);
		Destination_Address += 4;
		APP_Address += 4;
	}
}
void Backup_Copy()
{
	Erase_BackupSpace();
	LoadAppCodeToBackup();
}
/*******************************************************************************
* Function Name  : Ymodem_Receive
* Description    : Receive a file using the ymodem protocol
* Input          : Address of the first byte
* Output         : None
* Return         : The size of the file
*******************************************************************************/
s32 Ymodem_Receive (u8 *buf)
{
  u8 packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], file_size[FILE_SIZE_LENGTH], *file_ptr, *buf_ptr;
  s32 i, j, packet_length, session_done, file_done, packets_received, errors, session_begin, size = 0;
	u8 ack_count = 0;

  for (session_done = 0, errors = 0, session_begin = 0; ;)
  {
    for (packets_received = 0, file_done = 0, buf_ptr = buf; ;)
    {
      switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT))//* Return : 0: ��������
																																			 //*         -1: ��ʱ�����ݴ���
			 																															 //*          1: �û���ֹ
      {
        case 0:       //
          errors = 0;
//packet_length       0: �������
//*                  -1: �û���ֹ
//*                  >0: ���ؽ������ݳ���
          switch (packet_length)      //���հ��ĳ��ȣ�������128 ����1024���֣�
          {		
						case - 1:    /* Abort by sender */
              Send_Byte(ACK);
              return -3;
            case 0:     /* End of transmission ���ս���*/
              Send_Byte(ACK);
              file_done = 1;
              break;			
            default:    	/* Normal packet */
							if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))
							{
								Send_Byte(NAK);//��ű�������յ��ĸ�������Ӧ�����Ǵ���©֡��Ϊ
							}
							else     //���ݵ����ȡ����ȷ
							{
								STM_LED1 = 0;
								if (packets_received == 0)   //��ʾ���յ���һ֡��ָʾ���͵��ļ���Ϣ
								{
									/* Filename packet */
									if (packet_data[PACKET_HEADER] != 0){//���յ���Ч�ļ�������   
										/* Filename packet has valid data */
										//��ȡ���������ļ����ļ���������file_name����
										for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);){
											file_name[i++] = *file_ptr++;
										}
										file_name[i++] = '\0';
										//��ȡ���������ļ����ļ���С������file_size����
										for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);){
											file_size[i++] = *file_ptr++;
										}
										file_size[i++] = '\0';
										
										//��str�͵��ļ���Сת����int��
										Str2Int(file_size, &size);
										/* Test the size of the image to be sent */
										/* Image size is greater than Flash size */
										if (size > FileSize_Limit){ //��������ļ���ĵ�ַ
											/* End session */
											Send_Byte(CA);
											Send_Byte(CA);
											return -1;          //�ļ���С��������
										}
										/* Erase the needed pages where the user application will be loaded */
										/* Define the number of page to be erased */
										NbrOfPage = FLASH_PagesMask(size);
										/* Erase the FLASH pages */
										FLASH_Unlock();
										Backup_Copy();//����ԭ���ĳ���
										for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++){
											FLASHStatus = FLASH_ErasePage(FlashDestination + (PageSize * EraseCounter));
										}		
										FLASH_Lock();
										file_start = 1;
										Send_Byte(ACK);//���ν��յĻظ�
										Send_Byte(CRC16);//��������һ֡����
									}else{/* Filename packet is empty, end session */
										Send_Byte(ACK);
										file_done = 1;
										session_done = 1;
										break;
									}
								}
								/* ��ʾ���յ�Data packet */
								else{
									memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
									RamSource =(u32)buf;//��������ĵ�ַ
									FLASH_Unlock();
									for (j = 0;(j < packet_length) && (FlashDestination <  Download_ApplicationAddress + size);j += 4){
										/* Program the data received into STM32F10x Flash */
										
										FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
										
										if (*(u32*)FlashDestination != *(u32*)RamSource){
											/* End session ����flash�����뱣�浽��������ݲ�һ��*/
											Send_Byte(CA);
											Send_Byte(CA);
											return -2;
										}
										FlashDestination += 4;
										RamSource += 4;
									}
									FLASH_Lock();
									Send_Byte(ACK);
								}
								STM_LED1 = 1;
								packets_received ++;
								session_begin = 1;
							}
          }
          break;				
        case 1:    				//aborted by user					 
            Send_Byte(CA);
            Send_Byte(CA);
            return -3;
				//����ʧ�� �������ݴ��� û�е�����  timeout or packet error
        default:                     
						if (session_begin > 0){
							errors ++;
						}
						if (errors > MAX_ERRORS){
							Send_Byte(CA);
							Send_Byte(CA);
							return 0;
						}
						Send_Byte(CRC16);//����������
						delay_ms(1);
						if(!session_begin){//�ϲ㻹û�д������ݹ���ʱ�������²��������ݵĴ���
						if(ack_count++ > 10)//��������������10�Σ���û�����ݷ�������˵��ͨ��������
						{
							ack_count = 0;
							return -4;
						}
					}
						
						break;
      }
      if (file_done != 0){
        break;
      }
    }
    if (session_done != 0){
      break;
    }
  }
  return (s32)size;
}

/*******************(C)COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/