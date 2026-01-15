/**
 * @file w25q.h
 * @brief Driver para memorias SPI Flash W25Qxxx (Winbond)
 * @version 1.0
 *
 * Librería portable e independiente de hardware para sistemas embebidos
 */

#ifndef W25Q_H
#define W25Q_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Códigos de error
 */
typedef enum {
    W25Q_OK = 0,               ///< Operación exitosa
    W25Q_ERR_PARAM = -1,       ///< Parámetro inválido
    W25Q_ERR_SPI = -2,         ///< Error de comunicación SPI
    W25Q_ERR_TIMEOUT = -3,     ///< Timeout en operación
    W25Q_ERR_BUSY = -4,        ///< Dispositivo ocupado
    W25Q_ERR_PROTECT = -5,     ///< Protección de escritura activa
    W25Q_ERR_NOT_ERASED = -6,  ///< Sector no borrado antes de escribir
    W25Q_ERR_ADDR = -7,        ///< Dirección inválida
    W25Q_ERR_SIZE = -8,        ///< Tamaño inválido
    W25Q_ERR_UNSUPPORTED = -9  ///< Operación no soportada
} w25q_err_t;

/**
 * @brief Modo de direccionamiento
 */
typedef enum {
    W25Q_ADDR_3BYTE = 0,    ///< Direccionamiento de 3 bytes (≤ 128Mbit)
    W25Q_ADDR_4BYTE = 1     ///< Direccionamiento de 4 bytes (≥ 256Mbit)
} w25q_addr_mode_t;

/**
 * @brief Densidades de memoria soportadas (en bits)
 * Basado en JEDEC ID: Manufacturer(0xEF) + MemoryType(0x40) + CapacityID
 */
typedef enum {
    W25Q_CAPACITY_1MBIT   = 0x4011,  ///< W25Q10 (1Mbit) - Capacity ID: 0x11
    W25Q_CAPACITY_2MBIT   = 0x4012,  ///< W25Q20 (2Mbit) - Capacity ID: 0x12
    W25Q_CAPACITY_4MBIT   = 0x4013,  ///< W25Q40 (4Mbit) - Capacity ID: 0x13
    W25Q_CAPACITY_8MBIT   = 0x4014,  ///< W25Q80 (8Mbit) - Capacity ID: 0x14
    W25Q_CAPACITY_16MBIT  = 0x4015,  ///< W25Q16 (16Mbit) - Capacity ID: 0x15
    W25Q_CAPACITY_32MBIT  = 0x4016,  ///< W25Q32 (32Mbit) - Capacity ID: 0x16
    W25Q_CAPACITY_64MBIT  = 0x4017,  ///< W25Q64 (64Mbit) - Capacity ID: 0x17
    W25Q_CAPACITY_128MBIT = 0x4018,  ///< W25Q128 (128Mbit) - Capacity ID: 0x18
    W25Q_CAPACITY_256MBIT = 0x4019,  ///< W25Q256 (256Mbit) - Capacity ID: 0x19
    W25Q_CAPACITY_512MBIT = 0x4020   ///< W25Q512 (512Mbit) - Capacity ID: 0x20
} w25q_capacity_t;

/**
 * @brief Estructura para callbacks de hardware
 */
typedef struct {
    /**
     * @brief Inicialización del SPI (opcional)
     * @param user_ctx Contexto de usuario
     * @return 0 si éxito, negativo si error
     */
    int32_t (*spi_init)(void *user_ctx);

    /**
     * @brief Transferencia SPI (lectura/escritura)
     * @param user_ctx Contexto de usuario
     * @param tx Buffer de transmisión (puede ser NULL)
     * @param rx Buffer de recepción (puede ser NULL)
     * @param len Longitud de la transferencia
     * @return 0 si éxito, negativo si error
     */
    int32_t (*spi_write_read)(void *user_ctx, const uint8_t *tx, uint8_t *rx, size_t len);

    /**
     * @brief Control de Chip Select (opcional)
     * @param user_ctx Contexto de usuario
     * @param level true=seleccionar, false=deseleccionar
     */
    void (*cs_control)(void *user_ctx, bool level);

    /**
     * @brief Delay en milisegundos
     * @param ms Milisegundos a esperar
     */
    void (*delay_ms)(uint32_t ms);

    /**
     * @brief Lock para thread safety (opcional)
     * @param user_ctx Contexto de usuario
     */
    void (*lock)(void *user_ctx);

    /**
     * @brief Unlock para thread safety (opcional)
     * @param user_ctx Contexto de usuario
     */
    void (*unlock)(void *user_ctx);
} w25q_hw_t;

/**
 * @brief Configuración del dispositivo
 */
typedef struct {
    const w25q_hw_t *hw;       ///< Callbacks de hardware
    void *user_ctx;            ///< Contexto de usuario para callbacks
    uint32_t timeout_ms;       ///< Timeout por defecto (opcional)
    bool software_cs;          ///< true si CS se maneja por software
} w25q_config_t;

/**
 * @brief Información del dispositivo
 */
typedef struct {
    uint32_t capacity_bytes;   ///< Capacidad en bytes
    uint16_t page_size;        ///< Tamaño de página en bytes
    uint16_t sector_size;      ///< Tamaño de sector en bytes
    uint32_t num_sectors;      ///< Número de sectores
    uint8_t manufacturer_id;   ///< ID del fabricante
    uint8_t memory_type;       ///< Tipo de memoria
    uint8_t capacity_id;       ///< ID de capacidad
    w25q_addr_mode_t addr_mode; ///< Modo de direccionamiento
} w25q_info_t;

/**
 * @brief Handle del dispositivo
 */
typedef struct w25q_handle {
    const w25q_hw_t *hw;           ///< Callbacks de hardware
    void *user_ctx;                ///< Contexto de usuario
    w25q_info_t info;              ///< Información del dispositivo
    uint32_t timeout_ms;           ///< Timeout por defecto
    bool software_cs;              ///< Control de CS por software
    bool initialized;              ///< Bandera de inicialización
} w25q_handle_t;

// ============================================================================
// API Pública
// ============================================================================

/**
 * @brief Inicializa el dispositivo
 * @param handle Handle del dispositivo
 * @param config Configuración
 * @return Código de error
 */
w25q_err_t w25q_init(w25q_handle_t *handle, const w25q_config_t *config);

/**
 * @brief Desinicializa el dispositivo
 * @param handle Handle del dispositivo
 * @return Código de error
 */
w25q_err_t w25q_deinit(w25q_handle_t *handle);

/**
 * @brief Obtiene información del dispositivo
 * @param handle Handle del dispositivo
 * @param info Estructura para información
 * @return Código de error
 */
w25q_err_t w25q_get_info(w25q_handle_t *handle, w25q_info_t *info);

/**
 * @brief Lee el JEDEC ID
 * @param handle Handle del dispositivo
 * @param id Buffer para ID (3 bytes)
 * @return Código de error
 */
w25q_err_t w25q_read_jedec_id(w25q_handle_t *handle, uint8_t id[3]);

/**
 * @brief Obtiene la capacidad en bytes
 * @param handle Handle del dispositivo
 * @param bytes Capacidad en bytes
 * @return Código de error
 */
w25q_err_t w25q_get_capacity(w25q_handle_t *handle, uint32_t *bytes);

/**
 * @brief Lee el registro de estado
 * @param handle Handle del dispositivo
 * @param status Valor del registro
 * @return Código de error
 */
w25q_err_t w25q_read_status(w25q_handle_t *handle, uint8_t *status);

/**
 * @brief Escribe el registro de estado
 * @param handle Handle del dispositivo
 * @param status Valor a escribir
 * @return Código de error
 */
w25q_err_t w25q_write_status(w25q_handle_t *handle, uint8_t status);

/**
 * @brief Habilita escritura
 * @param handle Handle del dispositivo
 * @return Código de error
 */
w25q_err_t w25q_write_enable(w25q_handle_t *handle);

/**
 * @brief Deshabilita escritura
 * @param handle Handle del dispositivo
 * @return Código de error
 */
w25q_err_t w25q_write_disable(w25q_handle_t *handle);

/**
 * @brief Espera a que termine operación actual
 * @param handle Handle del dispositivo
 * @param timeout_ms Timeout en milisegundos
 * @return Código de error
 */
w25q_err_t w25q_wait_busy(w25q_handle_t *handle, uint32_t timeout_ms);

/**
 * @brief Lectura de datos
 * @param handle Handle del dispositivo
 * @param addr Dirección de inicio
 * @param data Buffer de datos
 * @param len Longitud a leer
 * @return Código de error
 */
w25q_err_t w25q_read(w25q_handle_t *handle, uint32_t addr, uint8_t *data, size_t len);

/**
 * @brief Programación de página
 * @param handle Handle del dispositivo
 * @param addr Dirección de inicio
 * @param data Datos a escribir
 * @param len Longitud (máximo page_size)
 * @return Código de error
 */
w25q_err_t w25q_page_program(w25q_handle_t *handle, uint32_t addr, const uint8_t *data, size_t len);

/**
 * @brief Escritura de datos (automática)
 * @param handle Handle del dispositivo
 * @param addr Dirección de inicio
 * @param data Datos a escribir
 * @param len Longitud
 * @return Código de error
 */
w25q_err_t w25q_write(w25q_handle_t *handle, uint32_t addr, const uint8_t *data, size_t len);

/**
 * @brief Borrado de sector (4KB)
 * @param handle Handle del dispositivo
 * @param sector_addr Dirección del sector
 * @return Código de error
 */
w25q_err_t w25q_erase_sector(w25q_handle_t *handle, uint32_t sector_addr);

/**
 * @brief Borrado de bloque 32KB
 * @param handle Handle del dispositivo
 * @param block_addr Dirección del bloque
 * @return Código de error
 */
w25q_err_t w25q_erase_block_32k(w25q_handle_t *handle, uint32_t block_addr);

/**
 * @brief Borrado de bloque 64KB
 * @param handle Handle del dispositivo
 * @param block_addr Dirección del bloque
 * @return Código de error
 */
w25q_err_t w25q_erase_block_64k(w25q_handle_t *handle, uint32_t block_addr);

/**
 * @brief Borrado completo del chip
 * @param handle Handle del dispositivo
 * @return Código de error
 */
w25q_err_t w25q_chip_erase(w25q_handle_t *handle);

/**
 * @brief Modo de bajo consumo
 * @param handle Handle del dispositivo
 * @return Código de error
 */
w25q_err_t w25q_power_down(w25q_handle_t *handle);

/**
 * @brief Salida del modo de bajo consumo
 * @param handle Handle del dispositivo
 * @return Código de error
 */
w25q_err_t w25q_release_power_down(w25q_handle_t *handle);

/**
 * @brief Configura protección de escritura
 * @param handle Handle del dispositivo
 * @param enable true=habilitar protección
 * @return Código de error
 */
w25q_err_t w25q_set_write_protect(w25q_handle_t *handle, bool enable);

/**
 * @brief Lee Unique ID (64-bit)
 * @param handle Handle del dispositivo
 * @param uid Buffer para UID (8 bytes)
 * @return Código de error
 */
w25q_err_t w25q_read_uid(w25q_handle_t *handle, uint8_t uid[8]);

/**
 * @brief Verifica si una región está borrada
 * @param handle Handle del dispositivo
 * @param addr Dirección de inicio
 * @param len Longitud a verificar
 * @param is_erased Resultado (true=borrado)
 * @return Código de error
 */
w25q_err_t w25q_is_erased(w25q_handle_t *handle, uint32_t addr, size_t len, bool *is_erased);

#ifdef __cplusplus
}
#endif

#endif // W25Q_H
