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
#include "littlefs_w25q_interface.h"
#include "lfs.h"
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
static int32_t w25q_spi_init(void *user_ctx)
{
	MX_SPI1_Init();
	// Inicializar hardware SPI aquí
	printf("SPI inicializado\r\n");
	return 0;
}

static int32_t w25q_spi_write_read(void *user_ctx, const uint8_t *tx, uint8_t *rx, size_t len)
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

static void w25q_cs_control(void *user_ctx, bool level)
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, level ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static void w25q_delay_ms(uint32_t ms)
{
	HAL_Delay(ms);
}

static void w25q_lock(void *user_ctx) {}
static void w25q_unlock(void *user_ctx) {}

static w25q_hw_t w25q_hw = {
		.spi_init = w25q_spi_init,
		.spi_write_read = w25q_spi_write_read,
		.cs_control = w25q_cs_control,
		.delay_ms = w25q_delay_ms,
		.lock = w25q_lock,
		.unlock = w25q_unlock
};

w25q_handle_t flash;



// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;
struct lfs_config cfg;
char data[256];
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */
	w25q_config_t config = {
			.hw = &w25q_hw,
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

	// Configure LittleFS for W25Q
	littlefs_w25q_config_t littlefs_cfg = {
			.flash_handle = &flash,
			.start_address = 0,                    // Start from beginning
			.total_size = info.capacity_bytes,              // Use 512KB for LittleFS
			.block_size = info.sector_size,        // 4096 bytes
			.block_count = info.num_sectors,    	// 128 blocks
			.read_size = 256,                      // Optimal for W25Q
			.prog_size = 256,                      // Optimal for W25Q
			.cache_size = 256,                     // Same as prog_size
			.lookahead_size = 16,                  // Default value
			.block_cycles = 500,                   // Wear leveling cycles
	};

	// Initialize LittleFS
	int err = littlefs_w25q_init(&littlefs_cfg, &lfs, &cfg);
	if (err != LFS_ERR_OK) {
		printf("Failed to initialize LittleFS: %d\n", err);
		return -1;
	}

	printf("LittleFS mounted successfully!\n");

	// Now you can use LittleFS operations

	// Example: Create and write to a file
	lfs_file_t file;
	// read current count
	uint32_t boot_count = 0;
	lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
	lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

	// update boot count
	boot_count += 1;
	lfs_file_rewind(&lfs, &file);
	lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

	// remember the storage is not updated until the file is closed successfully
	lfs_file_close(&lfs, &file);

	if(lfs_file_open(&lfs, &file, "marcaA/modeloA/config.json", LFS_O_RDONLY) == LFS_ERR_OK)
	{
		lfs_file_read(&lfs, &file, data, sizeof(data) - 1);
		printf("read data: %s\r\n", data);

		lfs_file_close(&lfs, &file);
	}

	if(lfs_file_open(&lfs, &file, "/www/index.html", LFS_O_RDONLY) == LFS_ERR_OK)
	{
		lfs_file_read(&lfs, &file, data, sizeof(data) - 1);
		printf("read data: %s\r\n", data);

		lfs_file_close(&lfs, &file);
	}

	if(lfs_file_open(&lfs, &file, "/image/Logo tecna peru.jpg", LFS_O_RDONLY) == LFS_ERR_OK)
	{
		printf("image found\r\n");

		lfs_file_close(&lfs, &file);
	}
	// release any resources we were using
	lfs_unmount(&lfs);



	// print the boot count
	printf("boot_count: %ld\n", boot_count);
	// Cleanup
	littlefs_w25q_deinit(&lfs);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(100);
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
