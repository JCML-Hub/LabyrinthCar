#ifndef __HCSR04_H
#define __HCSR04_H

#define HCSR04Timer	htim7

void HC_SR04_Init(void);
void HC_SR04_CounterCallBack(void);
float GetDistance_CH1(void);
float GetDistance_CH2(void);
float ScanDistance_mm(uint8_t Which);//返回单位为毫米的距离结果


#endif
