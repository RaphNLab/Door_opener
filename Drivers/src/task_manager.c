#include "task_manager.h"

uint16_t ledDelay = 0;
uint8_t notifyAcc = 0;
uint8_t notifyTemp = 0;

Bool_T accTimerFlag = FALSE;
Bool_T tempTimerFlag = FALSE;

Bool_T taskEnablefFlag = TRUE;

const TickType_t tempTaskDelay = 10000 / portTICK_PERIOD_MS;
const TickType_t accTaskDelay = 2000 / portTICK_PERIOD_MS;

TaskHandle_t xledTaskHandle = NULL;
TaskHandle_t xuartTaskHandle = NULL;
TaskHandle_t xaccTaskHandle = NULL;
TaskHandle_t xtempTaskHandle = NULL;
TaskHandle_t xkeypadTaskHandle = NULL;
TaskHandle_t xLcdTaskHandle = NULL;

TaskHandle_t xtimerTaskHandle = NULL;
TimerHandle_t xAccTimerHandle = NULL;
TimerHandle_t xTempTimerHandle = NULL;

TaskType_T taskType = DEFAULT_TASK;

Bool_T password_state = FALSE;
uint8_t print_twice = 0;
//Bool_T stop_system = FALSE;

void vLedTaskHandler(void *params)
{
	while(1)
	{
		if((!password_state) && (print_twice < 2))
		{
			lcd_write_text("Enter PassWd:", NULL);
			print_twice++;
		}
		else
		{
			HAL_GPIO_TogglePin(GPIOA, LED5_PIN);
			vTaskDelay(500);
		}
	}
}

/* This function handle the watermeter task command*/
void vAccTaskHandler(void *params)
{
	uint16_t accXAxis = 0;
	uint16_t accYAxis = 0;
	uint16_t accZAxis = 0;
	uint16_t min = 256;
	uint16_t max = 402;
	float xAng = 0;
	float yAng = 0;
	float zAng = 0;

	while(1)
	{
		if(notifyAcc)
		{
			if(accTimerFlag)
			{
				MPU9250GetAccAxis(&accXAxis, &accYAxis, &accZAxis);
				pPrintf("MPU9250 Accelerometer X Axis: %d \n", accXAxis);
				pPrintf("MPU9250 Accelerometer Y Axis: %d \n", accYAxis);
				pPrintf("MPU9250 Accelerometer Z Axis: %d \n", accZAxis);

				xAng = map(accXAxis, min, max, -90, 90);
				yAng = map(accYAxis, min, max, -90, 90);
				zAng = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

				pPrintf("MPU9250 angle Z to G: %f \n", zAng);
			}
			accTimerFlag = FALSE;
		}
	}
}

float map(int16_t x, int16_t inMin, int16_t inMax, int16_t outMin, int16_t outMax)
{
	return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}


/* callback to notify timer overflow
 * When the 2s are reached
 * */
void accTaskCallback(TimerHandle_t xTimer)
{
	accTimerFlag = TRUE;
}

/* This function handle the temperature task command*/
void vTempTaskHandler(void *params)
{
	float temperature = 0.0;

	while(1)
	{
		if(notifyTemp)
		{
			if(tempTimerFlag)
			{
				//devAddr = MPU9250WhoAmI();
				MPU9250GetTemp(&temperature);
				pPrintf("MPU9250 Temperature: %d°C\n", (uint8_t)temperature);
				//pPrintf("MPU9250 ADDR: 0x%X\n", (uint8_t)devAddr);
			}

			tempTimerFlag = FALSE;
		}
	}
}

/* callback to notify timer overflow
 * When the 10s are reached
 * */
void tempTaskCallback(TimerHandle_t xTimer)
{
	tempTimerFlag = TRUE;
}


/* Handles incoming commands over uart */
void vUartCmdTaskHandler(void *params)
{
	while(1)
	{
		vUartHandleCmd(&uartDev);
	}
}

void vUartHandleCmd(UartDev_T *uartDev)
{
	if(uartDev->uartRxFlag == UART_RX_CMP)
	{
		if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_GET_TEMP], strlen(uartCmdList[AT_GET_TEMP]))) == 0)
		{
			// Call Temperature task
			// Blink LED 1/2 second
			if(taskEnablefFlag)
			{
				ledDelay = LED_DELAY_500_MS;
				notifyTemp = 1;
				vTaskSuspend(xaccTaskHandle);
				vTaskResume(xtempTaskHandle);
				taskType = TEMPERATURE_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_GET_ACC], strlen(uartCmdList[AT_GET_ACC]))) == 0)
		{
			// Call accelerometer
			// Print angle every 2 seconds
			// Turn LED every 1 second
			if(taskEnablefFlag)
			{
				ledDelay = LED_DELAY_1_S;
				notifyAcc = 1;
				vTaskSuspend(xtempTaskHandle);
				vTaskResume(xaccTaskHandle);
				taskType = ACCELEROMETER_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_LED_3_ON], strlen(uartCmdList[AT_LED_3_ON]))) == 0)
		{
			// Torn on LED
			if(taskEnablefFlag)
			{
				HAL_GPIO_WritePin(GPIOA, LED3_PIN, GPIO_PIN_SET);
				notifyAcc = 0;
				notifyTemp = 0;
				taskType = DEFAULT_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_LED_3_OFF], strlen(uartCmdList[AT_LED_3_ON]))) == 0)
		{
			// Turn off led
			if(taskEnablefFlag)
			{
				HAL_GPIO_WritePin(GPIOA, LED3_PIN, GPIO_PIN_RESET);
				notifyAcc = 0;
				notifyTemp = 0;
				taskType = DEFAULT_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_LED_4_ON], strlen(uartCmdList[AT_LED_4_ON]))) == 0)
		{
			// Torn on LED
			if(taskEnablefFlag)
			{
				HAL_GPIO_WritePin(GPIOA, LED4_PIN, GPIO_PIN_SET);
				notifyAcc = 0;
				notifyTemp = 0;
				taskType = DEFAULT_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_LED_4_OFF], strlen(uartCmdList[AT_LED_4_ON]))) == 0)
		{
			// Turn off led
			if(taskEnablefFlag)
			{
				HAL_GPIO_WritePin(GPIOA, LED4_PIN, GPIO_PIN_RESET);
				notifyAcc = 0;
				notifyTemp = 0;
				taskType = DEFAULT_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_LED_5_ON], strlen(uartCmdList[AT_LED_5_ON]))) == 0)
		{
			// Torn on LED
			if(taskEnablefFlag)
			{
				HAL_GPIO_WritePin(GPIOA, LED5_PIN, GPIO_PIN_SET);
				notifyAcc = 0;
				notifyTemp = 0;
				taskType = DEFAULT_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_LED_5_OFF], strlen(uartCmdList[AT_LED_5_ON]))) == 0)
		{
			// Turn off led
			if(taskEnablefFlag)
			{
				HAL_GPIO_WritePin(GPIOA, LED5_PIN, GPIO_PIN_RESET);
				notifyAcc = 0;
				notifyTemp = 0;
				taskType = DEFAULT_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_LED_6_ON], strlen(uartCmdList[AT_LED_6_OFF]))) == 0)
		{
			// Torn on LED
			if(taskEnablefFlag)
			{
				HAL_GPIO_WritePin(GPIOA, LED6_PIN, GPIO_PIN_SET);
				notifyAcc = 0;
				notifyTemp = 0;
				taskType = DEFAULT_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_LED_6_OFF], strlen(uartCmdList[AT_LED_6_OFF]))) == 0)
		{
			// Turn off led
			if(taskEnablefFlag)
			{
				HAL_GPIO_WritePin(GPIOA, LED6_PIN, GPIO_PIN_RESET);
				notifyAcc = 0;
				notifyTemp = 0;
				taskType = DEFAULT_TASK;
			}
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_STOP], strlen(uartCmdList[AT_STOP]))) == 0)
		{
			// Suspend all Task not the uart task to monitor incoming command
			taskEnablefFlag = FALSE;
			vTaskSuspend(xledTaskHandle);
			vTaskSuspend(xtempTaskHandle);
			vTaskSuspend(xaccTaskHandle);

			HAL_GPIO_WritePin(GPIOA, LED6_PIN, GPIO_PIN_RESET);

			notifyAcc = 0;
			notifyTemp = 0;
			taskType = DEFAULT_TASK;
		}
		else if((strncasecmp((const char*)uartDev->uartRxBuffer, (const char*)uartCmdList[AT_START], strlen(uartCmdList[AT_START]))) == 0)
		{
			taskEnablefFlag = TRUE;
			//Resume all other task
			vTaskResume(xledTaskHandle);
			vTaskResume(xtempTaskHandle);
			vTaskResume(xaccTaskHandle);

			// Turn off led
			HAL_GPIO_WritePin(GPIOA, LED6_PIN, GPIO_PIN_RESET);
			notifyAcc = 0;
			notifyTemp = 0;
			taskType = DEFAULT_TASK;
		}
		else
		{
			// Report invalid command
			uartSendStr(uartDev, (uint8_t *)"Invalid CMD\r\n", strlen((const char*)"Invalid CMD\r\n"));
			notifyAcc = 0;
			notifyTemp = 0;
			taskType = DEFAULT_TASK;
		}
		// Reset the flag
		uartDev->uartRxFlag = UART_NO_RX;
		memErase(uartDev->uartRxBuffer, uartDev->size);
	}
}

void vKeypadTaskHandler(void *params)
{
	static uint8_t count_wrong_password = 0;
	static char try[10];
	float temperature = 0.0;
	char temp_buf[10];
	char text_buf[20] = "TEMP: ";

	lcd_init();
	lcd_backlight(1);
	//char key = '\n';
	while(1)
	{
		//lcd_write_text("Enter PassWd:", NULL);
		get_pin_code(code_value);

		if(compare_pin(PASSWORD, code_value))
		{
			pPrintf("Good PassWd\n");
			lcd_write_text("Good PassWd:", NULL);
			HAL_Delay(2000);
			lcd_write_text("Welcome:", NULL);

			/* Turn on green LED */
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
			password_state = TRUE;
		}
		else if(compare_pin(RESET_KEY, code_value))
		{
			password_state = FALSE;
			print_twice = 0;

			//lcd_init();
			//lcd_backlight(1);
			lcd_clear();
			HAL_Delay(1);
			lcd_set_cursor(0, 0);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
			count_wrong_password = 0;
		}
		else if(compare_pin(TEMP_REQ_KEY, code_value) )
		{
			if(!password_state)
			{
				lcd_write_text("Not Allowed", NULL);
				HAL_Delay(2000);
				lcd_write_text("Enter PassWd:", NULL);
			}
			else
			{
				MPU9250GetTemp(&temperature);
				pPrintf("MPU9250 Temperature: %d°C\n", (uint8_t)temperature);

				// Convert float to string
				sprintf(temp_buf, "%d", (uint8_t)temperature);
				strcat(temp_buf," C");
				strcat(text_buf, temp_buf);

				lcd_write_text(text_buf, NULL);

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
				password_state = FALSE;
			}
		}
		else
		{
			count_wrong_password++;
			//itoa((WRONG_ENTRY_ALLOWED - count_wrong_password), try, 10);
			sprintf(try, "%d", (WRONG_ENTRY_ALLOWED - count_wrong_password));
			pPrintf("Wrong PassWd\n");

			/* Turn on red LED */
			if(count_wrong_password == 3)
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
				lcd_write_text("System Broken:", NULL);
				password_state = FALSE;
				vTaskEndScheduler();
			}
			lcd_write_text("Wrong PassWd:", NULL);
			HAL_Delay(2000);
			lcd_write_text("Try again:", NULL);
			HAL_Delay(2000);
			lcd_write_text(try, NULL);
			HAL_Delay(2000);
			lcd_write_text("Enter PassWd:", NULL);
			password_state = FALSE;
		}
	}
}


void vLcdTaskHandler(void *params)
{
	lcd_init();
	lcd_backlight(1);

	//HD44780_Init(2);
	while(1)
	{

		HAL_Delay(1);
		lcd_set_cursor(0, 0);
		HAL_Delay(1);
		lcd_write_string("Enter Password");
		HAL_Delay(1);
//		lcd_write_text("Enter PassWd:", NULL);

/*		HD44780_Clear();
		HD44780_SetCursor(0,0);
		HD44780_PrintStr("STM32 BLUE PILL");
		HD44780_SetCursor(0,1);
		HD44780_PrintStr("I2C LCD DEMO");*/

		vTaskDelay(200);
	}
}


