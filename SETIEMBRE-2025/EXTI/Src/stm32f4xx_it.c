/*
 * stm32f4xx_it.c
 *
 *  Created on: Aug 12, 2025
 *      Author: tecna-smart-lab
 */
#include "stm32f4xx.h"

__WEAK void EXTI_GPIO_Callback(uint32_t pin)
{

}


void EXTI_IRQ_Handler(uint32_t exti)
{
	/*verify*/
	if(EXTI->PR & 1<<exti)
	{
		/*clear*/
		EXTI->PR |= 1<<exti;
		/*callback*/
		EXTI_GPIO_Callback(exti);
	}

}

void EXTI0_IRQHandler(void)
{
	EXTI_IRQ_Handler(0);
}


void EXTI1_IRQHandler(void)
{
	EXTI_IRQ_Handler(1);
}


void EXTI2_IRQHandler(void)
{
	EXTI_IRQ_Handler(2);
}
