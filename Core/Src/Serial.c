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

#ifdef ENABLEUARTB
uint8_t WhichBufIsReady_CH2 = 0;         // 双缓存指示器。
uint8_t u1rxbuf_CH2[MAX_RX_LEN];         // 数据接收缓冲1
uint8_t u2rxbuf_CH2[MAX_RX_LEN];         // 数据接收缓冲2
uint8_t *RxData_CH2 = u2rxbuf;        // 指针——指向可以读取的那个缓冲
uint8_t *p_IsToReceive_CH2 = u1rxbuf; // 指针——指向被占用的那个缓冲

char RxDataStr_Ch2[MAX_RX_LEN];
uint8_t Rxflag_Ch2;
uint32_t data_length_Ch2;
#endif


/***********************************************************
*@fuction	:USART1_Init
*@brief		:串口初始化
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-01-17
***********************************************************/

void USART_Init(UART_HandleTypeDef *huart)
{
  // 开启串口空 闲中断
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  // 开启DMA发送通道的发送完成中断，才能实现封装发送函数里面的等待功能
  if (huart==&ENABLEUARTA) {
    __HAL_DMA_ENABLE_IT(&UART_DMA_CH1_Tx, DMA_IT_TC);
  }
#ifdef ENABLEUARTB
  if (huart==&ENABLEUARTB){
    __HAL_DMA_ENABLE_IT(&UART_DMA_CH2_Tx, DMA_IT_TC);
  }
#endif
  // 清除空闲标志位，防止中断误入
  __HAL_UART_CLEAR_IDLEFLAG(huart);
  // 立即就要打开DMA接收
  // 不然DMA没有提前准备，第一次接收的数据是读取不出来的
  if (huart==&ENABLEUARTA) {
    HAL_UART_Receive_DMA(huart, p_IsToReceive, MAX_RX_LEN);//开启接收
  }
#ifdef ENABLEUARTB
  if (huart==&ENABLEUARTB){
    HAL_UART_Receive_DMA(huart, p_IsToReceive_CH2, MAX_RX_LEN);
  }
#endif
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
	RxDataStr[data_length-1]='\0';
}
#ifdef ENABLEUARTB
void StringTrans_CH2(void)
{
  uint32_t i=0;
  while (i < data_length_Ch2)
  {
    if (RxData_CH2[i] == '#' ||RxData_CH2[i] == 0)
    {//检验是否以规定的通信方式传输
      break;
    }
    else {RxDataStr_Ch2[i]=RxData_CH2[i];}
    i++;
  }
  RxDataStr_Ch2[data_length_Ch2]='\0';
}
#endif
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

_Bool GetRxFlag(UART_HandleTypeDef *huart)
{
  if (huart==&ENABLEUARTA && Rxflag==1){
    StringTrans();Rxflag=0;return 1;
  }
#ifdef ENABLEUARTB
  else if (huart==&ENABLEUARTB && Rxflag_Ch2==1){
    StringTrans_CH2();Rxflag_Ch2=0;return 1;}
#endif
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
void MyPrintf(const char *format,...)//加载到DMA中发送//默认通道
{
  uint32_t length;
  va_list args;
  /* 清空发送缓冲区 */
  memset(TxBuf, 0, TX_BUF_LEN);
  /* 填充发送缓冲区 */
  va_start(args, format);
  length = vsnprintf((char *)TxBuf, sizeof(TxBuf), format, args);
  va_end(args);
  /* 往串口发送数据 */
  HAL_UART_Transmit_DMA(&ENABLEUARTA, (uint8_t*)&TxBuf, length);
  while(__HAL_UART_GET_FLAG(&ENABLEUARTA, UART_FLAG_TC) == RESET){}//如果数据未发完就将其卡在此处
}
/*多通道的*/
void MyPrintf_MIX(UART_HandleTypeDef *huart,const char *format,...)//加载到DMA中发送
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
  HAL_UART_Transmit_DMA(huart, (uint8_t*)&TxBuf, length);
  while(__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET){}
}

/***********************************************************
*@fuction	:USART1_IRQHandler
*@brief		:串口接收的回调函数
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-07-26
***********************************************************/

void USART_RX_CallBack(UART_HandleTypeDef *huart)
{
  if (RESET != __HAL_UART_GET_FLAG(&ENABLEUARTA, UART_FLAG_IDLE))
  {
    Rxflag = 1;
    HAL_UART_DMAStop(&ENABLEUARTA); // 把DMA接收停掉，防止速度过快导致中断重入，数据被覆写。
    data_length = MAX_RX_LEN - __HAL_DMA_GET_COUNTER(&UART_DMA_CH1_Rx);
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
    //从下面开始可以处理你接收到的数据啦！举个栗子，把你收到的数据原原本本的还回去
//    HAL_UART_Transmit_DMA(&ENABLEUARTA,RxData,data_length);//数据打回去，长度就是数据长度
    memset((uint8_t *)p_IsToReceive, 0, MAX_RX_LEN);	// 把接收数据的指针指向的缓冲区清空
  }
  HAL_UART_Receive_DMA(&ENABLEUARTA, p_IsToReceive, MAX_RX_LEN); //数据处理完毕，重新启动接收

#ifdef ENABLEUARTB
  if (RESET != __HAL_UART_GET_FLAG(&ENABLEUARTB, UART_FLAG_IDLE))
  {
    Rxflag_Ch2 = 1;
    HAL_UART_DMAStop(&ENABLEUARTB); // 把DMA接收停掉，防止速度过快导致中断重入，数据被覆写。
    data_length_Ch2 = MAX_RX_LEN - __HAL_DMA_GET_COUNTER(&UART_DMA_CH2_Rx);
    // 数据总长度=极限接收长度-DMA剩余的接收长度
    if (WhichBufIsReady_CH2)	//WhichBufIsReady=1
    {
      RxData_CH2 = u2rxbuf_CH2;        // u2rxbuf 可以读取，就绪指针指向它。
      p_IsToReceive_CH2 = u1rxbuf_CH2; // u1rxbuf 作为下一次DMA存储的缓冲，占用指针指向它。
      WhichBufIsReady_CH2 = 0;		//切换一下指示器状态
    }
    else				//WhichBufIsReady=0
    {
      RxData_CH2 = u1rxbuf_CH2;        // u1rxbuf 可以读取，就绪指针指向它。
      p_IsToReceive_CH2 = u2rxbuf_CH2; // u2rxbuf 作为下一次DMA存储的缓冲，占用指针指向它。
      WhichBufIsReady_CH2= 1;		//切换一下指示器状态
    }
    //从下面开始可以处理你接收到的数据啦！举个栗子，把你收到的数据原原本本的还回去
//    HAL_UART_Transmit_DMA(&ENABLEUARTB,RxData,data_length);//数据打回去，长度就是数据长度
    memset((uint8_t *)p_IsToReceive_CH2, 0, MAX_RX_LEN);	// 把接收数据的指针指向的缓冲区清空
  }
  HAL_UART_Receive_DMA(&ENABLEUARTB, p_IsToReceive_CH2, MAX_RX_LEN); //数据处理完毕，重新启动接收
#endif
}
/*/串口GetRxValue使用示例
 void GetValue(_Values *Value, _PID *pid)
{/
	if (strstr(RxDataStr, "SetSpeed")!=NULL)
	{//设定目标速度值
		Value->Target_Speed = GetRxValue((char *)RxDataStr, 15);
		Myprintf("Speed Seted:%.2f\r\n",Value->Target_Speed);
	}

	else if (strstr(RxDataStr, "SetKp")!=NULL)
	{//设定Kp值
		pid->Kp = GetRxValue((char *)RxDataStr, 15);
		Myprintf("Kp Seted:% 2f\r\n", pid->Kp);
	}

	else if (strstr(RxDataStr, "SetKi")!=NULL)
	{//设定Ki值
		 pid->Ki = GetRxValue((char *)RxDataStr, 15);
		Myprintf("Ki Seted:% 2f\r\n", pid->Ki);
	}

	else if (strstr(RxDataStr, "SetKd")!=NULL)
	{//设定Kd值
		 pid->Kp = GetRxValue((char *)RxDataStr, 15);
		Myprintf("Kd Seted:% 2f\r\n", pid->Kp);
	}

	else if (strstr(RxDataStr, "help")!=NULL)
	{//获取参数调整帮助
		Myprintf("调整速度：Speed;\r\n调整PID值分别为：Kp, Ki, Kd\r\n");
	}

	else if (strstr(RxDataStr, "Value")!=NULL)
	{//获取当前设定的参数值
		Myprintf("调整速度:%0f;\r\nKp:%f\r\nKi:%f\r\nKd:%f\r\n", Value->Target_Speed,
																		 pid->Kp,  pid->Ki,  pid->Kd);
	}
	else if (strstr(RxDataStr, "data")!=NULL)
	{//获取当前接收到的电磁杆和编码器的数据

	}
	else
	{
		Myprintf("Error Command!");
	}
}
 */

