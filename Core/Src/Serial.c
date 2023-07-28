/*************************************** Copyright ******************************
  *
  *                 (C) Copyright 2023,--,China, CUIT. ICEC
  *                            All Rights Reserved
  *                                 By(JCML)
  * FileName   : Serial.c   
  * Version    : v2.0
  * Author     : JCML			
  * Date       : 2023-07-26
  * Description: ����Hal��Ĵ����շ����庯��,����DMA�ݴ�����Լ����͵�����
  *             ��CUBE������ʱ��������DMA�Լ����жϼ��ɣ�����Ҫ����DMAΪ����ģʽ
******************************************************************************
 */
#include <stdlib.h>
#include "main.h"
#include "Serial.h"

uint8_t  TxBuf[MAX_TX_LEN];  /* ���ͻ����� */
uint8_t u1rxbuf[MAX_RX_LEN];         // ���ݽ��ջ���1
uint8_t u2rxbuf[MAX_RX_LEN];         // ���ݽ��ջ���2
uint8_t WhichBufIsReady = 0;         // ˫����ָʾ����
// 0:u1rxbuf ��DMAռ�ý���,  u2rxbuf ���Զ�ȡ.
// 0:u2rxbuf ��DMAռ�ý���,  u1rxbuf ���Զ�ȡ.
uint8_t *RxData = u2rxbuf;        // ָ�롪��ָ����Զ�ȡ���Ǹ�����
uint8_t *p_IsToReceive = u1rxbuf; // ָ�롪��ָ��ռ�õ��Ǹ�����

char RxDataStr[MAX_RX_LEN];
uint8_t Rxflag;
uint32_t data_length;

#ifdef ENABLEUARTB
uint8_t WhichBufIsReady_CH2 = 0;         // ˫����ָʾ����
uint8_t u1rxbuf_CH2[MAX_RX_LEN];         // ���ݽ��ջ���1
uint8_t u2rxbuf_CH2[MAX_RX_LEN];         // ���ݽ��ջ���2
uint8_t *RxData_CH2 = u2rxbuf;        // ָ�롪��ָ����Զ�ȡ���Ǹ�����
uint8_t *p_IsToReceive_CH2 = u1rxbuf; // ָ�롪��ָ��ռ�õ��Ǹ�����

char RxDataStr_Ch2[MAX_RX_LEN];
uint8_t Rxflag_Ch2;
uint32_t data_length_Ch2;
#endif


/***********************************************************
*@fuction	:USART1_Init
*@brief		:���ڳ�ʼ��
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-01-17
***********************************************************/

void USART_Init(UART_HandleTypeDef *huart)
{
  // �������ڿ� ���ж�
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  // ����DMA����ͨ���ķ�������жϣ�����ʵ�ַ�װ���ͺ�������ĵȴ�����
  if (huart==&ENABLEUARTA) {
    __HAL_DMA_ENABLE_IT(&UART_DMA_CH1_Tx, DMA_IT_TC);
  }
#ifdef ENABLEUARTB
  if (huart==&ENABLEUARTB){
    __HAL_DMA_ENABLE_IT(&UART_DMA_CH2_Tx, DMA_IT_TC);
  }
#endif
  // ������б�־λ����ֹ�ж�����
  __HAL_UART_CLEAR_IDLEFLAG(huart);
  // ������Ҫ��DMA����
  // ��ȻDMAû����ǰ׼������һ�ν��յ������Ƕ�ȡ��������
  if (huart==&ENABLEUARTA) {
    HAL_UART_Receive_DMA(huart, p_IsToReceive, MAX_RX_LEN);//��������
  }
#ifdef ENABLEUARTB
  if (huart==&ENABLEUARTB){
    HAL_UART_Receive_DMA(huart, p_IsToReceive_CH2, MAX_RX_LEN);
  }
#endif
}

/***********************************************************
*@fuction	:StringTrans
*@brief		:���ַ�����ת��Ϊ�ַ���Ҳ���ڸú��������if���������ض���ͨ�ŷ�ʽ
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
		{//�����Ƿ��Թ涨��ͨ�ŷ�ʽ����
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
    {//�����Ƿ��Թ涨��ͨ�ŷ�ʽ����
      break;
    }
    else {RxDataStr_Ch2[i]=RxData_CH2[i];}
    i++;
  }
  RxDataStr_Ch2[data_length_Ch2]='\0';
}
#endif
/***********************************************************
*@���� 	:GetRxValue
*@����	:ת����ȡ�Ӵ����н������� ���Ϊfloat��
*@����	:Strָ��Ŀ���ַ�����ָ�룬 lenth�ַ����ĳ��ȣ������ַ������ȼ���
*@����ֵ	:��ȡ����ֵ
*@����	:JCML
*@����	:2023-07-26
***********************************************************/

float GetRxValue(char *Str, uint8_t lenth)
{
	uint8_t i, j;
	for (i = 0; i<lenth; i++)
	{//�ж������ָ��Ż�������ʱ��RxDataStr�е�ֵǰ��
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
*@brief		:���ؽ������ݵı�־ͬʱ����ת��Ϊ�ַ���
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
*@brief		:���¶����printf����
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-07-26
***********************************************************/
void MyPrintf(const char *format,...)//���ص�DMA�з���//Ĭ��ͨ��
{
  uint32_t length;
  va_list args;
  /* ��շ��ͻ����� */
  memset(TxBuf, 0, TX_BUF_LEN);
  /* ��䷢�ͻ����� */
  va_start(args, format);
  length = vsnprintf((char *)TxBuf, sizeof(TxBuf), format, args);
  va_end(args);
  /* �����ڷ������� */
  HAL_UART_Transmit_DMA(&ENABLEUARTA, (uint8_t*)&TxBuf, length);
  while(__HAL_UART_GET_FLAG(&ENABLEUARTA, UART_FLAG_TC) == RESET){}//�������δ����ͽ��俨�ڴ˴�
}
/*��ͨ����*/
void MyPrintf_MIX(UART_HandleTypeDef *huart,const char *format,...)//���ص�DMA�з���
{
  uint32_t length;
  va_list args;
  /* ��շ��ͻ����� */
  memset(TxBuf, 0x0, TX_BUF_LEN);
  /* ��䷢�ͻ����� */
  va_start(args, format);
  length = vsnprintf((char *)TxBuf, sizeof(TxBuf), (char *)format, args);
  va_end(args);
  /* �����ڷ������� */
  HAL_UART_Transmit_DMA(huart, (uint8_t*)&TxBuf, length);
  while(__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET){}
}

/***********************************************************
*@fuction	:USART1_IRQHandler
*@brief		:���ڽ��յĻص�����
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
    HAL_UART_DMAStop(&ENABLEUARTA); // ��DMA����ͣ������ֹ�ٶȹ��쵼���ж����룬���ݱ���д��
    data_length = MAX_RX_LEN - __HAL_DMA_GET_COUNTER(&UART_DMA_CH1_Rx);
    // �����ܳ���=���޽��ճ���-DMAʣ��Ľ��ճ���
    if (WhichBufIsReady)	//WhichBufIsReady=1
    {
      RxData = u2rxbuf;        // u2rxbuf ���Զ�ȡ������ָ��ָ������
      p_IsToReceive = u1rxbuf; // u1rxbuf ��Ϊ��һ��DMA�洢�Ļ��壬ռ��ָ��ָ������
      WhichBufIsReady = 0;		//�л�һ��ָʾ��״̬
    }
    else				//WhichBufIsReady=0
    {
      RxData = u1rxbuf;        // u1rxbuf ���Զ�ȡ������ָ��ָ������
      p_IsToReceive = u2rxbuf; // u2rxbuf ��Ϊ��һ��DMA�洢�Ļ��壬ռ��ָ��ָ������
      WhichBufIsReady = 1;		//�л�һ��ָʾ��״̬
    }
    //�����濪ʼ���Դ�������յ������������ٸ����ӣ������յ�������ԭԭ�����Ļ���ȥ
//    HAL_UART_Transmit_DMA(&ENABLEUARTA,RxData,data_length);//���ݴ��ȥ�����Ⱦ������ݳ���
    memset((uint8_t *)p_IsToReceive, 0, MAX_RX_LEN);	// �ѽ������ݵ�ָ��ָ��Ļ��������
  }
  HAL_UART_Receive_DMA(&ENABLEUARTA, p_IsToReceive, MAX_RX_LEN); //���ݴ�����ϣ�������������

#ifdef ENABLEUARTB
  if (RESET != __HAL_UART_GET_FLAG(&ENABLEUARTB, UART_FLAG_IDLE))
  {
    Rxflag_Ch2 = 1;
    HAL_UART_DMAStop(&ENABLEUARTB); // ��DMA����ͣ������ֹ�ٶȹ��쵼���ж����룬���ݱ���д��
    data_length_Ch2 = MAX_RX_LEN - __HAL_DMA_GET_COUNTER(&UART_DMA_CH2_Rx);
    // �����ܳ���=���޽��ճ���-DMAʣ��Ľ��ճ���
    if (WhichBufIsReady_CH2)	//WhichBufIsReady=1
    {
      RxData_CH2 = u2rxbuf_CH2;        // u2rxbuf ���Զ�ȡ������ָ��ָ������
      p_IsToReceive_CH2 = u1rxbuf_CH2; // u1rxbuf ��Ϊ��һ��DMA�洢�Ļ��壬ռ��ָ��ָ������
      WhichBufIsReady_CH2 = 0;		//�л�һ��ָʾ��״̬
    }
    else				//WhichBufIsReady=0
    {
      RxData_CH2 = u1rxbuf_CH2;        // u1rxbuf ���Զ�ȡ������ָ��ָ������
      p_IsToReceive_CH2 = u2rxbuf_CH2; // u2rxbuf ��Ϊ��һ��DMA�洢�Ļ��壬ռ��ָ��ָ������
      WhichBufIsReady_CH2= 1;		//�л�һ��ָʾ��״̬
    }
    //�����濪ʼ���Դ�������յ������������ٸ����ӣ������յ�������ԭԭ�����Ļ���ȥ
//    HAL_UART_Transmit_DMA(&ENABLEUARTB,RxData,data_length);//���ݴ��ȥ�����Ⱦ������ݳ���
    memset((uint8_t *)p_IsToReceive_CH2, 0, MAX_RX_LEN);	// �ѽ������ݵ�ָ��ָ��Ļ��������
  }
  HAL_UART_Receive_DMA(&ENABLEUARTB, p_IsToReceive_CH2, MAX_RX_LEN); //���ݴ�����ϣ�������������
#endif
}
/*/����GetRxValueʹ��ʾ��
 void GetValue(_Values *Value, _PID *pid)
{/
	if (strstr(RxDataStr, "SetSpeed")!=NULL)
	{//�趨Ŀ���ٶ�ֵ
		Value->Target_Speed = GetRxValue((char *)RxDataStr, 15);
		Myprintf("Speed Seted:%.2f\r\n",Value->Target_Speed);
	}

	else if (strstr(RxDataStr, "SetKp")!=NULL)
	{//�趨Kpֵ
		pid->Kp = GetRxValue((char *)RxDataStr, 15);
		Myprintf("Kp Seted:% 2f\r\n", pid->Kp);
	}

	else if (strstr(RxDataStr, "SetKi")!=NULL)
	{//�趨Kiֵ
		 pid->Ki = GetRxValue((char *)RxDataStr, 15);
		Myprintf("Ki Seted:% 2f\r\n", pid->Ki);
	}

	else if (strstr(RxDataStr, "SetKd")!=NULL)
	{//�趨Kdֵ
		 pid->Kp = GetRxValue((char *)RxDataStr, 15);
		Myprintf("Kd Seted:% 2f\r\n", pid->Kp);
	}

	else if (strstr(RxDataStr, "help")!=NULL)
	{//��ȡ������������
		Myprintf("�����ٶȣ�Speed;\r\n����PIDֵ�ֱ�Ϊ��Kp, Ki, Kd\r\n");
	}

	else if (strstr(RxDataStr, "Value")!=NULL)
	{//��ȡ��ǰ�趨�Ĳ���ֵ
		Myprintf("�����ٶ�:%0f;\r\nKp:%f\r\nKi:%f\r\nKd:%f\r\n", Value->Target_Speed,
																		 pid->Kp,  pid->Ki,  pid->Kd);
	}
	else if (strstr(RxDataStr, "data")!=NULL)
	{//��ȡ��ǰ���յ��ĵ�Ÿ˺ͱ�����������

	}
	else
	{
		Myprintf("Error Command!");
	}
}
 */

