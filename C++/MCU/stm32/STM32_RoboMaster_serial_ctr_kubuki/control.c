#include "control.h"
#include "stdlib.h"
#include "delay.h"
#include "usart.h"
#include "can.h"
/************************************************
 
************************************************/

//ȫ�ֱ�������
TCtrl            TForkLift[2];   //����泵�ṹ�� ����
ptCtrl           ptForkLift;     //����泵�ṹ��ָ��
ptCtrl           ptForkLift2;    //����泵�ṹ��ָ��

#define ABS(x)		((x>0)? (x): (-x)) 

void abs_limit(float *a, float ABS_MAX){
    if(*a > ABS_MAX)
        *a = ABS_MAX;
    if(*a < -ABS_MAX)
        *a = -ABS_MAX;
}

/*λ��ʽPID�㷨*/
float POSITION_PID(ptCtrl ptCtrlCar, int CurrentSpeed)
{
	                         // ����ֵ  -               ����ֵ     -(10000 ~ 10000)   500ת/���� * 19 
	ptCtrlCar->s16error[0] = ptCtrlCar->s16speedwant - (float) CurrentSpeed;
	
	if((ptCtrlCar->u16ErrorDeadBand != 0)&&(ABS(ptCtrlCar->s16error[0])<ptCtrlCar->u16ErrorDeadBand))
	return 0;
	
	//printf("ptCtrlCar->s16ErrorSum =  %f\r\n",ptCtrlCar->s16ErrorSum);		
	ptCtrlCar->s16speedout = ptCtrlCar->s16speed_p*ptCtrlCar->s16error[0] 
							+ ptCtrlCar->s16speed_d*(ptCtrlCar->s16error[0] - ptCtrlCar->s16error[1])
							+ ptCtrlCar->s16speed_i*ptCtrlCar->s16ErrorSum;
	
	ptCtrlCar->s16error[1] = ptCtrlCar->s16error[0];//�ϴ����	
	/******wyw����Ϊ�˵õ� ����ʽPID*******/
	ptCtrlCar->s16error[2] = ptCtrlCar->s16error[1];//���ϴ����	
	ptCtrlCar->s16ErrorSum += ptCtrlCar->s16error[0];
	
	abs_limit(&(ptCtrlCar->s16speedout), MAX_Carrent_out);//�޷�
	return 1;
}

/*����ʽPID�㷨*/
float ZL_PID(ptCtrl ptCtrlCar,int  CurrentSpeed)
{
	
	ptCtrlCar->s16error[0] = ptCtrlCar->s16speedwant - (float) CurrentSpeed;	
	if((ptCtrlCar->u16ErrorDeadBand != 0)&&(ABS(ptCtrlCar->s16error[0])<ptCtrlCar->u16ErrorDeadBand))
		return 0;
	
	ptCtrlCar->s16speedout += ptCtrlCar->s16speed_p*(ptCtrlCar->s16error[0] - ptCtrlCar->s16error[1])						
							+ ptCtrlCar->s16speed_i*ptCtrlCar->s16error[0]
							+ ptCtrlCar->s16speed_d*(ptCtrlCar->s16error[0] - 2 * ptCtrlCar->s16error[1] + ptCtrlCar->s16error[2]);
	
	ptCtrlCar->s16error[1] = ptCtrlCar->s16error[0];//�ϴ����
	ptCtrlCar->s16error[2] = ptCtrlCar->s16error[1];//���ϴ����	
	
  abs_limit(&(ptCtrlCar->s16speedout), MAX_Carrent_out);//�޷�
	return 1;
}


void set_moto_current( s16 iq1, s16 iq2, s16 iq3, s16 iq4){

	//ptForkLift->Can_Txmessage.StdId=0x102;		// 0X201...0x204�ĸ�������յ� �����趨ֵ
	ptForkLift->Can_Txmessage.Data[0] = iq1 >> 8;//  ���� 201
	ptForkLift->Can_Txmessage.Data[1] = iq1;     //  
	ptForkLift->Can_Txmessage.Data[2] = iq2 >> 8;//  ���� 202
	ptForkLift->Can_Txmessage.Data[3] = iq2;
	ptForkLift->Can_Txmessage.Data[4] = 0 >> 8;
	ptForkLift->Can_Txmessage.Data[5] = 0;
	ptForkLift->Can_Txmessage.Data[6] = 0 >> 8;
	ptForkLift->Can_Txmessage.Data[7] = 0;
  Can_Send_Msg();  //������Ϣ�� ����ĵ��
}
