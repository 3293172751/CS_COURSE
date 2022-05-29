#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

#define MAX_PWM            1700   //PWM1500�ٶȶ�Ӧ490������
#define MIN_PWM            0
#define FOWARD             0
#define BACK               1
#define CanBoxPost         1
#define CanBoxPend         0
#define MotorPeriodIn      1
#define MotorPeriodOut     0

typedef struct TControl//�������صĲ���ȫ���ڴ˽ṹ����
{
	u16      u16PWM;     // ���PWMֵ
	u8       bDrection;  // ����
	u8		   u8CarID;    // ������
	
	s16      s16EncoderSpeed;// ���������ٶ�
	u16      u32ADCResult;   // ���ĸ�߲��õ���  ad����ֵ


//*************CAN�������*********************//	
	u8       bCanComBox;           // can ״̬��ʶ
	u16	     u16CanComCount[2];    //u16CanComCount[0] can �ж� ������Ϣ���� 
	u16      u16CanComPeriod;      //��¼ϵͳʱ��1ms�жϴ���  1~500 ÿ100ms  500ms �ж�һ��  ���� can �жϼ��� ״̬   
	
	u16	     u16RunRecvPeriodMotor;//��¼ϵͳʱ��1ms�жϴ��� 1~10 ÿʮ�μ��  ÿ10msִ��һ��
	u16	     u16RunSendPeriodMotor;//��¼ϵͳʱ��1ms�жϴ��� 1~50 ÿʮ�μ��  ÿ50msִ��һ��  ���� can ״̬
	u16	     u16RunDetADC;
	u16      u16RunPwmChange;
	
	CanTxMsg Can_Txmessage;        //can����������Ϣ����   ���� can״̬  Can_Txmessage.Data[0]  0~255   255 ������
  
	u16	     u16time_ms;           //

//*************CAN�������*********************//	
	
	u16      u16MotorControlFlag;
	
//volatile uint16_t ADC1ConvertedValue;//[5];
float ADC_Value;
u16   u16ADC_Value;

float I_Value;
u16   u16I_Value;

u8  ADC_convert_start; 
u8  PWM_change_start;
}TCtrl,*ptCtrl;

/*
class Ctrl1
{
	public:
  
    
	protected:
  
};
*/



// ȫ�ֱ���
extern TCtrl TForkLift;     //����泵�ṹ��
extern ptCtrl ptForkLift;   //����泵�ṹ��ָ��



void SetPwmDir(ptCtrl ptCtrlCar);
void SetPwmDir_new(ptCtrl ptCtrlCar);
void uart_init(u32 bound);


#endif
