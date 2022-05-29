#include "control.h"
#include "stdlib.h"
#include "delay.h"
#include "usart.h"
/************************************************
 
 �����ߣ�����ǿ
 ����ʱ�䣺2017/03/20
 �ļ�˵�����ǳ�����Ƽ���˾�泵�����������
 
 ����ϲ���Ƴ����ڴ��ļ���
 
************************************************/

//ȫ�ֱ�������
TCtrl            TForkLift;     //����泵�ṹ��
ptCtrl           ptForkLift;    //����泵�ṹ��ָ��
linkqueue        middle_filter_queue;
linkqueue*       ptmiddle_filter_queue;

extern u8 board_type;

/*�ϰ����β��� 1��PWM  1����ͨIO����ʽ*/
/*
void SetPwmDir(ptCtrl ptCtrlCar)
{
	if(ptCtrlCar->u16PWM>MAX_PWM)
	{
		ptCtrlCar->u16PWM=MAX_PWM;
	}
	else if(ptCtrlCar->u16PWM<=MIN_PWM)
	{
		ptCtrlCar->u16PWM=MIN_PWM;
	}
	TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
	
	//TIM1->CCR1 = ptCtrlCar->u16PWM;       //pwm
	
	if(ptCtrlCar->bDrection==FOWARD)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);  //dir	
		GPIO_SetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
	}
	else if(ptCtrlCar->bDrection==BACK)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);  //dir	
		GPIO_ResetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
		
	}	
}	
*/


// �ϰ������Ҳ���� �°���  ������Ϊ PWM�ڵ� ���  һ��������� pwm ���� ԭ�� PA9�� δ����
// ���� PWM�ͷ���ֵ
void SetPwmDir(ptCtrl ptCtrlCar)
{
	// pwm �޷� ���
	if(ptCtrlCar->u16PWM>MAX_PWM)
	{
		ptCtrlCar->u16PWM=MAX_PWM;
	}
	else if(ptCtrlCar->u16PWM<=MIN_PWM)
	{
		ptCtrlCar->u16PWM=MIN_PWM;
	}
	TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
	//TIM1->CCR1 = ptCtrlCar->u16PWM;       // ͨ��1 pwm
	
	// //PA.9 ���Ʒ��� �� ǰ��  �� ����
	if(ptCtrlCar->bDrection==FOWARD)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);  //dir	
		//GPIO_SetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
		TIM_SetCompare2(TIM1,1799);           // ͨ��2 pwm Ϊ 100% ���� ��Ϊ�ߵ�ƽ
//TIM_SetCompare2(TIM1,1800*10*2);           // ͨ��2 pwm Ϊ 100% ���� ��Ϊ�ߵ�ƽ
		
		//TIM1->CCR2 = ptCtrlCar->1799;       // ͨ��2 pwm
	}
	
	else if(ptCtrlCar->bDrection==BACK)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);  //dir	
		//GPIO_ResetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
	  TIM_SetCompare2(TIM1,0);           // ͨ��2 pwm Ϊ 0% ���� ��Ϊ�͵�ƽ
		//TIM1->CCR2 = ptCtrlCar->0;       // ͨ��2 pwm
		
	}	
}		


// ���� PWM�ͷ���ֵ �°���
void SetPwmDir_new(ptCtrl ptCtrlCar)
{
	// pwm �޷� ���
	if(ptCtrlCar->u16PWM>MAX_PWM)
	{
		ptCtrlCar->u16PWM=MAX_PWM;
	}
	else if(ptCtrlCar->u16PWM<=MIN_PWM)
	{
		ptCtrlCar->u16PWM=MIN_PWM;
	}
	
	//TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
	//TIM1->CCR1 = ptCtrlCar->u16PWM;       // ͨ��1 pwm
	
	// //PA.9 ���Ʒ��� �� ǰ��  �� ����
  //printf("dir %d\r\n",ptCtrlCar->bDrection);
	if(ptCtrlCar->bDrection==FOWARD)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);  //dir	
		//GPIO_SetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
		
	 /*�������෴�� �� �� if  �� else if ���ݻ��� */
		TIM_SetCompare1(TIM1,ptCtrlCar->u16PWM);
		TIM_SetCompare2(TIM1,0);           // ͨ��2 pwm Ϊ  0% ���� ��Ϊ�͵�ƽ

	}
	else if(ptCtrlCar->bDrection==BACK)
	{
		//HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);  //dir	
		//GPIO_ResetBits(GPIOA,GPIO_Pin_9);			//PA.9 �����
		
	 /*�������෴�� �� �� if  �� else if ���ݻ��� */
		TIM_SetCompare1(TIM1,0);                // ͨ��1 pwm Ϊ 0% ���� ��Ϊ�͵�ƽ
	  TIM_SetCompare2(TIM1,ptCtrlCar->u16PWM); 
	}	
}	




// ����ʽPID
/*
ptCtrlCar->s16speedout += ptCtrlCar->s16speed_p*(ptCtrlCar->s16error[0] - ptCtrlCar->s16error[1])						
							+ ptCtrlCar->s16speed_i*ptCtrlCar->s16error[0]
							+ ptCtrlCar->s16speed_d*(ptCtrlCar->s16error[0] - 2 * ptCtrlCar->s16error[1] + ptCtrlCar->s16error[2]);
*/ 

/*λ��ʽPID�㷨*/
void POSITION_PID(ptCtrl ptCtrlCar, int EncoderSpeed)
{
	
	ptCtrlCar->s16error[0] = ptCtrlCar->s16speedwant - (float)EncoderSpeed;
	
	//printf("ptCtrlCar->s16ErrorSum =  %f\r\n",ptCtrlCar->s16ErrorSum);		
	ptCtrlCar->s16speedout = ptCtrlCar->s16speed_p*ptCtrlCar->s16error[0] 
							+ ptCtrlCar->s16speed_d*(ptCtrlCar->s16error[0] - ptCtrlCar->s16error[1])
							+ ptCtrlCar->s16speed_i*ptCtrlCar->s16ErrorSum;
	
	ptCtrlCar->s16error[1] = ptCtrlCar->s16error[0];//�ϴ����
	
	/******wyw����Ϊ�˵õ� ����ʽPID*******/
	ptCtrlCar->s16error[2] = ptCtrlCar->s16error[1];//���ϴ����
	
	ptCtrlCar->s16ErrorSum += ptCtrlCar->s16error[0];
	
	/***************������Ϣ****************/
//	printf("\r\n\tPID_OUTPUT\t");
//printf("%f\r\n",ptCtrlCar->s16speedout);
	
	if(ptCtrlCar->s16speedout < 0)
	{
		ptCtrlCar->bDrection = BACK;
		ptCtrlCar->u16PWM= -ptCtrlCar->s16speedout;
	}
	
	else if(ptCtrlCar->s16speedout > 0)
	{
		ptCtrlCar->bDrection = FOWARD;
		ptCtrlCar->u16PWM=ptCtrlCar->s16speedout;
	}
	else
	{
		ptCtrlCar->u16PWM=0;
	}
	
	/***************������Ϣ****************/
//	printf("\r\n\tPWM_SET\t");
//printf("%d\r\n",ptCtrlCar->u16PWM);
	
	
	/*************wyw 2017.6.20 �������ϰ�������*************/
  //SetPwmDir(ptCtrlCar);
 // if(BOARD_version==0) SetPwmDir(ptCtrlCar);        //���  �ϰ���
	//else if(BOARD_version==1) SetPwmDir_new(ptCtrlCar);//�Ҳ� �°���

	
	
	
	if(board_type==0)
		SetPwmDir(ptCtrlCar);        //���  �ϰ���
	else
		SetPwmDir_new(ptCtrlCar);//�Ҳ� �°���

/*
		if (ptForkLift->u8CarID==1)
		SetPwmDir_new(ptForkLift);//  ��� ���º���
	else
		SetPwmDir_new_inv(ptForkLift);//  �Ҳ� ԭ�ȷ�ת�� ��������֪��ʲôԭ�� ��ת
*/
}


/*����ʽPID�㷨*/
void ZL_PID(ptCtrl ptCtrlCar,int EncoderSpeed)
{
	
	ptCtrlCar->s16error[0] = ptCtrlCar->s16speedwant - (float)EncoderSpeed;
	
	//printf("ptCtrlCar->s16ErrorSum =  %f\r\n",ptCtrlCar->s16ErrorSum);		
	ptCtrlCar->s16speedout += ptCtrlCar->s16speed_p*(ptCtrlCar->s16error[0] - ptCtrlCar->s16error[1])						
							+ ptCtrlCar->s16speed_i*ptCtrlCar->s16error[0]
							+ ptCtrlCar->s16speed_d*(ptCtrlCar->s16error[0] - 2 * ptCtrlCar->s16error[1] + ptCtrlCar->s16error[2]);
	
	ptCtrlCar->s16error[1] = ptCtrlCar->s16error[0];//�ϴ����
	
	/******wyw����Ϊ�˵õ� ����ʽPID*******/
	ptCtrlCar->s16error[2] = ptCtrlCar->s16error[1];//���ϴ����
	
	ptCtrlCar->s16ErrorSum += ptCtrlCar->s16error[0];
	
	/***************������Ϣ****************/
//	printf("\r\n\tPID_OUTPUT\t");
	//printf("%f\r\n",ptCtrlCar->s16speedout);
	
	if(ptCtrlCar->s16speedout < 0)
	{
		ptCtrlCar->bDrection = BACK;
		ptCtrlCar->u16PWM= -ptCtrlCar->s16speedout;
	}
	
	else if(ptCtrlCar->s16speedout > 0)
	{
		ptCtrlCar->bDrection = FOWARD;
		ptCtrlCar->u16PWM=ptCtrlCar->s16speedout;
	}
	else
	{
		ptCtrlCar->u16PWM=0;
	}
	
	/***************������Ϣ****************/
//	printf("\r\n\tPWM_SET\t");
//	printf("%d\r\n",ptCtrlCar->u16PWM);
	
	
	/*************wyw 2017.6.20 �������ϰ�������*************/
  //SetPwmDir(ptCtrlCar);
 // if(BOARD_version==0) SetPwmDir(ptCtrlCar);        //���  �ϰ���
	//else if(BOARD_version==1) SetPwmDir_new(ptCtrlCar);//�Ҳ� �°���

	if(board_type==0)
		SetPwmDir(ptCtrlCar);        //���  �ϰ���
	else 
		SetPwmDir_new(ptCtrlCar);//�Ҳ� �°���
}



void Bubble_Sort()
{
	int a[10] = {1,23,45,67,89,11,33,57,11,45};
	int temp,i,j;
	for(i = 0; i< 9;i++)
	{
		for(j=i;j<9;j++)
		{
			if(a[j] < a[i])
			{
				temp = a[j];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}
	
	
}

void Quick_Sort(linkqueue *  Fliter_q,int left,int right)
{
	int i = left;
	int j = right;

	
	int threshold = get_queue_value(Fliter_q,left);

	if (left > right)
	{
		return;
	}
	
	while (i < j)
	{
		while (get_queue_value(Fliter_q,j) >= threshold && i < j)
		{
			j--;
		}
		
		swap_data(Fliter_q,i,j);
		
		while (get_queue_value(Fliter_q,i) <= threshold && i < j)
		{
			i++;
		}
		
		swap_data(Fliter_q,i,j);
		

	}

	set_data(Fliter_q,i,threshold);

	
	Quick_Sort(Fliter_q, left, i - 1);

	Quick_Sort(Fliter_q, i + 1, right);
}

/*

����һ������
*/

void create_queue(linkqueue *  Fliter_q)
{
		
	Fliter_q->rear = Fliter_q->front = (Filter_Queue*)malloc(sizeof (Filter_Queue));
	
	if(!Fliter_q->front){

		gotoerror();
	}
	Fliter_q->front->next = NULL;
			
}

/*
   ��� 
*/
void en_queue(linkqueue *  Fliter_q, uint16_t data)
{
	Filter_Queue * node;
	node = (Filter_Queue*)malloc(sizeof (Filter_Queue));
	
	if(!node){
		gotoerror();
	}
	
	node->date=data;
	node->next=NULL;
	
	
	Fliter_q->rear->next = node; //�Ȱ������ڵ�������
	Fliter_q->rear = node;       //�ٽ�βָ������ƶ�һλ��ʹ��ָ���β
	
	//free(node);���ﲻ��free�����������ȥfree���Ǹոտ��ٵ��ڴ�ռ�ͱ��ͷ��ˣ�����ڵ���������

	
}

/*

 �ж�һ�������Ƿ�Ϊ�գ��շ���1�����շ���0

*/
int is_empty(linkqueue *  Fliter_q)
{
	return Fliter_q->front == Fliter_q->rear ? 1 : 0 ;
	
}


/*
 ���ӣ�ʧ�ܷ���0���ɹ�����1
*/
int de_queue(linkqueue *  Fliter_q){
	
	Filter_Queue*  node;
	
	if(is_empty(Fliter_q))
	{
		return 0;
	}
	node = Fliter_q->front;
	
	Fliter_q->front = node->next;
	
	free(node);//һ��Ҫ�ͷŵ�ͷ���ָ����ڴ�ռ�
	
	return 1;
	

	
}

/*
��ӡ���е�����ֵ

*/
void printf_queue(linkqueue *  Fliter_q)
{
	Filter_Queue * node;
	
	node = Fliter_q->front;
	
	while(node)
	{
		printf("%d \r\n",node->date);
		
		node = node->next;
		
	}
}


/*
���ض��еĵ�num���ڵ��data��Ա����
*/
uint16_t get_queue_value(linkqueue *  Fliter_q,int num)
{
	Filter_Queue * node;
	int i=0;
	
	node = Fliter_q->front;
	
	for(i=0;i<num;i++)
	{
		node = node->next;
	}
	
	return node->date;

}








/*
   ���������е�i���͵�j���ڵ��data��Ա����
*/
void swap_data(linkqueue *  Fliter_q,int i,int j)
{
	Filter_Queue * node1;
	
	Filter_Queue * node2;
	int k;
	uint16_t temp;
	
	node1 = Fliter_q->front;
	node2 = Fliter_q->front;
	
	for(k=0;k<i;k++)
	{
		node1 = node1->next;
	}
	
	for(k=0;k<j;k++)
	{
		node2 = node2->next;
	}
	
	temp = node1->date;
	
	node1->date = node2->date;
	node2->date = temp;
		
}


/*
   �������ö����е�i���ڵ��data��Ա������ֵ
*/
void set_data(linkqueue *  Fliter_q,int i,uint16_t value)
{
	Filter_Queue * node1;
	int k=0;
	node1 = Fliter_q->front;
	
	for(k=0;k<i;k++)
	{
		node1 = node1->next;
	}
	node1->date = value;
	
}


/*
   ��������ݵľ�ֵ
*/
int average_data(linkqueue *  Fliter_q)
{
	Filter_Queue * node;
	int avg = 0;
	node = Fliter_q->front;
	
	while(node)
	{
		avg += node->date;		
		node = node->next;
		//printf("%d ",avg);
	}
	//printf("\r\n");
	return avg/QUEUE_LENGTH;
}


void gotoerror(void)
{
	while(1){
		
		printf("error\r\n");
		delay_ms(500);//��ʱʱ�䲻�ܳ���1800������ʱ��Ҫ�����
	
	}
	
}










