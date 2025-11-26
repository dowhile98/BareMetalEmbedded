/*
 * dl100_bsp_gps.c
 *
 *  Created on: Nov 25, 2025
 *      Author: tecna-smart-lab
 */
#include "dl100_bsp_gps.h"
#include "usart.h"

typedef void (*gps_user_callback_t)(uint8_t *data, size_t len);

static uint8_t *byte = NULL;
gps_user_callback_t callback = NULL;



void BSP_GPS_Init(void)
{
	MX_USART2_UART_Init();
}

uint8_t BSP_GPS_Available(void)
{
	uint8_t ret = 0;
	//CMSIS
	if(USART2->SR & USART_SR_RXNE)
	{
		ret = 1;
	}
	//LL
	if(LL_USART_IsActiveFlag_RXNE(USART2))
	{
		ret = 1;
	}
	//HAL

	return ret;
}

uint8_t BSP_GPS_Read(uint8_t *buff, size_t len)
{
	size_t i = 0;
	while(len > 0)
	{
		if(LL_USART_IsActiveFlag_RXNE(USART2))
		{
			buff[i] = LL_USART_ReceiveData8(USART2);
			i += 1;
			len -= 1;
		}
	}

	return len;
}

uint8_t BSP_GPS_Write(uint8_t *buff, size_t len)
{

	return len;
}

uint8_t BSP_GPS_ReadIT(uint8_t *buff, size_t len, void *user_data)
{
	byte = buff;
	callback = (gps_user_callback_t)user_data;

	NVIC_EnableIRQ(USART2_IRQn);

	LL_USART_EnableIT_RXNE(USART2);

	LL_USART_Enable(USART2);

	return 0;
}

void BSP_GPS_IRQHandler(void)
{
	if(LL_USART_IsActiveFlag_RXNE(USART2))
	{
		*byte = LL_USART_ReceiveData8(USART2);

		if(callback != NULL)
		{
			callback(byte, 1);
		}
	}
}
