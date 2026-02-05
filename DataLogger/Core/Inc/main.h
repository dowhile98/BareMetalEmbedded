/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DB0_Pin GPIO_PIN_2
#define DB0_GPIO_Port GPIOE
#define DB1_Pin GPIO_PIN_3
#define DB1_GPIO_Port GPIOE
#define DB2_Pin GPIO_PIN_4
#define DB2_GPIO_Port GPIOE
#define DB4_Pin GPIO_PIN_5
#define DB4_GPIO_Port GPIOE
#define DB5_Pin GPIO_PIN_6
#define DB5_GPIO_Port GPIOE
#define LED0_Pin GPIO_PIN_2
#define LED0_GPIO_Port GPIOC
#define GPS_RST_Pin GPIO_PIN_4
#define GPS_RST_GPIO_Port GPIOA
#define GPS_PPS_Pin GPIO_PIN_5
#define GPS_PPS_GPIO_Port GPIOA
#define GPS_PPS_EXTI_IRQn EXTI5_IRQn
#define RF_CTRL1_Pin GPIO_PIN_4
#define RF_CTRL1_GPIO_Port GPIOC
#define RF_CTRL2_Pin GPIO_PIN_5
#define RF_CTRL2_GPIO_Port GPIOC
#define FREE_PIN17_Pin GPIO_PIN_7
#define FREE_PIN17_GPIO_Port GPIOE
#define LCD_SCK_Pin GPIO_PIN_13
#define LCD_SCK_GPIO_Port GPIOE
#define LCD_DC_Pin GPIO_PIN_14
#define LCD_DC_GPIO_Port GPIOE
#define LCD_MOSI_Pin GPIO_PIN_15
#define LCD_MOSI_GPIO_Port GPIOE
#define LCD_CS_Pin GPIO_PIN_10
#define LCD_CS_GPIO_Port GPIOB
#define LCD_RESET_Pin GPIO_PIN_12
#define LCD_RESET_GPIO_Port GPIOB
#define SIDE_BUTTON_Pin GPIO_PIN_15
#define SIDE_BUTTON_GPIO_Port GPIOB
#define PBOUT_Pin GPIO_PIN_10
#define PBOUT_GPIO_Port GPIOD
#define INT_Pin GPIO_PIN_11
#define INT_GPIO_Port GPIOD
#define INT_EXTI_IRQn EXTI11_IRQn
#define PSHOLD_Pin GPIO_PIN_12
#define PSHOLD_GPIO_Port GPIOD
#define GPIO_RESET_Pin GPIO_PIN_6
#define GPIO_RESET_GPIO_Port GPIOC
#define GPIO_DATA_READY_Pin GPIO_PIN_7
#define GPIO_DATA_READY_GPIO_Port GPIOC
#define GPIO_DATA_READY_EXTI_IRQn EXTI7_IRQn
#define HANDSHAKE_Pin GPIO_PIN_8
#define HANDSHAKE_GPIO_Port GPIOC
#define HANDSHAKE_EXTI_IRQn EXTI8_IRQn
#define SPI_CS_Pin GPIO_PIN_9
#define SPI_CS_GPIO_Port GPIOC
#define PFAULT2_Pin GPIO_PIN_15
#define PFAULT2_GPIO_Port GPIOA
#define PENABLE2_Pin GPIO_PIN_0
#define PENABLE2_GPIO_Port GPIOD
#define PFAULT1_Pin GPIO_PIN_1
#define PFAULT1_GPIO_Port GPIOD
#define PENABLE1_Pin GPIO_PIN_3
#define PENABLE1_GPIO_Port GPIOD
#define SY_TX_EN_Pin GPIO_PIN_4
#define SY_TX_EN_GPIO_Port GPIOD
#define SY_RX_EN_Pin GPIO_PIN_5
#define SY_RX_EN_GPIO_Port GPIOD
#define SY_TXD_Pin GPIO_PIN_6
#define SY_TXD_GPIO_Port GPIOD
#define SY_RXD_Pin GPIO_PIN_7
#define SY_RXD_GPIO_Port GPIOD
#define SIDE_LED_Pin GPIO_PIN_4
#define SIDE_LED_GPIO_Port GPIOB
#define LCD_LED_Pin GPIO_PIN_0
#define LCD_LED_GPIO_Port GPIOE
#define BUZZER_Pin GPIO_PIN_1
#define BUZZER_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
