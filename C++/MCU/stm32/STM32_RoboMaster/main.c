#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "can.h" 
#include "timer.h" 
#include "control.h"
 
/************************************************

���������  can ���� 0X201 ~0x208 ��� ���ת��ת����Ϣ  �ٶ� ������Ϣ
                ���� firefly 0x01 ������ �ٶ��趨ָ��   
								pid ���ƺ��� can��Ϣ 0x200 ��������
************************************************/
u8 debug_pid  =1;// Ϊ1��������  Ϊ0 ���ڵ��� pid

float encoder_fliter=0;
float encoder_fliter2=0;

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
	ptForkLift->s32EncoderCounter = 0;
	ptForkLift->bCanSendCounter = 0;
	ptForkLift->u8Can_Msg_Send_flag=0;//can��Ϣ���ͱ�־
	delay_ms(1000);
	
	TIM3_Int_Init();   

	printf("\r\nSystem Running\r\n");
	
 	while(1)
	{	
		
if(ptForkLift->u16RunRecvPeriodMotor==10)
	{			
		//				printf("u16RunRecvPeriodMotor %d\r\n",ptForkLift->u16RunRecvPeriodMotor );
		if(debug_pid==1)//���� pid����ʱΪ0  �������и�Ϊ1
		{	
      ///*			
			if((ptForkLift->bCanComBox == CanBoxPend)||(ptForkLift2->bCanComBox == CanBoxPend))//����״̬
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

		///*		
			else if((ptForkLift->bCanComBox == CanBoxPost)&&(ptForkLift2->bCanComBox == CanBoxPost))//����״̬ 
			{		
			 ptForkLift->s16speedwant = 000;
			 ptForkLift2->s16speedwant = 000;	
			}
			//*/												
    }	
	}		
		if(debug_pid)//���� pid����ʱΪ0  �������и�Ϊ1
		 {
			 /*
			if(ptForkLift->u8Can_Msg_Send_flag == 1)// 50ms����һ�� ����������  ���� ������ WHILE(1)��
					{
					
						ptForkLift->u8Can_Msg_Send_flag = 0;
						ptForkLift->bCanSendCounter = ptForkLift->bCanSendCounter + 1;

						ptForkLift->Can_Txmessage.Data[0] = (ptForkLift->s32EncoderCounter)       & 0xff;
						ptForkLift->Can_Txmessage.Data[1] = ((ptForkLift->s32EncoderCounter)>>8)  & 0xff;
						ptForkLift->Can_Txmessage.Data[2] = ((ptForkLift->s32EncoderCounter)>>16) & 0xff;
						ptForkLift->Can_Txmessage.Data[3] = ((ptForkLift->s32EncoderCounter)>>24) & 0xff;
						ptForkLift->Can_Txmessage.Data[4] = (u8)(ptForkLift->bCanSendCounter)     & 0xff;
						//ptForkLift->Can_Txmessage.Data[5] = ( ptForkLift->u16I_Value)             & 0xff;
						//ptForkLift->Can_Txmessage.Data[6] = ((ptForkLift->u16I_Value)>>8)         & 0xff;			
						ptForkLift->s32EncoderCounter = 0;			
						Can_Send_Msg(); //����: �ŵ���������			
					}
			 */
		 }				
 }		 
}


