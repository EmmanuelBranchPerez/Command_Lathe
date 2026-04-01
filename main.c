/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stepper.h" //Stepper's header file

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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM11_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */
MoveResult MoveSteps(int32_t milimeters); //We receivieng the actual position in the display
void StepperOn(void); //Function that makes the stepper turn on
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM11_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */
	// This timer is used for generating both of the PWM's f the STEP
  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 31999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 15999;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
	HAL_TIM_Base_Init(&htim2);
	HAL_TIM_PWM_Init(&htim2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //A4988 PWM's
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // ""
  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim5, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */
	HAL_TIM_Base_Init(&htim2); //Encoder Timer Mode
  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 15999;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 249;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */
	HAL_TIM_Base_Start_IT(&htim11); //Blinky Timer
  /* USER CODE END TIM11_Init 2 */

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
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Blinky_GPIO_Port, Blinky_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ENA4988_1_GPIO_Port, ENA4988_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Blinky_Pin */
  GPIO_InitStruct.Pin = Blinky_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Blinky_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ENA4988_1_Pin */
  GPIO_InitStruct.Pin = ENA4988_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ENA4988_1_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* ─────────────────────────────────────────────
   StepperEnable / StepperDisable
   A4988 EN pin is active-LOW on most boards.
   Adjust polarity if yours differs.
───────────────────────────────────────────── */
void StepperEnable(void)
{
	/* Active LOW: pull EN low to enable driver */
	HAL_GPIO_WritePin(ENA4988_1_GPIO_Port, ENA4988_1_Pin, GPIO_PIN_RESET);
}

void StepperDisable(void)
{
	HAL_GPIO_WritePin(ENA4988_1_GPIO_Port, ENA4988_1_Pin, GPIO_PIN_SET);
}

/* ─────────────────────────────────────────────
   MoveSteps
   milimeters > 0  →  forward  (encoder counts UP,   DIR = HIGH)
   milimeters < 0  →  backward (encoder counts DOWN,  DIR = LOW )
   milimeters = 0  →  MOVE_ERR_INVALID

   Returns MoveResult so the caller can react.
───────────────────────────────────────────── */
MoveResult MoveSteps(int32_t milimeters)
{
	/* ── 0. Reject zero movement ─────────────────────── */
	if (milimeters == 0) {
		return MOVE_ERR_INVALID;
	}

	/* ── 1. Convert mm → encoder counts (signed) ─────── */
	int32_t delta_counts = (int32_t)(milimeters * COUNTS_PER_MM);
	//  NOTE: fractional truncation here is intentional.
	//  At 23.66 counts/mm the rounding error is < 1 count (< 0.05 mm).
	//  If sub-count precision is needed later, accumulate remainder.

	/* ── 2. Read current encoder position ────────────── */
	int32_t act_counter = (int32_t)__HAL_TIM_GET_COUNTER(&htim5);

	/* ── 3. Compute intended target position ─────────── */
	int32_t target_counter = act_counter + delta_counts;

	/* ── 4. Security: check target against hard limits ── */
	//  We check the TARGET, not act_counter, so we catch over-travel
	//  before the motor even starts moving.
	if (target_counter > (int32_t)LIMIT_UPPER) {
		return MOVE_ERR_UPPER_LIMIT;   /* hard stop – motor never starts */
	}
	if (target_counter < (int32_t)LIMIT_LOWER) {
		return MOVE_ERR_LOWER_LIMIT;
	}

	/* ── 5. Set direction pin ────────────────────────── */
	if (milimeters > 0) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);   /* Forward */
	} else {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); /* Backward */
	}

	/* ── 6. Enable driver and start PWM ─────────────── */
	StepperEnable();
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

	/* ── 7. Poll encoder until target reached ────────── */
	//  We re-read the encoder every loop iteration.
	//  The motor pulses itself via PWM — we just watch.
	while (1)
	{
		act_counter = (int32_t)__HAL_TIM_GET_COUNTER(&htim5);

		/* ── 7a. In-motion limit safety re-check ──────
           Guards against mechanical slip or missed counts
           pushing us past the limit mid-move.             */
		if (act_counter >= (int32_t)LIMIT_UPPER) {
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			StepperDisable();
			return MOVE_ERR_UPPER_LIMIT;
		}
		if (act_counter <= (int32_t)LIMIT_LOWER) {
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			StepperDisable();
			return MOVE_ERR_LOWER_LIMIT;
		}

		/* ── 7b. Target reached? ───────────────────── */
		int8_t reached;
		if (milimeters > 0) {
			reached = (act_counter >= target_counter);
		} else {
			reached = (act_counter <= target_counter);
		}

		if (reached) {
			break;
		}
	}

	/* ── 8. Stop PWM and disable driver ─────────────── */
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	StepperDisable();

	return MOVE_OK;
}

////Stepper movement
//void StepperOn(void){
//	HAL_GPIO_WritePin(ENA4988_1_GPIO_Port, ENA4988_1_Pin, GPIO_PIN_SET);//Put 1 in the EN pin
//}
////Longitudinal and transversal cars depend on the steppe's movement
////This movement is parameterize with a number 23.66 --- for review
//uint32_t grLimit = 1250; //Maximum limit, this will be set when using the specific end of career's in both limit's
//uint32_t loLimit = 0; //Minimum limit, this will be set when using the specific end of career's in both limit's
//void MoveSteps(uint32_t milimeters, uint8_t direction){
//	int32_t pulses; //signed integer as we're going to use both negative and positive number's
//	pulses =  23.66*milimeters; //Convert to a unit the timer knows
//	//Problems her, as we need an  int number for the number of pulses we should check what we can do
//	//with the conversion since it can affect the precision of the system
//	uint32_t act_counter = __HAL_TIM_GET_COUNTER(&htim5); //Get the counter of the timer relate to the encoder
//
//	//Each count of the counter is the amount of pulses the encoder has identified
//	uint32_t mov_counter = act_counter + pulses;
//
//	//Security check movement
//	if (mov_counter > grLimit){
//		return;
//	}
//
//	if (mov_counter > loLimit){
//		return;
//	}
//while(act_counter < mov_counter){
//	StepperOn();
//
//	//Securityuserfunctions: Check zeros, check limits...
//}
//
//}



//TIMER CALLBACK'S
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM11){
		HAL_GPIO_TogglePin(Blinky_GPIO_Port, Blinky_Pin); //Blinky
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

