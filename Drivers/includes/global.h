/*
 * global.h
 *
 *  Created on: Mar 11, 2023
 *      Author: silvere
 */

#ifndef INCLUDES_GLOBAL_H_
#define INCLUDES_GLOBAL_H_

#include "FreeRTOS.h"
#include "stm32l1xx.h"
#include "stm32l1xx_nucleo.h"
#include "memory.h"
#include "math.h"

#include "task.h"
#include "timers.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>


typedef enum
{
	FALSE = 0,
	TRUE
}Bool_T;

#define PI 3.14
#define RAD_TO_DEG   57.295779513

#endif /* INCLUDES_GLOBAL_H_ */
