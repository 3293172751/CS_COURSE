#include "timer.h"
#include "led.h"
#include "usart.h"
#include "control.h"
#include "can.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
#define ENCODER_TIMER   TIM2  // ������ �������� ������ TIM2
u8 canbuf[8]={1},count=0;


// ϵͳ����ʱ��
void TIM3_sys_Init(void)
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
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;            //TIM3�ж�ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);           //ʹ��TIM3����				 
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		 TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIM3�����жϱ�־ 
		    
			ptForkLift->u16RunRecvPeriodMotor++;  //��� pwm ֵ ���� �ϴ� ����   0~10 ϵͳʱ���жϼ���
			ptForkLift->u16RunSendPeriodMotor++;  //��¼ϵͳʱ��1ms�жϴ��� 0~50 ÿʮ�μ��  ÿ50msִ��һ��
			ptForkLift->u16RunDetADC++;
			ptForkLift->u16RunPwmChange++;
			
			/*����CAN����״̬������״̬���ж�*/
			ptForkLift->u16CanComPeriod++;         // 0~500 ÿ1ms����һ��
			
			if(ptForkLift->u16RunRecvPeriodMotor==11)
			{
				ptForkLift->u16RunRecvPeriodMotor=1;// ÿms + 1   ��1 ~10֮��

			}

      if(ptForkLift->u16RunDetADC==10){// 50ms�ɼ�һ�� ���������ѹ
			  ptForkLift->ADC_convert_start=1;
				ptForkLift->u16RunDetADC=0;
			}
			
			if(ptForkLift->u16RunPwmChange==500){// 0.5s�ı�һ��PWM
			  ptForkLift->PWM_change_start=1;
				ptForkLift->u16RunPwmChange=0;
			}
			if(ptForkLift->u16CanComPeriod==100)   // 0.1s����һ�� can ���� �жϼ���
			{
				ptForkLift->u16CanComCount[1]=ptForkLift->u16CanComCount[0]; //u16CanComCount[0] can �ж� ������Ϣ���� 
			}
			
			else if(ptForkLift->u16CanComPeriod==500)// 0.5s �ж�һ�� can ���ռ���
			{
			  
				if(ptForkLift->u16CanComCount[0] > ptForkLift->u16CanComCount[1])// can �����жϼ������� ����λ����ϵ����
					ptForkLift->bCanComBox = CanBoxPost;                           // �������������ٶ�
				
				else
					ptForkLift->bCanComBox = CanBoxPend;                           // ���� ����λ��ʧȥ����ϵ  ֹͣ�����ٶ�
				
				ptForkLift->u16CanComPeriod=0;
				ptForkLift->u16CanComCount[1]=ptForkLift->u16CanComCount[0]=0;//����
				LED0=!LED0;//��ʾϵͳ��������
			
			}			
	
/*			
			if(ptForkLift->u16RunSendPeriodMotor == 50)//50ms ����һ�� can ״̬
			{
				Can_Send_Msg();
				ptForkLift->u16RunSendPeriodMotor = 0;//0~50  can�����жϼ���  ͬ 			
			}
*/			
			if( ptForkLift->u16time_ms < 3000 )
				ptForkLift->u16time_ms ++;
			
			if(ptForkLift->u16time_ms == 3000)
				ptForkLift->u16time_ms = 3001;
			
			ptForkLift->s16EncoderSpeed=(ENCODER_TIMER->CNT-15000)/4;// 1ms �ɼ����������ٶ�
			printf("%d\r\n",ptForkLift->s16EncoderSpeed);
			
			ENCODER_TIMER->CNT = 15000;
											
		}
}



// ��� PWM��� �Լ� �������  GPIOA.8 pwm  GPIOA.9 direction �ϰ汾
// ��� PWM��� �Լ� �������  GPIOA.8 pwm  GPIOA.9 ��  ��   GPIOA.8 �� GPIOA.9 pwm  ��   �°汾
//TIM1 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��

void TIM1_PWM_Init()
{  
	GPIO_InitTypeDef         GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//ʹ�ܶ�ʱ��1ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
	 
	/* �޸�Ϊ ����ͨ�� pwm*/
   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��	GPIOA.8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10; //TIM1_CH1   TIM1_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;      //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);               //��ʼ��GPIO
	
/*	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//�����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
  GPIO_SetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
*/	
	
	/*
	PWMƵ��=                    ϵͳʱ��                 72000000 
	            ������������������������������������������       =          ����������������������    =20 KHz
					��TIM_Period+1��*��TIM_Prescaler+1��         1800*2
	*/
	
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
	TIM_OCInitStructure.TIM_Pulse = 0;  //��ʼռ�ձ�ΪTIM_Pulse/TIM_Period = 0
	
	// ͨ��1  PA8  TIM1_CH1
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);           //����Tָ���Ĳ�����ʼ������TIM1 OC1
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���
	
	/*�¼���  ��PA9�� ����Ϊ  pwm ��  ֮ǰΪ  ��ͨIO��*/
	// ͨ��2  PA9	TIM1_CH2
 	TIM_OC2Init(TIM1, &TIM_OCInitStructure);           //����Tָ���Ĳ�����ʼ������TIM1 OC2
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
  // PA10   TIM1_CH3
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);           //����Tָ���Ĳ�����ʼ������TIM1 OC2
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
	// PA11   TIM1_CH4
 
 
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//ʹ��PWM���
	
}


// ������ �������� ��ʱ��
//TIM2 ���������ʼ��
void TIM2_Encoder_Init(void)
{
	  GPIO_InitTypeDef         GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef        TIM_ICInitStructure; 
	  NVIC_InitTypeDef         NVIC_InitStructure;
	
	// ����ʱ��ʹ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʹ��TIM2ʱ��  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  //PA0 A��  ,PA1 B��
    GPIO_StructInit(&GPIO_InitStructure);//��GPIO_InitStruct�еĲ�����ȱʡֵ����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA0 PA1��������  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    // 50����
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
	
   // ���ȼ� 1 0
	/* �ж��������ʼ�� Enable the TIM2 Update Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	
	  TIM_DeInit(ENCODER_TIMER);//������ģʽ  ENCODER_TIMER�궨��Ϊ tim2
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	//��Ƶ����
	  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
    TIM_TimeBaseStructure.TIM_Period = 30000;  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // ����ʱ�ӷָ� T_dts = T_ck_int	
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���  
    TIM_TimeBaseInit(ENCODER_TIMER, &TIM_TimeBaseStructure);
	
	// ������ģʽ
	 TIM_EncoderInterfaceConfig(ENCODER_TIMER, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//�����ؼ���
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 0;//ѡ������Ƚ��˲���
    TIM_ICInit(ENCODER_TIMER, &TIM_ICInitStructure);//ENCODER_TIMER�궨��Ϊ tim2
  
    // Clear all pending interrupts û���жϷ������Դ˴���ע��
    //TIM_ClearFlag(ENCODER_TIMER, TIM_FLAG_Update);
    //TIM_ITConfig(ENCODER_TIMER, TIM_IT_Update, ENABLE);//�ж�
	
    //Reset counter
    
	TIM_SetCounter(ENCODER_TIMER,15000); // ���ü���ֵ
	TIM_Cmd(ENCODER_TIMER, ENABLE);      //ʹ��
	
	
	
}





