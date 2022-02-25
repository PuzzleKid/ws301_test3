/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "app_lorawan.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lora_app.h"
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
  .stack_size = 1024 * 4
};
/* Definitions for nfcTask */
osThreadId_t nfcTaskHandle;
const osThreadAttr_t nfcTask_attributes = {
  .name = "nfcTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 1024 * 4
};
/* Definitions for systemTask */
osThreadId_t systemTaskHandle;
const osThreadAttr_t systemTask_attributes = {
  .name = "systemTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 768 * 4
};
/* Definitions for workTask */
osThreadId_t workTaskHandle;
const osThreadAttr_t workTask_attributes = {
  .name = "workTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartNFCTask(void *argument);
void StartSystemTask(void *argument);
void StartWorkTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

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

  /* creation of nfcTask */
  nfcTaskHandle = osThreadNew(StartNFCTask, NULL, &nfcTask_attributes);

  /* creation of systemTask */
  systemTaskHandle = osThreadNew(StartSystemTask, NULL, &systemTask_attributes);

  /* creation of workTask */
  workTaskHandle = osThreadNew(StartWorkTask, NULL, &workTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

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
  /* init code for LoRaWAN */
  MX_LoRaWAN_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	MX_LoRaWAN_Process();
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartNFCTask */
/**
* @brief Function implementing the nfcTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartNFCTask */
void StartNFCTask(void *argument)
{
  /* USER CODE BEGIN StartNFCTask */
  /* Infinite loop */
  for(;;)
  {
	  if(HAL_GPIO_ReadPin(SWITCH2_GPIO_Port, SWITCH2_Pin) == SWITCH2_PRESS){
		  osDelay(10);
		  if(HAL_GPIO_ReadPin(SWITCH2_GPIO_Port, SWITCH2_Pin) == SWITCH2_PRESS){
			 // printf("%d\r\n",uxTaskGetStackHighWaterMark (NULL));
			  LED_GREEN_TOGGLE;
			  SendTxData();
			  while(HAL_GPIO_ReadPin(SWITCH2_GPIO_Port, SWITCH2_Pin) == SWITCH2_PRESS){
				  osDelay(100);
			  }
		  }
	  }
	  osDelay(10);
  }
  /* USER CODE END StartNFCTask */
}

/* USER CODE BEGIN Header_StartSystemTask */
/**
* @brief Function implementing the systemTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSystemTask */
void StartSystemTask(void *argument)
{
  /* USER CODE BEGIN StartSystemTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartSystemTask */
}

/* USER CODE BEGIN Header_StartWorkTask */
/**
* @brief Function implementing the workTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWorkTask */
void StartWorkTask(void *argument)
{
  /* USER CODE BEGIN StartWorkTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartWorkTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
