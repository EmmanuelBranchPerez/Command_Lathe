/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Blinky_Pin GPIO_PIN_1
#define Blinky_GPIO_Port GPIOH
#define FC2_Pin GPIO_PIN_1
#define FC2_GPIO_Port GPIOC
#define FC2_EXTI_IRQn EXTI1_IRQn
#define FC3_Pin GPIO_PIN_2
#define FC3_GPIO_Port GPIOC
#define FC3_EXTI_IRQn EXTI2_IRQn
#define FC4_Pin GPIO_PIN_3
#define FC4_GPIO_Port GPIOC
#define FC4_EXTI_IRQn EXTI3_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define MS2_Pin GPIO_PIN_5
#define MS2_GPIO_Port GPIOC
#define DIR_Pin GPIO_PIN_12
#define DIR_GPIO_Port GPIOB
#define SET_ZERO_H_Pin GPIO_PIN_13
#define SET_ZERO_H_GPIO_Port GPIOB
#define SET_ZERO_H_EXTI_IRQn EXTI15_10_IRQn
#define FC1_Pin GPIO_PIN_14
#define FC1_GPIO_Port GPIOB
#define FC1_EXTI_IRQn EXTI15_10_IRQn
#define SET_ZERO_L_Pin GPIO_PIN_15
#define SET_ZERO_L_GPIO_Port GPIOB
#define SET_ZERO_L_EXTI_IRQn EXTI15_10_IRQn
#define MS1_Pin GPIO_PIN_6
#define MS1_GPIO_Port GPIOC
#define MS3_Pin GPIO_PIN_11
#define MS3_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

