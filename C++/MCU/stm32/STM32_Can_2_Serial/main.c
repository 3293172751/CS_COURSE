#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "can.h" 
#include "timer.h" 
#include "control.h"
#include <string.h>//memset

/************************************************

���������  can ����
            ������λ�� �����ٶȿ���ָ���
						ͨ��can�·��� �����can���������
						ͬʱ���� ����can���������� can��Ϣ(������������Ϣ �� ���������Ϣ �� )
						ͨ�������ϴ�����λ��
************************************************/
u8 debug_pid  =1;// Ϊ1��������  Ϊ0 ���ڵ��� pid


// �����յ����ַ�
char chr;
// ������λ���յ�������
char cmd;
// ��λ�����͵� �ַ���� ��������
char argv1[32];
char argv2[32];
char argv3[32];
// �������������� ����
float arg1=0;
float arg2=0;
float arg3=0;

int arg = 0;
int index_ = 0;

unsigned int format_vel=0;

#define VELOCITY      'v'
//#define LEFT_Motor_Encoder  'l'
//#define Right_Motor_Encoder 'r'
//#define Aam_Motor_Encoder   'a'

/*  ��� ������� */
void resetCommand() {
  cmd = NULL;
  memset(argv1, 0, sizeof(argv1));
  memset(argv2, 0, sizeof(argv2));
  memset(argv3, 0, sizeof(argv3));
  arg1 = 0;
  arg2 = 0;
  arg3 = 0;
  arg = 0;
  index_ = 0;
}
/* ����ָ��*/
void runCommand() {
// �ַ���ת���� stdlin.h  atof �������� atoi ������    atol������
  arg1 = atof(argv1);
  arg2 = atof(argv2);
  arg3 = atof(argv3);
// ���� v  
switch(cmd) {
  case VELOCITY:    // ������λ���� �ٶ�ָ��  v X Y w
		 ptForkLift->s16x=arg1;
	   ptForkLift->s16y=arg2;
	   ptForkLift->s16w=arg3;
	   ptForkLift->u8Can_Msg_Send_flag=1;//��λ  can�·���־
	  // printf("\r\nv %.3f\r\n",ptForkLift->s16x);
    break;
    
  default:
    printf("Inv Cmd");
    break;
  }
}

 int main(void)
 {	 

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ�(0~3)��2λ��Ӧ���ȼ�(0~3)
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200  ����λ��ͬ��
//printf("\r\nSystem init OK\r\n");
	delay_ms(500);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
  LED_Init();	
	 
	ptForkLift=&TForkLift; // can�����ж��� �в�����ֵ ��Ҫ�ȳ�ʼ������Ȼ�Ῠ��
	delay_ms(500);

	 
  if(debug_pid==1)
		
	// 360000/(1+4+7)/12=250Kbps
  CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,12,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������250Kbps    
  
	// 360000/(1+5+2)/9=500Kbps
	// CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_2tq,9,CAN_Mode_Normal);
	
	// 360000/(1+2+1)/9=1000Kbps=1Mbps
	//CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_2tq,CAN_BS1_1tq,9,CAN_Mode_Normal);
  	
	delay_ms(500);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
	
		 

	CarID_Select();
//	printf("CarID:%d\r\n",ptForkLift->u8CarID);


  if(ptForkLift->u8CarID==0)
		ptForkLift->Can_Txmessage.StdId=0x01;   //  can_2_serial  �䵱can����
	
  ptForkLift->Can_Txmessage.ExtId=0x12;				    // ������չ��ʾ�� 
  ptForkLift->Can_Txmessage.IDE=CAN_Id_Standard;  // ��׼֡
  ptForkLift->Can_Txmessage.RTR=CAN_RTR_Data;		  // ����֡
  ptForkLift->Can_Txmessage.DLC=8;				        // Ҫ���͵����ݳ���   
	
//��ʼ��	
	ptForkLift->s32EncoderCounter_l = 0;//������
	ptForkLift->s32EncoderCounter_r = 0;//
	ptForkLift->s32EncoderCounter_a = 0;
	ptForkLift->u16ADC_Carrent_l=0;     //����
	ptForkLift->u16ADC_Carrent_r=0;
	ptForkLift->u16ADC_Carrent_a=0;
	
	ptForkLift->bCanSendCounter = 0;
	ptForkLift->u8Can_Msg_Send_flag=0;//can��Ϣ���ͱ�־
  ptForkLift->u8Serial_Msg_Send_flag_Left=0; //������Ϣ���ͱ�־ ����
	ptForkLift->u8Serial_Msg_Send_flag_Right=0; //������Ϣ���ͱ�־ ����
	ptForkLift->u8Serial_Msg_Send_flag_Arm=0; //������Ϣ���ͱ�־  ��
	
	delay_ms(1000);
	
	TIM3_Int_Init();   

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//ʹ�ܴ����ж�
	
//	printf("\r\nSystem Running\r\n");
	
 	while(1)
	{	
		
if(ptForkLift->u16RunRecvPeriodMotor==10)
	{			
		//				printf("u16RunRecvPeriodMotor %d\r\n",ptForkLift->u16RunRecvPeriodMotor );
		if(debug_pid==1)//���� pid����ʱΪ0  �������и�Ϊ1
		{	
      ///*			
			if(ptForkLift->bCanComBox == CanBoxPend)//����״̬
			{
			}
     //*/	
		/*		
			else if(ptForkLift->bCanComBox == CanBoxPost)//����״̬ 
			{		}*/												
    }	
	}		
if(debug_pid)//���� pid����ʱΪ0  �������и�Ϊ1
		 {
			 
// can�·� ��������			 
			if(ptForkLift->u8Can_Msg_Send_flag == 1)//  
					{
						ptForkLift->u8Can_Msg_Send_flag = 0;
						ptForkLift->Can_Txmessage.Data[6] =	0x00;
						//2m/s�ȷ�256  x���ٶ�
						if(ptForkLift->s16x>0)
						  {
                 ptForkLift->Can_Txmessage.Data[6] |=		0x01<<4;	// 1��ʾ�� 0 ��ʾ��
								 format_vel = (unsigned int)(ptForkLift->s16x*127.5);//  255/0.8=318.75   255/2.00=127.5
						     ptForkLift->Can_Txmessage.Data[1] = (format_vel>=256)?255:format_vel;
							}
							else{
								 format_vel = (unsigned int)(-ptForkLift->s16x*127.5);
						     ptForkLift->Can_Txmessage.Data[1] = (format_vel>=256)?255:format_vel;
							}
						//3.14 rad/s �ȷ� 256	 ��ת w�ٶ�
						if(ptForkLift->s16w>0)
						  {
                 ptForkLift->Can_Txmessage.Data[6] |=		0x01<<3;
								 format_vel = (unsigned int)(ptForkLift->s16w*81.21); // 255/3.14=81.21
						     ptForkLift->Can_Txmessage.Data[0] = (format_vel>=256)?255:format_vel;
						  }
							else{
								 format_vel = (unsigned int)(-ptForkLift->s16w*81.21);// 255/3.14=81.21
						     ptForkLift->Can_Txmessage.Data[0] = (format_vel>=256)?255:format_vel;								
							}

					 	 ptForkLift->Can_Txmessage.Data[2] = 0x00;
					   ptForkLift->Can_Txmessage.Data[3] = 0x00;
						 ptForkLift->Can_Txmessage.Data[4] = 0x00;
						 ptForkLift->Can_Txmessage.Data[5] = 0x00;
						 ptForkLift->Can_Txmessage.Data[7] = 0x00;					
						 Can_Send_Msg(); 
          //   printf("\r\nw %d\r\n",ptForkLift->Can_Txmessage.Data[0]);
					// printf("\r\nx %d\r\n",ptForkLift->Can_Txmessage.Data[1]);
							
					}
			
// �����ϴ� ���� ���� �� �ı�������Ϣ�� ����ֵ	
///*					
					if(ptForkLift->u8Serial_Msg_Send_flag_Left==1){
						ptForkLift->u8Serial_Msg_Send_flag_Left=0;
						printf("l %d %d\r",ptForkLift->s32EncoderCounter_l, ptForkLift->u16ADC_Carrent_l);//����
					}
					if(ptForkLift->u8Serial_Msg_Send_flag_Right==1){
						ptForkLift->u8Serial_Msg_Send_flag_Right=0;
						printf("r %d %d\r",ptForkLift->s32EncoderCounter_r, ptForkLift->u16ADC_Carrent_r);//����
					}
					if(ptForkLift->u8Serial_Msg_Send_flag_Arm==1){
						ptForkLift->u8Serial_Msg_Send_flag_Arm=0;
						printf("a %d %d\r",ptForkLift->s32EncoderCounter_a, ptForkLift->u16ADC_Carrent_a);//��
					}
//					*/
					
		 }
	 
 }		 
}
 
// ���ڽ����ж�
void USART1_IRQHandler(void)
  { 
  if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET) 
  chr = USART_ReceiveData(USART1); 
  // printf("%c",chr);
	
	    // ���յ� ���������
    if (chr == 13) { // CR ASCII�� 13  �س�'/r'
      if (arg == 1)      argv1[index_] = NULL;// �ڶ������� ����±��λ������Ϊ \0 ��
      else if (arg == 2) argv2[index_] = NULL;// �������������
      else if (arg == 3) argv3[index_] = NULL;// ���ĸ��������
      runCommand(); // ��������
      resetCommand();
    }
    
    //���յ��ָ������в����� ' '�ո�
    else if (chr == ' ') {     //�����ո��ˣ��µĲ�����ʼ��
      if (arg == 0)    arg = 1;//�ڶ����ַ���ʼ
      else if (arg == 1)  {
        argv1[index_] = NULL;   //argv1 ����±괦��ֵ����
        arg = 2;                //�л�����һ���ַ�
        index_ = 0;             //�±����
      }
      else if (arg == 2)  {     
        argv2[index_] = NULL;   //argv2 ����±괦��ֵ����
        arg = 3;                //�л�����һ���ַ�
        index_ = 0;             //�±����
      }
       // continue; ���������� arg = 3�����
    }
    
 // �����ַ���
    else {
      if (arg == 0) {
        // ��һ��Ϊcmd �����ʶ�� (һ���ֽ�)
        cmd = chr;
      }
      else if (arg == 1) {
        // �ڶ�������
        argv1[index_] = chr;//�����ַ�
        index_++;
      }
      else if (arg == 2) {//���������� �����ַ�
        argv2[index_] = chr;
        index_++;
      }
      else if (arg == 3) {// ���ĸ�����
        argv3[index_] = chr;
        index_++;
      }
    }
		
}
