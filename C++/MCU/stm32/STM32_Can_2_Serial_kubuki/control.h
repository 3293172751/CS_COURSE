#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

#define MAX_Carrent_out    15000   // -15000 ~15000 ��Ӧ�������ֵ  -18A ~ 18A
#define CanBoxPost         1
#define CanBoxPend         0


typedef struct TControl//�������صĲ���ȫ���ڴ˽ṹ����
{

	u8		   u8CarID;    //  

	s32      s32EncoderCounter_l; //���� ��������ֵ
	s32      s32EncoderCounter_r; //���� ��������ֵ
	s32      s32EncoderCounter_a; //��   ��������ֵ

	u16      u16ADC_Carrent_l;//���� �������ֵ
	u16      u16ADC_Carrent_r;//���� �������ֵ
	u16      u16ADC_Carrent_a;//��   �������ֵ
	u16      u16EncoderSum_l;
	u16      u16EncoderSum_r;
	
	//���ֶ�����ĳ������ٶ�
	float     s16x;           // +- 1.0~1.0   m/s
	float     s16y;           // +- 1.0~1.0   m/s
	float     s16w;           // +- 0.5~0.5   rad/s
	
//���ֶ�����ı��ٶ�
	float     s16arm_x;       // +- 0.1~0.1   m/s
	
//*************CAN�������*********************//	
	u8       bCanComBox;        // can ״̬��ʶ
  u8			 bCanSendCounter;  //ÿ����һ���������ֵ++
	u16	     u16CanComCount[2]; //u16CanComCount[0] can �ж� ������Ϣ����
	u16      u16CanComPeriod;   //��¼ϵͳʱ��1ms�жϴ���  1~500 ÿ100ms  500ms �ж�һ��  ���� can �жϼ��� ״̬ 
	
	u16	     u16RunRecvPeriodMotor;
	u16	     u16RunSendPeriodMotor;
	CanTxMsg Can_Txmessage;
	u8		   u8Can_Msg_Send_flag;
	
  u8		   u8Serial_Msg_Send_flag_Left;
  u8		   u8Serial_Msg_Send_flag_Right;
	u8		   u8Serial_Msg_Send_flag_Arm;

}TCtrl,*ptCtrl;


extern TCtrl TForkLift;     //����泵�ṹ��
extern ptCtrl ptForkLift;   //����泵�ṹ��ָ��

void uart_init(u32 bound);

#endif
