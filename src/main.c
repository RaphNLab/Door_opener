/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "global.h"
#include "task_manager.h"

float dest1[3];
float dest2[3];

void SystemClock_Config(void);
void Error_Handler(void);

int main(void)
{
	/*Reset RCC clock configuration to the default reset state*/

	HAL_RCC_DeInit();

	/* Update the SystemCoreClock variable*/
	SystemCoreClockUpdate();

	uartDevConfig(&uartDev, &huart2, uartRxBuffer, uartTxBuffer, 0);

	init_keypad();
	i2cDevConfig(&i2cDev, &hi2c1);

	MPU9250Reset();
	MPU9250Init();
	MPU9250Calibrate(dest1, dest2);

	//xTaskCreate(vUartCmdTaskHandler, "Command handler", 500, NULL, 2, &xuartTaskHandle);
	xTaskCreate(vLedTaskHandler, "LED Task", 500, NULL, 2, &xledTaskHandle);

	//xTaskCreate(vAccTaskHandler, "Accelerometer task", 500, NULL, 2, &xaccTaskHandle);
	//xTaskCreate(vTempTaskHandler, "Temperature task", 500, NULL, 2, &xtempTaskHandle);

	xTaskCreate(vKeypadTaskHandler, "Keypad task", 500, NULL, 2, &xkeypadTaskHandle);

	//xTaskCreate(vLcdTaskHandler, "LCD task", 500, NULL, 2, &xLcdTaskHandle);

	//Start the timer
	//xTimerStart(xTempTimerHandle, portMAX_DELAY);
	//xTimerStart(xAccTimerHandle, portMAX_DELAY);

	/* Start the scheduler: This function will never return */
	pPrintf("Starting the scheduler\n");
	vTaskStartScheduler();

	for(;;);
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
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
}

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
