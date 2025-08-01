/*
 * stm32f4xx_IT.c
 *
 *  Created on: May 22, 2025
 *      Author: tecna-smart-lab
 */
#include "delay.h"

void SysTick_Handler(void){
	/*ticks*/
	tickInc(1);

	return;
}

