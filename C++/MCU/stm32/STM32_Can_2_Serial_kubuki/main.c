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
#include <stdbool.h>//bool��

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

#define TEST 

//kobukiͨ�Žӿں궨��
#define clearBuffer 0
#define waitingForStx 1
#define waitingForPayloadSize 2
#define waitingForPayloadToEtx 3
#define	waitingForEtx 4

//koubkiͨ�Žӿ���Ϣͷ
u8 STX[2];

//kobukiͨ�Žӿ� ״̬
u8 cmd_state;
u8 size_length_field;
u8 size_payload;
bool found_package;
u8 identifier;
s16 motor_speed;
s16 motor_radius;
bool retController;
bool retExtData;
u8   extDataFlag;

//����Buffer  (Kobuki)
char buffer[50];   

//only for test
char tempBuf[50]; 
char tempIndex;


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

// kubukiͨ�Žӿں���
/* ???????Firefly */
void sendBasicData()
{
	u8 cs;
	u8 i;

	//ʱ��� 0~65535
	u16 cur_time = ptForkLift->u16RunSendPeriodMotor;
	//�ۼƱ�����ֵ ����
	u16 encoder_left =  ptForkLift->u16EncoderSum_l;
	//�ۼƱ�����ֵ  ����
	u16 encoder_right =  ptForkLift->u16EncoderSum_r;
	
	//����������
	u8 sendBuf[21];
	
	sendBuf[0] = 0xAA;//Header 0
	sendBuf[1] = 0x55;//Header 1
	sendBuf[2] = 0x11;//payload?? = 16
	sendBuf[3] = 0x01;//payload id = 1
	sendBuf[4] = 0x0F;//payload length = 15
	sendBuf[5] = cur_time&0x00FF;//ayload ??? ??
	sendBuf[6] = (cur_time&0xFF00)>>8 ;//payload ??? ??
	sendBuf[7] = 0x00;//payload ???
	sendBuf[8] = 0x00;//payload ???
	sendBuf[9] = 0x00;//payload ????
	sendBuf[10] = encoder_left&0x00FF;//payload ?????????
	sendBuf[11] = (encoder_left&0xFF00)>>8;//payload ?????????
	sendBuf[12] = encoder_right&0x00FF;//payload ?????????
	sendBuf[13] = (encoder_right&0xFF00)>>8;//payload ?????????
	sendBuf[14] = 0x00;//payload PWM?
	sendBuf[15] = 0x00;//payload PWM?
	sendBuf[16] = 0x00;//payload button
	sendBuf[17] = 0x00;//payload charge
	sendBuf[18] = 0x00;//payload battery
	sendBuf[19] = 0x00;//payload ????

	//checksum
	cs = 0x00;
	for (i = 2; i < 20; i++)
	{
	  cs ^= sendBuf[i];
	}
	sendBuf[20] = cs;	

	for (i = 0; i < 21; i++)
		printf("%c",sendBuf[i]);	

}

void sendControllerData()
{
	u8 sendBuf[19];
	u8 cs;
	u8 i;
	
	sendBuf[0] = 0xAA;//Header 0
	sendBuf[1] = 0x55;//Header 1
	sendBuf[2] = 0x0F;//payload?? = 15
	sendBuf[3] = 0x15;//payload id = 21
	sendBuf[4] = 0x0D;//payload length = 13
	sendBuf[5] = 0x00;//pid_type
	sendBuf[6] = 0x00 ;//pid_p
	sendBuf[7] = 0x00 ;//pid_p
	sendBuf[8] = 0x00 ;//pid_p
	sendBuf[9] = 0x00 ;//pid_p
	sendBuf[10] = 0x00 ;//pid_i
	sendBuf[11] = 0x00 ;//pid_i
	sendBuf[12] = 0x00 ;//pid_i
	sendBuf[13] = 0x00 ;//pid_i
	sendBuf[14] = 0x00 ;//pid_d	
	sendBuf[15] = 0x00 ;//pid_d	
	sendBuf[16] = 0x00 ;//pid_d	
	sendBuf[17] = 0x00 ;//pid_d	

	//checksum
	cs = 0x00;
	for (i = 2; i < 18; i++)
	{
	  cs ^= sendBuf[i];
	}
	sendBuf[18] = cs;	

	//????	
	for (i = 0; i < 10; i++)
		printf("%c",sendBuf[i]);	
}

void sendExtData()
{
	//
	u8 sendBuf[30];
	u8 cs;
	u8 i;
	
	sendBuf[0] = 0xAA;//Header 0
	sendBuf[1] = 0x55;//Header 1
	sendBuf[2] = 0x1a;//payload?? = 26
	
	sendBuf[3] = 0x0a;//payload id = 10  ------ hardware version
	sendBuf[4] = 0x04;//payload length = 4
	sendBuf[5] = 0x00;//patch
	sendBuf[6] = 0x03 ;//minor
	sendBuf[7] = 0x01 ;//major
	sendBuf[8] = 0x00 ;//unused
	
	sendBuf[9] = 0x0b ;//payload id = 11 ---- firmware version
	sendBuf[10] = 0x04 ;//payload len = 4
	sendBuf[11] = 0x00 ;//patch
	sendBuf[12] = 0x03 ;//minor
	sendBuf[13] = 0x01 ;//major
	sendBuf[14] = 0x00 ;//unused	

	
	sendBuf[15] = 0x13 ;//payload id = 19 ---- UDID	
	sendBuf[16] = 0x0c ;//payload len
	sendBuf[17] = 0x00 ;//udid
	sendBuf[18] = 0x00 ;//udid
	sendBuf[19] = 0x00 ;//udid
	sendBuf[20] = 0x00 ;//udid
	sendBuf[21] = 0x00 ;//udid
	sendBuf[22] = 0x00 ;//udid
	sendBuf[23] = 0x00 ;//udid
	sendBuf[24] = 0x00 ;//udid
	sendBuf[25] = 0x00 ;//udid
	sendBuf[26] = 0x00 ;//udid
	sendBuf[27] = 0x00 ;//udid
	sendBuf[28] = 0x01 ;//udid

	// checksum
	cs = 0x00;
	for (i = 2; i < 29; i++)
	{
	  cs ^= sendBuf[i];
	}
	sendBuf[29] = cs;	

	for (i = 0; i < 30; i++)
		printf("%c",sendBuf[i]);
}

bool WaitForStx(u8 datum)
{
  bool found_stx;
  u8 i;

  found_stx = true;

  // add incoming datum
  buffer[index_] = datum;
  index_++;

  // check whether we have STX
  for (i = 0; i < index_ && i < 2; i++)
  {
    if (buffer[i] != STX[i])
    {
      found_stx = false;
      index_ = 0;
      break;
    }
  }
  
  return (found_stx && index_ == 2);
}

bool waitForPayloadSize(u8 incoming)
{
  buffer[index_]= incoming;
  index_ ++;

  // check when we need to wait for etx
  if (index_ < 2 + size_length_field)  //??buffer??? < (0xaa 0x55 + <payload siez> )
  {
    return false;
  }
  else
  {
    switch (size_length_field)
    {
      case 1: // kobuki
        size_payload = buffer[2];
        break;
      default:
        // put assertion failure
        size_payload = 1;
        break;
    }
    return true;
  }
}

bool waitForPayloadAndEtx(u8 incoming)
{
  buffer[index_] = incoming;
  index_++;
 
  if ( size_payload > 10 ) { //???????
    cmd_state = clearBuffer;
    return false;
  }

  // check when we need to wait for etx
  if (index_ < 2 + size_length_field + size_payload + 1)  
  {
    return false;
  }
  else
  {
//	  printf("found_package !!!!!!!!!!!!\r\n");
    found_package = true;

    return true;
  }
}


//???kobuki???????
bool update(u8 incoming)
{
	found_package = false;

	if ( cmd_state == clearBuffer ) {
	   memset(buffer, 0, sizeof(buffer));
	   index_ = 0;
	   found_package = false;
	   cmd_state = waitingForStx;
	 }

  	switch (cmd_state)
  	{
    case waitingForStx:
    	if (WaitForStx(incoming)==true)
      	{
          cmd_state = waitingForPayloadSize; // kobukibot
        }
        break;
    case waitingForPayloadSize:
      if (waitForPayloadSize(incoming)==true)
      {
        cmd_state = waitingForPayloadToEtx;
      }
      break;

    case waitingForPayloadToEtx:
      if (waitForPayloadAndEtx(incoming)==true)
      {
        cmd_state = clearBuffer;
      }
      break;

    default:
      cmd_state = waitingForStx;
      break;
  }

  if (found_package ==true) {
    return true;	//what happen if checksum is equal to false(== -1)?
  } else {
    return false;
  }	
}

void runCommand_Motor(s16 speed, s16 radius)
{
			if (radius==0)
			{
				 ptForkLift->s16x = speed / 1000.0;  
				 ptForkLift->s16y = 0;
				 ptForkLift->s16w = 0;
			}
			else if (radius==1)
			{
				 ptForkLift->s16x = 0;  
				 ptForkLift->s16y = 0;
				 ptForkLift->s16w = 2 * speed  / (1000*0.352);	
			}
			else if (radius>0)
			{
				ptForkLift->s16y = 0;
				ptForkLift->s16w = speed/(radius + 1000*0.352/2) ;
				ptForkLift->s16x = radius * ptForkLift->s16w / 1000.0 ;  
			}
			else
			{
				ptForkLift->s16y = 0;
				ptForkLift->s16x = speed*1.0/((1-1000*0.354/(2*radius))*1000);
				ptForkLift->s16w = ptForkLift->s16x * 1000 / radius;
			}
			 ptForkLift->u8Can_Msg_Send_flag=1;//??  can????		
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
	
	//kobuki, ͨ�Žӿڳ�ʼ��
	memset(buffer, 0, sizeof(buffer)); //For kobuki,????????
	STX[0] = 0xaa;
	STX[1] = 0x55;
	index_ = 0;
	size_length_field=1;
	cmd_state = clearBuffer;
	found_package = false;
	retController = false;
	retExtData = false;		
	

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
/*					
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
				*/				
					//64ms����һ��
					if ( (ptForkLift->u16RunSendPeriodMotor & 0x003F) == 0 )
						sendBasicData();

					//Controller
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

#ifdef TEST
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
#else

	tempBuf[tempIndex] = chr;
	tempIndex ++;
	if (tempIndex>10)
		tempIndex = 0;
   //Kobuki
   if (update(chr))
   {
	  //֡��ʶ�� identifier
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
