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
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTBʱ��	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10;				 // PB0
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //��������	  ����Ϊ0  ���� Ϊ1
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.0	 		
	 ptForkLift->u8CarID=(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0))+(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)<<1)+(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)<<2);
}
