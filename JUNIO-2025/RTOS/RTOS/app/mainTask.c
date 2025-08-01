/*
 * mainTask.c
 *
 *  Created on: Jul 8, 2025
 *      Author: jeffr
 */
#include "cmsis_os.h"

uint8_t state  = 0;

void mainTask(void const * argument)
{
	/* USER CODE BEGIN mainTask */
	//local variables
	char *pointer = pvPortMalloc(256);//malloc


	vPortFree(pointer);//free
	//call init function

	/* Infinite loop */
	for(;;)
	{
		//tomas mutex
		osMutexWait(mutexHandle, osWaitForever);
		state = 1;

		//liberas el mutex
		osMutexRelease(mutexHandle);


		osDelay(1);
	}
	/* USER CODE END mainTask */
}
