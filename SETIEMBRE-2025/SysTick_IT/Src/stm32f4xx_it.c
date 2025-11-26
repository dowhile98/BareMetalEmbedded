/*
 * stm32f4xx_it.c
 *
 *  Created on: Aug 7, 2025
 *      Author: tecna-smart-lab
 */

#include "stm32f4xx.h"
#include "delay.h"

void SysTick_Handler(void)
{
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
	{
		tickInc(1);
	}
}
