/*
 * task_manager.h
 *
 *  Created on: Mar 23, 2023
 *      Author: silvere
 */

#include "global.h"
#include "uart.h"
#include "i2c.h"
#include "spi.h"
#include "mpu_9250.h"
#include "keypad.h"


#ifndef INCLUDES_TASK_MANAGER_H_
#define INCLUDES_TASK_MANAGER_H_


typedef enum
{
	ACCELEROMETER_TASK = 0,
	GYROSCOPE_TASK,
	TEMPERATURE_TASK,
	DEFAULT_TASK
}TaskType_T;

extern const TickType_t tempTaskDelay;
extern const TickType_t accTaskDelay;

extern TaskHandle_t xledTaskHandle;
extern TaskHandle_t xuartTaskHandle;
extern TaskHandle_t xaccTaskHandle;
extern TaskHandle_t xtempTaskHandle;
extern TaskHandle_t xkeypadTaskHandle;

extern TaskHandle_t xtimerTaskHandle;
extern TimerHandle_t xAccTimerHandle;
extern TimerHandle_t xTempTimerHandle;

extern TaskType_T taskType;

/* functions prototypes */


void vLedTaskHandler(void *params);
void vUartCmdTaskHandler(void *params);
void vUartHandleCmd(UartDev_T *uartDev);
void vAccTaskHandler(void *params);
void vTempTaskHandler(void *params);
void vKeypadTaskHandler(void *params);

void accTaskCallback(TimerHandle_t xTimer);
void tempTaskCallback(TimerHandle_t xTimer);

float map(int16_t x, int16_t inMin, int16_t inMax, int16_t outMin, int16_t outMax);


#endif /* INCLUDES_TASK_MANAGER_H_ */
