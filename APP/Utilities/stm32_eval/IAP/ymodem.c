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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 file_name[FILE_NAME_LENGTH];
extern u32 FlashDestination; /* Flash user program offset */
u16 PageSize = 0x400;
u32 EraseCounter = 0x0;
u32 NbrOfPage = 0;
FLASH_Status FLASHStatus = FLASH_COMPLETE;
u32 RamSource;
extern u8 tab_1024[1024];

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
    if (SerialKeyPressed(c) == 1)
    {
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
  if (Receive_Byte(&c, timeout) != 0)//在timeout时间内，如果接收到数据则返回0 否则返回-1
  {
    return -1;
  }
  switch (c)    //判定接收到的数据是什么
  {
  case SOH:
    packet_size = PACKET_SIZE;
    break;
  case STX:
    packet_size = PACKET_1K_SIZE;
    break;
  case EOT:  //结束传输
    return 0;
  case CA:   //传输终止
    if ((Receive_Byte(&c, timeout) == 0) && (c == CA))  //在接收到传输终止后，若timeout时间内依然接收
																												//到数据且数据为传输终止指令
    {
      *length = -1;        //说明是主机终止传送，abort by sender
      return 0;
    }
    else
    {
      return -1;             //timeout or packet error
    }
  case ABORT1:               // abort by user  A、a
  case ABORT2:
    return 1;
  default:
    return -1;               //timeout or packet error
  }
  
	*data = c;
  for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++)//帧头3byte+校验段2byte+数据段pack_size
  {
    if (Receive_Byte(data + i, timeout) != 0)//将接收到的数据包存入data数组中（包括帧头后两位+数据+校验）
																							//接收到数据则返回0 否则返回-1
		{
      return -1;
    }
  }
  if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))//数据包编号不等于其反码
  {
    return -1;   //数据确认不正确
  }
  *length = packet_size;
  return 0;
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

  for (session_done = 0, errors = 0, session_begin = 0; ;)
  {
    for (packets_received = 0, file_done = 0, buf_ptr = buf; ;)
    {
      switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT))
      {
        case 0:       //
          errors = 0;
          switch (packet_length)      //接收包的长度（正常是128 或者1024两种）
          {
							/* Abort by sender */
						case - 1:    
              Send_Byte(ACK);
              return 0;
							/* End of transmission 没有收到数据*/
            case 0:     
              Send_Byte(ACK);
              file_done = 1;
              break;
						/* Normal packet */
            default:    
							//数据包编号不等于0，在第一次传输时
							if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))
							{
								Send_Byte(NAK);
							}						
							else     //第一包数据传输正确
							{
								if (packets_received == 0)   //表示接收到第一帧，指示传送的文件信息
								{
									/* Filename packet */
									if (packet_data[PACKET_HEADER] != 0)    
									{/* Filename packet has valid data */
										//获取即将传输文件的文件名并存入file_name数组
										for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
										{
											file_name[i++] = *file_ptr++;
										}
										file_name[i++] = '\0';
										//获取即将传输文件的文件大小并存入file_size数组
										for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
										{
											file_size[i++] = *file_ptr++;
										}
										file_size[i++] = '\0';
										
										//将str型的文件大小转化成int型
										Str2Int(file_size, &size);
										/* Test the size of the image to be sent */
										/* Image size is greater than Flash size */
										if (size > 0x0801FFFF) //计算存入文件后的地址（有效范围0x08000000-0x08002000）
										{
											/* End session */
											Send_Byte(CA);
											Send_Byte(CA);
											return -1;          //文件大小超过限制
										}
										/* Erase the needed pages where the user application will be loaded */
										/* Define the number of page to be erased */
										NbrOfPage = FLASH_PagesMask(size);
										/* Erase the FLASH pages */
										for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
										{
											FLASHStatus = FLASH_ErasePage(FlashDestination + (PageSize * EraseCounter));
										}
										Send_Byte(ACK);//本次接收的回复
										Send_Byte(CRC16);//请求传送下一帧数据
									}
									/* Filename packet is empty, end session */
									else   
									{
										Send_Byte(ACK);
										file_done = 1;
										session_done = 1;
										break;
									}
								}
								/* 表示接收到Data packet */
								else    
								{
									memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
									RamSource =(u32)buf;
									for (j = 0;(j < packet_length) && (FlashDestination <  ApplicationAddress + size);j += 4)
									{
										/* Program the data received into STM32F10x Flash */
										FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
										if (*(u32*)FlashDestination != *(u32*)RamSource)
										{
											/* End session 存入flash数据与保存到数组的数据不一致*/
											Send_Byte(CA);
											Send_Byte(CA);
											return -2;
										}
										FlashDestination += 4;
										RamSource += 4;
									}
									Send_Byte(ACK);
								}
								packets_received ++;
								session_begin = 1;
							}
          }
          break;
				//aborted by user
        case 1:    									 
            Send_Byte(CA);
            Send_Byte(CA);
            return -3;
				//接收失败 接收数据错误 没有到数据
        default:                     
						if (session_begin > 0)
						{
							errors ++;
						}
						if (errors > MAX_ERRORS)
						{
							Send_Byte(CA);
							Send_Byte(CA);
							return 0;
						}
						Send_Byte(CRC16);//请求发送数据
						break;
      }
      if (file_done != 0)
      {
        break;
      }
    }
    if (session_done != 0)
    {
      break;
    }
  }
  return (s32)size;
}

/*******************(C)COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
