/*
 * delay.c
 *
 *  Created on: May 22, 2025
 *      Author: tecna-smart-lab
 */
#include "delay.h"


/*Static variables ----------------------------------------------------------------*/
volatile uint32_t uwTicks = 0;

/*private functions ---------------------------------------------------------------*/

/*Function definition -------------------------------------------------------------*/
void tickInc(uint32_t i){

	uwTicks += 1;
}

uint32_t ticksGet(void){

	return uwTicks;
}

void delay_ms(uint32_t delay){

	uint32_t startTicks = ticksGet();

	while((ticksGet() - startTicks)<= delay){

	}
	return;
}
