//
// Created by mulai on 2023/7/25.
//

#include "Control.h"
#include "OLED.h"
#include "HCSR04.h"
#include "Motor.h"
#include "Serial.h"
#include "tim.h"
#include "Basic.h"
#include "inv_mpu.h"

_Values values;
_Flags flags;
_PID pid_Straight;
_PID pid_Turn;
float Edge_Error;
float Straight_Error;
float CurrentAngle;//��ǰС���ķ���
float TargetAngle;//С����Ŀ���ٶ�


void OLED_ShowInit(void){
  OLED_ShowString(16,0,"LabyrinthCar",15);
  OLED_ShowString(0,2,"M L:     R:",16);
  OLED_ShowString(0,4,"D L:     R:",16);
}
void ValuesShow(void){//������ʾ��ǰ��״̬���Լ�����������ֵ
  flags.OledShow_Flag=1;
  if (flags.OledShow_Flag==1){
    MyPrintf("%f,%f,%f\r\n",values.Distance_L,values.Distance_R,values.yaw);
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

//�������Դ��ڵ�����
void GetRX_Order(void){
  if (strstr(RxDataStr, "RestMPU")!=NULL){//����У׼MPU6050
    MPU_REST();
    MyPrintf("OK!\r\n");
  }

  else if (strstr(RxDataStr, "SetStrKp")!=NULL)
  {//�趨Kpֵ
    pid_Straight.Kp = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Kp);
  }	else if (strstr(RxDataStr, "SetStrKi")!=NULL)
  {//�趨Kiֵ
    pid_Straight.Ki = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Ki);
  }	else if (strstr(RxDataStr, "SetStrKd")!=NULL)
  {//�趨Kdֵ
    pid_Straight.Kd = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Kd);
  }

  else if (strstr(RxDataStr, "SetTurnKp")!=NULL)
  {//�趨Kpֵ
    pid_Straight.Kp = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Kp);
  }	else if (strstr(RxDataStr, "SetTurnKi")!=NULL)
  {//�趨Kiֵ
    pid_Straight.Ki = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Ki);
  }	else if (strstr(RxDataStr, "SetTurnKd")!=NULL)
  {//�趨Kdֵ
    pid_Straight.Kd = GetRxValue((char *)RxDataStr, 15);
    MyPrintf(&huart1,"Seted:%.2f\r\n", pid_Straight.Kd);
  }

  else if (strstr(RxDataStr, "Left")!=NULL)
  {//���Գ���
    flags.Front_Detected=TRUE;
    MyPrintf_MIX(&huart1,"OK!");
  }


  else if (strstr(RxDataStr, "Stop")!=NULL){
    //
  }
}
/*С����ֱ�߿���*/
void Go_Straight(void){
  static float Edge_Error_Last;
  if (flags.Left_Detected && flags.Right_Detected){//�˴���Ҫ�����⴫������λ�����ڳ�����ǰ��,�������ĳ��������붼����Ч��
    Edge_Error=values.TargetSpeed_L-values.TargetSpeed_R;//���������ʹ������������
  } else if (flags.Left_Detected && !flags.Right_Detected){//���ֻ�������Ч
    Edge_Error = values.TargetSpeed_L - Edge_Error_Last;
    Edge_Error_Last=values.TargetSpeed_L;
  } else if (!flags.Left_Detected && flags.Right_Detected) {//���ֻ���Ҳ���Ч
    Edge_Error = values.TargetSpeed_R - Edge_Error_Last;
    Edge_Error_Last=values.TargetSpeed_R;
  }
  Straight_Error=StraightValue_Weight*Edge_Error+(1-StraightValue_Weight)*values.yaw;//�˴��򵥼�һ��Ȩ�أ����ֶԴ����������γ̶�
  PID_realize(&pid_Straight,0,Straight_Error);
}

/*ת�����*/
void Turning(enum Turn_Direction dir)
{
  CurrentAngle=values.yaw;//��ȡ���һ�εĽǶ���Ϊ�Ƕȵĳ�ʼ
  switch (dir) {
    case Turn_Left:TargetAngle=90;//TODO������ĽǶȻ���Ҫ����
      break;
    case Turn_Right:TargetAngle=-90;
      break;
    case Turn_Back:TargetAngle=180;
      break;
  }
  PID_realize(&pid_Turn,TargetAngle,CurrentAngle);
  if (pid_Turn.error<=TurnFault_Allow && pid_Turn.error>=-TurnFault_Allow){//���ݴ�Χ����Ϊת��ɹ�
    flags.Turn_OK= TRUE;
//    MPU_REST();//TODO����֤�Ƿ��ܹ���MPU6050��ת��ɹ���У׼Ϊ��ȣ���У׼���ڴ˴����ܲ�����ȷ
  }
}
/* С���ķ���ѡ�� */
enum Turn_Direction WhichDirection(void){
  return Turn_Left;//TODO������ѡ��
}

void State_Update(void){//С����״̬����
  if (/*flags.Front_Detected  == */TRUE)//���ǰ������⵽
  {
    Turning(WhichDirection());
    values.TargetSpeed_L=CarSpeed+pid_Turn.result;
    values.TargetSpeed_R=CarSpeed-pid_Turn.result;//���ٶȲ�ķ�ʽ��ֵ
    if (flags.Turn_OK==TRUE){flags.Front_Detected=CleanFlag;}//ת����ɺ��ڽ��б�־λ�����

  } else{
    Go_Straight();
    values.TargetSpeed_L=CarSpeed+pid_Straight.result;
    values.TargetSpeed_R=CarSpeed-pid_Straight.result;//���ٶȲ�ķ�ʽ��ֵ
  }
}

void MPUData_Updata(void)//mpu6050�����ݸ���
{
  mpu_dmp_get_data(&values.pitch, &values.roll, &values.yaw);
//  while(mpu_dmp_get_data(&values.pitch, &values.roll, &values.yaw));//while���ڿ���������ڴ˴�ֱ�Ӷ�ȡ����
//  MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{//�ĸ����봫�����ĵ�ƽ���
  switch (GPIO_Pin) {
    case Front_Pin:flags.Front_Detected=TRUE;break;
//      if (HAL_GPIO_ReadPin(Front_GPIO_Port,Front_Pin)==0){//�ж����������ش��������½��ش���
//        flags.Front_Detected=TRUE;break;
//      } else{flags.Front_Detected=FALSE;break;}
    case Back_Pin:flags.Back_Detected=TRUE;break;
    case Left_Pin:flags.Left_Detected=TRUE;break;
    case Right_Pin:flags.Right_Detected=TRUE;break;
    case IN_Pin:{HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);MPUData_Updata();
      break;}
    default:break;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &HCSR04Timer)		//��ȡTIM3��ʱ���ĸ����жϱ�־λ
  {
    HC_SR04_CounterCallBack();
  } else if(htim==(&Encoder_TimeCounter))//100ms����һ���ж϶�ȡ����ֵ�Լ���������ֵ
  {
    Encode_CallBack();
    values.Distance_L = ScanDistance_mm(1);
    values.Distance_R = ScanDistance_mm(2);
  }
}