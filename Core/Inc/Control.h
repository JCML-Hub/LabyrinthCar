//
// Created by mulai on 2023/7/25.
//

#ifndef LABYRINTHCAR_CONTROL_H
#define LABYRINTHCAR_CONTROL_H
#include "main.h"
#include "PID.h"

#define TRUE  1
#define FALSE 0
#define CleanFlag 0

#define CarSpeed  0    //小车前进速度
#define StraightValue_Weight  0.5   //MPU6050和测距模块的影响权重
#define TurnFault_Allow  5.0   //转向容错

#define turnKp  -0.5f
#define turnKi  0.0f
#define turnKd  0.0f


typedef struct{
    float Distance_L;
    float Distance_R;
    float pitch;
    float yaw;
    float roll;
    float TargetSpeed_L;
    float TargetSpeed_R;
}_Values;

typedef struct {
    uint8_t OledShow_Flag;
    _Bool Front_Detected;
    _Bool Left_Detected;
    _Bool Right_Detected;
    _Bool Back_Detected;
    _Bool Turn_OK;
}_Flags;

enum Turn_Direction{
    Turn_Left=1,
    Turn_Right=2,
    Turn_Back=3,
    Turn_Ahead=0
};

enum Turn_Direction;

extern _Flags flags;
extern _Values values;
extern _PID pid_Straight;
extern _PID pid_Turn;

void OLED_ShowInit(void);
void ValuesShow(void);
void MPUData_Updata(void);
void GetRX_Order(void);

void State_Update(void);//小车的状态更新

#endif //LABYRINTHCAR_CONTROL_H
