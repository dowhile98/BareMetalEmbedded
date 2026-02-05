/**
 * @file stm32_uart_adapter.c
 * @brief Improved implementation of IUart interface using generic handles.
 *        Supports multiple instances via a registry.
 */

#include "stm32_uart_adapter.h"
#include "main.h"

#define MAX_UART_INSTANCES 4

typedef struct {
    UART_HandleTypeDef* handle;
    DL_UartCallback_t tx_cplt_cb;
    DL_UartCallback_t rx_cplt_cb;
    DL_UartCallback_t error_cb;
} UartRegistryEntry_t;

static UartRegistryEntry_t registry[MAX_UART_INSTANCES] = {0};

// --- Helper Functions ---

static UartRegistryEntry_t* find_or_create_entry(UART_HandleTypeDef* handle) {
    // 1. Search if already registered
    for(int i = 0; i < MAX_UART_INSTANCES; i++) {
        if(registry[i].handle == handle) {
            return &registry[i];
        }
    }
    // 2. Find free slot
    for(int i = 0; i < MAX_UART_INSTANCES; i++) {
        if(registry[i].handle == NULL) {
            registry[i].handle = handle;
            return &registry[i];
        }
    }
    return NULL;
}

static UartRegistryEntry_t* find_entry(UART_HandleTypeDef* handle) {
    for(int i = 0; i < MAX_UART_INSTANCES; i++) {
        if(registry[i].handle == handle) {
            return &registry[i];
        }
    }
    return NULL;
}

/* --- Interface Implementations --- */

static DL_Status_t _uart_init(DL_UartHandle_t handle) {
    UART_HandleTypeDef* huart = (UART_HandleTypeDef*)handle;
    if (!huart || !huart->Instance) return DL_ERROR_NULL_PTR;
    
    // Register the instance explicitly during init
    if(find_or_create_entry(huart) == NULL) {
        return DL_ERROR_INTERNAL; // Registry full
    }
    return DL_OK;
}

static DL_Status_t _uart_send(DL_UartHandle_t handle, const uint8_t* data, uint16_t length) {
    UART_HandleTypeDef* huart = (UART_HandleTypeDef*)handle;
    if (!huart || !data) return DL_ERROR_NULL_PTR;

    if (HAL_UART_Transmit_DMA(huart, data, length) != HAL_OK) {
        return DL_ERROR_BUSY;
    }
    return DL_OK;
}

static DL_Status_t _uart_receive(DL_UartHandle_t handle, uint8_t* buffer, uint16_t length) {
    UART_HandleTypeDef* huart = (UART_HandleTypeDef*)handle;
    if (!huart || !buffer) return DL_ERROR_NULL_PTR;

    if (HAL_UART_Receive_DMA(huart, buffer, length) != HAL_OK) {
        return DL_ERROR_BUSY;
    }
    return DL_OK;
}

static DL_Status_t _uart_register_callback(DL_UartHandle_t handle, DL_UartEvent_t event, DL_UartCallback_t callback) {
    UART_HandleTypeDef* huart = (UART_HandleTypeDef*)handle;
    if(!huart) return DL_ERROR_NULL_PTR;

    UartRegistryEntry_t* entry = find_or_create_entry(huart);
    if(!entry) return DL_ERROR_INTERNAL;

    switch (event) {
        case DL_UART_EVENT_TX_COMPLETE: entry->tx_cplt_cb = callback; break;
        case DL_UART_EVENT_RX_COMPLETE: entry->rx_cplt_cb = callback; break;
        case DL_UART_EVENT_ERROR:       entry->error_cb = callback; break;
        default: return DL_ERROR_INVALID_PARAM;
    }
    return DL_OK;
}

/* --- HAL Glue --- */

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    UartRegistryEntry_t* entry = find_entry(huart);
    if (entry && entry->tx_cplt_cb) {
        entry->tx_cplt_cb(DL_UART_EVENT_TX_COMPLETE);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    UartRegistryEntry_t* entry = find_entry(huart);
    if (entry && entry->rx_cplt_cb) {
        entry->rx_cplt_cb(DL_UART_EVENT_RX_COMPLETE);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    UartRegistryEntry_t* entry = find_entry(huart);
    if (entry && entry->error_cb) {
        entry->error_cb(DL_UART_EVENT_ERROR);
    }
}

/* --- Public Accessor --- */

static const IUart uart_interface = {
    .init = _uart_init,
    .send = _uart_send,
    .receive = _uart_receive,
    .register_callback = _uart_register_callback
};

const IUart* STM32_UART_GetApi(void) {
    return &uart_interface;
}
