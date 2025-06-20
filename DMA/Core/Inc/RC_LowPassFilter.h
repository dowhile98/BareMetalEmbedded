/*
 * RC_Filter.h
 *
 *  Created on: Dec 13, 2023
 *      Author: eplim
 */

#ifndef INC_RC_LOWPASSFILTER_H_
#define INC_RC_LOWPASSFILTER_H_

/*Includes --------------------------------------------------------------*/
#include <math.h>
/*Typedefs --------------------------------------------------------------*/
typedef struct{
	float a;
	float b;
	float x_1;
	float y_1;
}RC_LowPassFilter_t;
/*Defines ---------------------------------------------------------------*/

/*Function definition ---------------------------------------------------*/
void RC_LowPassFilter_Init(RC_LowPassFilter_t *F, float fc, float ts);

float RC_LowPassFilter_Appy(RC_LowPassFilter_t *F, float x);
#endif /* INC_RC_FILTER_H_ */
