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
  } else if (flags.OledShow_Flag==-1){
    OLED_Clear();
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