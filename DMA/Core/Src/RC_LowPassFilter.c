/*
 * RC_LowPassFilter.c
 *
 *  Created on: Dec 13, 2023
 *      Author: eplim
 */


#include "RC_LowPassFilter.h"

void RC_LowPassFilter_Init(RC_LowPassFilter_t *F, float fc, float ts){
	float rc = 1/( 2 * M_PI * fc);
	F->b = (ts /(ts + 2 * rc));
	F->a = (ts - 2 * rc)/(ts + 2 * rc);
	F->x_1 = 0;
	F->y_1 = 0;
	return;
}

float RC_LowPassFilter_Appy(RC_LowPassFilter_t *F, float x){
	/**
	 * y[n] = b(x[n] + x[n-1]) - a*y(n - 1)
	 */
	float out = F->b * (x + F->x_1) - F->a * F->y_1;
	F->x_1 = x;
	F->y_1 = out;
	return out;
}
