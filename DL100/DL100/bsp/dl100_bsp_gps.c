/*
 * dl100_bsp_gps.c
 *
 *  Created on: Nov 25, 2025
 *      Author: tecna-smart-lab
 */
#include "dl100_bsp_gps.h"
#include "usart.h"
#include "lwrb.h"

#ifndef RX_BUFF_SIZE
#define RX_BUFF_SIZE 1024*2
#endif

static uint8_t rx_rb_data[RX_BUFF_SIZE];
static lwrb_t rx_rb;
static uint8_t byte;

static void BSP_GPS_UART_RX_Callback(UART_HandleTypeDef *huart);

void BSP_GPS_Init(void)
{
	/*uart*/
	MX_USART2_UART_Init();
	/*rx buffer*/
	lwrb_init(&rx_rb, rx_rb_data, RX_BUFF_SIZE);

	HAL_UART_RegisterCallback(&huart2, HAL_UART_RX_COMPLETE_CB_ID, BSP_GPS_UART_RX_Callback);

	//start receive data
	HAL_UART_Receive_IT(&huart2, &byte, 1);
}

size_t BSP_GPS_Available(void)
{
	return lwrb_get_full(&rx_rb);


}

uint8_t BSP_GPS_Read(uint8_t *buff, size_t len)
{

	if(lwrb_read(&rx_rb, buff, len) >= 0)

	{
		return 1;
	}
//	if(HAL_UART_Receive(&huart2, buff, len, HAL_MAX_DELAY) != HAL_OK)
//	{
//		return 0;
//	}

	return 0;
}

uint8_t BSP_GPS_Write(uint8_t *buff, size_t len)
{

	return len;
}

uint8_t BSP_GPS_ReadIT(uint8_t *buff, size_t len, void *user_data)
{


	return 0;
}

static void BSP_GPS_UART_RX_Callback(UART_HandleTypeDef *huart)
{
	//put data to rb
	lwrb_write(&rx_rb, &byte, 1);
	//New data
	HAL_UART_Receive_IT(huart, &byte, 1);
}
