/**
 * @file i_i2c.h
 * @brief Improved Abstract Interface for I2C Communication.
 */

#ifndef I_I2C_H
#define I_I2C_H

#include <stdint.h>
#include "error_codes.h"

/**
 * @brief Opaque handle for I2C hardware instance (e.g., I2C_HandleTypeDef*).
 */
typedef void* DL_I2cHandle_t;

/**
 * @brief I2C V-Table Interface.
 */
typedef struct {
    /**
     * @brief Initialize the I2C Interface.
     * @param handle Pointer to hardware handle.
     * @return DL_OK on success.
     */
    DL_Status_t (*init)(DL_I2cHandle_t handle);

    /**
     * @brief Write to a specific register.
     * @param handle Pointer to hardware handle.
     * @param dev_addr Device Address.
     * @param reg_addr Register Address.
     * @param data Data buffer.
     * @param length Length.
     * @return DL_OK on success.
     */
    DL_Status_t (*write_reg)(DL_I2cHandle_t handle, uint16_t dev_addr, uint16_t reg_addr, const uint8_t* data, uint16_t length);

    /**
     * @brief Read from a specific register.
     * @param handle Pointer to hardware handle.
     * @param dev_addr Device Address.
     * @param reg_addr Register Address.
     * @param data Receive buffer.
     * @param length Length.
     * @return DL_OK on success.
     */
    DL_Status_t (*read_reg)(DL_I2cHandle_t handle, uint16_t dev_addr, uint16_t reg_addr, uint8_t* data, uint16_t length);

    /**
     * @brief Check if a device is ready.
     * @param handle Pointer to hardware handle.
     * @param dev_addr Device Address.
     * @return DL_OK if ready.
     */
    DL_Status_t (*is_ready)(DL_I2cHandle_t handle, uint16_t dev_addr);

} II2c;

#endif /* I_I2C_H */