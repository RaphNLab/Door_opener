/*
 * spi.c
 *
 *  Created on: Mar 11, 2023
 *      Author: silvere
 */


#include "spi.h"

uint8_t spiRxBuffer[10];
uint8_t spiTxBuffer[10];
SPI_HandleTypeDef hspi1;
SpiDev_T spiDev;


/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
void spiSetup(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_SLAVE;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
	  while(1);
  }
}

/**
* @brief SPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void spiGpioSetup(void)
{
	GPIO_InitTypeDef gpio_spi;

	/* Reset all struct members */
	memset(&gpio_spi, 0, sizeof(gpio_spi));

	gpio_spi.Pin = SPI_SLK|SPI_MISO|SPI_MOSI;
	gpio_spi.Mode = GPIO_MODE_AF_PP;
	gpio_spi.Pull = GPIO_NOPULL;
	gpio_spi.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_spi.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOB, &gpio_spi);

	/* SPI1 interrupt Init */
	HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(SPI1_IRQn);
}

void spiHardwareSetup(void)
{
	__HAL_RCC_SPI1_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	spiSetup();
	spiGpioSetup();
}

void spiDevConfig(SpiDev_T *spiDev, SPI_HandleTypeDef *hspi, uint8_t *rxBuffer, uint8_t *txBuffer, uint8_t size)
{
	if(spiDev == NULL)
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
		spiDev->spi = hspi;
		spiDev->spiRxBuffer = rxBuffer;
		spiDev->spiTxBuffer = txBuffer;
		spiDev->size = size;

		spiHardwareSetup();
	}
}

/*!
	* Send data over SPI
*/
void spiSend(SpiDev_T *spiDev, uint8_t *txBuffer, uint8_t size)
{
	if(SPI_MODE_SLAVE == spiDev->spi->Init.Mode)
	{
		//Send dummy by to activate clock for slave
	spiDev->spi->Instance->DR = *txBuffer;
	}

	for(uint8_t i = 0U; i < size; i++)
	{
		while(!(spiDev->spi->Instance->SR & SPI_SR_TXE));
		spiDev->spi->Instance->DR = txBuffer[i];

		// Wait for the BSY bit of the status register to become false
		while((spiDev->spi->Instance->SR & SPI_SR_BSY));
	}
}

/*!
	* Receive data over SPI
*/
void spiReceive(SpiDev_T *spiDev, uint8_t *rxBuffer, uint8_t size)
{
	if(SPI_MODE_SLAVE == spiDev->spi->Init.Mode)
	{
		//Send dummy by to activate clock
		spiSendReceive(spiDev, rxBuffer, rxBuffer, size);
	}
	else
	{
		for(uint8_t i = 0U; i < size; i++)
		{
			while(!(spiDev->spi->Instance->SR & SPI_SR_RXNE)); // Wait for the RX bit of the register to become empty
			rxBuffer[i] = spiDev->spi->Instance->DR; // Read one byte from DR

			// Wait for the BSY bit of the status register to become false
			while((spiDev->spi->Instance->SR & SPI_SR_BSY));
		}
	}
}

/*!
	*Send and receive each byte sent over SPI in blocking mode
*/
Bool_T spiSendReceive(SpiDev_T *spiDev, uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t size)
{
	Bool_T ret_val = FALSE;

	if(0U == size)
	{
		// Report size too small
	}
	else if(NULL == txBuffer)
	{
		// Report tx_buffer not initialized
	}
	else if(NULL == rxBuffer)
	{
		// Report rx_buffer not initialized
	}

	else
	{
		if(spiDev->spi->Init.Mode == SPI_MODE_SLAVE)
		{
			spiDev->spi->Instance->DR = *txBuffer; // Write dummy byte to enable clock for slave
		}

		int tx_allowed = 1U;
		for(uint8_t i = 0U; i < size; i++)
		{
			// Transmit each byte
			while((!(spiDev->spi->Instance->SR & SPI_SR_TXE)) && !tx_allowed);
			spiDev->spi->Instance->DR = txBuffer[i];
			tx_allowed = 0U;

			// Receive each byte
			while(!(spiDev->spi->Instance->SR & SPI_SR_RXNE)); // Wait for the RX bit of the register to become empty
			rxBuffer[i] = spiDev->spi->Instance->DR; // Read one byte from DR
			tx_allowed = 1U;

			if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12))
			{
				break;
			}
		}
		if(spiDev->spi->Init.Mode == SPI_MODE_MASTER)
		{
			// Wait for the BSY bit of the status register to become false
			while((spiDev->spi->Instance->SR & SPI_SR_BSY));
		}
		ret_val = TRUE;
	}
	return ret_val;
}


void spiWriteByte(SpiDev_T *spiDev, uint8_t reg, uint8_t data)
{
	uint8_t tmp[2] = {0};

	tmp[0] = reg;
	// Register reset value
	tmp[1] = data;

	spiSend(spiDev, tmp, 2);
}


uint8_t spiReadByte(SpiDev_T *spiDev, uint8_t reg)
{
	uint8_t tmp[2] = {0};

	tmp[0] = reg;

	spiSend(spiDev, tmp, 1);
	spiReceive(spiDev, (tmp+1), 1);

	return tmp[1];
}

void spiReadBytes(SpiDev_T *spiDev, uint8_t reg, uint8_t *dataBuffer, uint8_t size)
{
	uint8_t tmp[15] = {0};
	uint8_t daraReg[1] = {0};

	daraReg[0] = reg;

	spiSend(spiDev, daraReg, 1);
	spiReceive(spiDev, tmp, size);

	memCopy(tmp, dataBuffer, size);
}



/**
  * @brief This function handles SPI1 global interrupt.
  */
void SPI1_IRQHandler(void)
{


}

