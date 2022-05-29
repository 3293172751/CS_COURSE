#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "can.h" 
#include "timer.h" 
#include "control.h"
 
/************************************************
 ALIENTEKս��STM32������ʵ��26
 CANͨ��ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
 
 ���ο�����
 �����ߣ�����ǿ
 ����ʱ�䣺2017/03/20
 �ļ�˵�����ǳ�����Ƽ���˾�泵�����������
 
 ����log��
 2017/03/20  CANͨ���ȶ�
 2017/03/21  ��Ӵ��ڣ�ʱ��ϵͳ
 2017/04/24  ����������빦�ܣ�ʹ��TIM2
 
 ������ ����
 2017/06/18  �¾ɰ��� pwm ���
 2017/06/21  ���ĸ�ߵ���  ADC DMA �ж��˲�����
 

************************************************/


/************************************************

���������
���� :      PB6  , USART1_TX     PB7 , USART1_RX	 ����   115200bps
CANͨѶ:    PA11 , RX  PA12 , TX  ����> ���� TJA1050 ת��  CAN_H  CAN_L  ��ƽ�ź�
JLINK����:  PA13 , SWDIO ������   PA14��SWCLK ʱ����

��������  PA8  PA9 PWM��   TIM1  CH1  CH2
���ĸ�߲�����      ADC-DMA  PA6 
�����Ų������趨���°��� ��PB10  PB1  PB0            �ϰ��ӣ� PA2 PA3 PA4 PA5
������������� ��   TIM2  ��������  PA0 A�� ��  PA1   B��

ϵͳ����ָʾ�ƣ�    PB12



************************************************/
u8 board_type =1; //0 - �ɰ���   1 - �°���

 int main(void)
 {	 

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ�(0~3)��2λ��Ӧ���ȼ�(0~3)
	
	 /*           ��ռ���ȼ�         �����ȼ�3��
	  CAN1             	0                  	2
	  
	  TIM1 			   	    0				          	3   PWM���
	  TIM2			      	1					          0		��������
	  UART1            	3                  	3
	 	 
	 ADC-DMA            2                   1
	 */
	 

	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200

	printf("\r\nSystem init OK\r\n");


  LED_Init();	 
  
// CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,12,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������250Kbps    
  
	
	TIM1_PWM_Init();          //��ʱ��1����PWM�����Ƶ��20KHz
	TIM2_Encoder_Init();
	//TIM3_Int_Init();          //��ʱ��3����ϵͳʱ��,1ms����һ���ж�  	 
	
	 
	delay_ms(1000);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
	delay_ms(1000);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
	delay_ms(1000);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
		 
	ptForkLift=&TForkLift; //
	ptmiddle_filter_queue = &middle_filter_queue;
	
	CarID_Select();
	printf("CarID:%d\r\n",ptForkLift->u8CarID);

/*
	if(ptForkLift->u8CarID==1)
		ptForkLift->Can_Txmessage.StdId=0x02;		 // ��׼��ʶ��  carID = 1 canid = 2 ������� 
	else if(ptForkLift->u8CarID==0) 
		ptForkLift->Can_Txmessage.StdId=0x03;		// ��׼��ʶ��  carId = 0 canid = 3  �Ҳ�����
*/
  if(ptForkLift->u8CarID==1)
		ptForkLift->Can_Txmessage.StdId=0x04;   // ��׼��ʶ��  carID = 1 canid = 4  ǰ��
	else if(ptForkLift->u8CarID==2) 
		ptForkLift->Can_Txmessage.StdId=0x05;		// ��׼��ʶ��   carId = 2 canid = 5  ����
	else if(ptForkLift->u8CarID==3) 
		ptForkLift->Can_Txmessage.StdId=0x06;		// ��׼��ʶ��   carId = 3 canid = 6  ����
	else if(ptForkLift->u8CarID==4) 
		ptForkLift->Can_Txmessage.StdId=0x07;		// ��׼��ʶ��   carId = 4 canid = 7  ����
	else if(ptForkLift->u8CarID==5) 
		ptForkLift->Can_Txmessage.StdId=0x08;		// ��׼��ʶ��   carId = 5 canid = 8  ���²�
	else if(ptForkLift->u8CarID==6) 
		ptForkLift->Can_Txmessage.StdId=0x09;		// ��׼��ʶ��   carId = 6 canid = 9  ǰ���
	

  ptForkLift->Can_Txmessage.ExtId=0x12;				    // ������չ��ʾ�� 
  ptForkLift->Can_Txmessage.IDE=CAN_Id_Standard;  // ��׼֡
  ptForkLift->Can_Txmessage.RTR=CAN_RTR_Data;		  // ����֡
  ptForkLift->Can_Txmessage.DLC=8;				        // Ҫ���͵����ݳ���   
	
	/*
	//���ֳ� 25ms��������
	ptForkLift->s16speed_p = 0.6;    // 15  //
	ptForkLift->s16speed_i = 0.12;    // 1  //0.12
	ptForkLift->s16speed_d = 0.03;    // 3     //
	*/
	/*
  ptForkLift->s16speed_p = 0.405;    // 15  //
	ptForkLift->s16speed_i = 0.055;    // 1  //0.12
	ptForkLift->s16speed_d = 0.081;      // 3     //
	*/
	//pid������ʼ��
	//���ֳ� 1ms��������    0.19  0.03  3
	ptForkLift->s16speedwant = 0;    //ע����Ҫ���ٶ�ֻ����PID���ƺ����ⲿ���ã�������POSITION_PID��������	
	ptForkLift->s16speed_p = 0.18;
	ptForkLift->s16speed_i = 0.03;
	ptForkLift->s16speed_d = 3;
	
	
	
//�泵�������
///*
ptForkLift->s16fb_d=0.032; //ǰ�������ֱ�� 0.032m    3.2cm
ptForkLift->s16ud_d=0.032; //���²�����ֱ��
ptForkLift->s16dp_d=0.485; //����ֱ��      //0.485m   48.5cm
ptForkLift->s16dpl_d=0.125;//��������ֱ��   //0.125m   12.5cm

ptForkLift->u32dpl_econder=500;//����������
ptForkLift->u32dpl_econder_reduc=55;//�����ּ��ٱ�

ptForkLift->u32ud_econder=500;//����������
ptForkLift->u32ud_econder_reduc=55;//�����ּ��ٱ�

ptForkLift->u32fb_econder=500;//ǰ��������
ptForkLift->u32fb_econder_reduc=160;//ǰ���ּ��ٱ�
//*/



//��ʼ��
	ptForkLift->bDrection =FOWARD;  // BACK  FOWARD
	ptForkLift->u16PWM=00;
	ptForkLift->u16RunPIDControl = 0;
	ptForkLift->u32ADCResult = 0;
	ptForkLift->s32EncoderCounter = 0;
	ptForkLift->bCanSendCounter = 0;
	
	if (board_type ==0)
		SetPwmDir(ptForkLift);//�ϰ�  
	else
		SetPwmDir_new(ptForkLift);//�°���  
		
	create_queue(ptmiddle_filter_queue);   //����һ���������˲�����
	 
//  ���ϰ��� ������  control.h �ļ��� 
/***����ת��Ҫ�޸ĵĵط�*************************/
	 ptForkLift->s16speedwant = 00;   // 0~200
	// ptForkLift->s16speedwant =-50; //��ת   ��Χ +- 1700
	// ptForkLift->s16speedwant = 50;  //��ת   ��Χ +- 1700

  // Adc_Init();            // ��ͨ ADC 
	// u16 MotorCarrentADC1Value;
	// MotorCarrentADC1Value=Get_Adc_Average(ADC_Channel_0,5);
	// temp=(float)MotorCarrentADC1Value*(3.3/4096);
	
	//TIM1_PWM_Init();          //��ʱ��1����PWM�����Ƶ��20KHz
	//TIM2_Encoder_Init();
	
	motor_carrent_io_init();  // PA6������Ϊ  ADC1 DMA ���� ���ĸ�ߵ��������� ����	
		
	TIM3_Int_Init();          //��ʱ��3����ϵͳʱ��,1ms����һ���ж�  
  CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,12,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������250Kbps 
	
	printf("\r\nSystem Running\r\n");
	
 	while(1)
	{	
		
		if(ptForkLift->u16RunRecvPeriodMotor==10)
		{			
//				printf("u16RunRecvPeriodMotor %d\r\n",ptForkLift->u16RunRecvPeriodMotor );
if(1)//���� pid����ʱΪ0  �������и�Ϊ1
{	
///*			
			if(ptForkLift->bCanComBox == CanBoxPend)//����״̬
			{
				ptForkLift->s16speedwant = 0;
				ptForkLift->s16error[0] = 0;
				ptForkLift->s16error[1] = 0;
				ptForkLift->s16error[2] = 0;
				ptForkLift->s16ErrorSum = 0;
			}

//*/	
}	
		/*		
			else if(ptForkLift->bCanComBox == CanBoxPost)//����״̬ 
			{
				ptForkLift->Can_Txmessage.Data[0] = (ptForkLift->s32EncoderCounter) & 0xff;
				ptForkLift->Can_Txmessage.Data[1] = ((ptForkLift->s32EncoderCounter)>>8) & 0xff;
				ptForkLift->Can_Txmessage.Data[2] = ((ptForkLift->s32EncoderCounter)>>16) & 0xff;
				ptForkLift->Can_Txmessage.Data[3] = ((ptForkLift->s32EncoderCounter)>>24) & 0xff;
			}		
		*/	
/*				
				ptForkLift->Can_Txmessage.Data[0] = (ptForkLift->s16EncoderSpeed) & 0xff;
				ptForkLift->Can_Txmessage.Data[1] = ((ptForkLift->s16EncoderSpeed)>>8) & 0xff;
*/							
				/*				
				if(ptForkLift->u16time_ms < 3000 )
				{
					ptForkLift->Can_Txmessage.Data[0]=254;
				}
				else
				{
					ptForkLift->Can_Txmessage.Data[0]=255;
				}
*/					
//				printf("stop motor\r\n");
//				printf("%d \r\n",ptForkLift->s16EncoderSpeed);
							
		}
	/*	
			if(ptForkLift->u16RunSendPeriodMotor == 50)// 50ms����һ�� ����������  ���� ������ WHILE(1)��
			{
			
				ptForkLift->u16RunSendPeriodMotor = 0;
				ptForkLift->bCanSendCounter = ptForkLift->bCanSendCounter + 1;

				ptForkLift->Can_Txmessage.Data[0] = (ptForkLift->s32EncoderCounter)       & 0xff;
				ptForkLift->Can_Txmessage.Data[1] = ((ptForkLift->s32EncoderCounter)>>8)  & 0xff;
				ptForkLift->Can_Txmessage.Data[2] = ((ptForkLift->s32EncoderCounter)>>16) & 0xff;
				ptForkLift->Can_Txmessage.Data[3] = ((ptForkLift->s32EncoderCounter)>>24) & 0xff;
				ptForkLift->Can_Txmessage.Data[4] = (u8)(ptForkLift->bCanSendCounter)     & 0xff;
				ptForkLift->Can_Txmessage.Data[5] = ( ptForkLift->u16I_Value)             & 0xff;
				ptForkLift->Can_Txmessage.Data[6] = ((ptForkLift->u16I_Value)>>8)         & 0xff;			
				ptForkLift->s32EncoderCounter = 0;			
				Can_Send_Msg(); //����: �ŵ���������			
			}
		*/
	    //printf("EncoderSpeed=%d \r\n",ptForkLift->s16EncoderSpeed);
/*			
	 // ���� ADC����
		if(ptForkLift->u16RunDetADC==50){          // 50ms�ɼ�һ�� ���������ѹ
			 ptForkLift->u16RunDetADC=0;
	   	 ADC_SoftwareStartConvCmd(ADC1,ENABLE);  // ʹ��ADC1 �����ת������  ������������  ��������  			
		}
*/		
					
     }
		 
}



