#ifndef STM32_GPIO_ADAPTER_H
#define STM32_GPIO_ADAPTER_H

#include "i_gpio.h"

/**
 * @brief Get the STM32 GPIO Adapter instance.
 */
const IGpio* STM32_GPIO_GetApi(void);

#endif /* STM32_GPIO_ADAPTER_H */
