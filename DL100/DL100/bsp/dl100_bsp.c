/*
 * dl100_bsp.c
 *
 *  Created on: Nov 25, 2025
 *      Author: tecna-smart-lab
 */


#include "dl100_bsp.h"
#include "gpio.h"


typedef struct BSP_GPIO_Def
{
	GPIO_TypeDef *PORT;
	uint16_t pin;
	/*funcion, modo*/

}BSP_GPIO_Def_t;


static BSP_GPIO_Def_t Pins[BSP_PINS_MAX] = {
		{.PORT = LED0_GPIO_Port, .pin = LED0_Pin}, /*LED0*/
		{0},
		{0},
		{0},
		{.PORT = INPUT_GPIO_Port, .pin = INPUT_Pin}
};

void BSP_GPIO_Init(void)
{
	MX_GPIO_Init();

}

void BSP_GPIO_Write(BSP_Pins_t pin, uint8_t level)
{
	if(pin > BSP_PINS_MAX || Pins[pin].PORT == NULL)
	{
		return ;
	}

	HAL_GPIO_WritePin(Pins[pin].PORT, Pins[pin].pin, level);
}

uint8_t BSP_GPIO_Read(BSP_Pins_t pin)
{
	if(pin > BSP_PINS_MAX || Pins[pin].PORT == NULL)
	{
		return 0;
	}

	return HAL_GPIO_ReadPin(Pins[pin].PORT, Pins[pin].pin);

}

void BSP_GPIO_Toggle(BSP_Pins_t pin)
{
	if(pin > BSP_PINS_MAX || Pins[pin].PORT == NULL)
	{
		return ;
	}

	HAL_GPIO_TogglePin(Pins[pin].PORT, Pins[pin].pin);
}
