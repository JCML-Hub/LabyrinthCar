//
// Created by mulai on 2023/7/25.
//

#ifndef LABYRINTHCAR_CONTROL_H
#define LABYRINTHCAR_CONTROL_H
#include "main.h"

#define TRUE  1
#define FALSE 0

typedef struct{
    float Distance_L;
    float Distance_R;
    float pich;
}_Values;
typedef struct {
    uint8_t OledShow_Flag;
    _Bool Front_Detected;
    _Bool Left_Detected;
    _Bool Right_Detected;
    _Bool Back_Detected;
}_Flags;

extern _Flags flags;
extern _Values values;

void OLED_ShowInit(void);
void ValuesShow(void);
#endif //LABYRINTHCAR_CONTROL_H
