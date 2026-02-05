/**
 * @file stm32_i2c.c
 * @brief Improved implementation of II2c interface using generic handles.
 */

#include "i_i2c.h"
#include "main.h"

#define I2C_TIMEOUT 100

static DL_Status_t _i2c_init(DL_I2cHandle_t handle) {
    I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)handle;
    if (!hi2c || !hi2c->Instance) return DL_ERROR_NULL_PTR;
    return DL_OK;
}

static DL_Status_t _i2c_write_reg(DL_I2cHandle_t handle, uint16_t dev_addr, uint16_t reg_addr, const uint8_t* data, uint16_t length) {
    I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)handle;
    if (!hi2c) return DL_ERROR_NULL_PTR;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c, dev_addr << 1, reg_addr, I2C_MEMADD_SIZE_16BIT, (uint8_t*)data, length, I2C_TIMEOUT);
    return (status == HAL_OK) ? DL_OK : DL_ERROR_IO;
}

static DL_Status_t _i2c_read_reg(DL_I2cHandle_t handle, uint16_t dev_addr, uint16_t reg_addr, uint8_t* data, uint16_t length) {
    I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)handle;
    if (!hi2c) return DL_ERROR_NULL_PTR;

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c, dev_addr << 1, reg_addr, I2C_MEMADD_SIZE_16BIT, data, length, I2C_TIMEOUT);
    return (status == HAL_OK) ? DL_OK : DL_ERROR_IO;
}

static DL_Status_t _i2c_is_ready(DL_I2cHandle_t handle, uint16_t dev_addr) {
    I2C_HandleTypeDef* hi2c = (I2C_HandleTypeDef*)handle;
    if (!hi2c) return DL_ERROR_NULL_PTR;

    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(hi2c, dev_addr << 1, 3, I2C_TIMEOUT);
    return (status == HAL_OK) ? DL_OK : DL_ERROR_BUSY;
}

/* --- Public Accessor --- */

static const II2c i2c_interface = {
    .init = _i2c_init,
    .write_reg = _i2c_write_reg,
    .read_reg = _i2c_read_reg,
    .is_ready = _i2c_is_ready
};

const II2c* STM32_I2C_GetApi(void) {
    return &i2c_interface;
}