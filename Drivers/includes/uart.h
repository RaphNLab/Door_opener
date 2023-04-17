/*
 * uart.h
 *
 *  Created on: Mar 11, 2023
 *      Author: silvere
 */

#ifndef INCLUDES_UART_H_
#define INCLUDES_UART_H_

#include "global.h"


#define UART_TX GPIO_PIN_2
#define UART_RX GPIO_PIN_3

#define LED_DELAY_1_S 		50
#define LED_DELAY_500_MS	25

#define LED3_PIN GPIO_PIN_3
#define LED4_PIN GPIO_PIN_4
#define LED5_PIN GPIO_PIN_5
#define LED6_PIN GPIO_PIN_6

// Stringification for LED control
#define AT_LED(pin,	status) "AT+LED" #pin " " #status

typedef enum
{
	AT_GET_TEMP = 0,
	AT_GET_ACC,
	AT_LED_3_ON,
	AT_LED_3_OFF,
	AT_LED_4_ON,
	AT_LED_4_OFF,
	AT_LED_5_ON,
	AT_LED_5_OFF,
	AT_LED_6_ON,
	AT_LED_6_OFF,
	AT_STOP,
	AT_START
}UartAtCmd_t;


typedef enum
{
	UART_RX_CMP = 0,
	UART_TX_CMP,
	UART_NO_RX,
	UART_NO_TX
}UartItFlag_T;


typedef struct
{
	UART_HandleTypeDef *huart;
	uint8_t *uartRxBuffer;
	uint8_t *uartTxBuffer;
	UartItFlag_T uartRxFlag;
	UartItFlag_T uartTxFlag;
	uint8_t size;
}UartDev_T;

extern uint8_t uartRxBuffer[10];
extern uint8_t uartTxBuffer[10];
extern UART_HandleTypeDef huart2;
extern UartDev_T uartDev;
extern char *uartCmdList[];

void uartSetup(void);
void uartGpioSetup(void);
void uartHardwareSetup(void);
void uartPuts(unsigned char *c);

void uartDevConfig(UartDev_T *uartDev, UART_HandleTypeDef *huart, uint8_t *rxBuffer, uint8_t *txBuffer, uint8_t size);
void uartScanCmd(UartDev_T *uartDev, uint8_t *rxBuffer, uint8_t size);
void uartScanC(UartDev_T *uartDev, uint8_t *c);
void uartSendStr(UartDev_T *uartDev, uint8_t *txBuffer, uint8_t size);
void uartSendC(UartDev_T *uartDev, uint8_t c);


void pPrintf(const char *fmt, ...);
void vPrint(const char *fmt, va_list argp);

#endif /* INCLUDES_UART_H_ */
