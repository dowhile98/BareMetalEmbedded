/**
 * @file w25q.c
 * @brief Implementación del driver para memorias W25Qxxx
 */

#include "w25q.h"
#include <string.h>

// Comandos SPI
#define W25Q_CMD_WRITE_ENABLE          0x06
#define W25Q_CMD_WRITE_DISABLE         0x04
#define W25Q_CMD_READ_STATUS_REG1      0x05
#define W25Q_CMD_READ_STATUS_REG2      0x35
#define W25Q_CMD_WRITE_STATUS_REG      0x01
#define W25Q_CMD_PAGE_PROGRAM          0x02
#define W25Q_CMD_QUAD_PAGE_PROGRAM     0x32
#define W25Q_CMD_BLOCK_ERASE_4KB       0x20
#define W25Q_CMD_BLOCK_ERASE_32KB      0x52
#define W25Q_CMD_BLOCK_ERASE_64KB      0xD8
#define W25Q_CMD_CHIP_ERASE            0xC7
#define W25Q_CMD_ERASE_SUSPEND         0x75
#define W25Q_CMD_ERASE_RESUME          0x7A
#define W25Q_CMD_POWER_DOWN            0xB9
#define W25Q_CMD_HIGH_PERF_MODE        0xA3
#define W25Q_CMD_MODE_BIT_RESET        0xFF
#define W25Q_CMD_READ_DATA             0x03
#define W25Q_CMD_FAST_READ             0x0B
#define W25Q_CMD_RELEASE_POWER_DOWN    0xAB
#define W25Q_CMD_MANUFACTURER_DEVICE_ID 0x90
#define W25Q_CMD_READ_UNIQUE_ID        0x4B
#define W25Q_CMD_JEDEC_ID              0x9F
#define W25Q_CMD_ENABLE_4BYTE_ADDR     0xB7
#define W25Q_CMD_EXIT_4BYTE_ADDR       0xE9

// Bits del registro de estado
#define W25Q_STATUS_BUSY               (1 << 0)
#define W25Q_STATUS_WRITE_ENABLE       (1 << 1)
#define W25Q_STATUS_BLOCK_PROTECT_0    (1 << 2)
#define W25Q_STATUS_BLOCK_PROTECT_1    (1 << 3)
#define W25Q_STATUS_BLOCK_PROTECT_2    (1 << 4)
#define W25Q_STATUS_BLOCK_PROTECT_3    (1 << 5)

// Tamaños estándar
#define W25Q_PAGE_SIZE                 256
#define W25Q_SECTOR_SIZE               4096
#define W25Q_BLOCK_32K_SIZE            32768
#define W25Q_BLOCK_64K_SIZE            65536

// Timeouts por defecto
#define W25Q_DEFAULT_TIMEOUT_MS        5000
#define W25Q_CHIP_ERASE_TIMEOUT_MS     120000  // 2 minutos para borrado completo

// ============================================================================
// Funciones internas
// ============================================================================

/**
 * @brief Construye comando con dirección según el modo
 */
static size_t w25q_build_address_command(uint8_t *cmd, uint8_t command, uint32_t addr, w25q_addr_mode_t addr_mode)
{
	cmd[0] = command;

	if (addr_mode == W25Q_ADDR_4BYTE) {
		cmd[1] = (uint8_t)((addr >> 24) & 0xFF);
		cmd[2] = (uint8_t)((addr >> 16) & 0xFF);
		cmd[3] = (uint8_t)((addr >> 8) & 0xFF);
		cmd[4] = (uint8_t)(addr & 0xFF);
		return 5;
	} else {
		// Modo 3 bytes
		cmd[1] = (uint8_t)((addr >> 16) & 0xFF);
		cmd[2] = (uint8_t)((addr >> 8) & 0xFF);
		cmd[3] = (uint8_t)(addr & 0xFF);
		return 4;
	}
}

/**
 * @brief Ejecuta comando SPI simple
 */
static w25q_err_t w25q_spi_command(w25q_handle_t *handle, uint8_t cmd)
{
	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, true);
	}

	w25q_err_t ret = W25Q_OK;
	if (handle->hw->spi_write_read(handle->user_ctx, &cmd, NULL, 1) != 0) {
		ret = W25Q_ERR_SPI;
	}

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, false);
	}

	return ret;
}

/**
 * @brief Transferencia SPI con datos
 */
static w25q_err_t w25q_spi_transfer(w25q_handle_t *handle,
		const uint8_t *tx_data, uint8_t *rx_data,
		size_t len)
{
	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, true);
	}

	w25q_err_t ret = W25Q_OK;
	if (handle->hw->spi_write_read(handle->user_ctx, tx_data, rx_data, len) != 0) {
		ret = W25Q_ERR_SPI;
	}

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, false);
	}

	return ret;
}

/**
 * @brief Lock para thread safety
 */
static void w25q_lock(w25q_handle_t *handle)
{
	if (handle->hw->lock) {
		handle->hw->lock(handle->user_ctx);
	}
}

/**
 * @brief Unlock para thread safety
 */
static void w25q_unlock(w25q_handle_t *handle)
{
	if (handle->hw->unlock) {
		handle->hw->unlock(handle->user_ctx);
	}
}

/**
 * @brief Detecta capacidad basado en JEDEC ID
 */
static w25q_err_t w25q_detect_capacity(w25q_handle_t *handle, uint8_t manufacturer_id,
		uint8_t memory_type, uint8_t capacity_id)
{
	// Verificar fabricante Winbond
	if (manufacturer_id != 0xEF) {
		return W25Q_ERR_UNSUPPORTED;
	}

	// Verificar que sea de la familia W25Q (Memory Type = 0x40)
	if (memory_type != 0x40) {
		return W25Q_ERR_UNSUPPORTED;
	}

	// Combinar memory_type y capacity_id para comparar
	uint16_t device_id = (memory_type << 8) | capacity_id;

	switch (device_id) {
	case W25Q_CAPACITY_1MBIT:
		handle->info.capacity_bytes = 131072;      // 1Mbit = 128KB
		handle->info.addr_mode = W25Q_ADDR_3BYTE;
		break;
	case W25Q_CAPACITY_2MBIT:
		handle->info.capacity_bytes = 262144;      // 2Mbit = 256KB
		handle->info.addr_mode = W25Q_ADDR_3BYTE;
		break;
	case W25Q_CAPACITY_4MBIT:
		handle->info.capacity_bytes = 524288;      // 4Mbit = 512KB
		handle->info.addr_mode = W25Q_ADDR_3BYTE;
		break;
	case W25Q_CAPACITY_8MBIT:
		handle->info.capacity_bytes = 1048576;     // 8Mbit = 1MB
		handle->info.addr_mode = W25Q_ADDR_3BYTE;
		break;
	case W25Q_CAPACITY_16MBIT:
		handle->info.capacity_bytes = 2097152;     // 16Mbit = 2MB
		handle->info.addr_mode = W25Q_ADDR_3BYTE;
		break;
	case W25Q_CAPACITY_32MBIT:
		handle->info.capacity_bytes = 4194304;     // 32Mbit = 4MB
		handle->info.addr_mode = W25Q_ADDR_3BYTE;
		break;
	case W25Q_CAPACITY_64MBIT:
		handle->info.capacity_bytes = 8388608;     // 64Mbit = 8MB
		handle->info.addr_mode = W25Q_ADDR_3BYTE;
		break;
	case W25Q_CAPACITY_128MBIT:
		handle->info.capacity_bytes = 16777216;    // 128Mbit = 16MB
		handle->info.addr_mode = W25Q_ADDR_3BYTE;
		break;
	case W25Q_CAPACITY_256MBIT:
		handle->info.capacity_bytes = 33554432;    // 256Mbit = 32MB
		handle->info.addr_mode = W25Q_ADDR_4BYTE;
		break;
	case W25Q_CAPACITY_512MBIT:
		handle->info.capacity_bytes = 67108864;    // 512Mbit = 64MB
		handle->info.addr_mode = W25Q_ADDR_4BYTE;
		break;
	default:
		return W25Q_ERR_UNSUPPORTED;
	}

	// Configurar tamaños estándar
	handle->info.page_size = W25Q_PAGE_SIZE;
	handle->info.sector_size = W25Q_SECTOR_SIZE;
	handle->info.num_sectors = handle->info.capacity_bytes / W25Q_SECTOR_SIZE;
	handle->info.manufacturer_id = manufacturer_id;
	handle->info.memory_type = memory_type;
	handle->info.capacity_id = capacity_id;

	return W25Q_OK;
}

/**
 * @brief Verifica parámetros de dirección y longitud
 */
static w25q_err_t w25q_check_address(w25q_handle_t *handle, uint32_t addr, size_t len)
{
	if (!handle->initialized) {
		return W25Q_ERR_PARAM;
	}

	if (addr >= handle->info.capacity_bytes) {
		return W25Q_ERR_ADDR;
	}

	if (len == 0) {
		return W25Q_ERR_SIZE;
	}

	if ((addr + len) > handle->info.capacity_bytes) {
		return W25Q_ERR_SIZE;
	}

	return W25Q_OK;
}

// ============================================================================
// Implementación API pública
// ============================================================================

w25q_err_t w25q_init(w25q_handle_t *handle, const w25q_config_t *config)
{
	if (!handle || !config || !config->hw || !config->hw->spi_write_read || !config->hw->delay_ms) {
		return W25Q_ERR_PARAM;
	}

	// Inicializar estructura
	memset(handle, 0, sizeof(w25q_handle_t));
	handle->hw = config->hw;
	handle->user_ctx = config->user_ctx;
	handle->timeout_ms = config->timeout_ms ? config->timeout_ms : W25Q_DEFAULT_TIMEOUT_MS;
	handle->software_cs = config->software_cs;

	// Inicializar SPI si está disponible
	if (handle->hw->spi_init) {
		if (handle->hw->spi_init(handle->user_ctx) != 0) {
			return W25Q_ERR_SPI;
		}
	}

	w25q_lock(handle);

	// Liberar de power down si está
	w25q_release_power_down(handle);
	// Leer JEDEC ID para detectar dispositivo
	uint8_t jedec_id[3];
	w25q_err_t ret = w25q_read_jedec_id(handle, jedec_id);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	// Detectar capacidad
	ret = w25q_detect_capacity(handle, jedec_id[0], jedec_id[1], jedec_id[2]);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	// Si el dispositivo requiere direccionamiento de 4 bytes, habilitarlo
	if (handle->info.addr_mode == W25Q_ADDR_4BYTE) {
		ret = w25q_spi_command(handle, W25Q_CMD_ENABLE_4BYTE_ADDR);
		if (ret != W25Q_OK) {
			w25q_unlock(handle);
			return ret;
		}
	}

	// Liberar de power down si está
	w25q_release_power_down(handle);

	handle->initialized = true;
	w25q_unlock(handle);

	return W25Q_OK;
}

w25q_err_t w25q_deinit(w25q_handle_t *handle)
{
	if (!handle || !handle->initialized) {
		return W25Q_ERR_PARAM;
	}

	w25q_lock(handle);

	// Si estaba en modo 4 bytes, salir de él (opcional, pero buena práctica)
	if (handle->info.addr_mode == W25Q_ADDR_4BYTE) {
		w25q_spi_command(handle, W25Q_CMD_EXIT_4BYTE_ADDR);
	}

	// Poner en modo bajo consumo
	w25q_power_down(handle);

	handle->initialized = false;
	w25q_unlock(handle);

	return W25Q_OK;
}

w25q_err_t w25q_get_info(w25q_handle_t *handle, w25q_info_t *info)
{
	if (!handle || !info || !handle->initialized) {
		return W25Q_ERR_PARAM;
	}

	memcpy(info, &handle->info, sizeof(w25q_info_t));
	return W25Q_OK;
}

w25q_err_t w25q_read_jedec_id(w25q_handle_t *handle, uint8_t id[3])
{
	if (!handle || !id) {
		return W25Q_ERR_PARAM;
	}

	uint8_t cmd = W25Q_CMD_JEDEC_ID;
	uint8_t rx_buf[4] = {0};

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, true);
	}

	// Enviar comando y leer 3 bytes de ID
	if (handle->hw->spi_write_read(handle->user_ctx, &cmd, NULL, 1) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		return W25Q_ERR_SPI;
	}

	if (handle->hw->spi_write_read(handle->user_ctx, NULL, rx_buf, 3) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		return W25Q_ERR_SPI;
	}

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, false);
	}

	id[0] = rx_buf[0]; // Manufacturer ID
	id[1] = rx_buf[1]; // Memory Type
	id[2] = rx_buf[2]; // Capacity ID

	return W25Q_OK;
}

w25q_err_t w25q_get_capacity(w25q_handle_t *handle, uint32_t *bytes)
{
	if (!handle || !bytes || !handle->initialized) {
		return W25Q_ERR_PARAM;
	}

	*bytes = handle->info.capacity_bytes;
	return W25Q_OK;
}

w25q_err_t w25q_read_status(w25q_handle_t *handle, uint8_t *status)
{
	if (!handle || !status) {
		return W25Q_ERR_PARAM;
	}

	uint8_t cmd = W25Q_CMD_READ_STATUS_REG1;

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, true);
	}

	// Enviar comando y leer status
	if (handle->hw->spi_write_read(handle->user_ctx, &cmd, NULL, 1) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		return W25Q_ERR_SPI;
	}

	if (handle->hw->spi_write_read(handle->user_ctx, NULL, status, 1) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		return W25Q_ERR_SPI;
	}

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, false);
	}

	return W25Q_OK;
}

w25q_err_t w25q_write_status(w25q_handle_t *handle, uint8_t status)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	w25q_err_t ret = w25q_write_enable(handle);
	if (ret != W25Q_OK) {
		return ret;
	}

	uint8_t tx_buf[2] = {W25Q_CMD_WRITE_STATUS_REG, status};
	ret = w25q_spi_transfer(handle, tx_buf, NULL, 2);

	// Esperar a que termine la escritura
	if (ret == W25Q_OK) {
		ret = w25q_wait_busy(handle, handle->timeout_ms);
	}

	return ret;
}

w25q_err_t w25q_write_enable(w25q_handle_t *handle)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	return w25q_spi_command(handle, W25Q_CMD_WRITE_ENABLE);
}

w25q_err_t w25q_write_disable(w25q_handle_t *handle)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	return w25q_spi_command(handle, W25Q_CMD_WRITE_DISABLE);
}

w25q_err_t w25q_wait_busy(w25q_handle_t *handle, uint32_t timeout_ms)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}


	uint32_t elapsed = 0;

	while (elapsed < timeout_ms) {
		uint8_t status;
		w25q_err_t ret = w25q_read_status(handle, &status);
		if (ret != W25Q_OK) {
			return ret;
		}

		if (!(status & W25Q_STATUS_BUSY)) {
			return W25Q_OK;
		}

		handle->hw->delay_ms(1);
		elapsed++;
	}

	return W25Q_ERR_TIMEOUT;
}

w25q_err_t w25q_read(w25q_handle_t *handle, uint32_t addr, uint8_t *data, size_t len)
{
	if (!handle || !data) {
		return W25Q_ERR_PARAM;
	}

	w25q_err_t ret = w25q_check_address(handle, addr, len);
	if (ret != W25Q_OK) {
		return ret;
	}

	w25q_lock(handle);

	// Esperar si hay operación en curso
	ret = w25q_wait_busy(handle, handle->timeout_ms);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	// Comando de lectura + dirección
	uint8_t cmd[5];
	size_t cmd_len = w25q_build_address_command(cmd, W25Q_CMD_READ_DATA, addr, handle->info.addr_mode);

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, true);
	}

	// Enviar comando y dirección
	if (handle->hw->spi_write_read(handle->user_ctx, cmd, NULL, cmd_len) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		w25q_unlock(handle);
		return W25Q_ERR_SPI;
	}

	// Leer datos
	if (handle->hw->spi_write_read(handle->user_ctx, NULL, data, len) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		w25q_unlock(handle);
		return W25Q_ERR_SPI;
	}

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, false);
	}

	w25q_unlock(handle);
	return W25Q_OK;
}

w25q_err_t w25q_page_program(w25q_handle_t *handle, uint32_t addr, const uint8_t *data, size_t len)
{
	if (!handle || !data) {
		return W25Q_ERR_PARAM;
	}

	if (len > W25Q_PAGE_SIZE) {
		return W25Q_ERR_SIZE;
	}

	// Verificar que no cruce límite de página
	uint32_t page_start = addr & ~(W25Q_PAGE_SIZE - 1);
	uint32_t page_end = page_start + W25Q_PAGE_SIZE - 1;
	if ((addr + len - 1) > page_end) {
		return W25Q_ERR_SIZE;
	}

	w25q_err_t ret = w25q_check_address(handle, addr, len);
	if (ret != W25Q_OK) {
		return ret;
	}

	w25q_lock(handle);

	// Esperar dispositivo listo
	ret = w25q_wait_busy(handle, handle->timeout_ms);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	// Habilitar escritura
	ret = w25q_write_enable(handle);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	// Comando page program + dirección
	uint8_t cmd[5];
	size_t cmd_len = w25q_build_address_command(cmd, W25Q_CMD_PAGE_PROGRAM, addr, handle->info.addr_mode);

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, true);
	}

	// Enviar comando, dirección y datos
	if (handle->hw->spi_write_read(handle->user_ctx, cmd, NULL, cmd_len) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		w25q_unlock(handle);
		return W25Q_ERR_SPI;
	}

	if (handle->hw->spi_write_read(handle->user_ctx, data, NULL, len) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		w25q_unlock(handle);
		return W25Q_ERR_SPI;
	}

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, false);
	}

	// Esperar a que termine la programación
	ret = w25q_wait_busy(handle, handle->timeout_ms);

	w25q_unlock(handle);
	return ret;
}

w25q_err_t w25q_write(w25q_handle_t *handle, uint32_t addr, const uint8_t *data, size_t len)
{
	if (!handle || !data) {
		return W25Q_ERR_PARAM;
	}

	w25q_err_t ret = w25q_check_address(handle, addr, len);
	if (ret != W25Q_OK) {
		return ret;
	}

	w25q_lock(handle);

	size_t bytes_written = 0;

	while (bytes_written < len) {
		uint32_t current_addr = addr + bytes_written;
		size_t remaining = len - bytes_written;

		// Calcular bytes hasta el final de la página actual
		uint32_t page_boundary = (current_addr & ~(W25Q_PAGE_SIZE - 1)) + W25Q_PAGE_SIZE;
		size_t chunk_size = page_boundary - current_addr;

		if (chunk_size > remaining) {
			chunk_size = remaining;
		}

		ret = w25q_page_program(handle, current_addr, data + bytes_written, chunk_size);
		if (ret != W25Q_OK) {
			break;
		}

		bytes_written += chunk_size;
	}

	w25q_unlock(handle);
	return ret;
}

w25q_err_t w25q_erase_sector(w25q_handle_t *handle, uint32_t sector_addr)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	// Verificar alineación a sector
	if ((sector_addr & (W25Q_SECTOR_SIZE - 1)) != 0) {
		return W25Q_ERR_ADDR;
	}

	w25q_err_t ret = w25q_check_address(handle, sector_addr, W25Q_SECTOR_SIZE);
	if (ret != W25Q_OK) {
		return ret;
	}

	w25q_lock(handle);

	// Esperar dispositivo listo
	ret = w25q_wait_busy(handle, handle->timeout_ms);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	// Habilitar escritura
	ret = w25q_write_enable(handle);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	// Comando erase sector + dirección
	uint8_t cmd[5];
	size_t cmd_len = w25q_build_address_command(cmd, W25Q_CMD_BLOCK_ERASE_4KB, sector_addr, handle->info.addr_mode);

	ret = w25q_spi_transfer(handle, cmd, NULL, cmd_len);

	// Esperar a que termine el borrado
	if (ret == W25Q_OK) {
		ret = w25q_wait_busy(handle, handle->timeout_ms);
	}

	w25q_unlock(handle);
	return ret;
}

w25q_err_t w25q_erase_block_32k(w25q_handle_t *handle, uint32_t block_addr)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	// Verificar alineación a bloque 32K
	if ((block_addr & (W25Q_BLOCK_32K_SIZE - 1)) != 0) {
		return W25Q_ERR_ADDR;
	}

	w25q_err_t ret = w25q_check_address(handle, block_addr, W25Q_BLOCK_32K_SIZE);
	if (ret != W25Q_OK) {
		return ret;
	}

	w25q_lock(handle);

	ret = w25q_wait_busy(handle, handle->timeout_ms);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	ret = w25q_write_enable(handle);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	uint8_t cmd[5];
	size_t cmd_len = w25q_build_address_command(cmd, W25Q_CMD_BLOCK_ERASE_32KB, block_addr, handle->info.addr_mode);

	ret = w25q_spi_transfer(handle, cmd, NULL, cmd_len);

	if (ret == W25Q_OK) {
		ret = w25q_wait_busy(handle, handle->timeout_ms);
	}

	w25q_unlock(handle);
	return ret;
}

w25q_err_t w25q_erase_block_64k(w25q_handle_t *handle, uint32_t block_addr)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	// Verificar alineación a bloque 64K
	if ((block_addr & (W25Q_BLOCK_64K_SIZE - 1)) != 0) {
		return W25Q_ERR_ADDR;
	}

	w25q_err_t ret = w25q_check_address(handle, block_addr, W25Q_BLOCK_64K_SIZE);
	if (ret != W25Q_OK) {
		return ret;
	}

	w25q_lock(handle);

	ret = w25q_wait_busy(handle, handle->timeout_ms);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	ret = w25q_write_enable(handle);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	uint8_t cmd[5];
	size_t cmd_len = w25q_build_address_command(cmd, W25Q_CMD_BLOCK_ERASE_64KB, block_addr, handle->info.addr_mode);

	ret = w25q_spi_transfer(handle, cmd, NULL, cmd_len);

	if (ret == W25Q_OK) {
		ret = w25q_wait_busy(handle, handle->timeout_ms);
	}

	w25q_unlock(handle);
	return ret;
}

w25q_err_t w25q_chip_erase(w25q_handle_t *handle)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	w25q_lock(handle);

	w25q_err_t ret = w25q_wait_busy(handle, handle->timeout_ms);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	ret = w25q_write_enable(handle);
	if (ret != W25Q_OK) {
		w25q_unlock(handle);
		return ret;
	}

	ret = w25q_spi_command(handle, W25Q_CMD_CHIP_ERASE);

	if (ret == W25Q_OK) {
		ret = w25q_wait_busy(handle, W25Q_CHIP_ERASE_TIMEOUT_MS);
	}

	w25q_unlock(handle);
	return ret;
}

w25q_err_t w25q_power_down(w25q_handle_t *handle)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	return w25q_spi_command(handle, W25Q_CMD_POWER_DOWN);
}

w25q_err_t w25q_release_power_down(w25q_handle_t *handle)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	return w25q_spi_command(handle, W25Q_CMD_RELEASE_POWER_DOWN);
}

w25q_err_t w25q_set_write_protect(w25q_handle_t *handle, bool enable)
{
	if (!handle) {
		return W25Q_ERR_PARAM;
	}

	uint8_t status;
	w25q_err_t ret = w25q_read_status(handle, &status);
	if (ret != W25Q_OK) {
		return ret;
	}

	// Configurar bits de protección (BP0-BP3)
	if (enable) {
		status |= (W25Q_STATUS_BLOCK_PROTECT_0 | W25Q_STATUS_BLOCK_PROTECT_1 |
				W25Q_STATUS_BLOCK_PROTECT_2 | W25Q_STATUS_BLOCK_PROTECT_3);
	} else {
		status &= ~(W25Q_STATUS_BLOCK_PROTECT_0 | W25Q_STATUS_BLOCK_PROTECT_1 |
				W25Q_STATUS_BLOCK_PROTECT_2 | W25Q_STATUS_BLOCK_PROTECT_3);
	}

	return w25q_write_status(handle, status);
}

w25q_err_t w25q_read_uid(w25q_handle_t *handle, uint8_t uid[8])
{
	if (!handle || !uid) {
		return W25Q_ERR_PARAM;
	}

	uint8_t cmd = W25Q_CMD_READ_UNIQUE_ID;

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, true);
	}

	// Enviar comando y dummy bytes
	uint8_t dummy[4] = {0};
	if (handle->hw->spi_write_read(handle->user_ctx, &cmd, NULL, 1) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		return W25Q_ERR_SPI;
	}

	if (handle->hw->spi_write_read(handle->user_ctx, dummy, NULL, 4) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		return W25Q_ERR_SPI;
	}

	// Leer UID
	if (handle->hw->spi_write_read(handle->user_ctx, NULL, uid, 8) != 0) {
		if (handle->software_cs && handle->hw->cs_control) {
			handle->hw->cs_control(handle->user_ctx, false);
		}
		return W25Q_ERR_SPI;
	}

	if (handle->software_cs && handle->hw->cs_control) {
		handle->hw->cs_control(handle->user_ctx, false);
	}

	return W25Q_OK;
}

w25q_err_t w25q_is_erased(w25q_handle_t *handle, uint32_t addr, size_t len, bool *is_erased)
{
	if (!handle || !is_erased) {
		return W25Q_ERR_PARAM;
	}

	w25q_err_t ret = w25q_check_address(handle, addr, len);
	if (ret != W25Q_OK) {
		return ret;
	}

	// Buffer para verificación (puede optimizarse)
	uint8_t buffer[32];
	*is_erased = true;

	size_t bytes_checked = 0;
	while (bytes_checked < len) {
		size_t chunk_size = (len - bytes_checked) > sizeof(buffer) ? sizeof(buffer) : (len - bytes_checked);

		ret = w25q_read(handle, addr + bytes_checked, buffer, chunk_size);
		if (ret != W25Q_OK) {
			return ret;
		}

		// Verificar si todos los bytes son 0xFF
		for (size_t i = 0; i < chunk_size; i++) {
			if (buffer[i] != 0xFF) {
				*is_erased = false;
				return W25Q_OK;
			}
		}

		bytes_checked += chunk_size;
	}

	return W25Q_OK;
}
