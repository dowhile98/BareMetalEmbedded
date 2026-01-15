/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include "w25q.h"

#define NOR_FLASH_DEV_NAME             "norflash0"

#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG
#define FAL_USING_SFUD_PORT

/* ===================== Flash device Configuration ========================= */
extern struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &nor_flash0,                                                     \
}

/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                                 \
{                                                                                      \
    {FAL_PART_MAGIC_WORD,  "fdb_tsdb1",       "norflash0",           0, 1024*1024, 0}, \
    {FAL_PART_MAGIC_WORD,  "fdb_kvdb1",       "norflash0",   1024*1024, 1024*1024, 0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */
#endif /* _FAL_CFG_H_ */
