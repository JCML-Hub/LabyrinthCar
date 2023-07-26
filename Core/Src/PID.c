//
// Created by mulai on 2023/7/26.
//

#include "PID.h"
void PID_init(_PID *pid) {
  pid->error = 0;
  pid->error_last = 0;
  pid->integral = 0;
  pid->derivative = 0;
  pid->result = 0;
}
float PID_realize(_PID *pid, float target, float actual) {
  pid->error = target - actual;
  pid->integral += pid->error;
  pid->derivative = pid->error - pid->error_last;
  pid->error_last = pid->error;
  pid->result = pid->Kp * pid->error + pid->Ki * pid->integral + pid->Kd * pid->derivative;
  return pid->result;
}