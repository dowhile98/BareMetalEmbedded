/*
 * littlefs_w25q_interface.h
 *
 *  Created on: Oct 27, 2025
 *      Author: tecna-smart-lab
 */

/**
 * @file littlefs_w25q_interface.h
 * @brief LittleFS interface for W25Q SPI Flash
 */

#ifndef LITTLEFS_W25Q_INTERFACE_H
#define LITTLEFS_W25Q_INTERFACE_H

#include "lfs.h"
#include "w25q.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LittleFS W25Q configuration structure
 */
typedef struct {
    w25q_handle_t *flash_handle;   ///< Pointer to W25Q handle
    uint32_t start_address;        ///< Start address in flash for LittleFS
    uint32_t total_size;           ///< Total size allocated for LittleFS
    uint32_t block_size;           ///< Block size (should match sector size)
    uint32_t block_count;          ///< Number of blocks
    uint16_t read_size;            ///< Read size (typically 256 bytes)
    uint16_t prog_size;            ///< Program size (typically 256 bytes)
    uint16_t cache_size;           ///< Cache size
    uint16_t lookahead_size;       ///< Lookahead size
    int32_t block_cycles;          ///< Block cycles (wear leveling)
} littlefs_w25q_config_t;

/**
 * @brief Initialize LittleFS with W25Q backend
 * @param config Configuration parameters
 * @param lfs LittleFS instance
 * @param cfg LittleFS configuration
 * @return LFS error code
 */
int littlefs_w25q_init(const littlefs_w25q_config_t *config,
                      lfs_t *lfs,
                      struct lfs_config *cfg);

/**
 * @brief Format the LittleFS filesystem on W25Q
 * @param lfs LittleFS instance
 * @param cfg LittleFS configuration
 * @return LFS error code
 */
int littlefs_w25q_format(lfs_t *lfs, const struct lfs_config *cfg);

/**
 * @brief Deinitialize LittleFS
 * @param lfs LittleFS instance
 * @return LFS error code
 */
int littlefs_w25q_deinit(lfs_t *lfs);

// LittleFS block device operations
int littlefs_w25q_read(const struct lfs_config *c, lfs_block_t block,
                      lfs_off_t off, void *buffer, lfs_size_t size);
int littlefs_w25q_prog(const struct lfs_config *c, lfs_block_t block,
                      lfs_off_t off, const void *buffer, lfs_size_t size);
int littlefs_w25q_erase(const struct lfs_config *c, lfs_block_t block);
int littlefs_w25q_sync(const struct lfs_config *c);

#ifdef __cplusplus
}
#endif

#endif // LITTLEFS_W25Q_INTERFACE_H
