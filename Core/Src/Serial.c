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
#include "usart.h"
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


/***********************************************************
*@fuction	:USART1_Init
*@brief		:���ڳ�ʼ��
*@param		:void
*@return	:void
*@author	:JCML
*@date		:2023-01-17
***********************************************************/

void USART1_Init(void)
{
  // ��������1�� ���ж�
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  // ����DMA����ͨ���ķ�������жϣ�����ʵ�ַ�װ���ͺ�������ĵȴ�����
  __HAL_DMA_ENABLE_IT(&hdma_usart1_tx, DMA_IT_TC);
  // ������б�־λ����ֹ�ж�����
  __HAL_UART_CLEAR_IDLEFLAG(&huart1);
  // ������Ҫ��DMA����
  // ��ȻDMAû����ǰ׼������һ�ν��յ������Ƕ�ȡ��������
  HAL_UART_Receive_DMA(&huart1, p_IsToReceive, MAX_RX_LEN);
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
	RxDataStr[data_length]='\0';
}
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

_Bool GetRxFlag(void)
{
	if (Rxflag==1){StringTrans();Rxflag=0;return 1;}
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
void MyPrintf(const char *format, ...)//���ص�DMA�з���
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
  HAL_UART_Transmit_DMA(&UARTA, (uint8_t*)&TxBuf, length);
}

/***********************************************************
*@fuction	:USART1_IRQHandler
*@brief		:���ڽ��������жϴ���
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
    HAL_UART_DMAStop(&UARTA); // ��DMA����ͣ������ֹ�ٶȹ��쵼���ж����룬���ݱ���д��
	
    data_length = MAX_RX_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
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
    //�����濪ʼ���Դ�������յ����������������յ�������ԭԭ�����Ļ���ȥ
    //DMA_USART1_Tx_Data(RxData,data_length);//���ݴ��ȥ�����Ⱦ������ݳ���
    memset((uint8_t *)p_IsToReceive, 0, MAX_RX_LEN);	// �ѽ������ݵ�ָ��ָ��Ļ��������
    HAL_UART_Receive_DMA(&UARTA, p_IsToReceive, MAX_RX_LEN); //���ݴ�����ϣ�������������
  }
}

