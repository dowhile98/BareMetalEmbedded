/*
 * littlefs_w25q_interface.c
 *
 *  Created on: Oct 27, 2025
 *      Author: tecna-smart-lab
 */

/**
 * @file littlefs_w25q_interface.c
 * @brief LittleFS implementation for W25Q SPI Flash
 */

#include "littlefs_w25q_interface.h"
#include <string.h>
#include "w25q.h"

// Static context for LittleFS callbacks
static struct {
    w25q_handle_t *flash;
    uint32_t base_address;
    uint32_t block_size;
} littlefs_ctx;

/**
 * @brief Read data from W25Q for LittleFS
 */
int littlefs_w25q_read(const struct lfs_config *c, lfs_block_t block,
                      lfs_off_t off, void *buffer, lfs_size_t size)
{
    // Calculate physical address
    uint32_t address = littlefs_ctx.base_address + (block * littlefs_ctx.block_size) + off;

    // Perform read operation
    w25q_err_t ret = w25q_read(littlefs_ctx.flash, address, (uint8_t*)buffer, size);

    return (ret == W25Q_OK) ? LFS_ERR_OK : LFS_ERR_IO;
}

/**
 * @brief Program data to W25Q for LittleFS
 */
int littlefs_w25q_prog(const struct lfs_config *c, lfs_block_t block,
                      lfs_off_t off, const void *buffer, lfs_size_t size)
{
    // Calculate physical address
    uint32_t address = littlefs_ctx.base_address + (block * littlefs_ctx.block_size) + off;

    // For W25Q, we need to handle page boundaries
    uint32_t remaining = size;
    const uint8_t *data_ptr = (const uint8_t*)buffer;
    uint32_t current_addr = address;

    while (remaining > 0) {
        // Calculate current page boundary
        uint32_t page_boundary = (current_addr & ~(W25Q_PAGE_SIZE - 1)) + W25Q_PAGE_SIZE;
        uint32_t chunk_size = page_boundary - current_addr;

        if (chunk_size > remaining) {
            chunk_size = remaining;
        }

        // Program the chunk
        w25q_err_t ret = w25q_page_program(littlefs_ctx.flash, current_addr, data_ptr, chunk_size);
        if (ret != W25Q_OK) {
            return LFS_ERR_IO;
        }

        // Wait for write completion
        ret = w25q_wait_busy(littlefs_ctx.flash, 1000);
        if (ret != W25Q_OK) {
            return LFS_ERR_IO;
        }

        current_addr += chunk_size;
        data_ptr += chunk_size;
        remaining -= chunk_size;
    }

    return LFS_ERR_OK;
}

/**
 * @brief Erase a block in W25Q for LittleFS
 */
int littlefs_w25q_erase(const struct lfs_config *c, lfs_block_t block)
{
    // Calculate physical address
    uint32_t address = littlefs_ctx.base_address + (block * littlefs_ctx.block_size);

    // Perform erase operation
    w25q_err_t ret = w25q_erase_sector(littlefs_ctx.flash, address);

    if (ret != W25Q_OK) {
        return LFS_ERR_IO;
    }

    // Wait for erase completion
    ret = w25q_wait_busy(littlefs_ctx.flash, 5000); // 5s timeout for erase

    return (ret == W25Q_OK) ? LFS_ERR_OK : LFS_ERR_IO;
}

/**
 * @brief Sync operation for LittleFS (no-op for W25Q)
 */
int littlefs_w25q_sync(const struct lfs_config *c)
{
    // W25Q operations are synchronous, so no need for explicit sync
    return LFS_ERR_OK;
}

/**
 * @brief Initialize LittleFS with W25Q backend
 */
int littlefs_w25q_init(const littlefs_w25q_config_t *config,
                      lfs_t *lfs,
                      struct lfs_config *cfg)
{
    if (!config || !lfs || !cfg || !config->flash_handle) {
        return LFS_ERR_INVAL;
    }

    // Verify W25Q is initialized
    w25q_info_t info;
    if (w25q_get_info(config->flash_handle, &info) != W25Q_OK) {
        return LFS_ERR_IO;
    }

    // Verify configuration matches W25Q capabilities
    if (config->block_size != info.sector_size) {
        return LFS_ERR_INVAL; // Block size must match sector size
    }

    // Store context for callbacks
    littlefs_ctx.flash = config->flash_handle;
    littlefs_ctx.base_address = config->start_address;
    littlefs_ctx.block_size = config->block_size;

    // Configure LittleFS
    cfg->context = NULL; // We use static context

    // Block device operations
    cfg->read  = littlefs_w25q_read;
    cfg->prog  = littlefs_w25q_prog;
    cfg->erase = littlefs_w25q_erase;
    cfg->sync  = littlefs_w25q_sync;

    // Block device configuration
    cfg->read_size = config->read_size;
    cfg->prog_size = config->prog_size;
    cfg->block_size = config->block_size;
    cfg->block_count = config->block_count;
    cfg->cache_size = config->cache_size;
    cfg->lookahead_size = config->lookahead_size;
    cfg->block_cycles = config->block_cycles;

    // Try to mount the filesystem
    int err = lfs_mount(lfs, cfg);

    // If mount fails, format and try again
    if (err != LFS_ERR_OK) {
        err = lfs_format(lfs, cfg);
        if (err == LFS_ERR_OK) {
            err = lfs_mount(lfs, cfg);
        }
    }

    return err;
}

/**
 * @brief Format the LittleFS filesystem
 */
int littlefs_w25q_format(lfs_t *lfs, const struct lfs_config *cfg)
{
    return lfs_format(lfs, cfg);
}

/**
 * @brief Deinitialize LittleFS
 */
int littlefs_w25q_deinit(lfs_t *lfs)
{
    return lfs_unmount(lfs);
}
