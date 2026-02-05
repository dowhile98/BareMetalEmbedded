/**
 * @file elog.h
 * @brief Embedded Logging System (Zero-Buffer / Direct Write Version)
 * @author Architected by Gemini
 * @version 3.0
 */

#ifndef ELOG_H
#define ELOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

/* --- Configuration Macros --- */

#ifdef ELOG_CONFIG_FILE
#include ELOG_CONFIG_FILE
#endif

// Defines the standard printf-like function to use for direct output.
// Users can override this to use 'lwprintf_vprintf', etc.
// Important: This function MUST output to the hardware (UART/USB/ITM).
#ifndef ELOG_VPRINTF
#include <stdio.h>
#define ELOG_VPRINTF vprintf
#endif

#ifndef ELOG_PRINTF
#include <stdio.h>
#define ELOG_PRINTF
#endif

#ifndef ELOG_ENABLE_COLORS
#define ELOG_ENABLE_COLORS      1
#endif

#ifndef ELOG_GLOBAL_LEVEL
#define ELOG_GLOBAL_LEVEL       ELOG_LEVEL_VERBOSE
#endif

#ifndef ELOG_ENABLE_FILE_PATH
#define ELOG_ENABLE_FILE_PATH   0
#endif

/* --- Color Definitions (Compile Time) --- */
#if ELOG_ENABLE_COLORS
#define ELOG_COLOR_RESET   "\033[0m"
#define ELOG_COLOR_RED     "\033[0;31m"
#define ELOG_COLOR_GREEN   "\033[0;32m"
#define ELOG_COLOR_YELLOW  "\033[0;33m"
#define ELOG_COLOR_BLUE    "\033[0;34m"
#define ELOG_COLOR_CYAN    "\033[0;36m"
#else
#define ELOG_COLOR_RESET   ""
#define ELOG_COLOR_RED     ""
#define ELOG_COLOR_GREEN   ""
#define ELOG_COLOR_YELLOW  ""
#define ELOG_COLOR_BLUE    ""
#define ELOG_COLOR_CYAN    ""
#endif

/* --- Log Levels --- */
typedef enum {
    ELOG_LEVEL_NONE    = 0,
    ELOG_LEVEL_ERROR   = 1,
    ELOG_LEVEL_WARN    = 2,
    ELOG_LEVEL_INFO    = 3,
    ELOG_LEVEL_DEBUG   = 4,
    ELOG_LEVEL_VERBOSE = 5
} elog_level_t;

/* --- Type Definitions --- */

// Note: Output callback is removed in this version because ELOG_VPRINTF handles output directly.
typedef uint32_t (*elog_timestamp_fn)(void);
typedef void (*elog_lock_fn)(void);

typedef struct {
    // No output_cb here! We rely on ELOG_VPRINTF (which usually targets stdout/_write)
    elog_timestamp_fn   get_tick_cb;  /*!< Callback to get system time */
    elog_lock_fn        lock_cb;      /*!< Optional: Mutex Lock callback for RTOS */
    elog_lock_fn        unlock_cb;    /*!< Optional: Mutex Unlock callback for RTOS */
} elog_config_t;

/* --- Public API --- */

int elog_init(const elog_config_t *config);

/**
 * @brief Thread-safe wrapper around vprintf
 * @note This function is called by the macros.
 */
void elog_write(elog_level_t level, const char *format, ...);

/**
 * @brief Helper to expose timestamp to the macros
 */
uint32_t elog_port_get_tick(void);


/* --- Macros & Preprocessor Formatting --- */

#ifndef ELOG_LOCAL_LEVEL
#define ELOG_LOCAL_LEVEL  ELOG_GLOBAL_LEVEL
#endif

// Helper to get file name
#if ELOG_ENABLE_FILE_PATH
#define __ELOG_FILE_FMT__ " (%s:%d)"
#define __ELOG_FILE_ARGS__ ,__FILE__, __LINE__
#else
#define __ELOG_FILE_FMT__
#define __ELOG_FILE_ARGS__
#endif

// ----------------------------------------------------------------------------
// FORMAT DEFINITION
// Structure: [COLOR] LEV (Timestamp) [TAG] (File:Line) Message [RESET]\r\n
// Arguments order required by this format: Timestamp, Tag, (File, Line), UserArgs
// ----------------------------------------------------------------------------
#define ELOG_FORMAT(color, char_level, user_fmt) \
    color #char_level " (%lu) [%s]" __ELOG_FILE_FMT__ ": " user_fmt ELOG_COLOR_RESET "\r\n"

// The core macro
#define ELOG_LOG_IMPL(level, fmt_str, ...) do { \
        if (level <= ELOG_LOCAL_LEVEL) { \
            elog_write(level, fmt_str, elog_port_get_tick(), ##__VA_ARGS__); \
        } \
    } while(0)

// Specific Level Macros
// We construct the specific format string here using string concatenation
#define LOGE(tag, format, ...)  ELOG_LOG_IMPL(ELOG_LEVEL_ERROR,   ELOG_FORMAT(ELOG_COLOR_RED,    E, format) __ELOG_FILE_ARGS__, tag, ##__VA_ARGS__)
#define LOGW(tag, format, ...)  ELOG_LOG_IMPL(ELOG_LEVEL_WARN,    ELOG_FORMAT(ELOG_COLOR_YELLOW, W, format) __ELOG_FILE_ARGS__, tag, ##__VA_ARGS__)
#define LOGI(tag, format, ...)  ELOG_LOG_IMPL(ELOG_LEVEL_INFO,    ELOG_FORMAT(ELOG_COLOR_GREEN,  I, format) __ELOG_FILE_ARGS__, tag, ##__VA_ARGS__)
#define LOGD(tag, format, ...)  ELOG_LOG_IMPL(ELOG_LEVEL_DEBUG,   ELOG_FORMAT(ELOG_COLOR_CYAN,   D, format) __ELOG_FILE_ARGS__, tag, ##__VA_ARGS__)
#define LOGV(tag, format, ...)  ELOG_LOG_IMPL(ELOG_LEVEL_VERBOSE, ELOG_FORMAT(ELOG_COLOR_RESET,  V, format) __ELOG_FILE_ARGS__, tag, ##__VA_ARGS__)

// Hexdump helper remains
void elog_hexdump(const char *tag, const void *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // ELOG_H
