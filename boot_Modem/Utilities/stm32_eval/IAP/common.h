/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : common.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : 10/08/2007
* Description        : This file provides all the headers of the common functions.
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "string.h"
#include "stm32f10x_lib.h"
#include "ymodem.h" 
#include "stm32f10x.h"
#include "platform_config.h"
#include "core_cm3.h"
/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE     128


              
/* Exported macro ------------------------------------------------------------*/
/* Common routines */
#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

/* Exported functions ------------------------------------------------------- */
void Int2Str(u8* str,s32 intnum);
u32 Str2Int(u8 *inputstr,s32 *intnum);
u32 GetIntegerInput(s32 * num);
u32 SerialKeyPressed(u8 *key);
u8 GetKey(void);
void SerialPutChar(u8 c);
void SerialPutString(u8 *s);
void GetInputString(u8 * buffP);
u32 FLASH_PagesMask(vu32 Size);
void FLASH_DisableWriteProtectionPages(void);
void UpDataFile(void);
void SerialDownload(void);
void RunApp(void);

#endif  /* _COMMON_H */

/*******************(C)COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
