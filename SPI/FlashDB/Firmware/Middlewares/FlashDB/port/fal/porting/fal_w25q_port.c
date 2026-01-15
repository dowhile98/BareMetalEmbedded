/*
 * fal_w25q_port.c
 *
 *  Created on: Oct 25, 2025
 *      Author: tecna-smart-lab
 */

#include "fal_cfg.h"
#include "fal.h"



#ifndef FAL_USING_NOR_FLASH_DEV_NAME
#define FAL_USING_NOR_FLASH_DEV_NAME             "norflash0"
#endif

static int init(void);
static int read(long offset, uint8_t *buf, size_t size);
static int write(long offset, const uint8_t *buf, size_t size);
static int erase(long offset, size_t size);

extern w25q_handle_t flash;


struct fal_flash_dev nor_flash0 =
{
		.name       = FAL_USING_NOR_FLASH_DEV_NAME,
		.addr       = 0,
		.len        = 2 * 1024 * 1024,
		.blk_size   = 4096,
		.ops        = {init, read, write, erase},
		.write_gran = 1
};

static int init(void)
{
	w25q_info_t info;

	w25q_get_info(&flash, &info);
	/* update the flash chip information */
	nor_flash0.blk_size = info.sector_size;
	nor_flash0.len = info.capacity_bytes;

	return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    uint32_t physical_addr;

    if (buf == NULL || size == 0) {
        return 0;
    }

    // Verificar límites
    if ((offset + size) > nor_flash0.len) {
        return -1;
    }

    // Calcular dirección física
    physical_addr = nor_flash0.addr + offset;

    if (w25q_read(&flash, physical_addr, buf, size) != W25Q_OK) {
        return -1;
    }

    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    uint32_t physical_addr;

    if (buf == NULL || size == 0) {
        return 0;
    }

    // Verificar límites
    if ((offset + size) > nor_flash0.len) {
        return -1;
    }

    // Calcular dirección física
    physical_addr = nor_flash0.addr + offset;

    if (w25q_write(&flash, physical_addr, buf, size) != W25Q_OK) {
        return -1;
    }

    return size;
}

static int erase(long offset, size_t size)
{
    uint32_t physical_addr;
    size_t sector_count;
    size_t i;

    // FAL debería pasar offset y size alineados, pero verificamos por seguridad
    if ((offset % nor_flash0.blk_size != 0) || (size % nor_flash0.blk_size != 0)) {
        return -1;
    }

    // Calcular dirección física
    physical_addr = nor_flash0.addr + offset;

    // Verificar límites
    if ((offset + size) > nor_flash0.len) {
        return -1;
    }

    // Calcular número de sectores a borrar
    sector_count = size / nor_flash0.blk_size;

    // Borrar cada sector
    for (i = 0; i < sector_count; i++) {
        uint32_t current_addr = physical_addr + (i * nor_flash0.blk_size);

        if (w25q_erase_sector(&flash, current_addr) != W25Q_OK) {
            return -1;
        }
    }

    return size;  // Devolvemos el tamaño original solicitado (ya que se borró todo)
}
