/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : download.c
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : This file provides the software which allows to download
*                      an image to internal Flash.
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
#include "platform_config.h"
#include "stm32f10x_flash.h"
#include "stm32_eval.h"
FLASH_Status FLASHStatu = FLASH_COMPLETE;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern u8 file_name[FILE_NAME_LENGTH];
u8 tab_1024[1024]={0};
extern u32 file_start;

static uint8_t FeedBackOrder[5][14]={
{0x0E,0x00,0x02,0x00,0x01,0x01,0x04,0x01,0x00,0x00,0x00,0x00,0x57,0xc1},//更新成功
{0x0E,0x00,0x02,0x00,0x01,0x01,0x04,0x02,0x00,0x00,0x00,0x00,0x57,0x85},//接收失败
{0x0E,0x00,0x02,0x00,0x01,0x01,0x04,0x03,0x00,0x00,0x00,0x00,0x97,0xB8},//校验失败
{0x0E,0x00,0x02,0x00,0x01,0x01,0x04,0x04,0x00,0x00,0x00,0x00,0x57,0x0D},//固件太大
{0x0E,0x00,0x02,0x00,0x01,0x01,0x04,0x05,0x00,0x00,0x00,0x00,0x97,0x30} //用户中止
};
#define FBACK
#define DIR_LED
#define DIR_VOICE

void EraseAppSpace(void)
{
	u32 NbrOfPage = 0,Counter,FileSize = FileSize_Limit;
	
	NbrOfPage = FLASH_PagesMask(FileSize);
	for(Counter = 0; (Counter < NbrOfPage) && (FLASHStatu == FLASH_COMPLETE); Counter++){
#ifdef DIR_LED
		STM_LED1 = 0;
#endif
		FLASHStatu = FLASH_ErasePage(Download_ApplicationAddress + (0x400 * Counter));
#ifdef DIR_LED
		STM_LED1 = 1;
#endif
	}
}
void ReloadAppCode(void)
{
	u32 j,FileSize = FileSize_Limit,backup_Address = BackupStartAddress,APP_Address = Download_ApplicationAddress;

	for (j = 0;(j < FileSize) && (APP_Address <  Download_ApplicationAddress + FileSize);j += 4){
		/* Program the data received into STM32F10x Flash */
#ifdef DIR_LED
		STM_LED2 = 0;
#endif
		FLASH_ProgramWord(APP_Address,*(u32*)backup_Address);
		backup_Address += 4;
		APP_Address += 4;
#ifdef DIR_LED
		STM_LED2 = 1;
#endif
	}
}
void RestoreAppCode(void)
{
	FLASH_Unlock();
	EraseAppSpace();
	ReloadAppCode();
	FLASH_Lock();
#ifdef DIR_VOICE
	STM_BELL_ON;
	delay_ms(200);
	STM_BELL_OFF;
	delay_ms(400);
#endif
}
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : SerialDownload
* Description    : Download a file via serial port
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SerialDownload(void)
{
  u8 Number[10] = "          ";
  s32 Size;

  Size = Ymodem_Receive(&tab_1024[0]);
  if (Size > 0){//    SerialPutString("\n\n\r Programming Completed Successfully!\r\n Name: ");
//    SerialPutString(file_name);
//    Int2Str(Number, Size);
//    SerialPutString("\n\r Size: ");
//    SerialPutString(Number);
//    SerialPutString(" Bytes\r\n");
//    SerialPutString("-------------------\n");
		#ifdef FBACK
		SerialPuts(FeedBackOrder[0],14);
		#endif
  }else if (Size == -1){//    SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r");
		#ifdef FBACK
		SerialPuts(FeedBackOrder[3],14);
		#endif
  }else if (Size == -2){//    SerialPutString("\n\n\rVerification failed!\n\r");
		RestoreAppCode();
		#ifdef FBACK
		SerialPuts(FeedBackOrder[2],14);
		#endif
  }else if (Size == -3){//    SerialPutString("\r\n\nAborted by user.\n\r");
		if(file_start)
			RestoreAppCode();
		#ifdef FBACK
		SerialPuts(FeedBackOrder[4],14);
		#endif
  }else if(Size == 0){							//    SerialPutString("\n\rFailed to receive the file!\n\r");
		RestoreAppCode();
		#ifdef FBACK
		SerialPuts(FeedBackOrder[1],14);
		#endif
  }else{
		#ifdef DIR_LED
		STM_LED4 = 0;
		delay_ms(500);
		STM_LED4 = 1;
		#endif
		#ifdef FBACK
		SerialPuts(FeedBackOrder[1],14);
		#endif	
	}
}

/*******************(C)COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
