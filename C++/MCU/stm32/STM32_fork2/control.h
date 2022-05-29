#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

#define MAX_PWM           1700 //1800*10*2-1// 1800*2-1 //1700   //PWM1500�ٶȶ�Ӧ490������
#define MIN_PWM            0
#define FOWARD             0
#define BACK               1
#define CanBoxPost         1
#define CanBoxPend         0
#define MotorPeriodIn      1
#define MotorPeriodOut     0

#define QUEUE_LENGTH 15

#define BOARD_version 0   // 1 �°�   0 �ϰ�   ��ȷ�� PID��� ��ʽ



typedef struct TControl//�������صĲ���ȫ���ڴ˽ṹ����
{
	u16      u16PWM;
	u8       bDrection;
	u8		   u8CarID;    //  �ϰ���Ϊ 0 �Ҳ�  �°���Ϊ 1 ���
	
	u16     u16RunPIDControl;

	float	 s16error[3];
	float	 s16ErrorSum;
	float     s16speedwant;
	float     s16speedout;
	
	float     s16speed_p;
	float     s16speed_i;
	float     s16speed_d;
	
	s16      s16EncoderSpeed;
	u16      u32ADCResult;
	s32      s32EncoderCounter; //�������������ۼ�ֵ

//*************CAN�������*********************//	
	u8       bCanComBox;        // can ״̬��ʶ
  u8				bCanSendCounter;  //ÿ����һ���������ֵ++
	u16	     u16CanComCount[2]; //u16CanComCount[0] can �ж� ������Ϣ����
	u16      u16CanComPeriod;   //��¼ϵͳʱ��1ms�жϴ���  1~500 ÿ100ms  500ms �ж�һ��  ���� can �жϼ��� ״̬ 
	u16	     u16RunDetADC;      //�����������ʱ�����
	
	u16	     u16RunRecvPeriodMotor;
	u16	     u16RunSendPeriodMotor;
	CanTxMsg Can_Txmessage;
	u16	     u16time_ms;

//*************CAN�������*********************//	
	
	u16      u16MotorControlFlag;
	
	/***************wyw 2017.6.20 ���� ���ĸ�ߵ�������*****************/
	// wyw ���  DMA ��ADC1 6 PA6 �ڵ� ADC����ֵ
float ADC_Value;
u16   u16ADC_Value;

float I_Value;
u16   u16I_Value;

//u8  ADC_convert_start;

//�泵�������

///*	
//���ֶ�����Ĳ泵�����ٶ�
	float     s16w;           //0~255 -> 0~3.14rad/s
	float     s16x;           //0~255 -> 0~0.8m/s           
	float     s16y;           //0~255 -> 0~0.8m/s	
//���²� ǰ����ٶ�
	float     s16fb;          //0~255 -> 0~0.32m/s
	float     s16ud;          //0~255 -> 0~0.32m/s

 //�泵 �������
	float     s16dpl_d;          //��������ֱ��
	float     s16dp_d;           //����ֱ��
	float     s16ud_d;           //���²�����ֱ��
	float     s16fb_d;           //ǰ�������ֱ��
	
	u16      u32dpl_econder;      //�����ֱ���������
	u16      u32dpl_econder_reduc;//�����ֱ��������ٱ�
	u16      u32ud_econder;       //������ ����������
	u16      u32ud_econder_reduc; //������ ���������ٱ�
	u16      u32fb_econder;       //ǰ���ֱ���������
	u16      u32fb_econder_reduc; //ǰ���ֱ��������ٱ�
//*/	



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




extern TCtrl TForkLift;     //����泵�ṹ��
extern ptCtrl ptForkLift;   //����泵�ṹ��ָ��
extern linkqueue  middle_filter_queue;
extern linkqueue* ptmiddle_filter_queue;

void SetPwmDir(ptCtrl ptCtrlCar);
void SetPwmDir_new(ptCtrl ptCtrlCar);
void POSITION_PID(ptCtrl ptCtrlCar,int EncoderSpeed);
void ZL_PID(ptCtrl ptCtrlCar,int EncoderSpeed);
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
