/*
 * delay.c
 *
 *  Created on: May 22, 2025
 *      Author: tecna-smart-lab
 */
#include "delay.h"


/*Static variables ----------------------------------------------------------------*/
volatile uint32_t uwTicks = 0; // Variable to store the number of ticks since the start of the program

/*private functions ---------------------------------------------------------------*/

/*Function definition -------------------------------------------------------------*/
/**
 * @brief Increments the global tick counter.
 * @param i: Value to increment the tick counter by.
 */
void tickInc(uint32_t i){

    uwTicks += i; // Increment the tick counter by the given value
}

/**
 * @brief Gets the current value of the tick counter.
 * @return The current value of the tick counter.
 */
uint32_t ticksGet(void){

    return uwTicks; // Return the current value of the tick counter
}

/**
 * @brief Delays for a specified number of milliseconds using the SysTick timer.
 * @param delay: Number of milliseconds to delay.
 */
void delay_ms(uint32_t delay){

    uint32_t startTicks = ticksGet(); // Get the current tick value

    // Wait until the required number of milliseconds has passed
    while((ticksGet() - startTicks) < delay){ // Changed '<=' to '<' to avoid potential one tick overshoot
	}
}