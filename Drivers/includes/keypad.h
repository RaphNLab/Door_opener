/*
 * keypad.h
 *
 *  Created on: Apr 15, 2023
 *      Author: silvere
 */

#ifndef INCLUDES_KEYPAD_H_
#define INCLUDES_KEYPAD_H_

#include "global.h"
#include "uart.h"

#define COL0                5 //PC5
#define COL1                1 //PC6
#define COL2                0 //PC14
#define COL3                3 //PC15

#define RAW0                10 //PC0
#define RAW1                11 //PC11
#define RAW2                2 //PC2
#define RAW3                4 //PC3

#define KEYPAD_PORT			GPIOC	   // GPIO port C

#define COLUMN_SIZE         4U
#define RAW_SIZE            COLUMN_SIZE
#define KEYPAD_SIZE         (COLUMN_SIZE * RAW_SIZE)
#define END_KEY             '#'
#define KEY_DEBOUNCE_TIME   1000U
#define PASSWORD			"ABC1234"


extern uint32_t col_arr[COLUMN_SIZE];
extern uint32_t row_arr[RAW_SIZE];
extern char keypad_values[COLUMN_SIZE][RAW_SIZE];
extern char code_value[7];

void init_keypad(void);
char key_read(void);
char get_key_char(char key);
void get_pin_code(char *code);
Bool_T compare_pin(char* code, char* input);
uint8_t debounce_button(GPIO_TypeDef *port, uint32_t pin);


#endif /* INCLUDES_KEYPAD_H_ */
