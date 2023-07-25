/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define Left_Pin GPIO_PIN_4
#define Left_GPIO_Port GPIOC
#define Left_EXTI_IRQn EXTI4_IRQn
#define Back_Pin GPIO_PIN_0
#define Back_GPIO_Port GPIOB
#define Back_EXTI_IRQn EXTI0_IRQn
#define Right_Pin GPIO_PIN_2
#define Right_GPIO_Port GPIOB
#define Right_EXTI_IRQn EXTI2_IRQn
#define Front_Pin GPIO_PIN_11
#define Front_GPIO_Port GPIOB
#define Front_EXTI_IRQn EXTI15_10_IRQn
#define LED2_Pin GPIO_PIN_12
#define LED2_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_14
#define KEY1_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_15
#define KEY2_GPIO_Port GPIOB
#define BEEP_Pin GPIO_PIN_2
#define BEEP_GPIO_Port GPIOD
#define TrigCH2_Pin GPIO_PIN_4
#define TrigCH2_GPIO_Port GPIOB
#define EchoCH2_Pin GPIO_PIN_5
#define EchoCH2_GPIO_Port GPIOB
#define TrigCH1_Pin GPIO_PIN_6
#define TrigCH1_GPIO_Port GPIOB
#define EchoCH1_Pin GPIO_PIN_7
#define EchoCH1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
