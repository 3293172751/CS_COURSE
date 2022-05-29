/*******************************************************************************
** �ļ���: 		common.h
** �汾��  		1.0
** ��������: 	RealView MDK-ARM 4.14
** ����: 		wuguoyana
** ��������: 	2011-04-29
** ����:		common�ļ���ͷ�ļ�����
** ����ļ�:	string.h��stdio.h��stm32f10x.h��ymodem.h
** �޸���־��	2011-04-29   �����ĵ�
*******************************************************************************/

/* ��ֹ�ض��� ----------------------------------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* ����ͷ�ļ� *****************************************************************/
#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"
#include "ymodem.h"

//#define STM32F10X_MD
/* �������� -----------------------------------------------------------------*/
typedef  void (*pFunction)(void);

//* �� ------------------------------------------------------------------------*/
#define CMD_STRING_SIZE       128

// �û�����ʼ�� ��ַ  �����0x8000000��ʼ  IAP����(bootload)��С�ᳬ��2000 ���԰�ȫ��0x8003000��ʼд�û�����
#define ApplicationAddress    0x8006000  // ���߸���  0x8004000 ���IAP�������
// 0x8004000  ��ʼ���ڴ洢  ������־  0x8004000~0x8004003  һ����
// bootloade��ʼ��ַ 
#define BootApplicationAddress   0x8000000
//�û�������ֹ��ַ �� flash ��ʼ��ַ  0x8000000��ƫ����
#define ApplicationAddress_offset 0x6000  
// ������־ ��ַ
#define g_bUpdateFlag_Address    0x08005000  // 


#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
 #define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
 #define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#elif defined STM32F10X_CL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x40000)  /* 256 KBytes */
#elif defined STM32F10X_HD || defined (STM32F10X_HD_VL)
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x80000)  /* 512 KBytes */
#elif defined STM32F10X_XL
 #define PAGE_SIZE                         (0x800)    /* 2 Kbytes */
 #define FLASH_SIZE                        (0x100000) /* 1 MByte */
#else 
 #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif

//�����ϴ��ļ���С 
#define FLASH_IMAGE_SIZE                   (uint32_t) (FLASH_SIZE - (ApplicationAddress - 0x08000000))

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString(x) Serial_PutString((uint8_t*)(x))


/* �������� ------------------------------------------------------------------*/
void Int2Str(uint8_t* str,int32_t intnum);
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);
uint32_t GetIntegerInput(int32_t * num);
uint32_t SerialKeyPressed(uint8_t *key);
uint8_t GetKey(void);
void SerialPutChar(uint8_t c);
void Serial_PutString(uint8_t *s);
void GetInputString(uint8_t * buffP);
uint32_t FLASH_PagesMask(__IO uint32_t Size);
void FLASH_DisableWriteProtectionPages(void);
void Main_Menu(void);
void SerialDownload(void);
void SerialUpload(void);

void go_to_usr_application(void);         // ִ���û�����
void go_to_bootload_application(void);    // ִ��bootload����

void FLASH_WriteByte(u32 addr ,u16 flashdata1);
int FLASH_ReadByte(u32 addr);

#endif  /* _COMMON_H */

/*******************************�ļ�����***************************************/
