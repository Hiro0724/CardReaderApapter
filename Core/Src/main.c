/* USER CODE BEGIN Header */
/**

















  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ypserial.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define debag


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/********************************************************************/
/*  外部参照宣言	                                                */
/********************************************************************/
//extern void xxxx(void);
//extern	uint8_t	;


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_tx;

osThreadId MainControlTaskHandle;
uint32_t MainControlTaskBuffer[ 128 ];
osStaticThreadDef_t MainControlTaskControlBlock;
osThreadId CyclicHandle;
uint32_t CyclicBuffer[ 128 ];
osStaticThreadDef_t CyclicControlBlock;
osThreadId IdleHandle;
uint32_t IdleBuffer[ 128 ];
osStaticThreadDef_t IdleControlBlock;
osThreadId YpSerialRxHandle;
uint32_t YpSerialRxBuffer[ 128 ];
osStaticThreadDef_t YpSerialRxControlBlock;
osThreadId YpSerialTxHandle;
uint32_t YpSerialTxBuffer[ 128 ];
osStaticThreadDef_t YpSerialTxControlBlock;
osThreadId CrSerialRxHandle;
uint32_t CrSerialRxBuffer[ 128 ];
osStaticThreadDef_t CrSerialRxControlBlock;
osThreadId CrSerialTxHandle;
uint32_t CrSerialTxBuffer[ 128 ];
osStaticThreadDef_t CrSerialTxControlBlock;
osThreadId SubTaskHandle;
uint32_t SubTaskBuffer[ 128 ];
osStaticThreadDef_t SubTaskControlBlock;
osMessageQId YpSerialRxQueueHandle;
osMessageQId CrSerialRxQueueHandle;
osMessageQId WarningReqQueueHandle;
uint8_t WarningReqQueueBuffer[ 16 * sizeof( uint8_t ) ];
osStaticMessageQDef_t WarningReqQueueControlBlock;
osTimerId CyclicTimerHandle;
osStaticTimerDef_t CyclicTimerControlBlock;
osMutexId myMutex01Handle;
osStaticMutexDef_t myMutex01ControlBlock;
/* USER CODE BEGIN PV */
osMailQId YpSerialRxMailHandle;
osMailQId YpSerialTxMailHandle;
osMailQId CrSerialRxMailHandle;
osMailQId CrSerialTxMailHandle;
osMailQId MainControlMailHandle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_IWDG_Init(void);
void MainControlTaskEntry(void const * argument);
void CyclicTaskEntry(void const * argument);
void IdleTaskEntry(void const * argument);
void YpSerialRxTaskEntry(void const * argument);
void YpSerialTxTaskEntry(void const * argument);
void CrSerialRxTaskEntry(void const * argument);
void CrSerialTxTaskEntry(void const * argument);
void SubTaskEntry(void const * argument);
void CallbackCyclicTimer(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  WAIT_LOOP_MS(1000);	// 電源投入安定時間
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

//  MX_TIM2_Init();

#ifndef debag
  MX_IWDG_Init();
#endif



  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */
  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
//  osMutexStaticDef(myMutex01, &myMutex01ControlBlock);
//  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of CyclicTimer */
  osTimerStaticDef(CyclicTimer, CallbackCyclicTimer, &CyclicTimerControlBlock);
  CyclicTimerHandle = osTimerCreate(osTimer(CyclicTimer), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of YpSerialRxQueue */
  osMessageQDef(YpSerialRxQueue, 10, uint8_t);
  YpSerialRxQueueHandle = osMessageCreate(osMessageQ(YpSerialRxQueue), NULL);

  /* definition and creation of CrSerialRxQueue */
  osMessageQDef(CrSerialRxQueue, 100, uint8_t);
  CrSerialRxQueueHandle = osMessageCreate(osMessageQ(CrSerialRxQueue), NULL);

  /* definition and creation of WarningReqQueue */
  osMessageQStaticDef(WarningReqQueue, 16, uint8_t, WarningReqQueueBuffer, &WarningReqQueueControlBlock);
  WarningReqQueueHandle = osMessageCreate(osMessageQ(WarningReqQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  osMailQDef(YpSerialTxMail, 2, YpSerialBuff_t);
  YpSerialTxMailHandle = osMailCreate(osMailQ(YpSerialTxMail), NULL);

  osMailQDef(YpSerialRxMail, 2, YpSerialBuff_t);
  YpSerialRxMailHandle = osMailCreate(osMailQ(YpSerialRxMail), NULL);

  osMailQDef(CrSerialTxMail, 2, CrSerialBuff_t);
  CrSerialTxMailHandle = osMailCreate(osMailQ(CrSerialTxMail), NULL);

  osMailQDef(CrSerialRxMail, 2, CrSerialBuff_t);
  CrSerialRxMailHandle = osMailCreate(osMailQ(CrSerialRxMail), NULL);

  osMailQDef(MainControlMai, 2, CrSerialBuff_t);
  MainControlMailHandle = osMailCreate(osMailQ(MainControlMai), NULL);

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of MainControlTask */
  osThreadStaticDef(MainControlTask, MainControlTaskEntry, osPriorityNormal, 0, 128, MainControlTaskBuffer, &MainControlTaskControlBlock);
  MainControlTaskHandle = osThreadCreate(osThread(MainControlTask), NULL);

  /* definition and creation of Cyclic */
  osThreadStaticDef(Cyclic, CyclicTaskEntry, osPriorityBelowNormal, 0, 128, CyclicBuffer, &CyclicControlBlock);
  CyclicHandle = osThreadCreate(osThread(Cyclic), NULL);

  /* definition and creation of Idle */
  osThreadStaticDef(Idle, IdleTaskEntry, osPriorityIdle, 0, 128, IdleBuffer, &IdleControlBlock);
  IdleHandle = osThreadCreate(osThread(Idle), NULL);

  /* definition and creation of YpSerialRx */
  osThreadStaticDef(YpSerialRx, YpSerialRxTaskEntry, osPriorityAboveNormal, 0, 128, YpSerialRxBuffer, &YpSerialRxControlBlock);
  YpSerialRxHandle = osThreadCreate(osThread(YpSerialRx), NULL);

  /* definition and creation of YpSerialTx */
  osThreadStaticDef(YpSerialTx, YpSerialTxTaskEntry, osPriorityAboveNormal, 0, 128, YpSerialTxBuffer, &YpSerialTxControlBlock);
  YpSerialTxHandle = osThreadCreate(osThread(YpSerialTx), NULL);

  /* definition and creation of CrSerialRx */
  osThreadStaticDef(CrSerialRx, CrSerialRxTaskEntry, osPriorityAboveNormal, 0, 128, CrSerialRxBuffer, &CrSerialRxControlBlock);
  CrSerialRxHandle = osThreadCreate(osThread(CrSerialRx), NULL);

  /* definition and creation of CrSerialTx */
  osThreadStaticDef(CrSerialTx, CrSerialTxTaskEntry, osPriorityAboveNormal, 0, 128, CrSerialTxBuffer, &CrSerialTxControlBlock);
  CrSerialTxHandle = osThreadCreate(osThread(CrSerialTx), NULL);

  /* definition and creation of SubTask */
//  osThreadStaticDef(SubTask, SubTaskEntry, osPriorityNormal, 0, 128, SubTaskBuffer, &SubTaskControlBlock);
//  SubTaskHandle = osThreadCreate(osThread(SubTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */


	/* ウォッチドッグタイマーリセット */
//	MX_IWDG_Init();


	//
	Cyclic_Init();

	/* ５ｍSecタイマー起動 */
//	HAL_TIM_Base_Start_IT(&htim2);					// 5ms周期割り込み




  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
 
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks 
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 500-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000-1;
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
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_2;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED2_Pin|LED3_Pin|LED4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED2_Pin LED3_Pin LED4_Pin */
  GPIO_InitStruct.Pin = LED2_Pin|LED3_Pin|LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
__weak void UART1_TxCpltCallback(void)
{
}
__weak void UART2_TxCpltCallback(void)
{
}
#if 0
	// ?ｿｽ?ｿｽ?ｿｽg?ｿｽp
__weak void UART3_TxCpltCallback(void)
{
}
__weak void UART4_TxCpltCallback(void)
{
}
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if( huart->Instance == huart1.Instance )
    {
        UART1_TxCpltCallback();
    }
    else if( huart->Instance == huart2.Instance )
    {
        UART2_TxCpltCallback();
    }
#if 0
	// ?ｿｽ?ｿｽ?ｿｽg?ｿｽp
    else if( huart->Instance == huart3.Instance )
    {
        UART3_TxCpltCallback();
    }
    else if( huart->Instance == huart4.Instance )
    {
        UART4_TxCpltCallback();
    }
#endif
}

__weak void UART1_RxCpltCallback(void)
{
}
__weak void UART2_RxCpltCallback(void)
{
}
#if 0
	// ?ｿｽ?ｿｽ?ｿｽg?ｿｽp
__weak void UART3_RxCpltCallback(void)
{
}
__weak void UART4_RxCpltCallback(void)
{
}
#endif

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if( huart->Instance == huart1.Instance )
    {
        UART1_RxCpltCallback();
    }
    else if( huart->Instance == huart2.Instance )
    {
        UART2_RxCpltCallback();
    }
#if 0
	// ?ｿｽ?ｿｽ?ｿｽg?ｿｽp
    else if( huart->Instance == huart3.Instance )
    {
        UART3_RxCpltCallback();
    }
    else if( huart->Instance == huart4.Instance )
    {
        UART4_RxCpltCallback();
    }
#endif
}

__weak void UART1_ErrorCallback(void)
{
}
__weak void UART2_ErrorCallback(void)
{
}
#if 0
	// ?ｿｽ?ｿｽ?ｿｽg?ｿｽp
__weak void UART3_ErrorCallback(void)
{
}
__weak void UART4_ErrorCallback(void)
{
}
#endif

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if( huart->Instance == huart1.Instance )
    {
        UART1_ErrorCallback();
    }
    else if( huart->Instance == huart2.Instance )
    {
        UART2_ErrorCallback();
    }
#if 0
	// ?ｿｽ?ｿｽ?ｿｽg?ｿｽp
    else if( huart->Instance == huart3.Instance )
    {
        UART3_ErrorCallback();
    }
    else if( huart->Instance == huart4.Instance )
    {
        UART4_ErrorCallback();
    }
#endif
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_MainControlTaskEntry */
/**
  * @brief  Function implementing the MainControlTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_MainControlTaskEntry */
__weak void MainControlTaskEntry(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_CyclicTaskEntry */
/**
* @brief Function implementing the Cyclic thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CyclicTaskEntry */
__weak void CyclicTaskEntry(void const * argument)
{
  /* USER CODE BEGIN CyclicTaskEntry */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CyclicTaskEntry */
}

/* USER CODE BEGIN Header_IdleTaskEntry */
/**
* @brief Function implementing the Idle thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IdleTaskEntry */
__weak void IdleTaskEntry(void const * argument)
{
  /* USER CODE BEGIN IdleTaskEntry */
  /* Infinite loop */
  for(;;)
  {

#ifndef debag
	// IWDG REFRRESH
    HAL_IWDG_Refresh(&hiwdg);

	// SLEEP MODE SET
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

#else
	osDelay(1);

#endif
  }
  /* USER CODE END IdleTaskEntry */
}

/* USER CODE BEGIN Header_YpSerialRxTaskEntry */
/**
* @brief Function implementing the YpSerialRx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_YpSerialRxTaskEntry */
__weak void YpSerialRxTaskEntry(void const * argument)
{
  /* USER CODE BEGIN YpSerialRxTaskEntry */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END YpSerialRxTaskEntry */
}

/* USER CODE BEGIN Header_YpSerialTxTaskEntry */
/**
* @brief Function implementing the YpSerialTx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_YpSerialTxTaskEntry */
__weak void YpSerialTxTaskEntry(void const * argument)
{
  /* USER CODE BEGIN YpSerialTxTaskEntry */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END YpSerialTxTaskEntry */
}

/* USER CODE BEGIN Header_CrSerialRxTaskEntry */
/**
* @brief Function implementing the CrSerialRx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CrSerialRxTaskEntry */
__weak void CrSerialRxTaskEntry(void const * argument)
{
  /* USER CODE BEGIN CrSerialRxTaskEntry */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CrSerialRxTaskEntry */
}

/* USER CODE BEGIN Header_CrSerialTxTaskEntry */
/**
* @brief Function implementing the CrSerialTx thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CrSerialTxTaskEntry */
__weak void CrSerialTxTaskEntry(void const * argument)
{
  /* USER CODE BEGIN CrSerialTxTaskEntry */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CrSerialTxTaskEntry */
}

/* USER CODE BEGIN Header_SubTaskEntry */
/**
* @brief Function implementing the SubTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SubTaskEntry */
__weak void SubTaskEntry(void const * argument)
{
  /* USER CODE BEGIN SubTaskEntry */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END SubTaskEntry */
}

/* CallbackCyclicTimer function */
__weak void CallbackCyclicTimer(void const * argument)
{
  /* USER CODE BEGIN CallbackCyclicTimer */
  
  /* USER CODE END CallbackCyclicTimer */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
