/**
 * @file stm32_gpio.c
 * @brief Improved implementation of IGpio interface using generic handles.
 */

#include "i_gpio.h"
#include "main.h"

/* --- Interface Implementations --- */

static DL_Status_t _gpio_init(DL_GpioHandle_t handle, DL_GpioPin_t pin, DL_GpioMode_t mode) {
    GPIO_TypeDef* GPIOx = (GPIO_TypeDef*)handle;
    if (!GPIOx) return DL_ERROR_NULL_PTR;

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin; // STM32 uses bitmasks (GPIO_PIN_X)
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    switch(mode) {
        case DL_GPIO_MODE_INPUT: GPIO_InitStruct.Mode = GPIO_MODE_INPUT; break;
        case DL_GPIO_MODE_OUTPUT_PP: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; break;
        case DL_GPIO_MODE_OUTPUT_OD: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; break;
        case DL_GPIO_MODE_ANALOG: GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; break;
        case DL_GPIO_MODE_IT_RISING: GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; break;
        case DL_GPIO_MODE_IT_FALLING: GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; break;
        default: return DL_ERROR_INVALID_PARAM;
    }

    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
    return DL_OK;
}

static DL_Status_t _gpio_write(DL_GpioHandle_t handle, DL_GpioPin_t pin, DL_GpioState_t state) {
    GPIO_TypeDef* GPIOx = (GPIO_TypeDef*)handle;
    if (!GPIOx) return DL_ERROR_NULL_PTR;

    HAL_GPIO_WritePin(GPIOx, pin, (state == DL_GPIO_SET) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return DL_OK;
}

static DL_GpioState_t _gpio_read(DL_GpioHandle_t handle, DL_GpioPin_t pin) {
    GPIO_TypeDef* GPIOx = (GPIO_TypeDef*)handle;
    if (!GPIOx) return DL_GPIO_RESET;

    return (HAL_GPIO_ReadPin(GPIOx, pin) == GPIO_PIN_SET) ? DL_GPIO_SET : DL_GPIO_RESET;
}

static DL_Status_t _gpio_toggle(DL_GpioHandle_t handle, DL_GpioPin_t pin) {
    GPIO_TypeDef* GPIOx = (GPIO_TypeDef*)handle;
    if (!GPIOx) return DL_ERROR_NULL_PTR;

    HAL_GPIO_TogglePin(GPIOx, pin);
    return DL_OK;
}

/* --- Public Accessor --- */

static const IGpio gpio_interface = {
    .init = _gpio_init,
    .write = _gpio_write,
    .read = _gpio_read,
    .toggle = _gpio_toggle
};

const IGpio* STM32_GPIO_GetApi(void) {
    return &gpio_interface;
}