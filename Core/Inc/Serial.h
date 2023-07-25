#ifndef __SERIAL_H__
#define __SERIAL_H__

#define TX_BUF_LEN  512     /* ���ͻ�����������������Ҫ���е��� */

#define MAX_RX_LEN (256U) // һ���Կ��Խ��ܵ������ֽڳ��ȣ�������Լ����塣U��Unsigned����˼��
#define MAX_TX_LEN (512U) // һ���Կ��Է��͵������ֽڳ��ȣ�������Լ����塣
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