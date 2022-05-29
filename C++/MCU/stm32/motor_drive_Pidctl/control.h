#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

#define MAX_PWM            3500   //PWM1500�ٶȶ�Ӧ490������
#define MIN_PWM            0
#define FOWARD             0
#define BACK               1
#define CanBoxPost         1
#define CanBoxPend         0
#define MotorPeriodIn      1
#define MotorPeriodOut     0

#define 	Queue_Lenth      15


typedef struct TControl//�������صĲ���ȫ���ڴ˽ṹ����
{
	u16      u16PWM;     // ����ٶ� pwmֵ
	u8       bDrection;  // �������  0Ϊǰ��  1Ϊ����
	u8		   u8CarID;    // �ɲ�����ȷ���� ������
	//****************PID���Ʋ���**************************//	
	u16	     u16RunPIDControl;//PID���Ʊ�ʶ
	float	 s16error[3];//����������
	float	 s16ErrorSum;//����
	float      s16speedwant;//�����ٶ�
	float      s16speedout; //���������
   // pid ����
	float      s16speed_p;
	float      s16speed_i;
	float      s16speed_d;
	
	s16      s16EncoderSpeed;     //������������ ������ pid ���������ڣ�1ms������
	s32      s32EncoderSpeed_sum; //��λ������������ ������ �� ������  ��תΪ��  ��תΪ��
	u32      u32ADCResult;        //�����������


//*************CAN�������*********************//	
  u8       bCanComBox;           // can ״̬��ʶ
	u16	     u16CanComCount[2];    //u16CanComCount[0] can �ж� ������Ϣ���� 
	u16      u16CanComPeriod;      //��¼ϵͳʱ��1ms�жϴ���  1~500 ÿ100ms  500ms �ж�һ��  ���� can �жϼ��� ״̬   
	
	u16	     u16RunRecvPeriodMotor;//��¼ϵͳʱ��1ms�жϴ��� 1~10 ÿʮ�μ��  ÿ10msִ��һ��
	u16	     u16RunSendPeriodMotor;//��¼ϵͳʱ��1ms�жϴ��� 1~50 ÿʮ�μ��  ÿ50msִ��һ��  ���� can ״̬
	CanTxMsg Can_Txmessage;        //can����������Ϣ����   ���� can״̬  Can_Txmessage.Data[0]  0~255   255 ������
//**** ���� ������ֵ��־ wyw��� ***  
  u8   Encoder_can_Tx_flag;
	
	u16	     u16time_ms;

//*************CAN�������*********************//	
	
	u16      u16MotorControlFlag;
// wyw ���  DMA ��ADC1 6 PA6 �ڵ� ADC����ֵ
__IO uint16_t ADC1ConvertedValue;
	
}TCtrl,*ptCtrl;



//���� �˲�
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




extern TCtrl TForkLift;     //����泵�ṹ��
extern ptCtrl ptForkLift;   //����泵�ṹ��ָ��
extern linkqueue  middle_filter_queue;
extern linkqueue* ptmiddle_filter_queue;

void SetPwmDir(ptCtrl ptCtrlCar);
void SetPwmDir_new(ptCtrl ptCtrlCar);
void POSITION_PID(ptCtrl ptCtrlCar,int EncoderSpeed);
void uart_init(u32 bound);

void create_queue(linkqueue *  Fliter_q);
void en_queue(linkqueue *  Fliter_q, uint16_t data);
int de_queue(linkqueue *  Fliter_q);
void printf_queue(linkqueue *  Fliter_q);
uint16_t get_queue_value(linkqueue *  Fliter_q,int num);
void swap_data(linkqueue *  Fliter_q,int i,int j);
void set_data(linkqueue *  Fliter_q,int i,uint16_t value);
void Quick_Sort(linkqueue *  Fliter_q,int left,int right);
int average_data(linkqueue *  Fliter_q);

void gotoerror(void);
#endif
