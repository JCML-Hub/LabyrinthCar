//
// Created by mulai on 2023/7/25.
//

#ifndef LABYRINTHCAR_CONTROL_H
#define LABYRINTHCAR_CONTROL_H
#include "main.h"

typedef struct{
    float Distance_L;
    float Distance_R;
    float pich;
}_Values;
typedef struct {
    uint8_t OledShow_Flag;
}_Flags;

extern _Flags flags;
extern _Values values;

void OLED_ShowInit(void);
void ValuesShow(void);
#endif //LABYRINTHCAR_CONTROL_H
