/*
 * spi.h
 *
 *  Created on: Mar 11, 2023
 *      Author: silvere
 */

#ifndef INCLUDES_SPI_H_
#define INCLUDES_SPI_H_

#include "global.h"

#define SPI_SLK 	GPIO_PIN_3 //PB5
#define SPI_MISO 	GPIO_PIN_4 //PB6
#define SPI_MOSI	GPIO_PIN_5 //PB7
#define SPI_CS		GPIO_PIN_6 //PB6


typedef struct
{
	SPI_HandleTypeDef *spi;
	uint8_t *spiRxBuffer;
	uint8_t *spiTxBuffer;
	uint8_t size;
}SpiDev_T;

extern uint8_t spiRxBuffer[10];
extern uint8_t spiTxBuffer[10];
extern SPI_HandleTypeDef hspi1;
extern SpiDev_T spiDev;


void spiSetup(void);
void spiGpioSetup(void);
void spiHardwareSetup(void);

void spiDevConfig(SpiDev_T *spiDev, SPI_HandleTypeDef *hspi, uint8_t *rxBuffer, uint8_t *txBuffer, uint8_t size);
void spiSend(SpiDev_T *spiDev, uint8_t *rxBuffer, uint8_t size);
void spiReceive(SpiDev_T *spiDev, uint8_t *txBuffer, uint8_t size);
Bool_T spiSendReceive(SpiDev_T *spiDev, uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t size);

void spiWriteByte(SpiDev_T *i2cDev, uint8_t reg, uint8_t data);
uint8_t spiReadByte(SpiDev_T *i2cDev, uint8_t reg);
void spiReadBytes(SpiDev_T *spiDev, uint8_t reg, uint8_t *dataBuffer, uint8_t size);

#endif /* INCLUDES_SPI_H_ */
