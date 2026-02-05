/**
 * @file error_codes.h
 * @brief Standardized error handling enumeration for the Data Logger project.
 *
 * Defines the return codes used across the system to ensure consistent
 * error reporting between layers (HAL, Infrastructure, Domain, Application).
 */

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <stdint.h>

/**
 * @brief System Error Codes.
 */
typedef enum {
    DL_OK = 0,                  /**< Operation successful */
    DL_ERROR_GENERIC = 1,       /**< Generic/Unknown error */
    DL_ERROR_TIMEOUT = 2,       /**< Operation timed out */
    DL_ERROR_BUSY = 3,          /**< Resource is busy */
    DL_ERROR_NULL_PTR = 4,      /**< Null pointer argument */
    DL_ERROR_INVALID_PARAM = 5, /**< Invalid parameter provided */
    DL_ERROR_BUFFER_FULL = 6,   /**< Buffer/Queue is full */
    DL_ERROR_NOT_READY = 7,     /**< Peripheral/Device not ready */
    DL_ERROR_IO = 8             /**< Hardware I/O Error */
} DL_Status_t;

#endif /* ERROR_CODES_H */
