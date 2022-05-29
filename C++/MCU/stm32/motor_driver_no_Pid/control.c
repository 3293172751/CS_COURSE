#include "control.h"
#include "PidContoller.h"
/************************************************
 
 �����ߣ�����ǿ
 ����ʱ�䣺2017/03/20
 �ļ�˵�����ǳ�����Ƽ���˾�泵�����������
 
 ����ϲ���Ƴ����ڴ��ļ���
 
************************************************/

//ȫ�ֱ�������
TCtrl TForkLift;     //����泵�ṹ��
ptCtrl ptForkLift;   //����泵�ṹ��ָ��

// ���� PWM�ͷ���ֵ
void SetPwmDir(ptCtrl ptCtrlCar)
{
	// pwm �޷� ���
	if(ptCtrlCar->u16PWM>MAX_PWM)
	{
		ptCtrlCar->u16PWM=MAX_PWM;
	}
	else if(ptCtrlCar->u16PWM<=MIN_PWM)
	{
		ptCtrlCar->u16PWM=MIN_PWM;
	}
	
	//TIM_SetCompare3(TIM1,(ptCtrlCar->u16PWM)/2);//AD�����ж�
	//TIM_SetCompare3(TIM1,ptCtrlCar->u16PWM);//AD�����ж
	 TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
	//TIM1->CCR1 = ptCtrlCar->u16PWM;       // ͨ��1 pwm
	
	// //PA.9 ���Ʒ��� �� ǰ��  �� ����
	if(ptCtrlCar->bDrection==FOWARD)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);  //dir	
		//GPIO_SetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
		TIM_SetCompare2(TIM1,1799);           // ͨ��2 pwm Ϊ 100% ���� ��Ϊ�ߵ�ƽ
		//TIM1->CCR2 = ptCtrlCar->1799;       // ͨ��2 pwm
	}
	
	else if(ptCtrlCar->bDrection==BACK)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);  //dir	
		//GPIO_ResetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
	  TIM_SetCompare2(TIM1,0);           // ͨ��2 pwm Ϊ 0% ���� ��Ϊ�͵�ƽ
		//TIM1->CCR2 = ptCtrlCar->0;       // ͨ��2 pwm
		
	}	
}		


// ���� PWM�ͷ���ֵ
void SetPwmDir_new(ptCtrl ptCtrlCar)
{
	// pwm �޷� ���
	if(ptCtrlCar->u16PWM>MAX_PWM)
	{
		ptCtrlCar->u16PWM=MAX_PWM;
	}
	else if(ptCtrlCar->u16PWM<=MIN_PWM)
	{
		ptCtrlCar->u16PWM=MIN_PWM;
	}
	
	//TIM_SetCompare3(TIM1,(ptCtrlCar->u16PWM)/2);//AD�����ж�
	//TIM_SetCompare3(TIM1,ptCtrlCar->u16PWM);
	//TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
	//TIM1->CCR1 = ptCtrlCar->u16PWM;       // ͨ��1 pwm
	
	// //PA.9 ���Ʒ��� �� ǰ��  �� ����
	if(ptCtrlCar->bDrection==FOWARD)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);  //dir	
		//GPIO_SetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
		
	 /*�������෴�� �� �� if  �� else if ���ݻ��� */
		TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
		TIM_SetCompare2(TIM1,0);           // ͨ��2 pwm Ϊ  0% ���� ��Ϊ�͵�ƽ
	}
	else if(ptCtrlCar->bDrection==BACK)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);  //dir	
		//GPIO_ResetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
		
	 /*�������෴�� �� �� if  �� else if ���ݻ��� */
		TIM_SetCompare1(TIM1,0);                // ͨ��1 pwm Ϊ 0% ���� ��Ϊ�͵�ƽ
	  TIM_SetCompare2(TIM1,ptCtrlCar->u16PWM); 
	}	
}	


void PID_Control()
{
	
}





