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
float CurrentAngle;//当前小车的方向
float TargetAngle;//小车的目标速度


void OLED_ShowInit(void){
  OLED_ShowString(16,0,"LabyrinthCar",15);
  OLED_ShowString(0,2,"M L:     R:",16);
  OLED_ShowString(0,4,"D L:     R:",16);
}
void ValuesShow(void){//用于显示当前的状态，以及各个参数的值
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

//处理来自串口的命令
void GetRX_Order(void){
  if (strstr(RxDataStr, "RestMPU")!=NULL){//重新校准MPU6050
    MPU_REST();
    MyPrintf("OK!\r\n");
  }

  else if (strstr(RxDataStr, "SetStrKp")!=NULL)
  {//设定Kp值
    pid_Straight.Kp = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Kp);
  }	else if (strstr(RxDataStr, "SetStrKi")!=NULL)
  {//设定Ki值
    pid_Straight.Ki = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Ki);
  }	else if (strstr(RxDataStr, "SetStrKd")!=NULL)
  {//设定Kd值
    pid_Straight.Kd = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Kd);
  }

  else if (strstr(RxDataStr, "SetTurnKp")!=NULL)
  {//设定Kp值
    pid_Straight.Kp = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Kp);
  }	else if (strstr(RxDataStr, "SetTurnKi")!=NULL)
  {//设定Ki值
    pid_Straight.Ki = GetRxValue((char *)RxDataStr, 15);
    MyPrintf_MIX(&huart1,"Seted:%.2f\r\n", pid_Straight.Ki);
  }	else if (strstr(RxDataStr, "SetTurnKd")!=NULL)
  {//设定Kd值
    pid_Straight.Kd = GetRxValue((char *)RxDataStr, 15);
    MyPrintf(&huart1,"Seted:%.2f\r\n", pid_Straight.Kd);
  }

  else if (strstr(RxDataStr, "Left")!=NULL)
  {//测试程序
    flags.Front_Detected=TRUE;
    MyPrintf_MIX(&huart1,"OK!");
  }


  else if (strstr(RxDataStr, "Stop")!=NULL){
    //
  }
}
/*小车的直线控制*/
void Go_Straight(void){
  static float Edge_Error_Last;
  if (flags.Left_Detected && flags.Right_Detected){//此处需要将激光传感器的位置置于车辆的前方,如果两侧的超声波距离都是有效的
    Edge_Error=values.TargetSpeed_L-values.TargetSpeed_R;//两者做差即可使其修正到中心
  } else if (flags.Left_Detected && !flags.Right_Detected){//如果只是左侧有效
    Edge_Error = values.TargetSpeed_L - Edge_Error_Last;
    Edge_Error_Last=values.TargetSpeed_L;
  } else if (!flags.Left_Detected && flags.Right_Detected) {//如果只是右侧有效
    Edge_Error = values.TargetSpeed_R - Edge_Error_Last;
    Edge_Error_Last=values.TargetSpeed_R;
  }
  Straight_Error=StraightValue_Weight*Edge_Error+(1-StraightValue_Weight)*values.yaw;//此处简单加一个权重，表现对传感器的信任程度
  PID_realize(&pid_Straight,0,Straight_Error);
}

/*转向控制*/
void Turning(enum Turn_Direction dir)
{
  CurrentAngle=values.yaw;//读取最近一次的角度作为角度的初始
  switch (dir) {
    case Turn_Left:TargetAngle=90;//TODO：具体的角度还需要测量
      break;
    case Turn_Right:TargetAngle=-90;
      break;
    case Turn_Back:TargetAngle=180;
      break;
  }
  PID_realize(&pid_Turn,TargetAngle,CurrentAngle);
  if (pid_Turn.error<=TurnFault_Allow && pid_Turn.error>=-TurnFault_Allow){//在容错范围内认为转向成功
    flags.Turn_OK= TRUE;
//    MPU_REST();//TODO：验证是否能够让MPU6050在转向成功后校准为零度，且校准放在此处可能不大正确
  }
}
/* 小车的方向选择 */
enum Turn_Direction WhichDirection(void){
  return Turn_Left;//TODO：方向选择
}

void State_Update(void){//小车的状态更新
  if (/*flags.Front_Detected  == */TRUE)//如果前方被检测到
  {
    Turning(WhichDirection());
    values.TargetSpeed_L=CarSpeed+pid_Turn.result;
    values.TargetSpeed_R=CarSpeed-pid_Turn.result;//以速度差的方式赋值
    if (flags.Turn_OK==TRUE){flags.Front_Detected=CleanFlag;}//转向完成后在进行标志位的清楚

  } else{
    Go_Straight();
    values.TargetSpeed_L=CarSpeed+pid_Straight.result;
    values.TargetSpeed_R=CarSpeed-pid_Straight.result;//以速度差的方式赋值
  }
}

void MPUData_Updata(void)//mpu6050的数据更新
{
  mpu_dmp_get_data(&values.pitch, &values.roll, &values.yaw);
//  while(mpu_dmp_get_data(&values.pitch, &values.roll, &values.yaw));//while存在卡死情况故在此处直接读取即可
//  MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{//四个距离传感器的电平检测
  switch (GPIO_Pin) {
    case Front_Pin:flags.Front_Detected=TRUE;break;
//      if (HAL_GPIO_ReadPin(Front_GPIO_Port,Front_Pin)==0){//判断是由上升沿触发还是下降沿触发
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
  if (htim == &HCSR04Timer)		//获取TIM3定时器的更新中断标志位
  {
    HC_SR04_CounterCallBack();
  } else if(htim==(&Encoder_TimeCounter))//100ms进入一次中断读取距离值以及编码器的值
  {
    Encode_CallBack();
    values.Distance_L = ScanDistance_mm(1);
    values.Distance_R = ScanDistance_mm(2);
  }
}