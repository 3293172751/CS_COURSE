/*******************************************************************************
** �ļ���: 		mian.c
** �汾��  		1.0
** ��������: 	RealView MDK-ARM 5
** ����: 		wuguoyana
** ��������: 	2011-04-28
** ����:		USART��ʼ����RCC���ã�Ȼ���common.c��ִ�����˵�
** ����ļ�:	stm32f10x.h
** �޸���־��	2011-04-29   �����ĵ�
*******************************************************************************/
/* ����ͷ�ļ� *****************************************************************/
#include "common.h"
//#include "stm32f10x_flash.icf"
/* �������� ------------------------------------------------------------------*/
/* �� ------------------------------------------------------------------------*/
//#define LED2   GPIO_Pin_6
//#define LED3   GPIO_Pin_7
//#define LED4   GPIO_Pin_8
//#define LED5   GPIO_Pin_9
#define Sys_Led   GPIO_Pin_12   // PB12
#define Update_Key GPIO_Pin_10  // PB10
//#define STM32F10X_MD
/* ���� ----------------------------------------------------------------------*/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;

u8 g_bUpdateFlag=0x00;
u8 g_b_receive_UpdateFlag=0x00;
//extern void Serial_PutString(uint8_t *s);

//���ڽ���
//u8 RxBuffer[6]={'o'};
//u8 i=0;


/* �������� ------------------------------------------------------------------*/
void Delay(__IO uint32_t nCount);
void LED_Configuration(void);
static void IAP_Init(void);
void KEY_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);



/* �������� ------------------------------------------------------------------*/


/*******************************************************************************
  * @��������	main
  * @����˵��   ������
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
/*
 // IAP���� �ж�����������
NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   // NVIC_VectTab_FLASH �궨�� ֵΪ0x08000000
 // �û����� �ж�����������
NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000); // �� common.h �� ApplicationAddress ��ַ��Ӧ��IAP��С��Ӧ

*/
int main(void)
{
	
  //ϵͳ��λ���¼���ѡ����
 // NVIC_SystemReset();
    // �ж������� 0x8000000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); // ��ʼ���ж��������ַ  ��ֹ���û�������ת���� ������ı���
	
	  //FLASH_WriteByte(g_bUpdateFlag_Address,0x00);
    //Flash ����
    LED_Configuration(); // ���� ϵͳָʾ��
    KEY_Configuration(); // ������������
    IAP_Init();          // ���ô���
	
    //�����Ƿ��´����Ƿ��յ�����  �� U���Ƿ����(stm32��Ҫ��չSD����Ӳ������)  ���߼��flash�й̶�λ���ϵı�־(�д��ڷ������ݸı�)
    // http://blog.csdn.net/xiaobai20131/article/details/50890275	
	  // ��ȡĳ��FLASH��ַ �е�ֵ(������־)
    g_bUpdateFlag = FLASH_ReadByte(g_bUpdateFlag_Address);
		FLASH_WriteByte(g_bUpdateFlag_Address,0x00); //���� ������־
	
	//FLASHд���� ���� 
	 FLASH_Unlock();
	
    if ( g_bUpdateFlag==0x01)//||GPIO_ReadInputDataBit(GPIOB,Update_Key)  == 0x00))//||(g_b_receive_UpdateFlag==0x01))  
			//�ϵ�ʱ���� ����������־(�������� ) ���� ���ڷ������ñ�־����
    {
        //���簴������ ������������־
        //ִ��IAP�����������Flash����
/*
        SerialPutString("\r\n============================================================");
        SerialPutString("\r\n=              (C) COPYRIGHT 2017 Lierda                    =");
        SerialPutString("\r\n=                                                           =");
        SerialPutString("\r\n=   In-Application Programming Application  (Version 1.0.0) =");
        SerialPutString("\r\n=                                                           =");
        SerialPutString("\r\n=                By wyw                                     =");
        SerialPutString("\r\n=============================================================");
        SerialPutString("\r\n\r\n");
        Main_Menu();
			*/
			// ֱ�����س���
			SerialDownload();
			
    }
    //����ִ���û�����
    else
    {
       go_to_usr_application();
    }

    while (1)
    {
    }
}


/*******************************************************************************
  * @��������	LED_Configuration
  * @����˵��   ����ʹ��LED
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void LED_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //ʹ��LED����GPIO��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    //��ʼ��LED��GPIO
    GPIO_InitStructure.GPIO_Pin = Sys_Led;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB,Sys_Led);  //����Sys_Led

}

/*******************************************************************************
  * @��������	KEY_Configuration
  * @����˵��   ������ʼ��
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void KEY_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

		  //  ���������ӿ�IO����
	  GPIO_InitStructure.GPIO_Pin = Update_Key;				// PB10
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	// ��������
	  GPIO_Init(GPIOB, &GPIO_InitStructure);				  // ��ʼ��PB10
}

/*******************************************************************************
  * @��������	GPIO_Configuration
  * @����˵��   ����ʹ��USART1�����IO�ܽ� IAP_Init() ��������
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // ʹ�� USART1��B�˿��Լ� �˿ڸ��ù���  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	  GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE); //ʹ�ܶ˿���ӳ�� ���԰��չٷ� ��ӳ�� �����й��ܹܽŵ���ӳ��
    //USART1_TX ���͹ܽ�  GPIOB.6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;          //PB.6
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // �ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   // �����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);             // ��ʼ��GPIOB.6
   
    //USART1_RX	���չܽ�  GPIOB.7 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;            //PB7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);               //��ʼ��GPIOB.7  
}
   
/*******************************************************************************
  * @��������	IAP_Init
  * @����˵��   ����ʹ��IAP  ���ô�������λ��  �ն�ͨ��
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void IAP_Init(void)
{
    USART_InitTypeDef USART_InitStructure;

    /* USART1 ���� ------------------------------------------------------------
         USART1 ��������:
          - ������      = 115200 baud
          - �ֳ�        = 8 Bits
          - һ��ֹͣλ
          - ��У��
          - ��Ӳ��������
          - ���ܺͷ���ʹ��
    --------------------------------------------------------------------------*/
    USART_InitStructure.USART_BaudRate = 115200;  // ������ 115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;// ����λ 8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     // ֹͣλ 1λ
    USART_InitStructure.USART_Parity = USART_Parity_No;        // ����żУ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// ��Ӳ������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                // ���ڷ��ͺͽ���
/*
	//�����ж�����
	  NVIC_InitTypeDef NVIC_InitStructure;
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		// �����ж�ʹ��
 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
*/		
    GPIO_Configuration(); // �����շ��ܽ�����
    USART_Init(USART1, &USART_InitStructure);// ���ڳ�ʼ��
    USART_Cmd(USART1, ENABLE);    // ʹ�� USART1 ����1
}

/*******************************************************************************
  * @��������	Delay
  * @����˵��   ����һ����ʱʱ��
  * @�������   nCount: ָ����ʱʱ�䳤��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}


	/*
 ��FLASH��addr��д������ flashdata1
	void FLASH_WriteByte(u32 addr ,u16 flashdata1)
{
 FLASH_Status FLASHstatus = FLASH_COMPLETE;
 FLASH_Unlock();//??
 FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
 FLASHstatus=FLASH_ErasePage(addr);//????
 FLASHstatus=FLASH_ProgramHalfWord(addr, flashdata1);//????
 FLASH_Lock();//??
}
	#define Flash_update_flag_Address 0x08014000
	��ȡFLASH 
	rdata=*(u16 *)Flash_update_flag_Address
	*/

#ifdef  USE_FULL_ASSERT

/*******************************************************************************
  * @��������	assert_failed
  * @����˵��   �����ڼ�������������ʱ��Դ�ļ����ʹ�������
  * @�������   file: Դ�ļ���
  				line: ������������
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* �û����������Լ��Ĵ������ڱ��������ļ�������������,
       ���磺printf("�������ֵ: �ļ��� %s �� %d��\r\n", file, line) */

    //��ѭ��
    while (1)
    {
    }
}
#endif

/*
// ����1�Ľ����жϺ���
void USART1_IRQHandler(void)
{
	
//======================>> ���������ж�
    if( USART_GetITStatus( USART1, USART_IT_RXNE ) != RESET )
    {
		  RxBuffer[i] = USART_ReceiveData(USART1);
			i++;
			if(i>=6) i=0;
		if((RxBuffer[0]== 'u')&&(RxBuffer[1]== 'p')&&(RxBuffer[2]== 'd')&&(RxBuffer[3]== 'a')&&(RxBuffer[4]== 't')&&(RxBuffer[5]== 'e'))
		{
			  g_b_receive_UpdateFlag = 0x01;	
    }				
        USART_ClearITPendingBit( USART1, USART_IT_RXNE ); 
    }
}
*/


//  NVIC_SystemReset();    ����ϵͳ����


/*******************************�ļ�����***************************************/
