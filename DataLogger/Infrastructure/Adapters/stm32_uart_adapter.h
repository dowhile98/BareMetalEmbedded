#ifndef STM32_UART_ADAPTER_H
#define STM32_UART_ADAPTER_H

#include "i_uart.h"

/**
 * @brief Get the STM32 UART Adapter instance.
 */
const IUart* STM32_UART_GetApi(void);

#endif /* STM32_UART_ADAPTER_H */
