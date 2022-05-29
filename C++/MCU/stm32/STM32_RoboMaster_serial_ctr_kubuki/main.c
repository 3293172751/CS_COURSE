#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "can.h" 
#include "timer.h" 
#include "control.h"
#include <string.h>//ʹ��memset();
#include "kubuki.h"
/************************************************

���������  can ���� 0X201 ~0x208 ��� ���ת��ת����Ϣ  �ٶ� ������Ϣ
                ���� firefly 0x01 ������ �ٶ��趨ָ��   
								pid ���ƺ��� can��Ϣ 0x200 ��������
************************************************/
u8 debug_pid  =1;// Ϊ1��������  Ϊ0 ���ڵ��� pid

#define KUBUKI

float encoder_fliter=0;//�������˲�
float encoder_fliter2=0;

// �����յ����ַ�
//char chr;
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
//int index_ = 0;

unsigned int format_vel=0;

#define VELOCITY      'v'

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
	   ptForkLift->u8Cmd_receive_lag=1;//��λ  ������ձ�־
	   //ptForkLift->u8Can_speed_Msg_Send_flag=1;//��λ
   	 ptForkLift->u16SerialComCount[0]++;//���ڽ��ռ���
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
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	printf("\r\nSystem init OK\r\n");
	 
	ptForkLift=&TForkLift[0]; //���� �ṹ�� ָ��
	ptForkLift2=&TForkLift[1];//���� �ṹ�� ָ��
	 
	delay_ms(1000);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
  LED_Init();	
  if(debug_pid==1)
		
	// 360000/(1+4+7)/12=250Kbps
 // CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,12,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������250Kbps    
  
	// 360000/(1+4+7)/6=1000Kbps=1Mbps
  // CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,6,CAN_Mode_Normal);
	
	// 360000/(1+4+7)/3=1000Kbps=1Mbps
  	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_4tq,CAN_BS1_7tq,3,CAN_Mode_Normal);
	
	delay_ms(1000);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
	
	kubuki_init();//kubukiͨ�Žӿ�  ������ʼ�� 

	CarID_Select();
	printf("CarID:%d\r\n",ptForkLift->u8CarID);


  if(ptForkLift->u8CarID==0)//����
		ptForkLift->Can_Txmessage.StdId=0x200;   // ��׼��ʶ��  0x200���� 201~204 ���  0x1FF����205~208���
 
//	else if(ptForkLift->u8CarID==3)
//		ptForkLift->Can_Txmessage.StdId=0x203;   //  ��� 201~208

  ptForkLift->Can_Txmessage.ExtId=0x12;				    // ������չ��ʾ�� 
  ptForkLift->Can_Txmessage.IDE=CAN_Id_Standard;  // ��׼֡
  ptForkLift->Can_Txmessage.RTR=CAN_RTR_Data;		  // ����֡
  ptForkLift->Can_Txmessage.DLC=8;				        // Ҫ���͵����ݳ���   
	
	
// ��M3508���
	//����pid����
	ptForkLift->pid_time=4.0;//  �������ϴ� �ٶȷ���ֵΪ 250KHz  ��4ms����һ��    1.5  0.1
	ptForkLift->s16speedwant = 0;  //�ٶȷ�Χ  -500*19.2 ~ 500*19.2   //600 һ����30Ȧ  2�� һȦ
	ptForkLift->s16speed_p = 1.8;
	ptForkLift->s16speed_i = 0.03;
	ptForkLift->s16speed_d = 0.03;
	ptForkLift->u16ErrorDeadBand=10;
	
	//����pid����
	ptForkLift2->s16speedwant = 0;  //�ٶȷ�Χ  -500*19.2 ~ 500*19.2   //600 һ����30Ȧ  2�� һȦ
	ptForkLift2->s16speed_p = 1.8;
	ptForkLift2->s16speed_i = 0.03;
	ptForkLift2->s16speed_d = 0.03;
  ptForkLift2->u16ErrorDeadBand=10;
	
//�泵�������
/*
ptForkLift->s16fb_d=0.032; //ǰ�������ֱ�� 0.032m    3.2cm
ptForkLift->s16ud_d=0.032; //���²�����ֱ��
ptForkLift->s16dp_d=0.485; //����ֱ��      //0.485m   48.5cm
ptForkLift->s16dpl_d=0.125;//��������ֱ��   //0.125m   12.5cm

ptForkLift->u32dpl_econder=500;//����������
ptForkLift->u32dpl_econder_reduc=55;//�����ּ��ٱ�

ptForkLift->u32ud_econder=500;//����������
ptForkLift->u32ud_econder_reduc=55;//�����ּ��ٱ�

ptForkLift->u32fb_econder=500;//ǰ��������
ptForkLift->u32fb_econder_reduc=160;//ǰ���ּ��ٱ�
*/

//��ʼ��
	ptForkLift->u16RunPIDControl = 0;
	ptForkLift->bCanSendCounter = 0;
	ptForkLift->u16CanComCount[1]=ptForkLift->u16CanComCount[0]=0;
	ptForkLift2->u16CanComCount[1]=ptForkLift2->u16CanComCount[0]=0;
	ptForkLift->u16SerialComCount[1]=ptForkLift->u16SerialComCount[0]=0;
	//ptForkLift2->u16SerialComCount[1]=ptForkLift2->u16SerialComCount[0]=0;
	
	delay_ms(1000);
	
	TIM3_Int_Init();   

	printf("\r\nSystem Running\r\n");
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//ʹ�ܴ����ж�
	
	
 	while(1)
	{	
		
if(ptForkLift->u16RunRecvPeriodMotor>=10)
	{			
		//				printf("u16RunRecvPeriodMotor %d\r\n",ptForkLift->u16RunRecvPeriodMotor );
		if(debug_pid==1)//���� pid����ʱΪ0  �������и�Ϊ1
		{	
      ///*			
			if((ptForkLift->bCanComBox == CanBoxPend)||(ptForkLift2->bCanComBox == CanBoxPend)||(!(ptForkLift->u8SerialCmdFlag)))//����״̬
			{
				ptForkLift->s16speedwant = 0;
				ptForkLift->s16error[0] = 0;
				ptForkLift->s16error[1] = 0;
				ptForkLift->s16error[2] = 0;
				ptForkLift->s16ErrorSum = 0;
				
				ptForkLift2->s16speedwant = 0;
				ptForkLift2->s16error[0] = 0;
				ptForkLift2->s16error[1] = 0;
				ptForkLift2->s16error[2] = 0;
				ptForkLift2->s16ErrorSum = 0;
			}
     //*/	
	
			else if((ptForkLift->bCanComBox == CanBoxPost)&&(ptForkLift2->bCanComBox == CanBoxPost))//����״̬ 
			{		
			// ptForkLift->s16speedwant = 000;
			// ptForkLift2->s16speedwant = 000;	
			}				
    }	
	}		
		if(debug_pid)//���� pid����ʱΪ0  �������и�Ϊ1
		 {
			 	if((ptForkLift->u8Cmd_receive_lag == 1)&&(ptForkLift->u8SerialCmdFlag))// ���յ�����ȷ�� ����  �Ҵ��� ������������  �����ٶ��趨ֵ
					{
								ptForkLift->u8Cmd_receive_lag = 0;
								//ptForkLift->s16speedwant  = ((ptForkLift->s16x + ptForkLift->s16w*0.2)*60/3.14/0.1*19.1); //���̰뾶 0.2m ����ֱ�� 0.1m�յ���Ϊ 0.1m/s
								ptForkLift->s16speedwant  = ((ptForkLift->s16x - ptForkLift->s16w*disance_m/2.0)*3650);//���� 201  ���ٱ�19.1 ��ʱ����ת ������
								ptForkLift2->s16speedwant = ((-ptForkLift->s16x - ptForkLift->s16w*disance_m/2.0)*3650);//���� 202  ��ʱ����ת ������
					}
				
// ����kubuki��ʽ  �����ϴ���Ϣ					
 
				 if ( (ptForkLift->u16RunSendPeriodMotor & 0x003F) == 0 )
						sendBasicData();

					if (retController == true) {
						retController = false;
						sendControllerData();
						retController = false;
					}
					else if (retExtData == true) {
						sendExtData();
						retExtData = false;
					}
					
		 }				
 }		 
}

// ���ڽ����ж�
void USART1_IRQHandler(void)
  { 
		
  if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET) 
  chr = USART_ReceiveData(USART1); 
  // printf("%c",chr);
	
	
#ifndef KUBUKI	
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
		
// kubuki ͨ�Ÿ�ʽ
#else
		
   tempBuf[tempIndex] = chr;
	 tempIndex ++;
	 if (tempIndex>10)  tempIndex = 0;//����
   if (update(chr))
   {
	  //kubuki ͨ�Žӿڱ�ʶ�� identifier
	  identifier = buffer[3];

	  switch (identifier)
	  	{
	  	case 0x01: //Base Control
			motor_speed = buffer[5] | ( (u16)(buffer[6])<<8);
			motor_radius = buffer[7] |( (u16)(buffer[8])<<8); 
			runCommand_Motor(motor_speed,motor_radius);
			break;
		case 0x0e: //getController
			retController = true;
			break;
		case 0x09: //get ext data
			retExtData = true;
			extDataFlag = buffer[5];
			break;
		default:
			break;
	  	}  
   }
	 
#endif		
		
		
}
