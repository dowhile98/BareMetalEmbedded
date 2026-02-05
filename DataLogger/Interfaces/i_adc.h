/**
 * @file i_adc.h
 * @brief Improved Abstract Interface for Analog-to-Digital Conversion.
 */

#ifndef I_ADC_H
#define I_ADC_H

#include <stdint.h>
#include "error_codes.h"

/**
 * @brief Opaque handle for ADC hardware instance (e.g., ADC_HandleTypeDef*).
 */
typedef void* DL_AdcHandle_t;

/**
 * @brief ADC Channels (Abstract definition).
 */
typedef enum {
    DL_ADC_CHANNEL_PIPE_DC,
    DL_ADC_CHANNEL_PIPE_AC,
    DL_ADC_CHANNEL_BATTERY,
    DL_ADC_CHANNEL_TEMP
} DL_AdcChannel_t;

/**
 * @brief ADC V-Table Interface.
 */
typedef struct {
    /**
     * @brief Initialize the ADC Interface.
     * @param handle Pointer to hardware handle.
     * @return DL_OK on success.
     */
    DL_Status_t (*init)(DL_AdcHandle_t handle);

    /**
     * @brief Start conversion for a specific channel.
     * @param handle Pointer to hardware handle.
     * @param channel Abstract channel ID.
     * @return DL_OK on success.
     */
    DL_Status_t (*start_conversion)(DL_AdcHandle_t handle, DL_AdcChannel_t channel);

    /**
     * @brief Get the last converted value.
     * @param handle Pointer to hardware handle.
     * @param channel Abstract channel ID.
     * @param output Pointer to float.
     * @return DL_OK on success.
     */
    DL_Status_t (*get_value)(DL_AdcHandle_t handle, DL_AdcChannel_t channel, float* output);

    /**
     * @brief Perform self-calibration.
     * @param handle Pointer to hardware handle.
     * @return DL_OK on success.
     */
    DL_Status_t (*calibrate)(DL_AdcHandle_t handle);

} IAdc;

#endif /* I_ADC_H */