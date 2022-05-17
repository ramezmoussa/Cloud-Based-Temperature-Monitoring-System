/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>

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
I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
volatile uint8_t started = 0;
volatile float criticalTemp = 30;
uint8_t received_Temp[5] = {0};
uint8_t currentChar[1] = {0};
uint8_t currentIdx = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C3_Init(void);
static void MX_USART1_UART_Init(void);
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */


  HAL_UART_Receive_IT(&huart2,(uint8_t *)received_Temp, 5);


  // Start the Timer for the Buzzer
  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  TIM1->CCR1 = 500;
  __HAL_TIM_SET_PRESCALER(&htim1, 0);


  // Check if the RTC module is ready
  if (HAL_I2C_IsDeviceReady(&hi2c3, 0xD0, 10, HAL_MAX_DELAY) == HAL_OK)
  {
	  for (int i = 1; i<=10;i++) // indicator of ready device
	  {
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
	  HAL_Delay(250);
	  }
  }

  // buffers for time and date
  uint8_t secbuffer[2], minbuffer[2], hourbuffer[2];
  uint8_t daybuffer[2], datebuffer[2], monthbuffer[2], yearbuffer[2];



  // seconds
  secbuffer[0] = 0x00; //register address
  secbuffer[1] = 0x00; //data to put in register --> 0 sec
  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, secbuffer, 2, 10);

   // minutes
  minbuffer[0] = 0x01; //register address
  minbuffer[1] = 0x00; //data to put in register --> 40 min
  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, minbuffer, 2, 10);
  // hours
  hourbuffer[0] = 0x02; //register address
  hourbuffer[1] = 0x15; //data to put in register  --> 11 am
  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, hourbuffer, 2, 10);

  // Set day to 1 to represent Monday
  daybuffer[0] = 0x03; //register address
  daybuffer[1] = 0x01; //data to put in register  --> 11 am
  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, daybuffer, 2, 10);

  // Set date to the 11th
  datebuffer[0] = 0x04; //register address
  datebuffer[1] = 0x18; //data to put in register  --> 11 am
  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, datebuffer, 2, 10);

  // Set month to April
  monthbuffer[0] = 0x05; //register address
  monthbuffer[1] = 0x05; //data to put in register  --> 11 am
  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, monthbuffer, 2, 10);

  // Set year to 22
  yearbuffer[0] = 0x06; //register address
  yearbuffer[1] = 0x22; //data to put in register  --> 11 am
  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, yearbuffer, 2, 10);


  uint8_t h1,h2,m1,m2,s1,s2, day1, day2, month1, month2, year1, year2;
  uint8_t dayname[10] = "";


  uint8_t tempMSB[2], tempLSB[2], busy[2], control[2];
  char uartBuf[100] = "";

  busy[0] = 0x0f;
  control[0] = 0x0e;
  control[1] = 0x38;
  tempMSB[0] = 0x11;
  tempLSB[0] = 0x12;


  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, control, 2, 10);
  HAL_Delay(50);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	  //send seconds register address 00h to read from
	  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, secbuffer, 1, 10);
	  //read data of register 00h to secbuffer[1]
	  HAL_I2C_Master_Receive(&hi2c3, 0xD1, secbuffer+1, 1, 10);
	  //prepare UART output
	  s1 = secbuffer[1] >> 4;
	  s2 = secbuffer[1] & 0x0F;
	  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, minbuffer, 1, 10);
	  HAL_I2C_Master_Receive(&hi2c3, 0xD1, minbuffer+1, 1, 10);
	  m1 = minbuffer[1] >> 4;
	  m2 = minbuffer[1] & 0x0F;
	  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, hourbuffer, 1, 10);
	  HAL_I2C_Master_Receive(&hi2c3, 0xD1, hourbuffer+1, 1, 10);
	  h1 = (hourbuffer[1] >> 4) & 1;
	  h2 = hourbuffer[1] & 0x0F;

	  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, daybuffer, 1, 10);
	  HAL_I2C_Master_Receive(&hi2c3, 0xD1, daybuffer+1, 1, 10);


	  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, datebuffer, 1, 10);
	  HAL_I2C_Master_Receive(&hi2c3, 0xD1, datebuffer+1, 1, 10);

	  day1 = (datebuffer[1] >> 4) & 3;
	  day2 = datebuffer[1] & 0xf;

	  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, monthbuffer, 1, 10);
	  HAL_I2C_Master_Receive(&hi2c3, 0xD1, monthbuffer+1, 1, 10);

	  month1 = (monthbuffer[1] >> 4) & 1;
	  month2 = monthbuffer[1] & 0xf;

	  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, yearbuffer, 1, 10);
	  HAL_I2C_Master_Receive(&hi2c3, 0xD1, yearbuffer+1, 1, 10);

	  year1 = (yearbuffer[1] >> 4 & 15);
	  year2 = (yearbuffer[1] & 15);


	  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, busy, 1, 10);
	  HAL_I2C_Master_Receive(&hi2c3, 0xD1, busy+1, 1, 10);

	  // if not busy read the temperature and create a new conversion
	  if(!(busy[1] & 4))
	  {
		  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, tempMSB, 1, 10);
		  HAL_I2C_Master_Receive(&hi2c3, 0xD1, tempMSB+1, 1, 10);

		  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, tempLSB, 1, 10);
		  HAL_I2C_Master_Receive(&hi2c3, 0xD1, tempLSB+1, 1, 10);

		  HAL_I2C_Master_Transmit(&hi2c3, 0xD0, control, 2, 10);

		  signed char tempDigit = tempMSB[1];
		  char tempDigitChars[10];
		  itoa(tempDigit, tempDigitChars, 10);
		  char tempFraction[2];

		  switch(tempLSB[1] >> 6)
		  {
		  case 0:
			  strcpy(tempFraction, "00");
			  break;
		  case 1:
			  strcpy(tempFraction, "25");
			  break;
		  case 2:
			  strcpy(tempFraction, "50");
		  case 3:
			  strcpy(tempFraction, "75");
		  }



		  if(tempDigit >= criticalTemp)
		  {
				__HAL_TIM_SET_PRESCALER(&htim1, 16);
		  }
		  else
		  {
				__HAL_TIM_SET_PRESCALER(&htim1, 0);

		  }

		  sprintf(uartBuf, "~Timestamp: %d%d/%d%d/20%d%d %d%d:%d%d:%d%d Temperature: %s.%s \r\n#", day1, day2, month1, month2, year1, year2, h1,h2,m1,m2,s1,s2, tempDigitChars, tempFraction);

		  HAL_UART_Transmit(&huart1, (uint8_t *)uartBuf, sizeof(uartBuf), 100);

	  }
	  for(int i = 0; i < 5;i++)
		  HAL_Delay(1000);


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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
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
  hi2c3.Init.Timing = 0x00707CBB;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
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

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 319;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
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
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance  == USART2)
	{
		criticalTemp = atof((const char*)received_Temp);
		HAL_UART_Receive_IT(&huart2, (uint8_t *) received_Temp, 5);
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

