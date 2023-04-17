/*
 * memory.c
 *
 *  Created on: Mar 12, 2023
 *      Author: silvere
 */

#include "memory.h"

void memCopy(uint8_t *src, uint8_t *dest, uint8_t size)
{
	uint8_t i = 0;
	if(src == NULL)
	{
		//Report invalid source address
	}
	else if(dest == NULL)
	{
		// Report invalid destination address
	}
	else
	{
		while(i < size)
		{
			dest[i] = src[i];
			i++;
		}
	}
}

void memErase(uint8_t *buffer, uint8_t size)
{
	uint8_t i = 0;
	while(i < size)
	{
		buffer[i] = 0;
		i++;
	}
}


uint16_t memCRCCalc(uint8_t buffer, uint8_t size)
{
	uint16_t retVal = 0;

	return retVal;
}



