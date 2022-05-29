#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "control.h" 
 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//CAN���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/11
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:1~3; CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.��Χ:1~8;
//tbs1:ʱ���1��ʱ�䵥Ԫ.��Χ:1~16;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
//ע�����ϲ����κ�һ����������Ϊ0,�������.
//������=Fpclk1/((tsjw+tbs1+tbs2)*brp);
//mode:0,��ͨģʽ;1,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Normal_Init(1,8,7,5,1);
//������Ϊ:36M/((1+8+7)*5)=450Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{

	  GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
 	  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);		//��ʼ��IO
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��IO
	  
 	//CAN��Ԫ����
 	CAN_InitStructure.CAN_TTCM=DISABLE;						 //��ʱ�䴥��ͨ��ģʽ  //
 	CAN_InitStructure.CAN_ABOM=DISABLE;						 //����Զ����߹���	 //
  	CAN_InitStructure.CAN_AWUM=DISABLE;						 //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)//
  	CAN_InitStructure.CAN_NART=ENABLE;						 	//��ֹ�����Զ����� //
  	CAN_InitStructure.CAN_RFLM=DISABLE;						 //���Ĳ�����,�µĸ��Ǿɵ� // 
  	CAN_InitStructure.CAN_TXFP=DISABLE;						 //���ȼ��ɱ��ı�ʶ������ //
  	CAN_InitStructure.CAN_Mode= mode;	         //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; //
  	//���ò�����
  	CAN_InitStructure.CAN_SJW=tsjw;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;            //��Ƶϵ��(Fdiv)Ϊbrp+1	//
  	CAN_Init(CAN1, &CAN_InitStructure);            // ��ʼ��CAN1 

 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
   	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
 	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0

  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
#if CAN_RX0_INT_ENABLE
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            // �����ȼ�Ϊ2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               // ����can�ж�
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN_RX0_INT_ENABLE	//ʹ��RX0�ж�
//�жϷ�����			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	
//	i++;
//	if(i==100)
//	{
//		LED0=!LED0;//��ʾϵͳ��������	
//		i=0;
//	}
	
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	printf("id:%d",RxMessage.StdId);
	
	if(RxMessage.StdId == 0x01)//firelay ��������Ϣ�� can_stdid  Ϊ 0x01 
	{
//		printf("\r\ncan_receive�t");
//		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\r\n",RxMessage.Data[0],RxMessage.Data[1],RxMessage.Data[2],RxMessage.Data[3],RxMessage.Data[4],RxMessage.Data[5],RxMessage.Data[6],RxMessage.Data[7]);

/////////////���ֳ�
/*		
		if(ptForkLift->u8CarID==0)  //����
		{

			ptForkLift->bDrection = (RxMessage.Data[6] & 0x02)>>1;

			if ((RxMessage.Data[6] & 0x02)>>1)
				ptForkLift->s16speedwant = -(float)((RxMessage.Data[3]<<8)| RxMessage.Data[2])*2.5; //25ms�ڵ������� 
			else
				ptForkLift->s16speedwant = (float)((RxMessage.Data[3]<<8) | RxMessage.Data[2])*2.5;//25ms�ڵ������� 
		}
		
		if(ptForkLift->u8CarID==1) //����
		{
			ptForkLift->bDrection = (RxMessage.Data[6] & 0x01);

//			printf("bDrection:%d",ptForkLift->bDrection);


			if ((RxMessage.Data[6] & 0x01))
				ptForkLift->s16speedwant = -(float)((RxMessage.Data[1]<<8) | RxMessage.Data[0])*2.5; //25ms�ڵ������� 
			else
				ptForkLift->s16speedwant = (float)((RxMessage.Data[1]<<8) | RxMessage.Data[0])*2.5;//25ms�ڵ������� 
			}
*/
///////////////////////////////�泵//////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/*
���в��֣��泵��
��1���������
�ֽ�7��
X X X X 0 0 0 0 //message_type = 0x0
�ֽ�6��������������ƣ�
X X X X X - - -  
��bit7  �泵��ǰ��     1 ��ǰ 0 ���
��bit6  �泵������     1 ���� 0 ���£�
��bit5  �泵X����      1 ��   0 ����
��bit4  �泵Y����      1 ��   0 ����
��bit3  �泵��ת����   1 ��   0 ����
�ֽ�5������
�ֽ�4�����泵��ǰ��
X X X X X X X X //��0.32m/s �ȷ�256�� 
�ֽ�3�����泵���£�
X X X X X X X X //��0.32m/s �ȷ�256�� 
�ֽ�2�����泵X�ٶȣ�
X X X X X X X X //��0.8m/s �ȷ�256��
�ֽ�1 �����泵Y�ٶȣ�
X X X X X X X X //��0.8m/s �ȷ�256��
�ֽ�0 �������ٶȣ�
X X X X X X X X // ��3.14����/s �ȷ�256
�����ĸ�����תһȦ 27500������
����ֱ�� 0.125m   12.5cm
*/
/*
//���ٶ�
if ((RxMessage.Data[6] & 0x08)) // �ֽ�6 ��bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w =-(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//y���ٶ�
if ((RxMessage.Data[6] & 0x10)) // �ֽ�6 ��bit4
  ptForkLift->s16y  =(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16y =-(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
//x���ٶ�
if ((RxMessage.Data[6] & 0x20)) // �ֽ�6 ��bit5
  ptForkLift->s16x  =(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16x =-(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
//���²��ٶ�
if ((RxMessage.Data[6] & 0x40)) // �ֽ�6 ��bit5
  ptForkLift->s16ud =(float)(RxMessage.Data[3]/255.0*0.32); //0~255 -> 0~0.32m/s
else
  ptForkLift->s16ud=-(float)(RxMessage.Data[3]/255.0*0.32); //0~255 -> 0~0.32m/s�
//ǰ����ٶ�
if ((RxMessage.Data[6] & 0x80)) // �ֽ�6 ��bit5
  ptForkLift->s16fb =(float)(RxMessage.Data[4]/255.0*0.32); //0~255 -> 0~0.32m/s
else  ptForkLift->s16fb=-(float)(RxMessage.Data[4]/255.0*0.32); //0~255 -> 0~0.32m/s
*/
//printf("w:%f\r\n",ptForkLift->s16w);
/*
//�泵�������
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
///*

if(ptForkLift->u8CarID==1)  //ǰ��  v1=Vy+w*R
		{
	//		/*
//���ٶ�
if ((RxMessage.Data[6] & 0x08)) // �ֽ�6 ��bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w = -(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//y���ٶ�
if ((RxMessage.Data[6] & 0x10)) // �ֽ�6 ��bit4
  ptForkLift->s16y  =(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16y = -(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
//*/
			
ptForkLift->s16speedwant = (float)((ptForkLift->s16y + ptForkLift->s16w * ptForkLift->s16dp_d*0.5)/(3.14*ptForkLift->s16dpl_d)*ptForkLift->u32dpl_econder*ptForkLift->u32dpl_econder_reduc/1000); //1ms�ڵ������� 

}
		
	
if(ptForkLift->u8CarID==2)  //���� v1=-Vx+w*R
		{
//			/*
//���ٶ�
if ((RxMessage.Data[6] & 0x08)) // �ֽ�6 ��bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w = -(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//x���ٶ�
if ((RxMessage.Data[6] & 0x20)) // �ֽ�6 ��bit5
  ptForkLift->s16x  =(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16x = -(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
//*/
ptForkLift->s16speedwant = (float)((-ptForkLift->s16x + ptForkLift->s16w * ptForkLift->s16dp_d*0.5)/(3.14*ptForkLift->s16dpl_d)*ptForkLift->u32dpl_econder*ptForkLift->u32dpl_econder_reduc/1000); //1ms�ڵ������� 
		}
		
		
if(ptForkLift->u8CarID==3)  //���� v1=-Vy+w*R
		{
//			/*
//���ٶ�
if ((RxMessage.Data[6] & 0x08)) // �ֽ�6 ��bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w = -(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//y���ٶ�
if ((RxMessage.Data[6] & 0x10)) // �ֽ�6 ��bit4
  ptForkLift->s16y  =(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
  ptForkLift->s16y = -(float)(RxMessage.Data[1]/255.0*0.8);  //0~255 -> 0~0.8m/s
//*/
ptForkLift->s16speedwant = (float)((-ptForkLift->s16y + ptForkLift->s16w * ptForkLift->s16dp_d*0.5)/(3.14*ptForkLift->s16dpl_d)*ptForkLift->u32dpl_econder*ptForkLift->u32dpl_econder_reduc/1000); //1ms�ڵ������� 
		}
		
		
if(ptForkLift->u8CarID==4)  //���� v1=Vx+w*R
    {
//			/*
//���ٶ�
if ((RxMessage.Data[6] & 0x08)) // �ֽ�6 ��bit3
  ptForkLift->s16w  =(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
else
  ptForkLift->s16w = -(float)(RxMessage.Data[0]/255.0*3.14); //0~255 -> 0~3.14rad/s
//x���ٶ�
if ((RxMessage.Data[6] & 0x20)) // �ֽ�6 ��bit5
  ptForkLift->s16x  =(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
else
	ptForkLift->s16x = -(float)(RxMessage.Data[2]/255.0*0.8);  //0~255 -> 0~0.8m/s
//*/
ptForkLift->s16speedwant = (float)((ptForkLift->s16x + ptForkLift->s16w * ptForkLift->s16dp_d*0.5)/(3.14*ptForkLift->s16dpl_d)*ptForkLift->u32dpl_econder*ptForkLift->u32dpl_econder_reduc/1000); //1ms�ڵ������� 
		}
		
		
if(ptForkLift->u8CarID==5)  //���²�
		{
    	// ptForkLift->s16speedwant = (float)(ptForkLift->s16ud/(3.14*ptForkLift->s16ud_d)*ptForkLift->u32ud_econder*ptForkLift->u32ud_econder_reduc/1000); //1ms�ڵ�������
			ptForkLift->s16speedwant =0;
		}
if(ptForkLift->u8CarID==6)  //ǰ���
		{
//			/*
//ǰ����ٶ�
if ((RxMessage.Data[6] & 0x80)) // �ֽ�6 ��bit5
  ptForkLift->s16fb =(float)(RxMessage.Data[4]/255.0*0.32); //0~255 -> 0~0.32m/s
else
  ptForkLift->s16fb = -(float)(RxMessage.Data[4]/255.0*0.32); //0~255 -> 0~0.32m/s
//*/
ptForkLift->s16speedwant = (float)(ptForkLift->s16fb/(3.14*ptForkLift->s16fb_d)*ptForkLift->u32fb_econder*ptForkLift->u32fb_econder_reduc/1000); //1ms�ڵ������� 
		 //ptForkLift->s16speedwant =0;
		}
//*/

//printf("%f\r\n",ptForkLift->s16speedwant);

			if (ptForkLift->s16speedwant==0)
			{
				ptForkLift->s16error[0] = 0;
				ptForkLift->s16error[1] = 0;
				ptForkLift->s16error[2] = 0;
				ptForkLift->s16ErrorSum = 0;
			}
			

//		printf("direction: %d\t value: %f\t\r\n",ptForkLift->bDrection,ptForkLift->s16speedwant);
		ptForkLift->u16CanComCount[0]++;
		
	}
	   
	
	
	
	//for(i=0;i<8;i++)
	//printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 Can_Send_Msg(void)
{	
  u8 mbox;
  u16 i=0;

//	printf("can_send_msg:%d\r\t\n",ptForkLift->s32EncoderCounter);
	
  mbox= CAN_Transmit(CAN1, &ptForkLift->Can_Txmessage);   
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
  if(i>=0XFFF)return 1;
  return 0;		

}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}














