/*******************************************************************************
** �ļ���: 		common.c
** �汾��  		1.0
** ��������: 	RealView MDK-ARM 4.14
** ����: 		wuguoyana
** ��������: 	2011-04-29
** ����:		��ʾ���˵������˵�����ʾһϵ�в���������ض������ļ���
                ִ��Ӧ�ó����Լ���ֹд����(�������Flash��д����)��
** ����ļ�:	common.h��ymodem.h
** �޸���־��	2011-04-29   �����ĵ�
*******************************************************************************/

/* ����ͷ�ļ� *****************************************************************/
#include "common.h"
#include "ymodem.h"

/* ���� ----------------------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t BlockNbr = 0, UserMemoryMask = 0;
__IO uint32_t FlashProtection = 0;
extern uint32_t FlashDestination;


/*******************************************************************************
  * @��������	Int2Str
  * @����˵��   ��������ת���ַ���
  * @�������   intnum:����
  * @�������   str��ת��Ϊ���ַ���
  * @���ز���   ��
*******************************************************************************/
void Int2Str(uint8_t* str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;//ת���� ��Ӧ�� ASCII��

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/*******************************************************************************
  * @��������	Int2Str
  * @����˵��   �ַ���ת������
  * @�������   inputstr:��ת�����ַ���
  * @�������   intnum��ת�õ�����
  * @���ز���   ת�����
                1����ȷ
                0������
*******************************************************************************/
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //����1
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                //��Ч���뷵��0
                res = 0;
                break;
            }
        }

        if (i >= 11)
        {
            res = 0;
        }
    }
    else//���10Ϊ2����
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //����1
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                //��Ч���뷵��0
                res = 0;
                break;
            }
        }
        //����10λ��Ч������0
        if (i >= 11)
        {
            res = 0;
        }
    }

    return res;
}

/*******************************************************************************
  * @��������	Int2Str
  * @����˵��   �ַ���ת������
  * @�������   inputstr:��ת�����ַ���
  * @�������   intnum��ת�õ�����
  * @���ز���   ת�����
                1����ȷ
                0������
*******************************************************************************/
/**
  * @brief  Get an integer from the HyperTerminal
  * @param  num: The inetger
  * @retval 1: Correct
  *         0: Error
  */
uint32_t GetIntegerInput(int32_t * num)
{
    uint8_t inputstr[16];

    while (1)
    {
        GetInputString(inputstr);
        if (inputstr[0] == '\0') continue;
        if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
        {
            SerialPutString("User Cancelled \r\n");
            return 0;
        }

        if (Str2Int(inputstr, num) == 0)
        {
            SerialPutString("Error, Input again: \r\n");
        }
        else
        {
            return 1;
        }
    }
}

/*******************************************************************************
  * @��������	SerialKeyPressed
  * @����˵��   ���Գ����ն��Ƿ��а�������
  * @�������   key:����
  * @�������   ��
  * @���ز���   1����ȷ
                0������
*******************************************************************************/
uint32_t SerialKeyPressed(uint8_t *key)
{

    if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        *key = (uint8_t)USART1->DR; // ���ڽ��� һ���ֽ�
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
  * @��������	GetKey
  * @����˵��   ͨ�������жϻ�ȥ����
  * @�������   ��
  * @�������   ��
  * @���ز���   ���µļ���
*******************************************************************************/
uint8_t GetKey(void)
{
    uint8_t key = 0;

    //�ȴ���������
    while (1)
    {
        if (SerialKeyPressed((uint8_t*)&key)) break; //�տڽ���һ���ֽ�����
    }
    return key;

}

/*******************************************************************************
  * @��������	SerialPutChar
  * @����˵��   ���ڷ���һ���ַ�
  * @�������   C:�跢�͵��ַ�
  * @�������   i��
  * @���ز���   ��
*******************************************************************************/
void SerialPutChar(uint8_t c)
{
    USART_SendData(USART1, c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
}

/*******************************************************************************
  * @��������	SerialPutChar
  * @����˵��   ���ڷ���һ���ַ���
  * @�������   *s:�跢�͵��ַ���
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void Serial_PutString(uint8_t *s)
{
    while (*s != '\0')
    {
        SerialPutChar(*s);
        s++;
    }
}


/*******************************************************************************
  * @��������	GetInputString
  * @����˵��   �Ӵ��ڻ�ȡһ���ַ���
  * @�������   *s:���ַ����ĵ�ַ
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void GetInputString (uint8_t * buffP)
{
    uint32_t bytes_read = 0;
    uint8_t c = 0;
    do
    {
        c = GetKey();
        if (c == '\r')
            break;
        if (c == '\b')// Backspace ����  
        {
            if (bytes_read > 0)
            {
                SerialPutString("\b \b");
                bytes_read --;
            }
            continue;
        }
        if (bytes_read >= CMD_STRING_SIZE )
        {
            SerialPutString("Command string size overflow\r\n");
            bytes_read = 0;
            continue;
        }
        if (c >= 0x20 && c <= 0x7E)
        {
            buffP[bytes_read++] = c;
            SerialPutChar(c);
        }
    }
    while (1);
    SerialPutString(("\n\r"));
    buffP[bytes_read] = '\0';
}


// ִ��bootload����
void go_to_bootload_application(void){
	//__disable_irq(); //�ر������ж�
	 u32 gApplicationAddress = 0;
	 gApplicationAddress     = BootApplicationAddress;
	
	 __disable_irq(); //�ر������ж�
	
   if (((*(__IO uint32_t*)gApplicationAddress) & 0x2FFE0000 ) == 0x20000000) 
        {
            SerialPutString("Execute bootload\r\n\n");
            //��ת���û�����
            JumpAddress = *(__IO uint32_t*) (gApplicationAddress + 4);// ǰ4���ֽڴ��ջ����ַ
            Jump_To_Application = (pFunction) JumpAddress;
					
			   	// �¼��� Ϊ�˷�ֹ��ת����ʹ��	PSP
          // __set_PSP(*(volatile unsigned int*) gApplicationAddress);
          //  __set_CONTROL(0);
					
            //��ʼ���û�����Ķ�ջָ��
            __set_MSP(*(__IO uint32_t*) gApplicationAddress); // ��ջ��ʼ�� ��ջ����ַ�趨Ϊ�û�����ָ���ջ����ַ
					
			     NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); // ��ʼ���ж��������ַ  ��ֹ���û�������ת���� ������ı���
					
					 //__disable_irq();
            Jump_To_Application(); // ��ת��bootload����
				}
}

// ִ���û�����
void go_to_usr_application(void){
	//__disable_irq(); //�ر������ж�
	 u32 gApplicationAddress=0;
	 gApplicationAddress = ApplicationAddress;
 //�ж��û��Ƿ��Ѿ����س�����Ϊ��������´˵�ַ��ջ��ַ��
        //��û����һ��Ļ�����ʹû�����س���Ҳ�����������ܷɡ�
			// ApplicationAddress Ϊ�û����� ��ջջ����ַ ��ջ����ַָ�� RAM ��STM32��RAM����ʼ�ռ��ַΪ0x20000000
			//__disable_irq(); //�ر������ж�
        if (((*(__IO uint32_t*)gApplicationAddress) & 0x2FFE0000 ) == 0x20000000) 
        {
					  // ����ִ����Ϣ
            SerialPutString("Execute user Program\r\n\n");
            //��ת���û�����
            JumpAddress = *(__IO uint32_t*) (gApplicationAddress + 4);// ǰ4���ֽڴ��ջ����ַ
            Jump_To_Application = (pFunction) JumpAddress;
					
			   	// �¼��� Ϊ�˷�ֹ��ת����ʹ��	PSP
          // __set_PSP(*(volatile unsigned int*) gApplicationAddress);
          //  __set_CONTROL(0);
					
            //��ʼ���û�����Ķ�ջָ��
            __set_MSP(*(__IO uint32_t*) gApplicationAddress); // ��ջ��ʼ�� ��ջ����ַ�趨Ϊ�û�����ָ���ջ����ַ
					/*
			 	  u32 gApplicationAddress_offset = 0;
          gApplicationAddress_offset = ApplicationAddress_offset;	 
          NVIC_SetVectorTable(NVIC_VectTab_FLASH,gApplicationAddress_offset);
					*/
          NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x6000);             
         // __disable_irq(); //�ر������ж�
					
            Jump_To_Application(); // ��ת���û�����
        }
        else //δ�����û�����   ��ֻ�� IAP����
        {
					  // ���� ���û�������Ϣ
            SerialPutString("no user Program\r\n\n");
					  SerialPutString("go to bootload_application\r\n\n");
					  //__disable_irq();
					  go_to_bootload_application();
        }
}

/*******************************************************************************
  * @��������	FLASH_PagesMask
  * @����˵��   ����Falshҳ
  * @�������   Size:�ļ�����
  * @�������   ��
  * @���ز���   ҳ������
*******************************************************************************/
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;
}

/*******************************************************************************
  * @��������	FLASH_DisableWriteProtectionPages
  * @����˵��   �Ӵ�Flashд����
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void FLASH_DisableWriteProtectionPages(void)
{
    uint32_t useroptionbyte = 0, WRPR = 0;
    uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
    FLASH_Status status = FLASH_BUSY;

    WRPR = FLASH_GetWriteProtectionOptionByte();

    //�����Ƿ�д����
    if ((WRPR & UserMemoryMask) != UserMemoryMask)
    {
        useroptionbyte = FLASH_GetUserOptionByte();

        UserMemoryMask |= WRPR;

        status = FLASH_EraseOptionBytes();

        if (UserMemoryMask != 0xFFFFFFFF)
        {
            status = FLASH_EnableWriteProtection((uint32_t)~UserMemoryMask);
        }
        //�ô�ѡ�����Ƿ��б��
        if ((useroptionbyte & 0x07) != 0x07)
        {
            //���±���ѡ����
            if ((useroptionbyte & 0x01) == 0x0)
            {
                var1 = OB_IWDG_HW;
            }
            if ((useroptionbyte & 0x02) == 0x0)
            {
                var2 = OB_STOP_RST;
            }
            if ((useroptionbyte & 0x04) == 0x0)
            {
                var3 = OB_STDBY_RST;
            }

            FLASH_UserOptionByteConfig(var1, var2, var3);
        }

        if (status == FLASH_COMPLETE)
        {
            SerialPutString("Write Protection disabled...\r\n");

            SerialPutString("...and a System Reset will be generated to re-load the new option bytes\r\n");
            //ϵͳ��λ���¼���ѡ����
            NVIC_SystemReset();
        }
        else
        {
            SerialPutString("Error: Flash write unprotection failed...\r\n");
        }
    }
    else
    {
        SerialPutString("Flash memory not write protected\r\n");
    }
}

/*******************************************************************************
  * @��������	Main_Menu
  * @����˵��   ��ʾ�˵����ڳ����ն�
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void Main_Menu(void)
{
    uint8_t key = 0;
    BlockNbr = (FlashDestination - 0x08000000) >> 12;

 
#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
    UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
#else /* USE_STM3210E_EVAL */
    if (BlockNbr < 62)
    {
        UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
    }
    else
    {
        UserMemoryMask = ((uint32_t)0x80000000);
    }
#endif /* (STM32F10X_MD) || (STM32F10X_MD_VL) */

    if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
    {
        FlashProtection = 1;
    }
    else
    {
        FlashProtection = 0;
    }

    while (1)
    {
        SerialPutString("\r\n================== Main Menu ========================\r\n\n");
        SerialPutString("  Download Image To the STM32F10x Internal Flash --- 1\r\n\n");
        SerialPutString("  Upload Image From the STM32F10x Internal Flash --- 2\r\n\n");
        SerialPutString("  Execute The New Program -------------------------- 3\r\n\n");

        if (FlashProtection != 0) // ��� FLASH����д����  ��ʾ �ر�flash��д���� ��ѡ��
        {
            SerialPutString("  Disable the write protection ------------------------- 4\r\n\n");
        }

        SerialPutString("=====================================================\r\n\n");

        key = GetKey();

        if (key == 0x31)      // '1'
        {
            //�ȴ���λ�����س����� ��λ�� �û�����FLASH
            SerialDownload();
        }
        else if (key == 0x32) // '2'
        {
            //�ϴ���������λ��
            SerialUpload();
        }
        else if (key == 0x33) // '3'
        {
				FLASH_WriteByte(g_bUpdateFlag_Address,0x00); //���� ������־
			  NVIC_SystemReset();  //������ɺ�����ϵͳ  �����û�����
				//go_to_usr_application(); //ֱ����ת��������  
				
        }
        else if ((key == 0x34) && (FlashProtection == 1))
        {
            //���д����
            FLASH_DisableWriteProtectionPages();
        }
        else // ������� ��ʾ �������
        {
            if (FlashProtection == 0) // FLASH д�����ѽ���������   FLASH_Unlock();
            {
                SerialPutString("Invalid Number ! ==> The number should be either 1, 2 or 3\r");
            }
            else //FLASH д����δ���������� 
            {
                SerialPutString("Invalid Number ! ==> The number should be either 1, 2, 3 or 4\r");
            }
        }
    }
}


/*
��ָ��Flash��ַ��д������
*/
void FLASH_WriteByte(u32 addr ,u16 flashdata1)
{
 FLASH_Status FLASHstatus = FLASH_COMPLETE;
 FLASH_Unlock();//flash����
 FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
 FLASHstatus=FLASH_ErasePage(addr);//����
 FLASHstatus=FLASH_ProgramHalfWord(addr, flashdata1);//д��
 FLASH_Lock();//����
}
/*
��ȡָ��FLASH��ַλ���ڴ洢�ı���ֵ
*/
int FLASH_ReadByte(u32 addr)
{
	u16  rdata;  
  rdata=*(u32 *)addr;
	return rdata;
}


/*******************************�ļ�����***************************************/
