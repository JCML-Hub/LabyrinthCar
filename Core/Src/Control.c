//
// Created by mulai on 2023/7/25.
//

#include "Control.h"
#include "OLED.h"
#include "HCSR04.h"
#include "Motor.h"
#include "Serial.h"
#include "tim.h"

_Values values;
_Flags flags;


void OLED_ShowInit(void){
  OLED_ShowString(16,0,"LabyrinthCar",15);
  OLED_ShowString(0,2,"M L:     R:",16);
  OLED_ShowString(0,4,"D L:     R:",16);
}
void ValuesShow(void){//������ʾ��ǰ��״̬���Լ�����������ֵ
  flags.OledShow_Flag=1;
  if (flags.OledShow_Flag==1){
    OLED_ShowSignedNum(32,2,(int )motor.WheelSpeed_L,3,16);
    OLED_ShowSignedNum(88,2,(int )motor.WheelSpeed_R,3,16);
    OLED_ShowFloatNum(24,4,values.Distance_L,31,16);
    OLED_ShowFloatNum(80,4,values.Distance_R,31,16);
    OLED_ShowNum(0,6,flags.Front_Detected,1,16);
    OLED_ShowNum(32,6,flags.Back_Detected,1,16);
    OLED_ShowNum(64,6,flags.Left_Detected,1,16);
    OLED_ShowNum(96,6,flags.Right_Detected,1,16);
  } else if (flags.OledShow_Flag==0){
    OLED_Clear();
  }
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{//�ĸ����봫�����ĵ�ƽ���
  switch (GPIO_Pin) {
    case Front_Pin://�ж������Ǹ����Ŵ�����
      if (HAL_GPIO_ReadPin(Front_GPIO_Port,Front_Pin)==0){//�ж����������ش��������½��ش���
        flags.Front_Detected=TRUE;break;
      } else{flags.Front_Detected=FALSE;break;}
    case Back_Pin:
      if (HAL_GPIO_ReadPin(Back_GPIO_Port,Back_Pin)==0){
        flags.Back_Detected=TRUE;break;
      } else{flags.Back_Detected=FALSE;break;}
    case Left_Pin:
      if (HAL_GPIO_ReadPin(Left_GPIO_Port,Left_Pin)==0){
        flags.Left_Detected=TRUE;break;
      } else{flags.Left_Detected=FALSE;break;}
    case Right_Pin:
      if (HAL_GPIO_ReadPin(Right_GPIO_Port,Right_Pin)==0){
        flags.Right_Detected=TRUE;break;
      } else{flags.Right_Detected=FALSE;break;}

    default:break;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &HCSR04Timer)		//��ȡTIM3��ʱ���ĸ����жϱ�־λ
  {
    time++;
  } else if(htim==(&Encoder_TimeCounter))//100ms����һ���ж϶�ȡ����ֵ�Լ���������ֵ
  {
    Encode_CallBack();
    values.Distance_L = ScanDistance_mm(1);
    values.Distance_R = ScanDistance_mm(2);
  }
}