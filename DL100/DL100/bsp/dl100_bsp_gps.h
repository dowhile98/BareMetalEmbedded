/*
 * dl100_bsp_gps.h
 *
 *  Created on: Nov 25, 2025
 *      Author: tecna-smart-lab
 */

#ifndef BSP_DL100_BSP_GPS_H_
#define BSP_DL100_BSP_GPS_H_

#include "dl100_bsp.h"


void BSP_GPS_Init(void);


uint8_t BSP_GPS_Read(uint8_t *buff, size_t len);

uint8_t BSP_GPS_Write(uint8_t *buff, size_t len);

uint8_t BSP_GPS_ReadIT(uint8_t *buff, size_t len, void *user_data);

uint8_t BSP_GPS_Available(void);

void BSP_GPS_IRQHandler(void);

#endif /* BSP_DL100_BSP_GPS_H_ */
