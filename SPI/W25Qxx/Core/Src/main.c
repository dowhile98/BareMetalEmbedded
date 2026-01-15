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
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "w25q.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
	ITM_SendChar(ch);

	return ch;
}
// Callbacks de hardware de ejemplo
static int32_t example_spi_init(void *user_ctx)
{
	MX_SPI1_Init();
	// Inicializar hardware SPI aquí
	printf("SPI inicializado\r\n");
	return 0;
}

static int32_t example_spi_write_read(void *user_ctx, const uint8_t *tx, uint8_t *rx, size_t len)
{
	int32_t ret = HAL_OK;
	// Implementar transferencia SPI aquí
	if(tx == NULL)
	{
		ret = HAL_SPI_Receive(&hspi1, rx, len, HAL_MAX_DELAY);
	}
	else if(rx == NULL)
	{
		ret = HAL_SPI_Transmit(&hspi1, tx, len, HAL_MAX_DELAY);
	}
	else
	{
		ret = HAL_SPI_TransmitReceive(&hspi1, tx, rx, len, HAL_MAX_DELAY);
	}


	return ret;
}

static void example_cs_control(void *user_ctx, bool level)
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, level ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void example_delay_ms(uint32_t ms)
{
	HAL_Delay(ms);
}

static void example_lock(void *user_ctx) {}
static void example_unlock(void *user_ctx) {}

static w25q_hw_t example_hw = {
		.spi_init = example_spi_init,
		.spi_write_read = example_spi_write_read,
		.cs_control = example_cs_control,
		.delay_ms = example_delay_ms,
		.lock = example_lock,
		.unlock = example_unlock
};

w25q_handle_t flash;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	w25q_config_t config = {
			.hw = &example_hw,
			.user_ctx = NULL,
			.timeout_ms = 5000,
			.software_cs = true
	};
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	// Inicializar
	w25q_err_t ret = w25q_init(&flash, &config);
	if (ret != W25Q_OK) {
		printf("Error inicializando: %d\n\r", ret);
		return -1;
	}

	// Obtener información
	w25q_info_t info;
	w25q_get_info(&flash, &info);
	printf("Capacidad: %lu bytes\n\r", info.capacity_bytes);

	// Leer JEDEC ID
	uint8_t jedec_id[3];
	w25q_read_jedec_id(&flash, jedec_id);
	printf("JEDEC ID: %02X %02X %02X\n\r", jedec_id[0], jedec_id[1], jedec_id[2]);

	// Datos de prueba
	uint8_t write_data[] = "Hello W25Q Flash!";
	uint8_t read_data[sizeof(write_data)];

	// Borrar sector
	printf("Borrando sector...\n\r");
	w25q_erase_sector(&flash, 0);

	// Escribir datos
	printf("Escribiendo datos...\n\r");
	w25q_write(&flash, 0, write_data, sizeof(write_data));

	// Leer datos
	printf("Leyendo datos...\n\r");
	memset(read_data, 0, sizeof(write_data));
	w25q_read(&flash, 0, read_data, sizeof(write_data));

	// Verificar
	if (memcmp(write_data, read_data, sizeof(write_data)) == 0) {
		printf("Escritura y lectura exitosas!\n\r");
	} else {
		printf("Error en verificación!\n\r");
	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
#ifdef USE_FULL_ASSERT
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
