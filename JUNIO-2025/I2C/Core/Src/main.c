/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
} MPU6050_Data_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SLAVE			0x68<<1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
MPU6050_Data_t mpu;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * escribir datos
 *
 */
uint8_t i2c_reg_write(uint8_t regAddr, uint8_t *buff, uint16_t size){
	uint8_t ret = 0;
	/*enviar la direccion de la region*/
	ret = HAL_I2C_Master_Transmit(&hi2c1, SLAVE, &regAddr, 1, 1000);
	/*enviar datos*/
	ret |= HAL_I2C_Master_Transmit(&hi2c1, SLAVE, buff, size, HAL_MAX_DELAY);


	return ret;
}

uint8_t i2c_reg_read(uint8_t regAddr, uint8_t *buff, uint16_t size){
	uint8_t ret = 0;
	/*enviar la direccion de la region*/
	ret = HAL_I2C_Master_Transmit(&hi2c1, SLAVE, &regAddr, 1, 1000);
	/*recibir datos*/
	ret |= HAL_I2C_Master_Receive(&hi2c1, SLAVE, buff, size, HAL_MAX_DELAY);

	return ret;
}


void MPU6050_Init(void) {
	uint8_t data;
    // 1. Resetear el dispositivo (opcional pero recomendable)
    data = 0x80;
    i2c_reg_write(0x6B, &data, 1);  // PWR_MGMT_1: reset
    HAL_Delay(100); // Esperar a que reinicie

    // 2. Despertar el sensor (salir del modo de reposo)
    data = 0x00;
    i2c_reg_write(0x6B, &data, 1);  // PWR_MGMT_1: clear sleep

    // 3. Configurar el divisor de muestreo (opcional)
    data = 0x07; // 1kHz / (1 + 7) = 125Hz
    i2c_reg_write(0x19, &data, 1);  // SMPLRT_DIV

    // 4. Configurar el filtro DLPF
    data = 0x06; // 5Hz bandwidth
    i2c_reg_write(0x1A, &data, 1);  // CONFIG

    // 5. Configurar el rango del giroscopio
    data = 0x00; // ±250 °/s
    i2c_reg_write(0x1B, &data, 1);  // GYRO_CONFIG

    // 6. Configurar el rango del acelerómetro
    data = 0x00; // ±2g
    i2c_reg_write(0x1C, &data, 1);  // ACCEL_CONFIG
}

void MPU6050_Read_All(MPU6050_Data_t *data) {
    uint8_t rawData[14];  // 14 bytes: accel(6) + temp(2) + gyro(6)

    i2c_reg_read(0x3B, rawData, 14);  // ACCEL_XOUT_H es 0x3B

    data->ax = (int16_t)(rawData[0] << 8 | rawData[1]);
    data->ay = (int16_t)(rawData[2] << 8 | rawData[3]);
    data->az = (int16_t)(rawData[4] << 8 | rawData[5]);
    // Saltar TEMP (bytes 6 y 7)
    data->gx = (int16_t)(rawData[8] << 8 | rawData[9]);
    data->gy = (int16_t)(rawData[10] << 8 | rawData[11]);
    data->gz = (int16_t)(rawData[12] << 8 | rawData[13]);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	uint8_t reg = 0;
	uint8_t data[16];


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  if(i2c_reg_read(0x75, data, 1) != HAL_OK){
	  Error_Handler();
  }

  MPU6050_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_Delay(400);
//	  MPU6050_Read_All(&mpu);
	  i2c_reg_read(0x41, data, 2);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
