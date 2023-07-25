#ifndef __SERIAL_H__
#define __SERIAL_H__

#define TX_BUF_LEN  512     /* 发送缓冲区容量，根据需要进行调整 */

#define MAX_RX_LEN (256U) // 一次性可以接受的数据字节长度，你可以自己定义。U是Unsigned的意思。
#define MAX_TX_LEN (512U) // 一次性可以发送的数据字节长度，你可以自己定义。
#include "stdio.h"
#include "string.h"
#include <stdarg.h>

#define UARTA huart1


extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

extern uint8_t *RxData;
extern uint8_t *p_IsToReceive;
extern uint8_t WhichBufIsReady;
extern char RxDataStr[MAX_RX_LEN];


void USART1_Init(void);
void MyPrintf(const char *__format, ...);
_Bool GetRxFlag(void);
void StringTrans(void);
float GetRxValue(char *Str, uint8_t lenth);
#endif