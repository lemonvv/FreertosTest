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
#include "fifo.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

#define UART_BUFFSIZE 2048 // 定义缓冲区的大小

/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1 */
	COM2 = 1,	/* USART2 */
	COM3 = 2,	/* USART3 */
	COM4 = 3,	/* UART4 */
	COM5 = 4,	/* UART5 */
	COM6 = 5,	/* USART6 */
}COM_PORT_E;

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */

typedef struct
{
    __IO uint16_t len;
    uint8_t rxbuf[UART_BUFFSIZE];
    uint8_t txbuf[UART_BUFFSIZE];
} USART_DATA_T;


#define UBUFSIZE 1024
typedef struct
{
    USART_TypeDef *uart;
    FIFO_T rx;
    FIFO_T tx;
} UART_T;

extern UART_T g_uart1;
extern UART_T g_uart2;
extern UART_T g_uart3;

void Usart_IRQen_Init(void);
void Usart_IDLE_Callback(UART_HandleTypeDef *huart);
void Usart1_DMA_Send_Data(uint8_t *buf, uint16_t len);
void Usart2_DMA_Send_Data(uint8_t *buf, uint16_t len);
void Usart_Dma_TxDone_Callback(UART_HandleTypeDef *huart);
void Test_Send_DMA(void);
void TaskSend(void);
void BSP_Printf(char *format, ...);
void Usart1_Send_Str(char *str);
void clean_usart_data(uint8_t COM);
void usart_val_int(void);
void uart_rxcallback(UART_T *_uartp);
USART_DATA_T *get_usart_data_fifo(uint8_t COM);

void usart_send_data_queue(UART_T *_wuart, UART_T *_ruart);
void usart_send_data(UART_T *_wuart, uint8_t *buf, uint16_t len);
void usart_write_buf(UART_T *_ruart, uint8_t *buf, uint16_t len);
void Usart1_DMA_Send_Que(void);
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
