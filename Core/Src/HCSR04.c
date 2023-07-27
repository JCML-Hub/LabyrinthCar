/**
  ************************************* Copyright ******************************
  *

  *                 (C) Copyright 2023,--,China, CUIT.
  *                            All Rights Reserved
  *                     By(JCML)
  * FileName   : HCSR04.c
  * Version    : v1.0
  * Author     : JCML
  * Date       : 2023-02-10
  * Description: HCSR04���ģ�飬 ��������ʹ����TIM3�� �Լ�Delay
******************************************************************************
 */

#include "main.h"
#include "Delay.h"
#include "tim.h"
#include "HCSR04.h"

//�˴��������õĶ�ʱ��

float data1[10],data2[10];

uint64_t time_CH1=0;			//����������������ʱ
uint64_t time_CH2=0;			//����������������ʱ
uint64_t time_end=0;		//�����������洢�ز��ź�ʱ��


/***********************************************************
*@���� 	:HC_SR04_Init
*@����	:��ʼ�� ����TIM�ж�
*@����	:��
*@����ֵ	:��
*@����	:JCML
*@����	:2023-07-27
***********************************************************/

void HC_SR04_Init(void)//�˴��Ѿ���������ͨ���Ŀ���
{
  HAL_TIM_Base_Start_IT(&HCSR04Timer);
  HAL_GPIO_WritePin(TrigCH1_GPIO_Port,TrigCH1_Pin,GPIO_PIN_RESET);						//����͵�ƽ
  Delay_us(15);											//��ʱ15΢��
  HAL_GPIO_WritePin(TrigCH2_GPIO_Port,TrigCH1_Pin,GPIO_PIN_RESET);						//����͵�ƽ
  Delay_us(15);											//��ʱ15΢��
}
/***********************************************************
*@���� 	:HC_SR04_CounterCallBack
*@����	:��ʱ�������ص�����
*@����	:��
*@����ֵ	:��
*@����	:JCML
*@����	:2023-07-27
***********************************************************/
void HC_SR04_CounterCallBack(void){
  time_CH1++;
  time_CH2++;
}
/***********************************************************
*@���� 	:GetDistance
*@����	:��ȡ����
*@����	:��
*@����ֵ	:Distance��ʼֵ
*@����	:JCML
*@����	:2023-07-27
***********************************************************/

float GetDistance_CH1(void)									//��ಢ���ص�λΪ���׵ľ�����
{
  float Distance=0;
  HAL_GPIO_WritePin(TrigCH1_GPIO_Port,TrigCH1_Pin,GPIO_PIN_SET);//����ߵ�ƽ
  Delay_us(15);										//��ʱ15΢��
  HAL_GPIO_WritePin(TrigCH1_GPIO_Port,TrigCH1_Pin,GPIO_PIN_RESET);//����͵�ƽ
  while (HAL_GPIO_ReadPin(EchoCH1_GPIO_Port, EchoCH1_Pin) == 0);//�ȴ��͵�ƽ������תΪ�ߵ�ƽ��ʼ��ʱ
  time_CH1=0;												//��ʱ����
  while(HAL_GPIO_ReadPin(EchoCH1_GPIO_Port, EchoCH1_Pin) == 1);		//�ȴ��ߵ�ƽ������תΪ�͵�ƽ������ʱ
  time_end=time_CH1;										//��¼����ʱ��ʱ��
  if(time_end/100<38)									//�ж��Ƿ�С��38���룬����38����ľ��ǳ�ʱ��ֱ�ӵ������淵��0
  {
    Distance=((float )time_end*346)/2.0f;						//������룬25��C�����е�����Ϊ346m/s
  }
  return Distance;									//���ز����
}


float GetDistance_CH2(void)									//��ಢ���ص�λΪ���׵ľ�����
{
  float Distance=0;
  HAL_GPIO_WritePin(TrigCH2_GPIO_Port,TrigCH2_Pin,GPIO_PIN_SET);//����ߵ�ƽ
  Delay_us(15);										//��ʱ15΢��
  HAL_GPIO_WritePin(TrigCH2_GPIO_Port,TrigCH2_Pin,GPIO_PIN_RESET);//����͵�ƽ
  while (HAL_GPIO_ReadPin(EchoCH2_GPIO_Port, EchoCH2_Pin) == 0);//�ȴ��͵�ƽ������תΪ�ߵ�ƽ��ʼ��ʱ
  time_CH2=0;												//��ʱ����
  while(HAL_GPIO_ReadPin(EchoCH2_GPIO_Port, EchoCH2_Pin) == 1);		//�ȴ��ߵ�ƽ������תΪ�͵�ƽ������ʱ
  time_end=time_CH2;										//��¼����ʱ��ʱ��
  if(time_end/100<38)									//�ж��Ƿ�С��38���룬����38����ľ��ǳ�ʱ��ֱ�ӵ������淵��0
  {
    Distance=((float )time_end*346)/2.0f;						//������룬25��C�����е�����Ϊ346m/s
  }
  return Distance;									//���ز����
}
/***********************************************************
*@���� 	:ScanDistance_mm
*@����	:ת���õ��ľ���ֵ
*@����	:��
*@����ֵ	:��
*@����	:JCML
*@����	:2023-07-27
***********************************************************/
float averageFilterCH1(float in_data)
{
  float sum = 0;
  for(int i=0; i<9; i++)
  {
    data1[i]=data1[i+1];
    sum = sum + data1[i];
  }
  data1[9] = in_data;
  sum = sum + data1[9];
  return(sum/10);
}
float averageFilterCH2(float in_data)
{
  float sum = 0;
  for(int i=0; i<9; i++)
  {
    data2[i]=data2[i+1];
    sum = sum + data2[i];
  }
  data2[9] = in_data;
  sum = sum + data2[9];
  return(sum/10);
}
float ScanDistance_mm(uint8_t Which)//���ص�λΪ���׵ľ�����,����һ���򵥵ľ�ֵ�˲�
{
  float temp=0;
  if(Which==1){
    temp=GetDistance_CH1() / 100.0f;
    return averageFilterCH1(temp);
  } else if (Which==2){
    temp=GetDistance_CH2() / 100.0f;
    return averageFilterCH2(temp);
  }
}




/***********************************************************
*@���� 	:HAL_TIM_PeriodElapsedCallback
*@����	:�жϻص������˴����ڼ�ʱ
*@����	:��
*@����ֵ	:��
*@����	:JCML
*@����	:2023-07-27
***********************************************************/

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)			//�����жϺ�����������ʱ��ÿ10΢�����time��1
//{
//  if (htim == &HCSR04Timer)		//��ȡTIM3��ʱ���ĸ����жϱ�־λ
//  {
//    time++;
//  }
//}
