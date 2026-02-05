/**
 * @file i_uart.h
 * @brief Improved Abstract Interface for UART Communication.
 */

#ifndef I_UART_H
#define I_UART_H

#include <stdint.h>
#include <stdbool.h>
#include "error_codes.h"

/**
 * @brief Opaque handle for UART hardware instance (e.g., UART_HandleTypeDef*).
 */
typedef void* DL_UartHandle_t;

/**
 * @brief UART Events for callbacks.
 */
typedef enum {
    DL_UART_EVENT_TX_COMPLETE,
    DL_UART_EVENT_RX_COMPLETE,
    DL_UART_EVENT_ERROR
} DL_UartEvent_t;

/**
 * @brief Callback function prototype.
 */
typedef void (*DL_UartCallback_t)(DL_UartEvent_t event);

/**
 * @brief UART V-Table Interface.
 */
typedef struct {
    /**
     * @brief Initialize the UART Interface.
     * @param handle Pointer to hardware handle.
     * @return DL_OK on success.
     */
    DL_Status_t (*init)(DL_UartHandle_t handle);

    /**
     * @brief Send data via UART.
     * @param handle Pointer to hardware handle.
     * @param data Pointer to data buffer.
     * @param length Number of bytes to send.
     * @return DL_OK on success.
     */
    DL_Status_t (*send)(DL_UartHandle_t handle, const uint8_t* data, uint16_t length);

    /**
     * @brief Receive data via UART.
     * @param handle Pointer to hardware handle.
     * @param buffer Pointer to destination buffer.
     * @param length Number of bytes to receive.
     * @return DL_OK on success.
     */
    DL_Status_t (*receive)(DL_UartHandle_t handle, uint8_t* buffer, uint16_t length);

    /**
     * @brief Register a callback for UART events.
     * @param handle Pointer to hardware handle.
     * @param event Event type.
     * @param callback Function pointer.
     * @return DL_OK on success.
     */
    DL_Status_t (*register_callback)(DL_UartHandle_t handle, DL_UartEvent_t event, DL_UartCallback_t callback);

} IUart;

#endif /* I_UART_H */