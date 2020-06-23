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

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for myTaskLed */
osThreadId_t myTaskLedHandle;
const osThreadAttr_t myTaskLed_attributes = {
  .name = "myTaskLed",
  .priority = (osPriority_t) osPriorityAboveNormal7,
  .stack_size = 512 * 4
};
/* Definitions for LteTask */
osThreadId_t LteTaskHandle;
const osThreadAttr_t LteTask_attributes = {
  .name = "LteTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};
/* Definitions for MutexPrintf */
osMutexId_t MutexPrintfHandle;
const osMutexAttr_t MutexPrintf_attributes = {
  .name = "MutexPrintf"
};
/* Definitions for usart1_dma_rxSem */
osSemaphoreId_t usart1_dma_rxSemHandle;
const osSemaphoreAttr_t usart1_dma_rxSem_attributes = {
  .name = "usart1_dma_rxSem"
};
/* Definitions for usart1_dma_txSem */
osSemaphoreId_t usart1_dma_txSemHandle;
const osSemaphoreAttr_t usart1_dma_txSem_attributes = {
  .name = "usart1_dma_txSem"
};
/* Definitions for usart2_dma_rxSem */
osSemaphoreId_t usart2_dma_rxSemHandle;
const osSemaphoreAttr_t usart2_dma_rxSem_attributes = {
  .name = "usart2_dma_rxSem"
};
/* Definitions for usart2_dma_txSem */
osSemaphoreId_t usart2_dma_txSemHandle;
const osSemaphoreAttr_t usart2_dma_txSem_attributes = {
  .name = "usart2_dma_txSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void Test_Send_DMA(void);   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskLed(void *argument);
void StartTaskLte(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTaskLed */
  myTaskLedHandle = osThreadNew(StartTaskLed, NULL, &myTaskLed_attributes);

  /* creation of LteTask */
  LteTaskHandle = osThreadNew(StartTaskLte, NULL, &LteTask_attributes);

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
  for(;;)
  {
      osDelay(1000);
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
  /* Infinite loop */
    for(;;)
    {
        if(osSemaphoreAcquire(usart1_dma_rxSemHandle, osWaitForever) == osOK)
        {
            HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
            Test_Send_DMA();
        }
        
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
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskLte */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
