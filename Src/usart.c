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
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "fifo.h"
/* typedef struct
{
    uint8_t *rxbuf;
    uint8_t *txbuf;
    __IO uint16_t txWrite;
    __IO uint16_t txRead;
    __IO uint16_t txLen;
    __IO uint16_t rxWrite;
    __IO uint16_t rxRead;
    __IO uint16_t rxLen;
}UART_T; */

typedef struct
{
    FIFO_T rx;
    FIFO_T tx;
} UART_T;

UART_T g_uart1;
USART_DATA_T Usart1_Data =
    {
        .len = 0,
};

USART_DATA_T Usart2_Data =
    {
        .len = 0,
};

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

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
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (uartHandle->Instance == USART1)
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
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
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

        __HAL_LINKDMA(uartHandle, hdmarx, hdma_usart1_rx);

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

        __HAL_LINKDMA(uartHandle, hdmatx, hdma_usart1_tx);

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
    else if (uartHandle->Instance == USART2)
    {
        /* USER CODE BEGIN USART2_MspInit 0 */

        /* USER CODE END USART2_MspInit 0 */
        /* USART2 clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
        GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART2 DMA Init */
        /* USART2_RX Init */
        hdma_usart2_rx.Instance = DMA1_Stream5;
        hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
        hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(uartHandle, hdmarx, hdma_usart2_rx);

        /* USART2_TX Init */
        hdma_usart2_tx.Instance = DMA1_Stream6;
        hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart2_tx.Init.Mode = DMA_NORMAL;
        hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
        hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(uartHandle, hdmatx, hdma_usart2_tx);

        /* USART2 interrupt Init */
        HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspInit 1 */

        /* USER CODE END USART2_MspInit 1 */
    }
    else if (uartHandle->Instance == USART3)
    {
        /* USER CODE BEGIN USART3_MspInit 0 */

        /* USER CODE END USART3_MspInit 0 */
        /* USART3 clock enable */
        __HAL_RCC_USART3_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USART3 interrupt Init */
        HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
        /* USER CODE BEGIN USART3_MspInit 1 */

        /* USER CODE END USART3_MspInit 1 */
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{

    if (uartHandle->Instance == USART1)
    {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

        /* USART1 DMA DeInit */
        HAL_DMA_DeInit(uartHandle->hdmarx);
        HAL_DMA_DeInit(uartHandle->hdmatx);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
    else if (uartHandle->Instance == USART2)
    {
        /* USER CODE BEGIN USART2_MspDeInit 0 */

        /* USER CODE END USART2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART2_CLK_DISABLE();

        /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);

        /* USART2 DMA DeInit */
        HAL_DMA_DeInit(uartHandle->hdmarx);
        HAL_DMA_DeInit(uartHandle->hdmatx);

        /* USART2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspDeInit 1 */

        /* USER CODE END USART2_MspDeInit 1 */
    }
    else if (uartHandle->Instance == USART3)
    {
        /* USER CODE BEGIN USART3_MspDeInit 0 */

        /* USER CODE END USART3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART3_CLK_DISABLE();

        /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);

        /* USART3 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART3_IRQn);
        /* USER CODE BEGIN USART3_MspDeInit 1 */

        /* USER CODE END USART3_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */
/*使能串口空闲中断和设置DMA接收缓冲区*/
extern osSemaphoreId_t usart1_dma_txSemHandle;
void Usart_IRQen_Init(void)
{
    //需要放在串口初始化完成后
    __HAL_DMA_DISABLE(huart1.hdmarx);

    __HAL_DMA_SET_COUNTER(huart1.hdmarx, UART_BUFFSIZE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1, Usart1_Data.rxbuf, UART_BUFFSIZE);
    CLEAR_BIT(USART1->SR, USART_SR_TC);   /* 清除 TC 发送完成标志 */
    CLEAR_BIT(USART1->SR, USART_SR_RXNE); /* 清除 RXNE 接收标志 */
    CLEAR_BIT(USART1->SR, USART_SR_IDLE);

    __HAL_DMA_DISABLE(huart2.hdmarx);

    __HAL_DMA_SET_COUNTER(huart2.hdmarx, UART_BUFFSIZE);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart2, Usart2_Data.rxbuf, UART_BUFFSIZE);
    CLEAR_BIT(USART2->SR, USART_SR_TC);   /* 清除 TC 发送完成标志 */
    CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* 清除 RXNE 接收标志 */
    CLEAR_BIT(USART2->SR, USART_SR_IDLE);

    CLEAR_BIT(USART3->SR, USART_SR_TC);   /* 清除 TC 发送完成标志 */
    CLEAR_BIT(USART3->SR, USART_SR_RXNE); /* 清除 RXNE 接收标志 */
    CLEAR_BIT(USART3->SR, USART_SR_IDLE);
    SET_BIT(USART3->CR1, USART_CR1_RXNEIE); /* 使能 PE. RX 接受中断 */
}

extern osSemaphoreId_t usart1_dma_rxSemHandle;
extern osSemaphoreId_t usart2_dma_rxSemHandle;
extern osSemaphoreId_t usart2_dma_txSemHandle;
void Usart_IDLE_Callback(UART_HandleTypeDef *huart)
{
    //__HAL_UART_CLEAR_IDLEFLAG(&huart1);
    uint32_t irReturn = taskENTER_CRITICAL_FROM_ISR();
    if ((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET))
    {
        __HAL_DMA_DISABLE(huart->hdmarx);
        Usart1_Data.len = UART_BUFFSIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);

        //清除空闲标志
        huart->Instance->SR;
        huart->Instance->DR;
        //重新开启接收
        HAL_UART_Receive_DMA(&huart1, Usart1_Data.rxbuf, UART_BUFFSIZE);
        __HAL_DMA_ENABLE(huart->hdmarx);
        osSemaphoreRelease(usart1_dma_rxSemHandle);
    }
    else if ((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET))
    {
        __HAL_DMA_DISABLE(huart->hdmarx);
        Usart2_Data.len = UART_BUFFSIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);

        //清除空闲标志
        huart->Instance->SR;
        huart->Instance->DR;
        //重新开启接收
        HAL_UART_Receive_DMA(&huart2, Usart2_Data.rxbuf, UART_BUFFSIZE);
        __HAL_DMA_ENABLE(huart->hdmarx);
        osSemaphoreRelease(usart2_dma_rxSemHandle);
    }
    taskEXIT_CRITICAL_FROM_ISR(irReturn);
}

void Usart_Dma_TxDone_Callback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        osSemaphoreRelease(usart1_dma_txSemHandle);
    }
    else if (huart->Instance == USART2)
    {
        osSemaphoreRelease(usart2_dma_txSemHandle);
    }
}
/* 串口接收回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint32_t isrflags   = READ_REG(huart->Instance->SR);
    uint32_t cr1its     = READ_REG(huart->Instance->CR1);
    uint32_t cr3its     = READ_REG(huart->Instance->CR3);
    
}
void Usart1_Send_Str(char *str)
{
    Usart1_DMA_Send_Data((uint8_t *)str, strlen(str));
}
void Usart1_DMA_Send_Data(uint8_t *buf, uint16_t len)
{
    if (len == 0)
        return;

    osSemaphoreAcquire(usart1_dma_txSemHandle, osWaitForever);
    memcpy(Usart1_Data.txbuf, buf, len);
    HAL_UART_Transmit_DMA(&huart1, Usart1_Data.txbuf, len);
}

void Usart2_DMA_Send_Data(uint8_t *buf, uint16_t len)
{
    if (len == 0)
        return;

    osSemaphoreAcquire(usart2_dma_txSemHandle, osWaitForever);
    memcpy(Usart2_Data.txbuf, buf, len);
    HAL_UART_Transmit_DMA(&huart2, Usart2_Data.txbuf, len);
}
void Test_Send_DMA(void)
{
    Usart1_DMA_Send_Data(Usart1_Data.rxbuf, Usart1_Data.len);
}

void Test2_Send_DMA(void)
{
    Usart2_DMA_Send_Data(Usart2_Data.rxbuf, Usart2_Data.len);
}
void TaskSend(void)
{
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)"Hello\r\n", 7);
}

void clean_usart_data(uint8_t COM)
{
    if (COM == 1)
    {
        memset(Usart1_Data.rxbuf, 0, sizeof(Usart1_Data.rxbuf));
        Usart1_Data.len = 0;
    }
    else if (COM == 2)
    {
        memset(Usart2_Data.rxbuf, 0, sizeof(Usart2_Data.rxbuf));
        Usart2_Data.len = 0;
    }
}
USART_DATA_T *get_usart_data_fifo(uint8_t COM)
{
    if (COM == 1)
    {
        return &Usart1_Data;
    }
    else if (COM == 2)
    {
        return &Usart2_Data;
    }
    return 0;
}
extern osMutexId_t MutexPrintfHandle;

/* 
    连续调用发送会造成DMA缓冲区数据被覆盖的问题，
    这个互斥只是Printf互斥，但是这时DMA数据还不一定已经发送完成，所以数据有可能被覆盖 
*/
void BSP_Printf(char *format, ...)
{
    uint16_t uLen;
    va_list arg;
    //uint8_t tx_buf[100];
    /* 这不知道还需不需要互斥， 因为在DMA发送时启用了二值信号量，先不管 */
    osMutexWait(MutexPrintfHandle, osWaitForever);
    va_start(arg, format);
    uLen = vsnprintf((char *)Usart1_Data.txbuf, sizeof(Usart1_Data.txbuf), (char *)format, arg);
    //uLen = vsnprintf((char *)tx_buf, sizeof(tx_buf), (char *)format, arg);
    va_end(arg);
    Usart1_DMA_Send_Data(Usart1_Data.txbuf, uLen);
    //Usart1_DMA_Send_Data(tx_buf, uLen);
    osMutexRelease(MutexPrintfHandle);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
