/**
 * @file i_gpio.h
 * @brief Improved Abstract Interface for General Purpose I/O.
 *
 * Uses generic handles (void*) to allow passing hardware instances directly,
 * enhancing decoupling and supporting multiple instances.
 */

#ifndef I_GPIO_H
#define I_GPIO_H

#include <stdint.h>
#include "error_codes.h"

/**
 * @brief Opaque handle for GPIO hardware instance (e.g., GPIO_TypeDef*).
 */
typedef void* DL_GpioHandle_t;

/**
 * @brief GPIO Pin definitions (0-15 typical).
 */
typedef uint16_t DL_GpioPin_t;

/**
 * @brief GPIO Pin States.
 */
typedef enum {
    DL_GPIO_RESET = 0,
    DL_GPIO_SET = 1
} DL_GpioState_t;

/**
 * @brief GPIO Modes.
 */
typedef enum {
    DL_GPIO_MODE_INPUT,
    DL_GPIO_MODE_OUTPUT_PP,
    DL_GPIO_MODE_OUTPUT_OD,
    DL_GPIO_MODE_ANALOG,
    DL_GPIO_MODE_IT_RISING,
    DL_GPIO_MODE_IT_FALLING
} DL_GpioMode_t;

/**
 * @brief GPIO V-Table Interface.
 */
typedef struct {
    /**
     * @brief Initialize a GPIO pin.
     * @param handle Pointer to the GPIO port/hardware instance.
     * @param pin Pin bitmask or index.
     * @param mode Operational mode.
     * @return DL_OK on success.
     */
    DL_Status_t (*init)(DL_GpioHandle_t handle, DL_GpioPin_t pin, DL_GpioMode_t mode);

    /**
     * @brief Write state to a GPIO pin.
     * @param handle Pointer to the GPIO port/hardware instance.
     * @param pin Pin bitmask or index.
     * @param state State to write (SET/RESET).
     * @return DL_OK on success.
     */
    DL_Status_t (*write)(DL_GpioHandle_t handle, DL_GpioPin_t pin, DL_GpioState_t state);

    /**
     * @brief Read state from a GPIO pin.
     * @param handle Pointer to the GPIO port/hardware instance.
     * @param pin Pin bitmask or index.
     * @return State of the pin (0/1).
     */
    DL_GpioState_t (*read)(DL_GpioHandle_t handle, DL_GpioPin_t pin);

    /**
     * @brief Toggle the state of a GPIO pin.
     * @param handle Pointer to the GPIO port/hardware instance.
     * @param pin Pin bitmask or index.
     * @return DL_OK on success.
     */
    DL_Status_t (*toggle)(DL_GpioHandle_t handle, DL_GpioPin_t pin);

} IGpio;

#endif /* I_GPIO_H */