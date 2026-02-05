---
name: c-adapter-dev
description: Guía especializada para implementar código C en la capa Infrastructure/Adapters siguiendo Clean Architecture. Úsalo para crear adaptadores de hardware (ADC, GPS, I2C) que cumplan con las interfaces del Dominio de Protección Catódica.
---

# C Adapter Development (Clean Architecture)

## Overview

Este skill facilita la creación de **Adapters** en `Infrastructure/Adapters/`. Su función es actuar como puente entre las **Interfaces** (contratos abstractos en `Interfaces/`) y los **Drivers/HAL** (implementaciones concretas en `Infrastructure/Drivers/` o `Drivers/STM32U5xx_HAL_Driver/`).

En este proyecto (DataLogger de Protección Catódica), el objetivo es que el **Dominio** sea agnóstico del hardware.

## Workflow de Implementación

1.  **Identificar el Contrato**: Revisa el `.h` en `Interfaces/` (ej. `i_measure.h`). Identifica la `struct` que actúa como V-Table.
2.  **Identificar el Driver**: Localiza el driver de bajo nivel en `Infrastructure/Drivers/` o las funciones HAL necesarias.
3.  **Crear el Adaptador**:
    *   **Archivo**: `Infrastructure/Adapters/stm32_<modulo>_adapter.c`
    *   **Responsabilidad**: Mapear tipos de datos del dominio a tipos de la HAL y viceversa.
4.  **Implementar la V-Table**: Define las funciones estáticas y asígnalas a la instancia constante de la interfaz.

## Patrón de Diseño: V-Table en C

Para lograr polimorfismo, seguimos el patrón de punteros a función definido en el proyecto:

```c
// 1. Implementación privada (estática)
static float Stm32Measure_GetVoltageDC(void* handle) {
    ADC_HandleTypeDef* hadc = (ADC_HandleTypeDef*)handle;
    // ... lógica de lectura HAL
    return voltage;
}

// 2. Definición del objeto de interfaz (Constante)
const IMeasurement_VTable stm32_measure_vtable = {
    .get_voltage_dc = Stm32Measure_GetVoltageDC,
    .get_voltage_ac = Stm32Measure_GetVoltageAC
};

// 3. Instancia del adaptador
const IMeasurement stm32_measure_adapter = {
    .vptr = &stm32_measure_vtable
};
```

## Reglas de Oro

*   **Inward Dependency**: Los adaptadores pueden incluir headers de `Interfaces/`, `Domain/` y `HAL`. El **Dominio NUNCA** debe incluir headers del adaptador ni de la HAL.
*   **Opaque Handles**: Usa `void* handle` para pasar instancias de hardware (ej. `&hadc1`) a las funciones del adaptador para mantener el desacoplamiento.
*   **Mapeo de Errores**: Traduce los códigos `HAL_StatusTypeDef` a `DL_Status_t` de la capa común.

## Módulos Críticos

*   **Medición (ADC)**: Lectura de potenciales DC y AC RMS.
*   **Geolocalización (GPS)**: Parsing de sentencias NMEA vía `lwgps`.
*   **Persistencia (EEPROM)**: Almacenamiento circular en I2C.

## Referencias Internas
*   Ver `references/adapter_pattern.md` para ejemplos de implementación de GPIO e I2C.
