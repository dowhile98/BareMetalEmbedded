/**
 * @file stm32_adc.c
 * @brief Improved implementation of IAdc interface using generic handles.
 */

#include "i_adc.h"
#include "main.h"

static uint32_t _map_channel(DL_AdcChannel_t channel) {
    switch(channel) {
        case DL_ADC_CHANNEL_PIPE_DC: return ADC_CHANNEL_1;
        case DL_ADC_CHANNEL_PIPE_AC: return ADC_CHANNEL_2;
        case DL_ADC_CHANNEL_BATTERY: return ADC_CHANNEL_3;
        case DL_ADC_CHANNEL_TEMP:    return ADC_CHANNEL_TEMPSENSOR;
        default: return ADC_CHANNEL_0;
    }
}

static DL_Status_t _adc_init(DL_AdcHandle_t handle) {
    ADC_HandleTypeDef* hadc = (ADC_HandleTypeDef*)handle;
    if (!hadc || !hadc->Instance) return DL_ERROR_NULL_PTR;
    
    if (HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK) {
        return DL_ERROR_IO;
    }
    return DL_OK;
}

static DL_Status_t _adc_start_conversion(DL_AdcHandle_t handle, DL_AdcChannel_t channel) {
    ADC_HandleTypeDef* hadc = (ADC_HandleTypeDef*)handle;
    if (!hadc) return DL_ERROR_NULL_PTR;

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = _map_channel(channel);
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;

    HAL_ADC_ConfigChannel(hadc, &sConfig);
    if (HAL_ADC_Start(hadc) != HAL_OK) return DL_ERROR_IO;
    
    return DL_OK;
}

static DL_Status_t _adc_get_value(DL_AdcHandle_t handle, DL_AdcChannel_t channel, float* output) {
    ADC_HandleTypeDef* hadc = (ADC_HandleTypeDef*)handle;
    if (!hadc || !output) return DL_ERROR_NULL_PTR;

    if (HAL_ADC_PollForConversion(hadc, 10) != HAL_OK) return DL_ERROR_TIMEOUT;

    uint32_t raw = HAL_ADC_GetValue(hadc);
    *output = (raw * 3.3f) / 16383.0f; // 14-bit U5 assumption

    return DL_OK;
}

static DL_Status_t _adc_calibrate(DL_AdcHandle_t handle) {
    ADC_HandleTypeDef* hadc = (ADC_HandleTypeDef*)handle;
    if (!hadc) return DL_ERROR_NULL_PTR;
    return (HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) == HAL_OK) ? DL_OK : DL_ERROR_IO;
}

/* --- Public Accessor --- */

static const IAdc adc_interface = {
    .init = _adc_init,
    .start_conversion = _adc_start_conversion,
    .get_value = _adc_get_value,
    .calibrate = _adc_calibrate
};

const IAdc* STM32_ADC_GetApi(void) {
    return &adc_interface;
}