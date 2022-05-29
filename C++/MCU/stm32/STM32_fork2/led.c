#include "led.h"
#include "control.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////
/*
GPIO ģʽ
(1)GPIO_Mode_AIN         ģ������ 
(2)GPIO_Mode_IN_FLOATING ��������
(3)GPIO_Mode_IPD         ��������
(4)GPIO_Mode_IPU         ��������
(5)GPIO_Mode_Out_OD      ��©���
(6)GPIO_Mode_Out_PP      �������
(7)GPIO_Mode_AF_OD       ���ÿ�©���
(8)GPIO_Mode_AF_PP       �����������
GPIO_Speed_10MHz         ������Ƶ�� 10MHz 
GPIO_Speed_2MHz          ������Ƶ�� 2MHz 
GPIO_Speed_50MHz         ������Ƶ�� 50MHz
*/


#define ADC1_DR_Address    ((uint32_t)0x4001244C)
// #define ADC3_DR_Address    ((uint32_t)0x40013C4C)
// #define ADC1_DR_Address    ((u32)0x40012400+0x4c)   

// �ο��ֲ� P178  0x4c  ΪADC1_DR �ľ����ַ
//0x40012400  ΪADC�Ĵ������׵�ַ    P28 

// �������� ��ֵ�˲�
#define num  2
__IO uint16_t ADC1ConvertedValue_[num]={0};

//__IO uint16_t ADC1ConvertedValue = 0;

// define     __IO    volatile  ��ʧ�ͱ���  ��Ҫֱ�Ӷ�ȡ�ڴ��ַ�е�ֵ 
// typedef unsigned short int  uint16_t;
 


/*****************ϵͳ����ָʾ�� PB12**************/
//��ʼ��PB12Ϊ�����.��ʹ������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				       //LED0-->PB.12 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		   //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		   //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					         //�����趨������ʼ��GPIOB.12
	 GPIO_ResetBits(GPIOB,GPIO_Pin_12);						           //PB.12 �����
}


void CarID_Select(void)
{
	/* �ϰ���  PA2 PA3 PA4 PA5  ȫ���͵�ƽ */
	/* �°���  PB0  PB1  PB10   ȫ���ߵ�ƽ */
	/*
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTAʱ��	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 // PA5
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������	  �ϰ���Ϊ 0  �°���Ϊ 1
	 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.5
	 ptForkLift->u8CarID=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5);//   0 �Ҳ�   1���(����)

	*/

	/* �°���1  PB0  PB1  PB10   ���� �ӵ� Ϊ0 */
	/* �°���2  PB0  PB1  PB10   ���� ���� �������� ȫ���ߵ�ƽ */	
	/*
   GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTBʱ��	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 // PB0
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������	  ����Ϊ0  ���� Ϊ1
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.0
	 		
	 ptForkLift->u8CarID=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);//   0 �Ҳ�   1���(����)
	 */
	 //�泵
	 	/* car_id
	ǰ�� 1   PB0���ӵ�    PB1        PB10�ӵ�
	���� 2   PB0  �ӵ�    PB1���ӵ�  PB10�ӵ�
	���� 3   PB0  ���ӵ�  PB1���ӵ�  PB10�ӵ�
	���� 4   PB0  �ӵ�    PB1�ӵ�    PB10���ӵ�
	
	���� 5   PB0  ���ӵ�  PB1�ӵ�    PB10���ӵ�
	ǰ�� 6   PB0  �ӵ�    PB1���ӵ�    PB10���ӵ�
	*/
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTBʱ��	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10;				 // PB0
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������	  ����Ϊ0  ���� Ϊ1
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.0
	 		
	 ptForkLift->u8CarID=(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0))+(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)<<1)+(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)<<2);
	
}





// ADC  DMA���� ���ĸ�ߵ����� �˿�����   PA6

void motor_carrent_io_init(void)
 {

	 /*�ϰ����°��� ���ĸ�ߵ������� ��Ϊ  PA6��*/
	 // �������� Ϊ ���� 0.01R ����  ��Ϊ 0.05R  ��5mR	     �ڲ��̶��Ŵ�10*2��   ����1.8 �� 3.3 �����ѹ
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
	
//DMA�ж�	 
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	//�˿�ʱ������	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); // ADC1ʱ��ʹ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//GPIOA�� ʱ��ʹ��
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);   // ���� DMA1 ��ʱ��
	 
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);                   // ADC����ʱ��Ϊ 72MHz / 6  = 12MHz    ��߲��ܳ���14MHz
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //����PA6��Ϊ����������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
  DMA_DeInit(DMA1_Channel1);//��λ DMA1 ������DMA1��ȫ���Ĵ�������Ϊȱʡֵ 
  //����DMA���䷽ʽ
  DMA_InitStructure.DMA_PeripheralBaseAddr=  ADC1_DR_Address;   //�������ַ (uint32_t) (u32)&(ADC1->DR);  ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr= (u32)&(ADC1ConvertedValue_); //�ڴ����ַ  u32 ��������  (uint32_t ��̫��!!!)
  
	DMA_InitStructure.DMA_DIR= DMA_DIR_PeripheralSRC; //DMA���䷽��    ���赽�ڴ�
  /**��Ҫ�޸ĵĵط�**/
	DMA_InitStructure.DMA_BufferSize = num;             // DMAͨ����DMA����Ĵ�С ���䵥λ  �����Ҫ�˲� �����ö�� ���жϴ���
  DMA_InitStructure.DMA_PeripheralInc= DMA_PeripheralInc_Disable;//�����ַָ��������Ϊ���Զ����� �����ַ�Ĵ�������
  DMA_InitStructure.DMA_MemoryInc= DMA_MemoryInc_Enable; //�ڴ��ַָ��������Ϊ���Զ����� �ڴ��ַ�Ĵ���Ҳ����  ��� ���䵥λ����1 ��Ӧ����Ϊ DMA_MemoryInc_Enable
  
	DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_HalfWord;//�������ݰ��� 2���ֽ�     16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;       //�ڴ����ݴ�С��Ϊ����     16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;    //DMA���䷽ʽ��Ϊѭ������   ѭ��д//�ڴ����ַ buffer  // DMA_Mode_Normal;  //��������������ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA���ȼ���Ϊ��           // DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
  /*������ȼ� 00�� 01 �� 10 ��  11��ߣ�Ӳ�����ȼ� ͬ������ȼ� ͨ���ŵ͵� ���ȼ���  DMA1 1~7��ͨ�� DMA2 1~5��*/
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;       //�ڴ浽�ڴ�Ĵ���ʧ��
  DMA_Init(DMA1_Channel1,&DMA_InitStructure);        //��ʼ��DMA1������ͨ��1
  //DMA_Cmd(DMA1_Channel1,ENABLE);                   //ʹ�� DMA ͨ��1  ���� ADCУ׼��ʼ����


// DMA�ж�ʹ��  ������  ������ �ڿ����ж�
 DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); //ʹ�� DMA1�������Transfer complete�жϣ��ﵽָ����DMA_BufferSize��С�� �������ж��˲�

//DMA �ж�����
  // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); �ж�����������
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ���߸�Ϊ3 ����
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&NVIC_InitStructure);          // Enable the DMA Interrup
/////////////////////


// ADC ģʽ����
  ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  // ADC �����ڶ���ģʽ
 
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;       // ��ͨ������ ɨ��ر� ENABLE ʹ�ܶ�ͨ������ ������Ǹ�Ϊ ENABLE ��һ��
  /**��Ҫ�޸ĵĵط�**/	
  ADC_InitStructure.ADC_ContinuousConvMode= DISABLE;   // ����ת��ģʽ ����ͣ�� ����ADת��  DISABLEΪ����ת��ģʽ  ���߸�Ϊ DISABLE  ��ͨ�����β��� 
  
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;// ���ⲿ����  ��������ƴ���ת��  �������Ϊ ���� ADC_CR2 �Ĵ���ADON 
  ADC_InitStructure.ADC_DataAlign= ADC_DataAlign_Right;              // �����Ҷ���  λ��12  ����16λ�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;                            // ����һ��ͨ��	
  ADC_Init(ADC1,&ADC_InitStructure);
	
	//����ADC1��ͨ��6 PA6 ת������Ϊ1   ����ʱ��  Ϊ28.5����������  ADC_SampleTime_55Cycles5  ADC_SampleTime_239Cycles5
  ADC_RegularChannelConfig(ADC1,ADC_Channel_6,1,ADC_SampleTime_28Cycles5);
  ADC_DMACmd(ADC1,ENABLE);//ʹ��DMA-ADC
  ADC_Cmd(ADC1,ENABLE);   //ʹ��ADC1
	
	ADC_ResetCalibration(ADC1);                //��λУ׼�Ĵ���
  while(ADC_GetResetCalibrationStatus(ADC1));/* ��� ����У׼�Ĵ��� ��״̬  �ȴ���λУ׼����*/
  ADC_StartCalibration(ADC1);                //��ʼУ׼ Start ADC1 calibration 
  while(ADC_GetCalibrationStatus(ADC1));     // ���У׼�Ĵ�����״̬    �ȴ�У׼����
	
	 ADC_SoftwareStartConvCmd(ADC1,ENABLE);     // ʹ��ADC1 �����ת������  ������������  ��������
	
 // ADC_SoftwareStartConvCmd(ADC1,ENABLE);     // ʹ��ADC1 �����ת������
	
	//	ADC_Value = (float)(ptForkLift->ADC1ConvertedValue)/4096*3.3; λ��ѹֵ
	
	 DMA_Cmd(DMA1_Channel1,ENABLE);                     //ʹ�� DMA ͨ��1	
 }

 //�жϱ�־λ��ʽ
 //ADC_SoftwareStartConvCmd(ADC1, ENABLE);                                       
//while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )); 
//ADC_GetConversionValue(ADC1);

 // DMA1_Channel1 ���жϷ�����
 
//float ADC_Value1;
//#define N 5   // Ӧ���� DMA_InitStructure.DMA_BufferSize  һ����  ����ͨ��
void filter(void)
{
  u16  sum = 0;
  u8 count;
    for ( count=0;count<num;count++)
    {
       sum += ADC1ConvertedValue_[count];// ע�� ptForkLift->ADC1ConvertedValue Ӧ����Ϊ��Ӧ��С������  ADC1ConvertedValue[10]
    // sum  += ptForkLift->ADC1ConvertedValue;   
		}
    ptForkLift->u32ADCResult = sum/num;
    sum=0;
		ptForkLift->ADC_Value = (float)(ptForkLift->u32ADCResult)*33/40960;
		//ptForkLift->u16ADC_Value = (unsigned int)(ptForkLift->u32ADCResult)*3300>>12;  // �Ŵ�1000��
		//ptForkLift->u16ADC_Value = (unsigned int)(ptForkLift->u32ADCResult)*3300/4096;  // �Ŵ�1000��
		ptForkLift->u16ADC_Value = (u16)(ptForkLift->ADC_Value*1000.0);// �Ŵ�1000��
		
		//ptForkLift->I_Value = (float)(ptForkLift->ADC_Value)/3.3*(1.8+3.3)/20/0.005;
		//ptForkLift->I_Value = (float)__fabs(1.597 - ptForkLift->ADC_Value)*15.4545454545454545;
		
		//ǰ
		if(ptForkLift->u8CarID==0){
		ptForkLift->I_Value = (float)__fabs(1.596 - ptForkLift->ADC_Value)*5.934;//*15.455*0.384;
		ptForkLift->u16I_Value = (u16)(ptForkLift->I_Value*1000);  // �Ŵ�1000
		}
		//����
		else if(ptForkLift->u8CarID==1){
		ptForkLift->I_Value = (float)__fabs(1.599 - ptForkLift->ADC_Value)*5.934;//15.455*0.49;
		ptForkLift->u16I_Value = (u16)(ptForkLift->I_Value*1000);  // �Ŵ�1000
		}
		//printf("\r\n�����������\t");
		//printf("%d\r\n",ptForkLift->u16I_Value);
	//printf("%f\r\n",ptForkLift->ADC_Value);
		
}
 
 void DMA1_Channel1_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
  {
  filter();
  DMA_ClearITPendingBit(DMA1_IT_TC1);  //���жϱ�־λ
	//DMA_ClearFlag(DMA1_FLAG_TC1);        /*�¼���*/
  }
}
 
 

//��ʼ��ADC ��ͨ 
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��6																   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA6 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1 �����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);	 //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}				  
//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			     
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������		 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
// Get_Adc_Average(ADC_Channel_6 , 5);
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 

 
