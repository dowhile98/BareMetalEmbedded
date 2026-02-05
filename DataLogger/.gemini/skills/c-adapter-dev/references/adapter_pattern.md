# Patrón de Implementación de Adaptadores (C Clean Architecture)

Este documento detalla cómo implementar adaptadores en la capa `Infrastructure/Adapters` que cumplan con los contratos definidos en `Interfaces/`.

## Estructura de un Adaptador

Cada adaptador debe seguir una estructura de cuatro bloques:

1.  **Inclusiones**: Headers de la Interfaz, la HAL y Tipos Comunes.
2.  **Funciones Privadas**: Implementaciones estáticas que realizan el trabajo pesado con la HAL.
3.  **V-Table (Tabla de Funciones)**: Instancia de la estructura de interfaz.
4.  **Exposición**: Variable global constante para ser inyectada.

## Ejemplo: Adaptador de ADC (Medición de Potencial)

Basado en `Interfaces/i_adc.h`.

### 1. Archivo de Cabecera (`Infrastructure/Adapters/stm32_adc_adapter.h`)

```c
#ifndef STM32_ADC_ADAPTER_H
#define STM32_ADC_ADAPTER_H

#include "i_adc.h"

// Exponemos la interfaz para que el inyector de dependencias pueda usarla
extern const IAdc stm32_adc_adapter;

#endif
```

### 2. Archivo de Fuente (`Infrastructure/Adapters/stm32_adc_adapter.c`)

```c
#include "stm32_adc_adapter.h"
#include "main.h" // Contiene la HAL de STM32U5
#include <math.h>

// --- Bloque 2: Funciones Privadas (Mapeo HAL) ---

static DL_Status_t Stm32Adc_Init(DL_AdcHandle_t handle) {
    if (handle == NULL) return DL_INVALID_PARAM;
    // La inicialización base usualmente la hace CubeMX en main.c
    // Aquí podemos hacer calibraciones específicas.
    HAL_ADCEx_Calibration_Start((ADC_HandleTypeDef*)handle, ADC_SINGLE_ENDED);
    return DL_OK;
}

static DL_Status_t Stm32Adc_StartConversion(DL_AdcHandle_t handle, DL_AdcChannel_t channel) {
    ADC_HandleTypeDef* hadc = (ADC_HandleTypeDef*)handle;
    
    // Mapeo de canales abstractos a canales de hardware
    uint32_t hal_channel;
    switch(channel) {
        case DL_ADC_CHANNEL_PIPE_DC: hal_channel = ADC_CHANNEL_1; break;
        case DL_ADC_CHANNEL_PIPE_AC: hal_channel = ADC_CHANNEL_2; break;
        default: return DL_INVALID_PARAM;
    }
    
    // Configuración de canal dinámica (si no se usa secuenciador)
    // ... lógica HAL_ADC_ConfigChannel ...
    
    return (HAL_ADC_START(hadc) == HAL_OK) ? DL_OK : DL_ERROR;
}

static DL_Status_t Stm32Adc_GetValue(DL_AdcHandle_t handle, DL_AdcChannel_t channel, float* output) {
    ADC_HandleTypeDef* hadc = (ADC_HandleTypeDef*)handle;
    uint32_t raw_value = HAL_ADC_GetValue(hadc);
    
    // Conversión de RAW a Voltaje (Ejemplo 14-bit, 3.3V)
    *output = ((float)raw_value * 3.3f) / 16384.0f;
    
    return DL_OK;
}

// --- Bloque 3: Definición de la V-Table ---

const IAdc stm32_adc_adapter = {
    .init = Stm32Adc_Init,
    .start_conversion = Stm32Adc_StartConversion,
    .get_value = Stm32Adc_GetValue,
    .calibrate = Stm32Adc_Init // Reutilizamos init si incluye calibración
};
```

## Reglas Críticas para Clean Architecture

1.  **Casting de Handles**: El adaptador es el único lugar donde se permite convertir `void* handle` a tipos específicos como `ADC_HandleTypeDef*`.
2.  **Mapeo de Tipos**: Si la interfaz usa un enum `DL_UartEvent_t`, el adaptador debe traducirlo internamente desde/hacia los flags de la HAL.
3.  **Sin Lógica de Negocio**: El adaptador no debe decidir *cuándo* medir, solo *cómo* realizar la medición técnica. La lógica de "si el voltaje es bajo, activar alarma" pertenece al **Domain**.
4.  **Dependencias**: 
    *   `Adapters` -> `Interfaces` (OK)
    *   `Adapters` -> `HAL` (OK)
    *   `Domain` -> `Adapters` (**ERROR**)
    *   `Interfaces` -> `HAL` (**ERROR**)

## Uso en la Aplicación

Para inyectar el adaptador en una tarea de ThreadX:

```c
// Application/Tasks/logger_task.c
#include "i_adc.h"
#include "stm32_adc_adapter.h"

void logger_thread_entry(ULONG input) {
    // Usamos la interfaz abstracta
    const IAdc* adc = &stm32_adc_adapter;
    extern ADC_HandleTypeDef hadc1; // El handle real vive en la infraestructura
    
    adc->init(&hadc1);
    // ...
}
```