/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VBAT_DECT_EN_Pin GPIO_PIN_4
#define VBAT_DECT_EN_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_6
#define LED2_GPIO_Port GPIOB
#define MCS_PWR_EN_Pin GPIO_PIN_0
#define MCS_PWR_EN_GPIO_Port GPIOA
#define MCS_OUT_Pin GPIO_PIN_1
#define MCS_OUT_GPIO_Port GPIOA
#define ANT_TX_CTRL_Pin GPIO_PIN_8
#define ANT_TX_CTRL_GPIO_Port GPIOA
#define ANT_RX_CTRL_Pin GPIO_PIN_2
#define ANT_RX_CTRL_GPIO_Port GPIOB
#define SWITCH2_Pin GPIO_PIN_12
#define SWITCH2_GPIO_Port GPIOB
#define NFC_GPO_Pin GPIO_PIN_10
#define NFC_GPO_GPIO_Port GPIOA
#define NFC_PWR_EN_Pin GPIO_PIN_13
#define NFC_PWR_EN_GPIO_Port GPIOA
#define SWITCH1_Pin GPIO_PIN_13
#define SWITCH1_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */
#define LED_RED_Pin GPIO_PIN_5
#define LED_RED_GPIO_Port GPIOB
#define LED_GREEN_Pin GPIO_PIN_6
#define LED_GREEN_GPIO_Port GPIOB

#define SWITCH2_RELEASE GPIO_PIN_SET
#define SWITCH2_PRESS GPIO_PIN_RESET
#define LED_RED_ON HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET)
#define LED_RED_OFF HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET)
#define LED_RED_TOGGLE HAL_GPIO_TogglePin(LED_RED_GPIO_Port,LED_RED_Pin)
#define LED_GREEN_ON HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET)
#define LED_GREEN_OFF HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET)
#define LED_GREEN_TOGGLE HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
