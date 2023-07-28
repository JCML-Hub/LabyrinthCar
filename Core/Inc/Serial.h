#ifndef __SERIAL_H__
#define __SERIAL_H__

#define TX_BUF_LEN  512     /* 发送缓冲区容量，根据需要进行调整 */

#define MAX_RX_LEN (256U) // 一次性可以接受的数据字节长度，你可以自己定义。U是Unsigned的意思。
#define MAX_TX_LEN (512U) // 一次性可以发送的数据字节长度，你可以自己定义。

#include "stdio.h"
#include "string.h"
#include <stdarg.h>
#include "usart.h"

/*使能串口，同时定义其接口*/
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
void MyPrintf(const char *format,...);//默认以 ENABLEUARTA 通信
void MyPrintf_MIX(UART_HandleTypeDef *huart,const char *format,...);
_Bool GetRxFlag(UART_HandleTypeDef *huart);
void StringTrans(void);
float GetRxValue(char *Str, uint8_t lenth);
void USART_RX_CallBack(UART_HandleTypeDef *huart);//需要将其置于 "stm32f1xx_it.h" 中
#endif