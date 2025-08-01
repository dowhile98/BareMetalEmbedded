/*
 * buttonTask.c
 *
 *  Created on: Jul 8, 2025
 *      Author: jeffr
 */
/*Includes --------------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

#include "cmsis_os.h"



/*task definition -------------------------------------------------------------------*/

void buttonTaskEntry(void const * argument)
{
	/* USER CODE BEGIN buttonTaskEntry */


	/* Infinite loop */
	for(;;)
	{

		osMutexWait(mutexHandle, osWaitForever);
		if(state == 1){
			state = 2;
		}

		osMutexRelease(mutexHandle);
		osDelay(1);
	}
	/* USER CODE END buttonTaskEntry */
}
