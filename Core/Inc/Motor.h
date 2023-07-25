//
// Created by mulai on 2023/6/18.
//

#ifndef _MOTOR_H
#define _MOTOR_H
#include "main.h"

#define DISABLE_TIM1//���ʹ�õ������һ��timer��ɾ���ö�
#define DISABLE_ENCODER
#undef DISABLE_ENCODER
//�������õĶ�ʱ��//
#define MotorTimer1 htim8
#define MotorTimer2 htim1
//����ÿ��������õ�ͨ��//
#define MotorA_Ch1 TIM_CHANNEL_4
#define MotorA_Ch2 TIM_CHANNEL_3
#define MotorB_Ch1 TIM_CHANNEL_2
#define MotorB_Ch2 TIM_CHANNEL_1

#define MotorC_Ch1 TIM_CHANNEL_1
#define MotorC_Ch2 TIM_CHANNEL_2
#define MotorD_Ch1 TIM_CHANNEL_3
#define MotorD_Ch2 TIM_CHANNEL_4

//����������Ľ�������
#define Encoder_Timer1 htim2
#define Encoder_Timer2 htim3
#define Encoder_TimeCounter htim6

typedef struct
{
    float WheelSpeed_L;
    float WheelSpeed_R;
}_Motor;
extern _Motor motor;

void Motor_Init(void);
void Control_A(int32_t Speed);
void Control_B(int32_t Speed);
void Control_C(int32_t Speed);
void Control_D(int32_t Speed);
void Servo1_Set(uint16_t Angle);
void GetSpeed(void);
void Encode_CallBack(void);
#endif
