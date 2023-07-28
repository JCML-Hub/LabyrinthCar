#ifndef __SERIAL_H__
#define __SERIAL_H__

#define TX_BUF_LEN  512     /* ���ͻ�����������������Ҫ���е��� */

#define MAX_RX_LEN (256U) // һ���Կ��Խ��ܵ������ֽڳ��ȣ�������Լ����塣U��Unsigned����˼��
#define MAX_TX_LEN (512U) // һ���Կ��Է��͵������ֽڳ��ȣ�������Լ����塣

#include "stdio.h"
#include "string.h"
#include <stdarg.h>
#include "usart.h"

/*ʹ�ܴ��ڣ�ͬʱ������ӿ�*/
#define ENABLEUARTA huart1
#define ENABLEUARTB huart2

#ifdef ENABLEUARTA
extern char RxDataStr[MAX_RX_LEN];

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
#define UART_DMA_CH1_Rx hdma_usart1_rx
#define UART_DMA_CH1_Tx hdma_usart1_tx

#endif

#ifdef ENABLEUARTB
extern char RxDataStr_Ch2[MAX_RX_LEN];

extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
#define UART_DMA_CH2_Rx hdma_usart2_rx
#define UART_DMA_CH2_Tx hdma_usart2_tx

#endif

void USART_Init(UART_HandleTypeDef *huart);
void MyPrintf(const char *format,...);//Ĭ���� ENABLEUARTA ͨ��
void MyPrintf_MIX(UART_HandleTypeDef *huart,const char *format,...);
_Bool GetRxFlag(UART_HandleTypeDef *huart);
void StringTrans(void);
float GetRxValue(char *Str, uint8_t lenth);
void USART_RX_CallBack(UART_HandleTypeDef *huart);//��Ҫ�������� "stm32f1xx_it.h" ��
#endif