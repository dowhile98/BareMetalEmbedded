/*
 * delay.h
 *
 *  Created on: May 22, 2025
 *      Author: tecna-smart-lab
 */

#ifndef DELAY_DELAY_H_
#define DELAY_DELAY_H_

#include <stdint.h>

void delay_ms(uint32_t delay);

void tickInc(uint32_t i);

uint32_t ticksGet(void);


#endif /* DELAY_DELAY_H_ */
