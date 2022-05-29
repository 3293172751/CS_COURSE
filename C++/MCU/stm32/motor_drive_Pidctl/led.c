#include "led.h"
#include "control.h"
//////////////////////////////////////////////////////////////////////////////////	 
/*
GPIO ģʽ
(1)GPIO_Mode_AIN         ģ������ 
(2)GPIO_Mode_IN_FLOATING ��������
(3)GPIO_Mode_IPD         ��������
(4)GPIO_Mode_IPU         ��������
(5)GPIO_Mode_Out_OD      ��©���
(6)GPIO_Mode_Out_PP      ��������
(7)GPIO_Mode_AF_OD       ���ÿ�©���
(8)GPIO_Mode_AF_PP       �����Ƽ����
GPIO_Speed_10MHz         ������Ƶ�� 10MHz 
GPIO_Speed_2MHz          ������Ƶ�� 2MHz 
GPIO_Speed_50MHz         ������Ƶ�� 50MHz
*/						  
////////////////////////////////////////////////////////////////////////////////// 	   
#include "usart.h"	

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

//__IO uint16_t ADC1ConvertedValue = 0;

// ϵͳ����ָʾ�� PB12
//��ʼ��PB12 �����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	// ϵͳ������ָʾ�� PB12
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB �˿�ʱ��		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				     //LED0-->PB.12 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					       //�����趨������ʼ��GPIOB.12
	 GPIO_ResetBits(GPIOB,GPIO_Pin_12);						         //PB.12 ����͵�ƽ
	
/*	
	// �������ת���ƿ�  PA 9   Ҫ�޸�  �� TM3������
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PA �˿�ʱ��		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				       //motor_dir -->PA.9 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		   //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		   //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOA, &GPIO_InitStructure);					         //�����趨������ʼ��GPIOA.9
	 GPIO_ResetBits(GPIOA,GPIO_Pin_9);						           //PA.9 ����͵�ƽ
	*/
}

// ADC ���ĸ�ߵ����� �˿�����   PA6

void motor_carrent_io_init(void)
 {

	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
	//�˿�ʱ������	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); // ADC1ʱ��ʹ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//GPIOA�� ʱ��ʹ��
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);   // ���� DMA1 ��ʱ��
	 
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);                   // ADC����ʱ��Ϊ 72MHz / 6  = 12MHz   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //����PA6��Ϊ����������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
  DMA_DeInit(DMA1_Channel1);
  //����DMA���䷽ʽ
	ptForkLift->ADC1ConvertedValue=0;
  DMA_InitStructure.DMA_PeripheralBaseAddr= ADC1_DR_Address;           //�������ַ
  DMA_InitStructure.DMA_MemoryBaseAddr= (uint32_t)&(ptForkLift->ADC1ConvertedValue); //�ڴ����ַ
  DMA_InitStructure.DMA_DIR= DMA_DIR_PeripheralSRC; //DMA���䷽��
  DMA_InitStructure.DMA_BufferSize = 1;             //���䵥λ
  DMA_InitStructure.DMA_PeripheralInc= DMA_PeripheralInc_Disable;//�����ַָ��������Ϊ���Զ�����
  DMA_InitStructure.DMA_MemoryInc= DMA_MemoryInc_Disable;        //�ڴ��ַָ��������Ϊ���Զ�����
  DMA_InitStructure.DMA_PeripheralDataSize= DMA_PeripheralDataSize_HalfWord;//�������ݰ���
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;       //�ڴ����ݴ�С��Ϊ����
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;    //DMA���䷽ʽ��Ϊѭ������
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA���ȼ���Ϊ��
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;       //�ڴ浽�ڴ�Ĵ���ʧ��
  DMA_Init(DMA1_Channel1,&DMA_InitStructure);        //��ʼ��DMA1������ͨ��1
  DMA_Cmd(DMA1_Channel1,ENABLE);                     //ʹ�� DMA ͨ��1

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // ADC �����ڶ���ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;       // ��ͨ������
  ADC_InitStructure.ADC_ContinuousConvMode= ENABLE;  // ��������
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;// ���ⲿ����
  ADC_InitStructure.ADC_DataAlign= ADC_DataAlign_Right;              // �����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1;                            // ����һ��ͨ��

  ADC_Init(ADC1,&ADC_InitStructure);
	
	//����ADC1��ͨ��6  PA6 ת������Ϊ1   ����ʱ��  Ϊ28.5����������  ADC_SampleTime_55Cycles5
  ADC_RegularChannelConfig(ADC1,ADC_Channel_6,1,ADC_SampleTime_28Cycles5);
  ADC_DMACmd(ADC1,ENABLE);//ʹ��DMA-ADC
  ADC_Cmd(ADC1,ENABLE);//ʹ��ADC1
	
	ADC_ResetCalibration(ADC1);//��λУ׼�Ĵ���
  while(ADC_GetResetCalibrationStatus(ADC1));/* ��� ����У׼�Ĵ��� ��״̬*/
  ADC_StartCalibration(ADC1);//��ʼУ׼ Start ADC1 calibration 
  while(ADC_GetCalibrationStatus(ADC1)); // ���У׼�Ĵ�����״̬ 
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);  // ʹ��ADC1 �����ת������
	
	//	ADC_Value = (float)(ptForkLift->ADC1ConvertedValue)/4096*3.3; λ��ѹֵ
		
 }


// ������ ���� PB0 PB1  PB10			
void CarID_Select(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
		
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTAʱ��	
	
	 //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;
	
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������
	
	// GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	
	
//�����̽ӿ����� PB0 PB1  PB10
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTBʱ��	
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10; // ʹ��PB0 PB1  PB10			
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 // ��������

	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO PB0 PB1  PB10 
	 // byte b=a   |  (b<<1)  | (c<<2)  ��λ  ���ж�8�����
	 ptForkLift->u8CarID= GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) | (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)<<1)| (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)<<2);
			
	 
}










 
