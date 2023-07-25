//
// Created by mulai on 2023/6/18.
//
#include "tim.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_gpio.h"
#include "Motor.h"

_Motor motor;
short motorA,motorB;

/***********************************************************
*@���� :Motor_Init
*@����	:Motor_Init����������PWM�ĳ�ʼ���Լ���ת��������ʱ���ĳ�ʼ��
*@����	:��
*@����ֵ	:��
*@����	:JCML
*@����	:2023-04-06
***********************************************************/

void Motor_Init(void)
{
  HAL_TIM_PWM_Start(&MotorTimer1,MotorA_Ch1);
  HAL_TIM_PWM_Start(&MotorTimer1,MotorA_Ch2);
  HAL_TIM_PWM_Start(&MotorTimer1,MotorB_Ch1);
  HAL_TIM_PWM_Start(&MotorTimer1,MotorB_Ch2);
#ifndef DISABLE_TIM1
  HAL_TIM_PWM_Start(&MotorTimer2,MotorC_Ch1);
  HAL_TIM_PWM_Start(&MotorTimer2,MotorC_Ch2);
  HAL_TIM_PWM_Start(&MotorTimer2,MotorD_Ch1);
  HAL_TIM_PWM_Start(&MotorTimer2,MotorD_Ch2);
#endif
#ifndef DISABLE_ENCODER
  HAL_TIM_Encoder_Start(&Encoder_Timer1, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&Encoder_Timer2, TIM_CHANNEL_ALL);//������ʱ������
  HAL_TIM_Base_Start_IT(&Encoder_TimeCounter);//�������ڻ�ȡ�����Ķ�ʱ��
#endif
}

/***********************************************************
*@���� :Control_A  & 	Control_B  & 	Control_C  & 	Control_D
*@����	:�ĸ����ӵ�PWM��ֵ����
*@����	:Speed���������� ��Χ��Tim�趨���Զ���װֵ����
*@����ֵ	:��
*@����	:JCML
*@����	:2023-07-11
***********************************************************/
void Control_A(int32_t Speed) {
  Speed=Speed;//�������������ڴ˴��޸�
  if (Speed >= 0) {//wΪ��ֹ�����������˴�ֱ�ӽ�����һ����PWM��Ϊ���˴�Ϊ���ж����
            __HAL_TIM_SetCompare(&MotorTimer1, MotorA_Ch1, Speed);
            __HAL_TIM_SetCompare(&MotorTimer1, MotorA_Ch2, 0);

  } else {
            __HAL_TIM_SetCompare(&MotorTimer1, MotorA_Ch1, 0);
            __HAL_TIM_SetCompare(&MotorTimer1, MotorA_Ch2, -Speed);}//�ٶ�Ϊ��ȡ�෴��
}
void Control_B(int32_t Speed) {
  Speed=-Speed;
  if (Speed >= 0) {//wΪ��ֹ�����������˴�ֱ�ӽ�����һ����PWM��Ϊ���˴�Ϊ���ж����
            __HAL_TIM_SetCompare(&MotorTimer1, MotorB_Ch1, Speed);
            __HAL_TIM_SetCompare(&MotorTimer1, MotorB_Ch2, 0);
  } else {
            __HAL_TIM_SetCompare(&MotorTimer1, MotorB_Ch1, 0);
            __HAL_TIM_SetCompare(&MotorTimer1, MotorB_Ch2, -Speed);}//�ٶ�Ϊ��ȡ�෴��
}
#ifndef DISABLE_TIM1
void Control_C(int32_t Speed) {
  if (Speed >= 0) {//wΪ��ֹ�����������˴�ֱ�ӽ�����һ����PWM��Ϊ���˴�Ϊ���ж����
            __HAL_TIM_SetCompare(&MotorTimer2, MotorC_Ch1, Speed);
            __HAL_TIM_SetCompare(&MotorTimer2, MotorC_Ch2, 0);
  } else {
            __HAL_TIM_SetCompare(&MotorTimer2, MotorC_Ch1, 0);
            __HAL_TIM_SetCompare(&MotorTimer2, MotorC_Ch2, -Speed);}//�ٶ�Ϊ��ȡ�෴��
}
void Control_D(int32_t Speed) {
  if (Speed >= 0) {//wΪ��ֹ�����������˴�ֱ�ӽ�����һ����PWM��Ϊ���˴�Ϊ���ж����
            __HAL_TIM_SetCompare(&MotorTimer2, MotorD_Ch1, Speed);
            __HAL_TIM_SetCompare(&MotorTimer2, MotorD_Ch2, 0);
  } else {
            __HAL_TIM_SetCompare(&MotorTimer2, MotorD_Ch1, 0);
            __HAL_TIM_SetCompare(&MotorTimer2, MotorD_Ch2, -Speed);}//�ٶ�Ϊ��ȡ�෴��
}
#endif

#ifndef DISABLE_ENCODER
/***********************************************************
*@���� :GetSpeed
*@����	:��ȡ��ת���������ٶ�ֵ
*@����	:_Motor�ṹ�壬�ú�������whileѭ����ȡ
*@����ֵ	:��
*@����	:JCML
*@����	:2023-06-18
***********************************************************/
void GetSpeed(void)
{
  motor.WheelSpeed_R = (float )motorA;
  motor.WheelSpeed_L = -(float )motorB;//��ȡ��ǰ���ӵ��ٶ�,�˴�ֱ�ӽ��ٶ�תΪfloat�ͣ����ں�������˲�
}


void Encode_CallBack(void)
{
  motorA = ((short)__HAL_TIM_GET_COUNTER(&Encoder_Timer1));//��ȡM1����ת����,�˴�ȡ���ǻ���������һ�ܼ�����һ
  motorB = ((short)__HAL_TIM_GET_COUNTER(&Encoder_Timer2));//��ȡM2����ת����
  __HAL_TIM_SET_COUNTER(&Encoder_Timer1, 0);
  __HAL_TIM_SET_COUNTER(&Encoder_Timer2, 0);//����������������
}
/***********************************************************
*@���� :HAL_TIM_PeriodElapsedCallback
*@����	:��ʱ���Ļص�����,����г�ͻ�뽫�����main.c�У�������Encode_CallBack���¼���ֵ
*@����	:TIM_HandleTypeDef
*@����ֵ	:��
*@����	:JCML
*@����	:2023-06-18
***********************************************************/
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//ÿ100ms��һ���ж�
//{
//  if(htim==(&Encoder_TimeCounter))
//  {
//    Encode_CallBack();
//  }
//}

#endif