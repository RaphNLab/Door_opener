/*
 * keypad.c
 *
 *  Created on: Apr 15, 2023
 *      Author: silvere
 */

#include "keypad.h"

uint32_t col_arr[COLUMN_SIZE] = {(1 << COL0), (1 << COL1), (1 << COL2),	(1 << COL3)};
uint32_t raw_arr[RAW_SIZE] = {
							(1 << RAW0),
							(1 << RAW1),
							(1 << RAW2),
							(1 << RAW3)
							};

char keypad_values[RAW_SIZE][COLUMN_SIZE] = {
									 {'1', '2', '3', 'A'},
									 {'4', '5', '6', 'B'},
									 {'7', '8', '9', 'C'},
									 {'*', '0', '#', 'D'}
									 };


char code_value[7] = {0};

/**
 * @brief Debounce the button and give the button status back
 *
 * @param pin
 * @return uint8_t
 */
uint8_t debounce_button(GPIO_TypeDef *port, uint32_t pin)
{
	static uint8_t button_status = 0;
	uint8_t button_value = 0;

	if((button_status == 0) && !(HAL_GPIO_ReadPin(port, pin)))
	{
		button_status = 1;
		button_value = 1;
	}
	else if((button_status == 1) && !(HAL_GPIO_ReadPin(port, pin)))
	{
		button_status = 2;
		button_value = 0;
	}
	else if((button_status == 2) && (HAL_GPIO_ReadPin(port, pin)))
	{
		button_status = 3;
		button_value = 0;
	}
	else if((button_status == 3) && (HAL_GPIO_ReadPin(port, pin)))
	{
		button_status = 0;
		button_value = 0;
	}

	return button_value;
}


/**
 * @brief Initialize the keypad
 * 		  Set the column pins as input and the row pins as output
 *
 */
void init_keypad(void)
{
	/* Set all Column pin to Input */
	/* Set all Row pins as output pulled up. So pressing a key will put the pin to Low*/

	GPIO_InitTypeDef keypad_gpio_raw;
	GPIO_InitTypeDef keypad_gpio_col;
	GPIO_InitTypeDef led_gpio;

	// Enable the GPIOA and GPIOC clock
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	led_gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	led_gpio.Mode = GPIO_MODE_OUTPUT_PP;
	led_gpio.Pull = GPIO_NOPULL;
	led_gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &led_gpio);

	/* Set column pin as input */
	for(int i = 0; i < COLUMN_SIZE; i++)
	{
		keypad_gpio_col.Pin |= col_arr[i];
	}

	keypad_gpio_col.Mode = GPIO_MODE_INPUT;
	keypad_gpio_col.Pull = GPIO_PULLDOWN;
	keypad_gpio_col.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(KEYPAD_PORT, &keypad_gpio_col);


	/* Set row pins as output */
	for(int i = 0; i < RAW_SIZE; i++)
	{
		keypad_gpio_raw.Pin |= raw_arr[i];
	}

	keypad_gpio_raw.Mode = GPIO_MODE_OUTPUT_PP;
	keypad_gpio_raw.Pull = GPIO_NOPULL;
	keypad_gpio_raw.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(KEYPAD_PORT, &keypad_gpio_raw);

	// Reset all raw to 0
	for(int i = 0; i < RAW_SIZE; i++)
	{
		HAL_GPIO_WritePin(KEYPAD_PORT, raw_arr[i], GPIO_PIN_RESET);
	}
}

/**
 * @brief Read a specific key of the keypad
 * @param void
 * @return key
 */
char  key_read(void)
{	char key = '\r';
	for(int raw = 0; raw < RAW_SIZE; raw++)
	{
		/* Set column pin successively to high */
		HAL_GPIO_WritePin(KEYPAD_PORT, raw_arr[raw], GPIO_PIN_SET);
		HAL_Delay(2);
		/* Read each row for the current column */
		for(int column = 0; column < COLUMN_SIZE; column++)
		{
			if(HAL_GPIO_ReadPin(KEYPAD_PORT, col_arr[column]))
			{
				// Debounce the button
				HAL_Delay(150);
				if(HAL_GPIO_ReadPin(KEYPAD_PORT, col_arr[column]))
				{
					key = keypad_values[raw][column];
				}
			}
		}

		/* Reset previous row pin */
		HAL_GPIO_WritePin(KEYPAD_PORT, raw_arr[raw], GPIO_PIN_RESET);
		HAL_Delay(2);
	}
	return key;
}

/**
 * @brief Get the key char object
 *
 * @param key
 * @return char
 */
char get_key_char(char key)
{
	/* initialize the key with actual pin value */
	if(key >= '0' && key <= '9')
	{
		key += 30;
	}
	return key;
}


/**
 * @brief Get the pin code object
 * @param code pointer to store the pin code
 * @return void
 */
void get_pin_code(char *code)
{
	char key = 0;
	uint8_t  i = 0;

	while (key != '#')
	{
		key = key_read();

		if((key != '\r') && (key != '#'))
		{
			code[i] = key;
			i++;
		}
	}
}

/**
 * This function comares the input pin code with the real password
 * @param: code the real code
 * @param: input the user input value
 * */

Bool_T compare_pin(char *code, char *input)
{
	Bool_T result = FALSE;
	if((strncasecmp((const char*)code, (const char*)input, strlen(code))) == 0)
	{
		result = TRUE;
	}
	else
	{
		result = FALSE;
	}
	return result;
}

