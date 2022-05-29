#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

#define MAX_Carrent_out    15000   // -15000 ~15000 ��Ӧ�������ֵ  -18A ~ 18A    -16384~16384
#define CanBoxPost         1
#define CanBoxPend         0


typedef struct TControl//�������صĲ���ȫ���ڴ˽ṹ����
{
//	u16      u16PWM;    //�˴�����  pwm��ֵ  �ǵ��ת�ӵ���ֵ  ����ֵԽ��ת��Խ�� -15000~15000 ��Ӧ -18A~18A   -16384~16384
//	u8       bDrection;
	u8		   u8CarID;    //  
	
	u16     u16RunPIDControl;

	float	 s16error[3];
	float	 s16ErrorSum;
	float     s16speedwant;// ���ת���ٶ� -500*19.2  ~ 500*19.2 rpm
	float     s16speedout; // ���������� -15000 ~ 15000
  u16     u16ErrorDeadBand;//������
	
	float     s16speed_p;
	float     s16speed_i;
	float     s16speed_d;
	int       pid_time;//ms��

	s32      s32EncoderCounter; //�������������ۼ�ֵ

	
	// ��ˢ��� ����
u16 angle;        //ת�ӽǶ�  0~8191  >>>  0~360
u16 last_angle;

int speed_rpm;    //����ֵ ת��ת��  rpm ת/����   -10000 ~ 10000 ת��ת��   ��ת�� /19.2  ���ٱ� 3951/:187 =19.2  
int last_speed_rpm;

int real_current;//

float tempature;

	
//*************CAN�������*********************//	
	u8       bCanComBox;        // can ״̬��ʶ
  u8			 bCanSendCounter;  //ÿ����һ���������ֵ++
	u16	     u16CanComCount[2]; //u16CanComCount[0] can �ж� ������Ϣ����
	u16      u16CanComPeriod;   //��¼ϵͳʱ��1ms�жϴ���  1~500 ÿ100ms  500ms �ж�һ��  ���� can �жϼ��� ״̬ 
//u16	     u16RunDetADC;      //�����������ʱ�����
	
	u16	     u16RunRecvPeriodMotor;
	u16	     u16RunSendPeriodMotor;
	CanTxMsg Can_Txmessage;
	u16	     u16time_ms;
	u8		   u8Can_Msg_Send_flag;

//*************CAN�������*********************//	
	
	u16      u16MotorControlFlag;
	

}TCtrl,*ptCtrl;


typedef struct Filter_Queue_node{
	
	s16 date;
	struct Filter_Queue_node* next;
	
}Filter_Queue, *queue_ptr;
  

typedef struct Linkqueue{

	Filter_Queue*  front;//ǰָ��
	Filter_Queue*  rear;//��ָ��
	
}linkqueue;

/*
class Ctrl1
{
	public:
  
    
	protected:
  
};
*/



//extern TCtrl TForkLift;     //����泵�ṹ��
extern TCtrl TForkLift[2];     //����泵�ṹ�� ����
extern ptCtrl ptForkLift;   //����泵�ṹ��ָ��
extern ptCtrl ptForkLift2;   //����泵�ṹ��ָ��
//extern TCtrl RoboMaster[2];     //����泵�ṹ������

void SetPwmDir(ptCtrl ptCtrlCar);
void SetPwmDir_new(ptCtrl ptCtrlCar);
float POSITION_PID(ptCtrl ptCtrlCar,int EncoderSpeed);
float ZL_PID(ptCtrl ptCtrlCar,int EncoderSpeed);
void uart_init(u32 bound);
void set_moto_current( s16 iq1, s16 iq2, s16 iq3, s16 iq4);

#endif
