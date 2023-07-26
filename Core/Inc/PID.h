//
// Created by mulai on 2023/7/26.
//

#ifndef LABYRINTHCAR_PID_H
#define LABYRINTHCAR_PID_H
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float error;
    float error_last;
    float integral;
    float derivative;
    float result;
} _PID;

void PID_init(_PID *pid);
float PID_realize(_PID *pid, float target, float actual);
#endif //LABYRINTHCAR_PID_H
