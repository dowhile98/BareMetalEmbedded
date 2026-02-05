/**
 * @file elog.c
 * @brief Implementation of Embedded Logging System (Direct Write / Zero Buffer)
 */

#include "elog.h"

/* --- Internal State --- */
static elog_config_t s_config = {0};
static bool s_is_initialized = false;

/* --- Public Functions --- */

int elog_init(const elog_config_t *config) {
	if (config == NULL) {
		return -1;
	}
	s_config = *config;
	s_is_initialized = true;
	return 0;
}

// Exposed helper for the Macros in .h
uint32_t elog_port_get_tick(void) {
	if (s_is_initialized && s_config.get_tick_cb) {
		return s_config.get_tick_cb();
	}
	return 0;
}

// Now this function is extremely lightweight.
// It assumes the 'format' string already contains all prefixes/suffixes/colors
// because they were built by the preprocessor macros.
void elog_write(elog_level_t level, const char *format, ...) {
	if (!s_is_initialized) return;

	// Optional: Runtime level filtering could happen here if global variable was used
	// if (level > s_runtime_level) return;

	va_list args;
	va_start(args, format);

	// CRITICAL SECTION START
	if (s_config.lock_cb)
	{
		s_config.lock_cb();
	}
	// Direct write to the output stream.
	// This relies on ELOG_VPRINTF (vprintf by default) being hooked to UART/ITM.
	ELOG_VPRINTF(format, args);

	if (s_config.unlock_cb)
	{
		s_config.unlock_cb();
	}
	// CRITICAL SECTION END

	va_end(args);
}

void elog_hexdump(const char *tag, const void *data, uint16_t len) {
	const uint8_t *ptr = (const uint8_t *)data;

	// Header
	elog_write(ELOG_LEVEL_DEBUG, ELOG_FORMAT(ELOG_COLOR_CYAN, D, "Dump %d bytes:"), elog_port_get_tick(), tag, len);

	// For hexdump, we need a small buffer because we are constructing the string manually
	// independent of the preprocessor macros.
	// Or we can print byte by byte if we really want to avoid stack usage,
	// but a small line buffer is usually acceptable here.

	if (s_config.lock_cb) s_config.lock_cb();

	for (int i = 0; i < len; i++) {
		// Simple direct printing using the configured printf
		if (i % 16 == 0) {
			ELOG_PRINTF("\r\n  %04X: ", i);
		}
		ELOG_PRINTF("%02X ", ptr[i]);
	}
	ELOG_PRINTF("\r\n");

	if (s_config.unlock_cb) s_config.unlock_cb();
}
