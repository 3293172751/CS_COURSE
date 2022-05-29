#include "timer.h"
#include "led.h"
#include "usart.h"
#include "control.h"
#include "can.h"
extern float encoder_fliter;
extern float encoder_fliter2;
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/4
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,����TIM3_PWM_Init������
//2,����LED0_PWM_VAL�궨�壬����TIM3_CH2����									  
//////////////////////////////////////////////////////////////////////////////////  
   	  
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
#define ENCODER_TIMER   TIM2  // Encoder unit connected to TIM2
u8 canbuf[8]={1},count=0;

// ���ĸ�ߵ���  ��ŷ���� ��ѹ���� ֵ
//float ADC_Value;


//static int queue_lenth =0;
	int fliter_speed;




void TIM3_Int_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	/*
	
	ʱ���ж���72000000/71+1/1000=1000 HZ
	
	��1 ms
	  
	*/	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 1000; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =71; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx		
	
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		  TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		    
			ptForkLift->u16RunRecvPeriodMotor++;//can �ڽ���״̬���� ����
			ptForkLift->u16RunSendPeriodMotor++;//�����ϴ���Ϣ ��ʱ
			ptForkLift->u16RunPIDControl++;//pid��������ʱ
				
			if(ptForkLift->u16RunRecvPeriodMotor>10) // can ״̬�������  10ms
			{
				ptForkLift->u16RunRecvPeriodMotor=1;
			}

			/*����CAN����״̬������״̬���ж�*/
			ptForkLift->u16CanComPeriod++; //CAN 201 ���ռ���  
			ptForkLift2->u16CanComPeriod++; //CAN 202 ���ռ��� 
			
			if(ptForkLift->u16CanComPeriod==100)
			{
				ptForkLift->u16CanComCount[1]=ptForkLift->u16CanComCount[0];
				ptForkLift2->u16CanComCount[1]=ptForkLift2->u16CanComCount[0];
				ptForkLift->u16SerialComCount[1]=ptForkLift->u16SerialComCount[0];//���ڽ��� ���� ���� 
			}		
			else if(ptForkLift->u16CanComPeriod==500)
			{	
				if(ptForkLift->u16CanComCount[0]>ptForkLift->u16CanComCount[1])
					ptForkLift->bCanComBox = CanBoxPost; //CAN 1 �շ�����
				else
					ptForkLift->bCanComBox = CanBoxPend; //CAN 1 �շ�������
				
				if(ptForkLift2->u16CanComCount[0]>ptForkLift2->u16CanComCount[1])
					ptForkLift2->bCanComBox = CanBoxPost; //CAN 2 �շ�����
				else
					ptForkLift2->bCanComBox = CanBoxPend; //CAN 2 �շ�������
				
				if(ptForkLift->u16SerialComCount[0] > ptForkLift->u16SerialComCount[1])
					ptForkLift->u8SerialCmdFlag=1;//���ڽ��� ���� ����
				else 
					ptForkLift->u8SerialCmdFlag=0;
				
				ptForkLift->u16CanComPeriod=0;
				ptForkLift->u16CanComCount[1]=ptForkLift->u16CanComCount[0]=0;
				ptForkLift2->u16CanComCount[1]=ptForkLift2->u16CanComCount[0]=0;
				ptForkLift->u16SerialComCount[1]=ptForkLift->u16SerialComCount[0]=0;
				LED0=!LED0;//��ʾϵͳ��������
			}

		// ת��  ��ֵ
		/*
			ptForkLift->s32speed50ms += ptForkLift->speed_rpm;//ת�ٺ�
			ptForkLift2->s32speed50ms += ptForkLift2->speed_rpm;//ת�ٺ�		
      if(ptForkLift->u16RunSendPeriodMotor >= 50)// 50ms����һ��  ���� ������ WHILE(1)��
			{
				  ptForkLift->u16RunSendPeriodMotor =0;
			//	ptForkLift->s16MeanSpeed = ptForkLift->s32speed50ms/50.0; //50ms�ھ�ֵrpm   /60*0.1*3.14 Ϊ�ٶ� m/s
			//	ptForkLift2->s16MeanSpeed = ptForkLift2->s32speed50ms/50.0;
					ptForkLift->s16MeanSpeed = ptForkLift->s32speed50ms/50.0; //50ms�ھ�ֵrpm   /60*0.1*3.14 Ϊ�ٶ� m/s
			  	ptForkLift2->s16MeanSpeed = ptForkLift2->s32speed50ms/50.0;
				  ptForkLift->s32speed50ms=0;
				  ptForkLift2->s32speed50ms=0;
				  ptForkLift->u8Can_Msg_Send_flag=1;
			}
			*/
			// �������� ֮��  ������Ҫ ��ת�� ת���� ������ֵ
			ptForkLift->u16EncoderSum += ptForkLift->speed_rpm;
			ptForkLift2->u16EncoderSum += ptForkLift2->speed_rpm;


			//if(ptForkLift->u16RunPIDControl == ptForkLift->pid_time) // ����������� PID�� �������� ��λ 1ms  5 ����5ms  10����10ms
			if(ptForkLift->u16RunPIDControl >= ptForkLift->pid_time) 
			{					
				encoder_fliter = (ptForkLift->speed_rpm + encoder_fliter*4 ) / 5;    //���ַ���ֵ  ��ֵ�˲�
				encoder_fliter2 = (ptForkLift2->speed_rpm + encoder_fliter2*4 ) / 5; //���ַ���ֵ 
				//	printf("%d\r\n",(int)(encoder_fliter));
// pid ����  �������ֵ				
				POSITION_PID(ptForkLift,(int)(encoder_fliter));//����pid����    201   
				POSITION_PID(ptForkLift2,(int)(encoder_fliter2));//����pid����  202		
				// POSITION_PID(ptForkLift,ptForkLift->speed_rpm);
				// ZL_PID(ptForkLift,ptForkLift->speed_rpm);
// ͨ�� can ͨ�Žӿ� �·� ����ָ�� �� ��� C620 ������ˢ���ת��
				set_moto_current((int16_t)ptForkLift->s16speedout,(int16_t)ptForkLift2->s16speedout,0,0);
				// set_moto_current( ptForkLift,120,0,0,0);// 130 ת
				ptForkLift->u16RunPIDControl = 0;
			}
						
		}
}



//TIM1 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_PWM_Init()
{  
	//if(ptForkLift->u8CarID==1) //�°汾   pA8 PA9��Ϊpwm��
	//if(BOARD_version==1)
	//	{
	GPIO_InitTypeDef         GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//ʹ�ܶ�ʱ��1ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	 
   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��	GPIOA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
	
	/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//�����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
  GPIO_SetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
	*/
	
	/*
	PWMƵ��=                    ϵͳʱ��                          72000000 
	            ������������������������������������������  = ������������������������=20 KHz
					��TIM_Period+1��*��TIM_Prescaler+1��           1800*2
	
	
	*/
	
   //��ʼ��TIM1
	TIM_TimeBaseStructure.TIM_Period = 1799;//1800*10*2-1;//1799; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);//����ARRӰ�ӼĴ���
	
	//��ʼ��TIM1 Channel PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; 
	TIM_OCInitStructure.TIM_Pulse = 0;//��ʼռ�ձ�ΪTIM_Pulse/TIM_Period
	
	
	// ͨ��1  PA8
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);           //����Tָ���Ĳ�����ʼ������TIM1 OC1
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���
	
	/*�¼���  ��PA9�� ����Ϊ  pwm ��  ֮ǰΪ  ��ͨIO��*/
	// ͨ��2  PA9	
 	TIM_OC2Init(TIM1, &TIM_OCInitStructure);           //����Tָ���Ĳ�����ʼ������TIM1 OC2
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
 
 
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//ʹ��PWM���
	
		//printf("\r\n\t������汾\t");
	  //printf("%d\r\n",ptForkLift->u8CarID);
		//printf("\r\n\t �°汾 \t");
		
	
 
	/*�ϰ汾 PA8  Pwm �� PA9 ��ͨIO�� ȷ������*/
		
	//else if(ptForkLift->u8CarID==0){
	//else if(BOARD_version==0)	{
 /*	
	GPIO_InitTypeDef         GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//ʹ�ܶ�ʱ��1ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO������ӳ�书��ģ��ʱ��
	
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	
   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��	GPIOA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������   ������PWM����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO


	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//�����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
  GPIO_ResetBits(GPIOA,GPIO_Pin_9);		  //PA.9 �����

	
	
   //��ʼ��TIM1
	TIM_TimeBaseStructure.TIM_Period = 1799; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);//����ARRӰ�ӼĴ���
	
	//��ʼ��TIM1 Channel PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; 
	TIM_OCInitStructure.TIM_Pulse = 0;//��ʼռ�ձ�ΪTIM_Pulse/TIM_Period
	
	
	// ͨ��1  PA8
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);           //����Tָ���Ĳ�����ʼ������TIM1 OC1
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���
	
	//�¼���  ��PA9�� ����Ϊ  pwm ��  ֮ǰΪ  ��ͨIO��
	// ͨ��2  PA9	
 	//TIM_OC2Init(TIM1, &TIM_OCInitStructure);           //����Tָ���Ĳ�����ʼ������TIM1 OC2
	//TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
 
 
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//ʹ��PWM���
	
		printf("\r\n\t������汾\t");
	  printf("%d\r\n",ptForkLift->u8CarID);
		printf("\r\n\t �ϰ汾 \t");
		*/
	//}
	
}

//TIM2 ���������ʼ��
void TIM2_Encoder_Init(void)
{
	  GPIO_InitTypeDef         GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef        TIM_ICInitStructure; 
	 NVIC_InitTypeDef          NVIC_InitStructure;
	
	    //PA0 ch1  ,PA1 ch2 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ��TIM2ʱ��  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

    GPIO_StructInit(&GPIO_InitStructure);//��GPIO_InitStruct�еĲ�����ȱʡֵ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA0 PA1��������  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
	
	
	/* �ж��������ʼ�� Enable the TIM3 Update Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	
	TIM_DeInit(ENCODER_TIMER);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	//��Ƶ����
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
    TIM_TimeBaseStructure.TIM_Period = 30000;  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
    TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);
	
	
	TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 0;//ѡ������Ƚ��˲���
    TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);
  
    // Clear all pending interrupts û���жϷ������Դ˴���ע��
    //TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);
    //TIM_ITConfig(ENCODER_TIMER, TIM_IT_Update, ENABLE);//�ж�
	
    //Reset counter
    
	TIM_SetCounter(ENCODER_TIMER,15000);
	TIM_Cmd(ENCODER_TIMER, ENABLE);
	
	
	
}





