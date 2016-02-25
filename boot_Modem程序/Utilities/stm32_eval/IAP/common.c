/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : common.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : This file provides all the common functions.
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
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32_eval.h"


//#define SecureCRT

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction Jump_To_Application;
u32 JumpAddress;
u32 BlockNbr = 0, UserMemoryMask = 0;
bool FlashProtection = FALSE;
extern u32 FlashDestination;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Int2Str
* Description    : Convert an Integer to a string
* Input          : - str: The string
*                  - intnum: The intger to be converted
* Output         : None
* Return         : None
*******************************************************************************/
void Int2Str(u8* str, s32 intnum)
{
  u32 i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)//为什么是10？因为s32类型最大的数据量为2147483647。一共有10位，所以需要对10位分别转换
  {
    str[j++] = (intnum / Div) + 48;//为什么是48？ 因为48对应字符char的"0"    ascii的'30'对应0  

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0)){
      j = 0;
    }else{
      Status++;
    }
  }
}

/*******************************************************************************
* Function Name  : Str2Int
* Description    : Convert a string to an integer
* Input 1        : - inputstr: The string to be converted
*                  - intnum: The intger value
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
u32 Str2Int(u8 *inputstr, s32 *intnum)
{
  u32 i = 0, res = 0;
  u32 val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X')){
    if (inputstr[2] == '\0'){//数据包为0 
      return 0;
    }
    for (i = 2; i < 11; i++){//为什么是11？
      if (inputstr[i] == '\0'){//若数据包第一个数字为0，数据大小为0
        *intnum = val;
        /* return 1; */
        res = 1; 
        break;
      }
      if (ISVALIDHEX(inputstr[i])){//filesize中的数据是否有效（A-F || a-f || 0-9），十六进制只能到F      
        val = (val << 4) + CONVERTHEX(inputstr[i]);//若值为“0”-”9“则变化成0-9，否则需要判定是否为A-F，
																									 //若为“A”-“F”，则变化成A-F，否则转化为a-f
      }else{   //若标识size的数值无效（4uk）
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* over 8 digit hex --invalid */
    if (i >= 11){
      res = 0; 
    }
  }
  else /* max 10-digit decimal input ，filesize数组不是以0x开头*/
  {
    for (i = 0;i < 11;i++)//由于filesize数组的最大值为11，（该程序中是16）
    {
      if (inputstr[i] == '\0'){//文件为0，直接返回1     
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0)){ //文件表示为1k，
																																			//直接将size赋值为0x400（1024）(<<10)
        val = val << 10;//（1024 - 1k）
        *intnum = val;
        res = 1;
        break;
      }else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0)){//文件大小为M，表示1M，
																																			//直接将size赋值为0x100000 (<<20)
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
#ifdef SecureCRT
      else if (ISVALIDDEC(inputstr[i])){//判定数字是否属于“0“-”9“。按照十进制组合各个位的数值
				val = val * 10 + CONVERTDEC(inputstr[i]);
			}
#else            //使用android方式发送，
			else if (ISVALIDHEX(inputstr[i])){//filesize中的数据是否有效（A-F || a-f || 0-9），十六进制只能到F
        val = (val << 4) + CONVERTHEX(inputstr[i]);//若值为“0”-”9“则变化成0-9，否则需要判定是否为A-F，
																									 //若为“A”-“F”，则变化成A-F，否则转化为a-f
      }
#endif
      else{  
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --inval	id */
    if (i >= 11){
      res = 0;
    }  
  }
  return (res);
}

/*******************************************************************************
* Function Name  : GetIntegerInput
* Description    : Get an integer from the HyperTerminal
* Input          : - num: The inetger
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
u32 GetIntegerInput(s32 * num)
{
  u8 inputstr[16];

  while (1)
  {
    GetInputString(inputstr);
    if (inputstr[0] == '\0') continue;
    if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0'){
      SerialPutString("User Cancelled \r\n");
      return 0;
    }

    if (Str2Int(inputstr, num) == 0){
      SerialPutString("Error, Input again: \r\n");
    }else{
      return 1;
    }
  }
}

/*******************************************************************************
* Function Name  : SerialKeyPressed
* Description    : Test to see if a key has been pressed on the HyperTerminal
* Input          : - key: The key pressed
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
u32 SerialKeyPressed(u8 *key)
{
  if ( USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) != RESET){
    *key = (u8)USARTx->DR;
    return 1;
  }else{
    return 0;
  }
}

/*******************************************************************************
* Function Name  : GetKey
* Description    : Get a key from the HyperTerminal
* Input          : None
* Output         : None
* Return         : The Key Pressed
*******************************************************************************/
u8 GetKey(void)
{
  u8 key = 0;
  
  /* Waiting for user input */
  while (1)
  {
    if (SerialKeyPressed((u8*)&key)) break;
  }
  return key;

}

/*******************************************************************************
* Function Name  : SerialPutChar
* Description    : Print a character on the HyperTerminal
* Input          : - c: The character to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutChar(u8 c)
{
  USART_SendData(USARTx, c);
  while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

/*******************************************************************************
* Function Name  : SerialPutString
* Description    : Print a string on the HyperTerminal
* Input          : - s: The string to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutString(u8 *s)
{
  while (*s != '\0')
  {
    SerialPutChar(*s);
    s ++;
  }
}
void SerialPuts(u8 *s, u8 len)
{
	u8 i;
for(i = 0; i < len; i++)
{
	SerialPutChar(*(s+i));
}
delay_ms(10);
}
/*******************************************************************************
* Function Name  : GetInputString
* Description    : Get Input string from the HyperTerminal
* Input          : - buffP: The input string
* Output         : None
* Return         : None
*******************************************************************************/
void GetInputString (u8 * buffP)
{
  u32 bytes_read = 0;
  u8 c = 0;
  do
  {
    c = GetKey();
    if (c == '\r'){
			break;
		}
    if (c == '\b'){ /* Backspace */
      if (bytes_read > 0){
        SerialPutString("\b \b");
        bytes_read --;
      }
      continue;
    }
    if (bytes_read >= CMD_STRING_SIZE ){
      SerialPutString("Command string size overflow\r\n");
      bytes_read = 0;
      continue;
    }
    if (c >= 0x20 && c <= 0x7E){
      buffP[bytes_read++] = c;
      SerialPutChar(c);
    }
  }
  while (1);
  SerialPutString("\n\r");
  buffP[bytes_read] = '\0';
}

/*******************************************************************************
* Function Name  : FLASH_PagesMask
* Description    : Calculate the number of pages
* Input          : - Size: The image size
* Output         : None
* Return         : The number of pages
*******************************************************************************/
u32 FLASH_PagesMask(vu32 Size)
{
  u32 pagenumber = 0x0;
  u32 size = Size;

  if((size % 0x400) != 0){
    pagenumber = (size / 0x400) + 1;
  }else{
    pagenumber = size / 0x400;
  }
  return pagenumber;

}

/*******************************************************************************
* Function Name  : FLASH_DisableWriteProtectionPages
* Description    : Disable the write protection of desired pages
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_DisableWriteProtectionPages(void)
{
  u32 useroptionbyte = 0, WRPR = 0;
  u16 var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
  FLASH_Status status = FLASH_BUSY;

  WRPR = FLASH_GetWriteProtectionOptionByte();

  /* Test if user memory is write protected */
  if ((WRPR & UserMemoryMask) != UserMemoryMask){
    useroptionbyte = FLASH_GetUserOptionByte();

    UserMemoryMask |= WRPR;

    status = FLASH_EraseOptionBytes();

    if(UserMemoryMask != 0xFFFFFFFF){
      status = FLASH_EnableWriteProtection((u32)~UserMemoryMask);
    }

    /* Test if user Option Bytes are programmed */
    if((useroptionbyte & 0x07) != 0x07){ /* Restore user Option Bytes */
      if((useroptionbyte & 0x01) == 0x0){
        var1 = OB_IWDG_HW;
      }
      if((useroptionbyte & 0x02) == 0x0){
        var2 = OB_STOP_RST;
      }
      if((useroptionbyte & 0x04) == 0x0){
        var3 = OB_STDBY_RST;
      }

      FLASH_UserOptionByteConfig(var1, var2, var3);	
    }

    if (status == FLASH_COMPLETE){
      SerialPutString("Write Protection disabled...\r\n");

      SerialPutString("...and a System Reset will be generated to re-load the new option bytes\r\n");

      /* Enable WWDG clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

       /* Generate a system Reset to re-load the new option bytes: enable WWDG and set
         counter value to 0x4F, as T6 bit is cleared this will generate a WWDG reset */
      WWDG_Enable(0x4F);
    }else{
      SerialPutString("Error: Flash write unprotection failed...\r\n");
    }
  }else{
    SerialPutString("Flash memory not write protected\r\n");
  }
}

void CloseCon(void)
{
//		SerialPutString("jump to app\r\n\n");
		USART_Cmd(USARTx, DISABLE);
		USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
		USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);
		USART_ITConfig(USARTx, USART_IT_TC, DISABLE);

		USART_ClearITPendingBit(USARTx, USART_IT_TXE);
		USART_ClearITPendingBit(USARTx, USART_IT_TC);
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
}
void JumpToApp(void)
{
		JumpAddress = *(vu32*) (Download_ApplicationAddress + 4);  //the address value
		/* Jump to user application */
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(vu32*) Download_ApplicationAddress);
		Jump_To_Application();
}
void RunApp(void)
{
	CloseCon();
	JumpToApp();
}
/*******************************************************************************
* Function Name  : Main_Menu
* Description    : Display the Main Menu on to HyperTerminal
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UpDataFile(void)
{
//    SerialPutString("  Download Image To the STM32F10x Internal Flash ------- 1\r\n\n");

		SerialDownload();
		RunApp();
}

/*******************(C)COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
