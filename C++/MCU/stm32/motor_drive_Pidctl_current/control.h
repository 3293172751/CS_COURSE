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


extern	u8 board_type;  // ȫ�ֱ��� ���� �����ط�ʹ����Ҫ���� u8 board_type  1 �°�   0 �ϰ�   ��ȷ�� PID��� ��ʽ

typedef struct TControl//�������صĲ���ȫ���ڴ˽ṹ����
{
	u16      u16PWM;
	u8       bDrection;
	u8		   u8CarID;       //  �ϰ���Ϊ 0 �Ҳ�  �°���Ϊ 1 ���
	
//	u8       u8
	
	u16     u16RunPIDControl;
	float	 s16error[3];
	float	 s16ErrorSum;
	float     s16speedwant;
	float     s16speedout;
	
	float     s16speed_p;
	float     s16speed_i;
	float     s16speed_d;
	
	s16      s16EncoderSpeed;
	s32      s32EncoderCounter;
	
	u16      u32ADCResult;

//*************CAN�������*********************//	
	u8       bCanComBox;
	u16	     u16CanComCount[2];
	u16      u16CanComPeriod;
	
	u16	     u16RunRecvPeriodMotor;
	u16	     u16RunSendPeriodMotor;
	CanTxMsg Can_Txmessage;
  
	u16	     u16time_ms;

//*************CAN�������*********************//	
	
	u16      u16MotorControlFlag;
	
	/***************wyw 2017.6.20 ���� ������ �汾��־*****************/
	u8  u8DriverBoardVersion;  //0Ϊ�ϰ�   1Ϊ �°�
	
	/***************wyw 2017.6.20 ���� ���ĸ�ߵ�������*****************/
	// wyw ���  DMA ��ADC1 6 PA6 �ڵ� ADC����ֵ
//__IO uint16_t ADC1ConvertedValue;
//uint16_t ADC1ConvertedValue;
//uint32_t ADC1ConvertedValue[5];  // vu16
__IO uint16_t ADC1ConvertedValue[5];
float ADC_Value;
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
