/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "queue.h"
#include "semphr.h"
#include "lte.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* 创建队列集合句柄 */
QueueSetHandle_t xQueueSet = NULL;
SemaphoreHandle_t xSemaphore = NULL;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};
/* Definitions for myTaskLed */
osThreadId_t myTaskLedHandle;
const osThreadAttr_t myTaskLed_attributes = {
    .name = "myTaskLed",
    .priority = (osPriority_t)osPriorityAboveNormal7,
    .stack_size = 512 * 4};
/* Definitions for LteTask */
osThreadId_t LteTaskHandle;
const osThreadAttr_t LteTask_attributes = {
    .name = "LteTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 256 * 4};
/* Definitions for myTaskUsart3 */
osThreadId_t myTaskUsart3Handle;
const osThreadAttr_t myTaskUsart3_attributes = {
    .name = "myTaskUsart3",
    .priority = (osPriority_t)osPriorityLow7,
    .stack_size = 128 * 4};
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
    .name = "myQueue01"};
/* Definitions for MutexPrintf */
osMutexId_t MutexPrintfHandle;
const osMutexAttr_t MutexPrintf_attributes = {
    .name = "MutexPrintf"};
/* Definitions for usart1_dma_rxSem */
osSemaphoreId_t usart1_dma_rxSemHandle;
const osSemaphoreAttr_t usart1_dma_rxSem_attributes = {
    .name = "usart1_dma_rxSem"};
/* Definitions for usart1_dma_txSem */
osSemaphoreId_t usart1_dma_txSemHandle;
const osSemaphoreAttr_t usart1_dma_txSem_attributes = {
    .name = "usart1_dma_txSem"};
/* Definitions for usart2_dma_rxSem */
osSemaphoreId_t usart2_dma_rxSemHandle;
const osSemaphoreAttr_t usart2_dma_rxSem_attributes = {
    .name = "usart2_dma_rxSem"};
/* Definitions for usart2_dma_txSem */
osSemaphoreId_t usart2_dma_txSemHandle;
const osSemaphoreAttr_t usart2_dma_txSem_attributes = {
    .name = "usart2_dma_txSem"};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void Test_Send_DMA(void);
extern void Test2_Send_DMA(void);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskLed(void *argument);
void StartTaskLte(void *argument);
void StartTaskUsart3(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */
    /* Create the mutex(es) */
    /* creation of MutexPrintf */
    MutexPrintfHandle = osMutexNew(&MutexPrintf_attributes);

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* Create the semaphores(s) */
    /* creation of usart1_dma_rxSem */
    usart1_dma_rxSemHandle = osSemaphoreNew(1, 1, &usart1_dma_rxSem_attributes);

    /* creation of usart1_dma_txSem */
    usart1_dma_txSemHandle = osSemaphoreNew(1, 1, &usart1_dma_txSem_attributes);

    /* creation of usart2_dma_rxSem */
    usart2_dma_rxSemHandle = osSemaphoreNew(1, 1, &usart2_dma_rxSem_attributes);

    /* creation of usart2_dma_txSem */
    usart2_dma_txSemHandle = osSemaphoreNew(1, 1, &usart2_dma_txSem_attributes);

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the queue(s) */
    /* creation of myQueue01 */
    myQueue01Handle = osMessageQueueNew(2, sizeof(uint8_t), &myQueue01_attributes);

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* 如果不调用一次信号获取，在写进队列集合时会出错，不知道是不是封装的API BUG 原生API创建的二值信号没这个问题*/
    osSemaphoreAcquire(usart1_dma_rxSemHandle, 1);
    osSemaphoreAcquire(usart2_dma_rxSemHandle, 1);
    /* 创建队列集合，长度为2 用于存放两个串口的二值信号量 */
    xQueueSet = xQueueCreateSet(2);
    if (xQueueAddToSet(usart1_dma_rxSemHandle, xQueueSet) != pdPASS)
    {
    }
    xQueueAddToSet(usart2_dma_rxSemHandle, xQueueSet);
    //osSemaphoreRelease(usart1_dma_rxSemHandle);
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* creation of myTaskLed */
    myTaskLedHandle = osThreadNew(StartTaskLed, NULL, &myTaskLed_attributes);

    /* creation of LteTask */
    LteTaskHandle = osThreadNew(StartTaskLte, NULL, &LteTask_attributes);

    /* creation of myTaskUsart3 */
    myTaskUsart3Handle = osThreadNew(StartTaskUsart3, NULL, &myTaskUsart3_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */

    /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;)
    {
        osDelay(500);
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        //BSP_Printf("hello\r\n");
    }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskLed */
/**
* @brief Function implementing the myTaskLed thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskLed */
void StartTaskLed(void *argument)
{
    /* USER CODE BEGIN StartTaskLed */
    QueueSetMemberHandle_t xActivatedMember;
    /* Infinite loop */
    for (;;)
    {
#if 0
        if(osSemaphoreAcquire(usart1_dma_rxSemHandle, osWaitForever) == osOK)
        {
            HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
            Test_Send_DMA();
        }
#else
        /* 多事件等待 等待两个串口的二值信号量 */
        xActivatedMember = xQueueSelectFromSet(xQueueSet, osWaitForever);
        if (xActivatedMember == usart1_dma_rxSemHandle)
        {
            osSemaphoreAcquire(xActivatedMember, 0);
            HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
            USART_DATA_T *udata = get_usart_data_fifo(1);
            usart_write_buf(&g_uart1, udata->rxbuf, udata->len);
            //Test_Send_DMA();
        }
        else if (xActivatedMember == usart2_dma_rxSemHandle)
        {
            osSemaphoreAcquire(xActivatedMember, 0);
            HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
            USART_DATA_T *udata = get_usart_data_fifo(2);
            usart_write_buf(&g_uart2, udata->rxbuf, udata->len);
            //Test2_Send_DMA();
        }
#endif
        //osDelay(1);
    }
    /* USER CODE END StartTaskLed */
}

/* USER CODE BEGIN Header_StartTaskLte */
/**
* @brief Function implementing the LteTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskLte */
void StartTaskLte(void *argument)
{
    /* USER CODE BEGIN StartTaskLte */
    /* Infinite loop */
    for (;;)
    {
        ec20_run();
        osDelay(1);
    }
    /* USER CODE END StartTaskLte */
}

/* USER CODE BEGIN Header_StartTaskUsart3 */
/**
* @brief Function implementing the myTaskUsart3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUsart3 */
void StartTaskUsart3(void *argument)
{
    /* USER CODE BEGIN StartTaskUsart3 */
    /* Infinite loop */
    for (;;)
    {
        usart_send_data_queue(&g_uart3, &g_uart3);
        usart_send_data_queue(&g_uart3, &g_uart1);
        //osDelay(1000);
    }
    /* USER CODE END StartTaskUsart3 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
