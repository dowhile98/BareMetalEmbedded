#ifndef BSP_H
#define BSP_H

#include <stdint.h>

/**
 * @brief Abstract GPIO Identifiers.
 *        Decouples the Application/Adapters from hardware pin names.
 */
typedef enum {
    BSP_GPIO_LED_STATUS,
    BSP_GPIO_GPS_RST,
    BSP_GPIO_SIDE_BUTTON,
    // Add others as needed from main.h
} BspGpioId_t;

/**
 * @brief Abstract UART Identifiers.
 */
typedef enum {
    BSP_UART_GPS,
    BSP_UART_CONSOLE,
    // Add others as needed
} BspUartId_t;

// --- GPIO Accessors ---

/**
 * @brief Get the standard hardware handle (e.g. GPIO_TypeDef*) for a given ID.
 */
void* BSP_GetGpioHandle(BspGpioId_t id);

/**
 * @brief Get the hardware pin bitmask (e.g. GPIO_PIN_x) for a given ID.
 */
uint16_t BSP_GetGpioPin(BspGpioId_t id);

// --- UART Accessors ---

/**
 * @brief Get the standard hardware handle (e.g. UART_HandleTypeDef*) for a given ID.
 */
void* BSP_GetUartHandle(BspUartId_t id);

#endif /* BSP_H */
