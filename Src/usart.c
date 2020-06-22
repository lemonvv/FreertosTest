/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "cmsis_os.h"
#include "string.h"
#define UART_BUFFSIZE    2048   // 定义缓冲区的大小
typedef struct
{
	uint16_t Uart_SendLens;  //待发送数据长度
	uint16_t Uart_RecvLens;  //接收到的数据长度
	uint16_t RecvQue_Head;   //新接收数据环形队列头指针
	uint16_t RecvQue_Tail;   //新接收数据环形队列尾指针
	uint8_t Uart_SentBuff[UART_BUFFSIZE];
	uint8_t Uart_RecvBuff[UART_BUFFSIZE];
}UART_STR;
UART_STR USART1_Que;

typedef struct
{
    uint16_t len;
    uint8_t rxbuf[UART_BUFFSIZE];
    uint8_t txbuf[UART_BUFFSIZE];
}USART_DATA_T;
USART_DATA_T Usart1_comData = 
{
    .len = 0,
};

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */
    //不能放这里，需要等待串口初始化硬件初始化完成再调用
//    __HAL_DMA_DISABLE(huart1.hdmarx);
//    CLEAR_BIT(USART1->SR, USART_SR_TC); /* 清除 TC 发送完成标志 */
//    CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* 清除 RXNE 接收标志 */
//    __HAL_DMA_SET_COUNTER(huart1.hdmarx, UART_BUFFSIZE);
//    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
//    HAL_UART_Receive_DMA(&huart1, Usart1_RxData.rxbuf, UART_BUFFSIZE); 
  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/*使能串口空闲中断和设置DMA接收缓冲区*/
void Usart_IRQen_Init(void)
{
      //需要放在串口初始化完成后
    __HAL_DMA_DISABLE(huart1.hdmarx);
    CLEAR_BIT(USART1->SR, USART_SR_TC); /* 清除 TC 发送完成标志 */
    CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* 清除 RXNE 接收标志 */
    __HAL_DMA_SET_COUNTER(huart1.hdmarx, UART_BUFFSIZE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1, Usart1_comData.rxbuf, UART_BUFFSIZE); 
}
extern osSemaphoreId_t myBinarySem01Handle;
void Usart_IDLE_Callback(UART_HandleTypeDef *huart)
{
    //__HAL_UART_CLEAR_IDLEFLAG(&huart1);
    uint32_t irReturn = taskENTER_CRITICAL_FROM_ISR();
    if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) != RESET))  
    {   
        __HAL_DMA_DISABLE(huart1.hdmarx);
        Usart1_comData.len = UART_BUFFSIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
        
        //清除空闲标志
        huart1.Instance->SR;
        huart1.Instance->DR;
        //重新开启接收
        HAL_UART_Receive_DMA(&huart1, Usart1_comData.rxbuf, UART_BUFFSIZE);
        __HAL_DMA_ENABLE(huart1.hdmarx);
        osSemaphoreRelease(myBinarySem01Handle);

    } 
    taskEXIT_CRITICAL_FROM_ISR(irReturn);
    
}

void Usart_DMA_Send_Data(uint8_t *buf, uint16_t len)
{
    memcpy(Usart1_comData.txbuf, buf, len);
    HAL_UART_Transmit_DMA(&huart1, Usart1_comData.txbuf, len);
}

void Test_Send_DMA(void)
{
    Usart_DMA_Send_Data(Usart1_comData.rxbuf, Usart1_comData.len);
}

void TaskSend(void)
{
    HAL_UART_Transmit_DMA(&huart1,(uint8_t *)"Hello\r\n", 7);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
