/*
 * memory.h
 *
 *  Created on: Mar 12, 2023
 *      Author: silvere
 */

#ifndef INCLUDES_MEMORY_H_
#define INCLUDES_MEMORY_H_

#include <stdint.h>
#include <stdio.h>

void memCopy(uint8_t *scr, uint8_t *dest, uint8_t size);
void memErase(uint8_t *buffer, uint8_t size);
uint16_t memCRCCalc(uint8_t buffer, uint8_t size);



#endif /* INCLUDES_MEMORY_H_ */
