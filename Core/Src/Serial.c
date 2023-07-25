/*************************************** Copyright ******************************
  *
  *                 (C) Copyright 2023,--,China, CUIT. ICEC
  *                            All Rights Reserved
  *                                 By(JCML)
  * FileName   : Serial.c   
  * Version    : v2.0
  * Author     : JCML			
  * Date       : 2023-07-26
  * Description: 基于Hal库的串口收发定义函数,加有DMA暂存接收以及发送的数据
  *             在CUBE中配置时候仅需添加DMA以及打开中断即可，不需要配置DMA为连续模式
******************************************************************************
 */
#include <stdlib.h>
#include "main.h"
#include "usart.h"
#include "Serial.h"

uint8_t  TxBuf[MAX_TX_LEN];  /* 发送缓冲区 */
uint8_t u1rxbuf[MAX_RX_LEN];         // 数据接收缓冲1
uint8_t u2rxbuf[MAX_RX_LEN];         // 数据接收缓冲2
uint8_t WhichBufIsReady = 0;         // 双缓存指示器。
// 0:u1rxbuf 被DMA占用接收,  u2rxbuf 可以读取.
// 0:u2rxbuf 被DMA占用接收,  u1rxbuf 可以读取.
uint8_t *RxData = u2rxbuf;        // 指针——指向可以读取的那个缓冲
uint8_t *p_IsToReceive = u1rxbuf; // 指针——指向被占用的那个缓冲

char RxDataStr[MAX_RX_LEN];
uint8_t Rxflag;
uint32_t data_length;


/***********************************************************
*@fuction	:USART1_Init
*@brief		:串口初始化
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-01-17
***********************************************************/

void USART1_Init(void)
{
  // 开启串口1空 闲中断
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  // 开启DMA发送通道的发送完成中断，才能实现封装发送函数里面的等待功能
  __HAL_DMA_ENABLE_IT(&hdma_usart1_tx, DMA_IT_TC);
  // 清除空闲标志位，防止中断误入
  __HAL_UART_CLEAR_IDLEFLAG(&huart1);
  // 立即就要打开DMA接收
  // 不然DMA没有提前准备，第一次接收的数据是读取不出来的
  HAL_UART_Receive_DMA(&huart1, p_IsToReceive, MAX_RX_LEN);
}

/***********************************************************
*@fuction	:StringTrans
*@brief		:将字符数组转化为字符串也可在该函数中添加if条件构成特定的通信方式
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-07-26
***********************************************************/

void StringTrans(void)
{
	uint32_t i=0;
	while (i < data_length)
	{
		if (RxData[i] == '#' ||RxData[i] == 0)
		{//检验是否以规定的通信方式传输
			break;
		}
		else {RxDataStr[i]=RxData[i];}
		i++;
	}
	RxDataStr[data_length]='\0';
}
/***********************************************************
*@名称 	:GetRxValue
*@描述	:转换获取从串口中接收数字 输出为float型
*@参数	:Str指向目标字符串的指针， lenth字符串的长度，大于字符串长度即可
*@返回值	:提取出的值
*@作者	:JCML
*@日期	:2023-07-26
***********************************************************/

float GetRxValue(char *Str, uint8_t lenth)
{
	uint8_t i, j;
	for (i = 0; i<lenth; i++)
	{//判读当出现负号或者数字时将RxDataStr中的值前移
		if ((Str[i]=='-'&&Str[i+1]<='9'&&Str[i+1]>='0')||
															(Str[i]<='9'&&Str[i]>='0'))
		{
			for(j=0;Str[j]!='\0';j++)
			{
				Str[j] = Str[j+i];
			}
			Str[j]='\0';
			break;
		}
	}
	return atof(Str);//
}


/***********************************************************
*@fuction	:GetRxFlag
*@brief		:返回接收数据的标志同时将其转换为字符串
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-07-26
***********************************************************/

_Bool GetRxFlag(void)
{
	if (Rxflag==1){StringTrans();Rxflag=0;return 1;}
	else {return 0;}
}
/***********************************************************
*@fuction	:MyPrintf
*@brief		:重新定义的printf函数
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-07-26
***********************************************************/
void MyPrintf(const char *format, ...)//加载到DMA中发送
{
  uint32_t length;
  va_list args;
  /* 清空发送缓冲区 */
  memset(TxBuf, 0x0, TX_BUF_LEN);
  /* 填充发送缓冲区 */
  va_start(args, format);
  length = vsnprintf((char *)TxBuf, sizeof(TxBuf), (char *)format, args);
  va_end(args);
  /* 往串口发送数据 */
  HAL_UART_Transmit_DMA(&UARTA, (uint8_t*)&TxBuf, length);
}

/***********************************************************
*@fuction	:USART1_IRQHandler
*@brief		:串口接收数据中断处理
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-07-26
***********************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart==&UARTA)
  { 
	  Rxflag = 1;
    HAL_UART_DMAStop(&UARTA); // 把DMA接收停掉，防止速度过快导致中断重入，数据被覆写。
	
    data_length = MAX_RX_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
    // 数据总长度=极限接收长度-DMA剩余的接收长度
    if (WhichBufIsReady)	//WhichBufIsReady=1
    {
      RxData = u2rxbuf;        // u2rxbuf 可以读取，就绪指针指向它。
      p_IsToReceive = u1rxbuf; // u1rxbuf 作为下一次DMA存储的缓冲，占用指针指向它。
      WhichBufIsReady = 0;		//切换一下指示器状态
    }
    else				//WhichBufIsReady=0
    {
      RxData = u1rxbuf;        // u1rxbuf 可以读取，就绪指针指向它。
      p_IsToReceive = u2rxbuf; // u2rxbuf 作为下一次DMA存储的缓冲，占用指针指向它。
      WhichBufIsReady = 1;		//切换一下指示器状态
    }
    //从下面开始可以处理你接收到的数据啦！把你收到的数据原原本本的还回去
    //DMA_USART1_Tx_Data(RxData,data_length);//数据打回去，长度就是数据长度
    memset((uint8_t *)p_IsToReceive, 0, MAX_RX_LEN);	// 把接收数据的指针指向的缓冲区清空
    HAL_UART_Receive_DMA(&UARTA, p_IsToReceive, MAX_RX_LEN); //数据处理完毕，重新启动接收
  }
}

