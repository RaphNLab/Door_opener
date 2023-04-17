/*
 * uart.c
 *
 *  Created on: Mar 11, 2023
 *      Author: silvere
 */

#include "uart.h"

uint8_t uartRxBuffer[10];
uint8_t uartTxBuffer[10];
UartDev_T uartDev;
UART_HandleTypeDef huart2;


uint8_t isrCnt = 0;

char *uartCmdList[] =
{
	"AT+GETTEMP",
	"AT+GETACC",
	AT_LED(3, EIN),
	AT_LED(3, AUS),
	AT_LED(4, EIN),
	AT_LED(4, AUS),
	AT_LED(5, EIN),
	AT_LED(5, AUS),
	AT_LED(6, EIN),
	AT_LED(6, AUS),
	"AT+STOP",
	"AT+START"
};


/*
 * Configure UART peripheral speed and various parameters
 * */
void uartSetup(void)
{
	/* Reset all struct members */
	memset(&huart2, 0, sizeof(huart2));

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.StopBits = USART_STOPBITS_1;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.WordLength = USART_WORDLENGTH_8B;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_Init(&huart2);
}

/*
 * Configure GPIO pins for RX and TX pin of the UART peripheral
 * as alternate function 7
 * Configure GPIO pin 5 for the LED as utput
 * */
void uartGpioSetup(void)
{
	GPIO_InitTypeDef gpio_uart;
	GPIO_InitTypeDef gpio_led;

	/* Reset all struct members */
	memset(&gpio_uart, 0, sizeof(gpio_uart));
	memset(&gpio_led, 0, sizeof(gpio_led));


	/* Configure alternate function */
	gpio_uart.Pin = UART_TX | UART_RX;
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull = GPIO_PULLUP;
	gpio_uart.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_led.Pin = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6); // LED 3 must be configured as well
	gpio_led.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_led.Pull = GPIO_NOPULL;
	gpio_led.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gpio_led);


	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/*
 * This function is used to send strings over UART
 * */
void uartPuts(unsigned char *c)
{
	HAL_UART_Transmit(&huart2, c, strlen((char*) c), HAL_MAX_DELAY);
}

/*
 * Enable the peripheral clock of both devices GPIO and UART
 *
 * */
void uartHardwareSetup(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();

	/* Init GPIOA */
	uartGpioSetup();
	uartSetup();
}

/**
 * This function configures the UART device as well as its transmit and receive buffers
 */

void uartDevConfig(UartDev_T *uartDev, UART_HandleTypeDef *huart, uint8_t *rxBuffer, uint8_t *txBuffer, uint8_t size)
{
	if(uartDev == NULL)
	{
		// Report invalid device pointer
	}
	else if (txBuffer == NULL)
	{
		// Report invalid buffer pointer
	}
	else if (rxBuffer == NULL)
	{
		// Report invalid buffer pointer
	}
	else
	{
		uartDev->huart = huart;
		uartDev->uartRxBuffer = rxBuffer;
		uartDev->uartTxBuffer = txBuffer;
		uartDev->uartRxFlag = UART_NO_RX;
		uartDev->uartTxFlag = UART_NO_TX;
		uartDev->size = size;

		uartHardwareSetup();

	    /* Enable the UART Data Register not empty Interrupt */
	    __HAL_UART_ENABLE_IT(uartDev->huart, UART_IT_RXNE);
	}
}


/*
 * This function is used to request commands coming from serial port in blocking mode
 * */
void uartScanCmd(UartDev_T *uartDev, uint8_t *rxBuffer, uint8_t size)
{
	if(uartDev == NULL)
	{
		// Report invalid device pointer
	}
	else if (rxBuffer == NULL)
	{
		// Report invalid buffer pointer
	}
	else
	{
		while(size--)
		{
			uartScanC(uartDev, rxBuffer);

			if(*rxBuffer == '\r')
			{
				break;
			}
			rxBuffer++;
		}
	}
}


/*
 * This function is used to strings over serial port in blocking mode
 * */
void uartSendStr(UartDev_T *uartDev, uint8_t *txBuffer, uint8_t size)
{
	if(uartDev == NULL)
	{
		// Report invalid device pointer
	}
	else if (txBuffer == NULL)
	{
		// Report invalid buffer pointer
	}
	else
	{
		while(size--)
		{
			uartSendC(uartDev, *(txBuffer++));
		}
	}
}


void uartSendC(UartDev_T *uartDev, uint8_t c)
{
	// wait for TX flag to
	while(!(uartDev->huart->Instance->SR & UART_FLAG_TXE));
	uartDev->huart->Instance->DR = (c & (uint8_t)0xFF);
}


void uartScanC(UartDev_T *uartDev, uint8_t *c)
{
	// wait for RX flag to
	while(!(uartDev->huart->Instance->SR & UART_FLAG_RXNE));
	*c = (uartDev->huart->Instance->DR  & (uint8_t)0xFF);
}


/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
	if(uartDev.huart->Instance->SR & UART_IT_RXNE)
	{
		if(uartDev.huart->Instance->DR != '\r')
		{
			uartDev.uartRxBuffer[isrCnt] = (uartDev.huart->Instance->DR  & (uint8_t)0xFF);
			isrCnt++;
		}
		else // If the enter character is received
		{
			uartDev.uartRxFlag = UART_RX_CMP;
			isrCnt = 0;
		}
	}
}


void vPrint(const char *fmt, va_list argp)
{
  char string[200];
  char ret = '\r';
  char *p = string;

  if(0 < vsprintf(string,fmt,argp)) // build string
  {
    while(*p != '\n')
    {
    	uartSendC(&uartDev, (uint8_t)*(p++));
    }
    //CR
    uartSendC(&uartDev, (uint8_t)ret);
    uartSendC(&uartDev, (uint8_t)'\n');
  }
}

// This is a custum printf function

void pPrintf(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vPrint(fmt, argp);
    va_end(argp);
}

