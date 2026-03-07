/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "lcd_display.h"
#include "mpu6000.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
	CMD_ID_MOVE, // Command to set the Base Voltage

	CMD_ID_UNKNOWN
} CommandID_t; //Structure type which will helps us to create different variables

typedef struct {
	const char* command_str;  //Maps the command written by the user

	CommandID_t command_id; //Maps the CommandID_t associated to the user's string chain

} Command_t; ////Structure type which will helps us to create different variables
/* ====================================================*/

/* ====================================================*/
/*------------------PIN POGN'S BUFFERS-----------------*/
typedef enum{
	BUFFER_A, //Let's call it Ma Long, ranked as the best table pin pon's player of all time
	BUFFER_B //Let's call this one Jan-Ove Waldne, ranked as the second best pin pong's player of all time
} ActiveBuffer;

typedef struct {
	uint8_t*  buffer;
	uint16_t  size;
} DataPacket;

/*--------------COMMANDS ESTABLISHMENT-----------------*/

const Command_t command_table[] = { //We make a diccionarie where the string commands are the key, and the pre-established commands are the values

		{ "MOVE",         CMD_ID_MOVE },

};
const int num_commands = sizeof(command_table) / sizeof(Command_t); //Size of command table get the full size and divides it by the just one size of the structure, so it obtains the number of elements of the table
/*=====================================================*/
/* ====================================================*/
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_RX_BUFFER_SIZE 64 //Buffer reception size, let's say, each package we receive trough UART has 63 useful bytes and 1 more for '\0'

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* ====================================================*/
/*---------------PING PONG PRIVATE VARIABLES-----------*/

/* Ping-pong UART */
uint8_t rx_buffer_a[UART_RX_BUFFER_SIZE]; //Ma Long "height"
uint8_t rx_buffer_b[UART_RX_BUFFER_SIZE]; //Jan-Ove Waldne "height"
volatile DataPacket  data_ready_packet = { .buffer = NULL, .size = 0 };
volatile ActiveBuffer dma_active_buffer = BUFFER_A;
/* ====================================================*/
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
uint16_t zeroLongitud = 0;
uint16_t zeroTransversal = 0;
uint16_t befDistance_l = 0;
uint16_t befDistance_t = 0;
uint16_t distanceToMove_l = 0;
uint16_t distanceToMove_t = 0;
uint16_t distance_l = 0;
uint16_t distance_t = 0;
uint16_t dir_l = 0;
uint16_t dir_t = 0;
uint16_t duttyValue_l = 0;
uint16_t duttyValue_t = 0;
LCD_HandleTypeDef hlcd;
MPU_HandleTypeDef hmpu;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM11_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM5_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
void movemotors_l(char* params);

/*=====================================================*/
/*----------------PING PONG'S HANDLERS-----------------*/
CommandID_t find_command_id(const char* command_str);
void parse_and_execute_command(uint8_t* buffer, uint16_t size);
void dispatch_command(CommandID_t id, char* command, char* params);
void command_unrecognized_handler(char* command); //Handler for not recognized commands
void print_available_commands(void); //Shows available commands for UART
/*=====================================================*/

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM11_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  dma_active_buffer = BUFFER_A;
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer_a, UART_RX_BUFFER_SIZE);

  //Show available commands
  print_available_commands();
  /*=============================================*/
  /* LCD INIT */
  /*=============================================*/
  hlcd.hi2c = &hi2c1;
  hlcd.I2C_ADDRESS = (LCD_ADDRR << 1);
  hlcd.DISPLAY_EN = 1;
  hlcd.COLUMNS = LCD_COLUMNS;
  hlcd.ROWS = LCD_ROWS;

  LCD_Init(&hlcd);
  LCD_Clear(&hlcd);

  /*=============================================*/
  /* MPU INIT */
  /*=============================================*/
  hmpu.hi2c = &hi2c3;
  hmpu.I2C_ADRRESS = MPU_ADDR_BASE_0X68;

  //MPU verify

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	float ax = 0.0f, ay = 0.0f, az = 0.0f;
	char buffer[21]; // 20 caracteres + null terminator

	while (1)
	{
		if (data_ready_packet.size > 0)
		{
			uint8_t*  proc_buffer = data_ready_packet.buffer;
			uint16_t  proc_size   = data_ready_packet.size;

			data_ready_packet.buffer = NULL;
			data_ready_packet.size   = 0;

			parse_and_execute_command(proc_buffer, proc_size);
		}
		// Leer aceleración del MPU-6000
		if (MPU_GetAcceleration(&hmpu, &ax, &ay, &az) == HAL_OK) {

			// Mostrar Aceleración X y Y en Fila 2
			// Formato: AX: 0.00 AY: 0.00
			snprintf(buffer, sizeof(buffer), "AX: %5.2f AY: %5.2f", ax, ay);
			LCD_GotoXY(&hlcd, 2, 0);
			LCD_Print(&hlcd, buffer);

			// Mostrar Aceleración Z en Fila 3
			// Formato: AZ: 0.00 g
			snprintf(buffer, sizeof(buffer), "AZ: %5.2f g", az);
			LCD_GotoXY(&hlcd, 3, 0);
			LCD_Print(&hlcd, buffer);
		} else {
			// Si la lectura falla (e.g., I2C error)
			LCD_GotoXY(&hlcd, 2, 0);
			LCD_Print(&hlcd, "MPU Read Fail!      ");
			LCD_GotoXY(&hlcd, 3, 0);
			LCD_Print(&hlcd, "                    ");
		}

		HAL_Delay(100); // Pequeña espera para no sobrecargar el bus I2C y actualizar la pantalla
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 400000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 16;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */
	HAL_TIM_Base_Init(&htim1);
	HAL_TIM_Base_Start(&htim1);
  /* USER CODE END TIM1_Init 2 */

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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 31999;
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
  sConfigOC.Pulse = 0;
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
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
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
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;
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
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
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
	HAL_TIM_Base_Start_IT(&htim11);
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Blinky_GPIO_Port, Blinky_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, MS2_Pin|MS1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MS3_GPIO_Port, MS3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Blinky_Pin */
  GPIO_InitStruct.Pin = Blinky_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Blinky_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FC2_Pin FC3_Pin FC4_Pin */
  GPIO_InitStruct.Pin = FC2_Pin|FC3_Pin|FC4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : MS2_Pin MS1_Pin */
  GPIO_InitStruct.Pin = MS2_Pin|MS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : DIR_Pin */
  GPIO_InitStruct.Pin = DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DIR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SET_ZERO_H_Pin FC1_Pin SET_ZERO_L_Pin */
  GPIO_InitStruct.Pin = SET_ZERO_H_Pin|FC1_Pin|SET_ZERO_L_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MS3_Pin */
  GPIO_InitStruct.Pin = MS3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MS3_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void parse_and_execute_command(uint8_t* buffer, uint16_t size)
{
	// 1) Asegurar terminación nula
	if (size >= UART_RX_BUFFER_SIZE) {
		buffer[UART_RX_BUFFER_SIZE - 1] = '\0';
	} else {
		buffer[size] = '\0';
	}

	// 2) Trim de caracteres raros al final (\r, \n)
	for (int i = 0; i < size; i++) {
		if (buffer[i] == '\r' || buffer[i] == '\n')
			buffer[i] = '\0';
	}

	// 3) Extraer comando (primer token)
	char* command_str = strtok((char*)buffer, " ");
	// 4) Extraer parámetros (todo lo que siga)
	char* params      = strtok(NULL, "");

	if (command_str == NULL)
		return;

	CommandID_t id = find_command_id(command_str);
	dispatch_command(id, command_str, params);
}

CommandID_t find_command_id(const char* command_str)
{
	for (int i = 0; i < num_commands; i++) {
		if (strcmp(command_str, command_table[i].command_str) == 0) { //Compares if the command in the table is the same of the string, if it is, then if == True and the function returns the command id
			return command_table[i].command_id;
		}
	}
	return CMD_ID_UNKNOWN;
}

void dispatch_command(CommandID_t id, char* command, char* params) //Here, each id is linked to its handler
{
	switch (id)

	{
	case CMD_ID_MOVE:

		movemotors_l(params);

		break;

	case CMD_ID_UNKNOWN:

	default:

		command_unrecognized_handler(command);

		break;

	}
}

//Unrecognized handler
void command_unrecognized_handler(char* command)
{
	char msg[64];

	// Manejo defensivo: si por alguna razón command es NULL
	if (command == NULL)
	{
		const char* err = "Error: comando no reconocido.\r\n";
		HAL_UART_Transmit(&huart2, (uint8_t*)err, strlen(err), 100);
		return;
	}

	int len = snprintf(msg, sizeof(msg),
			"Comando no reconocido: '%s'\r\n", command);

	HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, 100);
}

void movemotors_l(char* params){
	//get distance in mm and the desire Dutty via UART uint16_t D_mm, uint16_t setDutty
	// Parseo del parámetro <steps>

	char* enddptr1;
	char* enddptr2;

	//Parameter setting from UART
	char* distance_char = strtok(params, " \r\n");
	char* dutty_char = strtok(NULL, " \r\n");

	long val1 = strtol(distance_char, &enddptr1, 10);
	long val2 = strtol(dutty_char, &enddptr2, 10);

	int16_t distance_l = (int16_t) val1;
	int16_t duttyValue_l = (int16_t) val2;

	befDistance_l = __HAL_TIM_GET_COUNTER(&htim5);

	distance_l =  23.66*distance_l;
	if(distance_l < 0){
		if(__HAL_TIM_GET_COUNTER(&htim5) == 0){
			//MENSAJE POR USART ERROR!!!
		}
		else {
			distanceToMove_l = befDistance_l + distance_l;
			if (distanceToMove_l < 0){
				//MENSAJE POR USART ERROR!!!
			}
			else{
				while(__HAL_TIM_GET_COUNTER(&htim5) != distanceToMove_l){
					dir_l = 1;
					HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
					duttyValue_l = __HAL_TIM_GET_COMPARE(&htim5, TIM_CHANNEL_1);
					duttyValue_l += setDutty;
					HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
				}
			}
		}
	}
	if(distance_l > 0){
		if(__HAL_TIM_GET_COUNTER(&htim5) == 1527){
			//Solo se pone por que se debe probar primero
			//MENSAJE POR USAR ERROR
		}
		else {
			distanceToMove_l = befDistance_l + distance_l;
			if (distanceToMove_l > 1527){
				//MENSAJE POR USART ERROR!!!
			}
			else{
				while(__HAL_TIM_GET_COUNTER(&htim5) != distanceToMove_l){
					dir_l = 1;
					HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
					duttyValue_l = __HAL_TIM_GET_COMPARE(&htim5, TIM_CHANNEL_1);
					duttyValue_l += setDutty;
					HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
				}
			}
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM11){
		HAL_GPIO_TogglePin(Blinky_GPIO_Port, Blinky_Pin); //Blinky
	}
}

//In the callbacks we set the zeros of the material
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == SET_ZERO_L_Pin){
		/* Save Zero by looking the position */
		zeroLongitud = __HAL_TIM_GET_COUNTER(&htim5); //Save zero
	}
	if(GPIO_Pin == SET_ZERO_H_Pin){
		/* Save Zero by looking the position */
		zeroTransversal = __HAL_TIM_GET_COUNTER(&htim5); //Save zero
	}
	if(GPIO_Pin == FC1_Pin){ //Final de carrera home 0 position
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
		dir_l = 0;//Falta activar este pin por software
		//Mensaje por Usart "You have reached the limit"
	}
	if(GPIO_Pin == FC2_Pin){
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
		dir_t = 1;//Falta activar este pin por software
		//Mensaje por Usart "You have reached the limit"

	}
	if(GPIO_Pin == FC3_Pin){
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);//Falta configurar
		dir_l = 1;//Falta activar este pin por software
		//Mensaje por Usart "You have reached the limit"
	}
	if(GPIO_Pin == FC4_Pin){
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
		dir_t = 0;//Falta activar este pin por software
		//Mensaje por Usart "You have reached the limit"
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART2)
	{
		if (dma_active_buffer == BUFFER_A)
		{
			data_ready_packet.buffer = rx_buffer_a;
			data_ready_packet.size   = Size;

			dma_active_buffer = BUFFER_B;
			HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer_b, UART_RX_BUFFER_SIZE);
		}
		else
		{
			data_ready_packet.buffer = rx_buffer_b;
			data_ready_packet.size   = Size;

			dma_active_buffer = BUFFER_A;
			HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer_a, UART_RX_BUFFER_SIZE);
		}
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

