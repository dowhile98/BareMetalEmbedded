#include "bsp.h"
#include "main.h" // The ONLY place allowed to see HAL

/* --- External Handles --- */
extern UART_HandleTypeDef huart2;

/* --- GPIO Accessors --- */

void* BSP_GetGpioHandle(BspGpioId_t id) {
    switch(id) {
        case BSP_GPIO_LED_STATUS:  return LED0_GPIO_Port;
        case BSP_GPIO_GPS_RST:     return GPS_RST_GPIO_Port;
        case BSP_GPIO_SIDE_BUTTON: return SIDE_BUTTON_GPIO_Port;
        default: return NULL;
    }
}

uint16_t BSP_GetGpioPin(BspGpioId_t id) {
    switch(id) {
        case BSP_GPIO_LED_STATUS:  return LED0_Pin;
        case BSP_GPIO_GPS_RST:     return GPS_RST_Pin;
        case BSP_GPIO_SIDE_BUTTON: return SIDE_BUTTON_Pin;
        default: return 0;
    }
}

/* --- UART Accessors --- */

void* BSP_GetUartHandle(BspUartId_t id) {
    switch(id) {
        case BSP_UART_GPS:     return &huart2;
        // case BSP_UART_CONSOLE: return &huart1; // Example
        default: return NULL;
    }
}
