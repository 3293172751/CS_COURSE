#ifndef _LED_H_
#define _LED_H_
/*Led��˸��*/
/*
stm32 GPIO����ģ  �������룬������� ÿ������������Ϊ�ж�ģʽ 
typedef enum
{ GPIO_Mode_AIN = 0x0,         //ģ������
  GPIO_Mode_IN_FLOATING = 0x04,//��������
  GPIO_Mode_IPD = 0x28,        //����������
  GPIO_Mode_IPU = 0x48,        //��������
  GPIO_Mode_Out_OD = 0x14,     //��©���
  GPIO_Mode_Out_PP = 0x10,     //�������
  GPIO_Mode_AF_OD = 0x1C,      //���ÿ�©���
  GPIO_Mode_AF_PP = 0x18       //�����������
}GPIOMode_TypeDef;
*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

class Led{
	public:
		Led(){
			ledInit();
		};
		void led1On();
		void led1Off();
		void led2On();
		void led2Off();
		void ledDelay();
	private:
		void ledInit(void);
		GPIO_InitTypeDef GPIO_InitStruct;	// //GPIO_InitStruct:????GPIO_InitTypeDef???,?????GPIO?????
};

void Led::ledInit(void)
{
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;//����ѡ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG, ENABLE); // �˿�ʱ��ʹ��
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOG, &GPIO_InitStruct);	
}

void Led::led1Off()
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);	
}
void Led::led2Off()
{
	GPIO_ResetBits(GPIOG, GPIO_Pin_14);
}

void Led::led1On()
{
	GPIO_SetBits(GPIOD, GPIO_Pin_13);	
}

void Led::led2On()
{
	GPIO_SetBits(GPIOG, GPIO_Pin_14);
}

void Led::ledDelay()
{
	for(int32_t i=0xfffef; i>0; i--);
}

#endif
