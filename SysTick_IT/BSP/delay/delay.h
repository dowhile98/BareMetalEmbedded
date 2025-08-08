/*
 * delay.h
 *
 *  Created on: May 22, 2025
 *      Author: tecna-smart-lab
 *  @file delay.h
 *  @brief This file contains the prototypes for delay functions using SysTick.
 */

#ifndef DELAY_DELAY_H_
#define DELAY_DELAY_H_

#include <stdint.h>

/**
 * @brief Delays for a specified number of milliseconds.
 * @param delay: Number of milliseconds to delay.
 */
void delay_ms(uint32_t delay);

/**
 * @brief Increments the tick counter. This function is usually called from the SysTick interrupt handler.
 * @param i: Value to increment the tick counter by.
 */
void tickInc(uint32_t i);

/**
 * @brief Gets the current value of the tick counter.
 * @return The current value of the tick counter.
 */
uint32_t ticksGet(void);


#endif /* DELAY_DELAY_H_ */
