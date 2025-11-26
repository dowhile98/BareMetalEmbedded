/*
 * dl100_bsp.h
 *
 *  Created on: Nov 25, 2025
 *      Author: tecna-smart-lab
 */

#ifndef BSP_DL100_BSP_H_
#define BSP_DL100_BSP_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>




typedef enum BSP_Pins
{
	LED0 = 0,
	LED1,
	LED2,
	LED3,
	BUTTON,

	BSP_PINS_MAX
}BSP_Pins_t;

void BSP_GPIO_Init(void);

void BSP_GPIO_Write(BSP_Pins_t pin, uint8_t level);

uint8_t BSP_GPIO_Read(BSP_Pins_t pin);

void BSP_GPIO_Toggle(BSP_Pins_t pin);

#endif /* BSP_DL100_BSP_H_ */
