#include "main.h"

/**
  * @brief  ΢�뼶��ʱ
  * @param  xus ��ʱʱ������Χ��0~233015
  * @retval ��
  */
/*******************************************************************************
* �� �� ��         : delay_us
* ��������           : ��ʱ��������ʱus
* ��    ��         : i
* ��    ��         : ��
*******************************************************************************/
void Delay_us(uint32_t i)
{
  uint32_t temp;
  SysTick->LOAD=9*i;         //������װ��ֵ, 72MHZʱ
  SysTick->CTRL=0X01;         //ʹ�ܣ����������޶����������ⲿʱ��Դ
  SysTick->VAL=0;                //���������
  do
  {
    temp=SysTick->CTRL;           //��ȡ��ǰ������ֵ
  }
  while((temp&0x01)&&(!(temp&(1<<16))));     //�ȴ�ʱ�䵽��
  SysTick->CTRL=0;    //�رռ�����
  SysTick->VAL=0;        //��ռ�����
}
//void Delay_us(uint32_t xus)
//{
//  SysTick->LOAD = 72 * xus;				//���ö�ʱ����װֵ
//  SysTick->VAL = 0x00;					//��յ�ǰ����ֵ
//  SysTick->CTRL = 0x00000005;				//����ʱ��ԴΪHCLK��������ʱ��
//  while(!(SysTick->CTRL & 0x00010000));	//�ȴ�������0
//  SysTick->CTRL = 0x00000004;				//�رն�ʱ��
//}

/**
  * @brief  ���뼶��ʱ
  * @param  xms ��ʱʱ������Χ��0~4294967295
  * @retval ��
  */
void Delay_ms(uint32_t xms)
{
  while(xms--)
  {
    Delay_us(1000);
  }
}

/**
  * @brief  �뼶��ʱ
  * @param  xs ��ʱʱ������Χ��0~4294967295
  * @retval ��
  */
void Delay_s(uint32_t xs)
{
  while(xs--)
  {
    Delay_ms(1000);
  }
}
