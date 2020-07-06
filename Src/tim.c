/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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
#include "tim.h"

/* USER CODE BEGIN 0 */
#include "usart.h"

	#define TIM_HARD					TIM2
	#define	RCC_TIM_HARD_CLK_ENABLE()	__HAL_RCC_TIM2_CLK_ENABLE()
	#define TIM_HARD_IRQn				TIM2_IRQn
	#define TIM_HARD_IRQHandler			TIM2_IRQHandler
    
    /* ���� TIM��ʱ�жϵ���ִ�еĻص�����ָ�� */
static void (*s_TIM_CallBack1)(void);
static void (*s_TIM_CallBack2)(void);
static void (*s_TIM_CallBack3)(void);
static void (*s_TIM_CallBack4)(void);
/* USER CODE END 0 */

TIM_HandleTypeDef htim2;

/* TIM2 init function */
void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFFFFFFFF;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */
    HAL_TIM_Base_Start(&htim2);
  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

/*
*********************************************************************************************************
*	�� �� ��: bsp_StartHardTimer
*	����˵��: ʹ��TIM2-5�����ζ�ʱ��ʹ��, ��ʱʱ�䵽��ִ�лص�����������ͬʱ����4����ʱ��ͨ�����������š�
*             ��ʱ��������1us ����Ҫ�ķ��ڵ��ñ�������ִ��ʱ�䣩
*			  TIM2��TIM5 ��32λ��ʱ������ʱ��Χ�ܴ�
*			  TIM3��TIM4 ��16λ��ʱ����
*	��    ��: _CC : ����Ƚ�ͨ������1��2��3, 4
*             _uiTimeOut : ��ʱʱ��, ��λ 1us. ����16λ��ʱ������� 65.5ms; ����32λ��ʱ������� 4294��
*             _pCallBack : ��ʱʱ�䵽�󣬱�ִ�еĺ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack)
{
    uint32_t cnt_now;
    uint32_t cnt_tar;
	TIM_TypeDef* TIMx = TIM_HARD;
	
    /* ���貹���ӳ٣�ʵ�⾫������1us */
    
    cnt_now = TIMx->CNT; 
    cnt_tar = cnt_now + _uiTimeOut;			/* ���㲶��ļ�����ֵ */
    if (_CC == 1)
    {
        s_TIM_CallBack1 = (void (*)(void))_pCallBack;

		TIMx->CCR1 = cnt_tar; 			    /* ���ò���Ƚϼ�����CC1 */
        TIMx->SR = (uint16_t)~TIM_IT_CC1;   /* ���CC1�жϱ�־ */
		TIMx->DIER |= TIM_IT_CC1;			/* ʹ��CC1�ж� */
	}
    else if (_CC == 2)
    {
		s_TIM_CallBack2 = (void (*)(void))_pCallBack;

		TIMx->CCR2 = cnt_tar;				/* ���ò���Ƚϼ�����CC2 */
        TIMx->SR = (uint16_t)~TIM_IT_CC2;	/* ���CC2�жϱ�־ */
		TIMx->DIER |= TIM_IT_CC2;			/* ʹ��CC2�ж� */
    }
    else if (_CC == 3)
    {
        s_TIM_CallBack3 = (void (*)(void))_pCallBack;

		TIMx->CCR3 = cnt_tar;				/* ���ò���Ƚϼ�����CC3 */
        TIMx->SR = (uint16_t)~TIM_IT_CC3;	/* ���CC3�жϱ�־ */
		TIMx->DIER |= TIM_IT_CC3;			/* ʹ��CC3�ж� */
    }
    else if (_CC == 4)
    {
        s_TIM_CallBack4 = (void (*)(void))_pCallBack;

		TIMx->CCR4 = cnt_tar;				/* ���ò���Ƚϼ�����CC4 */
        TIMx->SR = (uint16_t)~TIM_IT_CC4;	/* ���CC4�жϱ�־ */
		TIMx->DIER |= TIM_IT_CC4;			/* ʹ��CC4�ж� */
    }
	else
    {
        return;
    }
}


void Timx_CC_Callback(TIM_HandleTypeDef* tim_baseHandle)
{
    uint16_t itstatus = 0x0, itenable = 0x0;
  	itstatus = tim_baseHandle->Instance->SR & TIM_IT_CC1;
	itenable = tim_baseHandle->Instance->DIER & TIM_IT_CC1;
    
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		tim_baseHandle->Instance->SR = (uint16_t)~TIM_IT_CC1;
		tim_baseHandle->Instance->DIER &= (uint16_t)~TIM_IT_CC1;		/* ����CC1�ж� */	

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack1();
    }

	itstatus = tim_baseHandle->Instance->SR & TIM_IT_CC2;
	itenable = tim_baseHandle->Instance->DIER & TIM_IT_CC2;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		tim_baseHandle->Instance->SR = (uint16_t)~TIM_IT_CC2;
		tim_baseHandle->Instance->DIER &= (uint16_t)~TIM_IT_CC2;		/* ����CC2�ж� */	

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack2();
    }

	itstatus = tim_baseHandle->Instance->SR & TIM_IT_CC3;
	itenable = tim_baseHandle->Instance->DIER & TIM_IT_CC3;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		tim_baseHandle->Instance->SR = (uint16_t)~TIM_IT_CC3;
		tim_baseHandle->Instance->DIER &= (uint16_t)~TIM_IT_CC3;		/* ����CC2�ж� */	

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack3();
    }

	itstatus = tim_baseHandle->Instance->SR & TIM_IT_CC4;
	itenable = tim_baseHandle->Instance->DIER & TIM_IT_CC4;
	if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
	{
		tim_baseHandle->Instance->SR = (uint16_t)~TIM_IT_CC4;
		tim_baseHandle->Instance->DIER &= (uint16_t)~TIM_IT_CC4;		/* ����CC4�ж� */	

        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
        s_TIM_CallBack4();
    }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
