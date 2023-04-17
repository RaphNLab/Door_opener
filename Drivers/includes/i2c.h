/*
 * i2c.h
 *
 *  Created on: Mar 11, 2023
 *      Author: silvere
 */

#ifndef INCLUDES_I2C_H_
#define INCLUDES_I2C_H_


#include "global.h"

#define I2C_SCL GPIO_PIN_8
#define I2C_SDA GPIO_PIN_9

typedef struct
{
	I2C_HandleTypeDef *hi2c;
	uint8_t *i2cRxBuffer;
	uint8_t *i2cTxBuffer;
	uint8_t size;
}I2cDev_T;


extern uint8_t i2cRxBuffer[10];
extern uint8_t i2cTxBuffer[10];
extern I2C_HandleTypeDef hi2c1;
extern I2cDev_T i2cDev;

void i2cSetup(void);
void i2cGpioSetup(void);
void i2cHardwareSetup(void);

void i2cWriteByte(I2cDev_T *i2cDev, uint8_t devAddr, uint8_t reg, uint8_t data);
uint8_t i2cReadByte(I2cDev_T *i2cDev, uint8_t devAddr, uint8_t reg);
void i2cReadBytes(I2cDev_T *i2cDev, uint8_t devAddr, uint8_t reg, uint8_t * dataBuffer, uint8_t size);
void i2cDevConfig(I2cDev_T *i2cDev, I2C_HandleTypeDef *hi2c);

#endif /* INCLUDES_I2C_H_ */
