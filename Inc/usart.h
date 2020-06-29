/**
  ******************************************************************************
  * File Name          : USART.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define UART_BUFFSIZE 2048 // 定义缓冲区的大小
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

typedef struct
{
    __IO uint16_t len;
    uint8_t rxbuf[UART_BUFFSIZE];
    uint8_t txbuf[UART_BUFFSIZE];
} USART_DATA_T;

void Usart_IRQen_Init(void);
void Usart_IDLE_Callback(UART_HandleTypeDef *huart);
void Usart1_DMA_Send_Data(uint8_t *buf, uint16_t len);
void Usart2_DMA_Send_Data(uint8_t *buf, uint16_t len);
void Usart_Dma_TxDone_Callback(UART_HandleTypeDef *huart);
void Test_Send_DMA(void);
void TaskSend(void);
void BSP_Printf(char *format, ...);
void clean_usart_data(uint8_t COM);

USART_DATA_T *get_usart_data_fifo(uint8_t COM);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
