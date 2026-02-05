# DataLogger STM32U5 - Protección Catódica (Cathodic Protection)

## 1. Visión General del Proyecto

Este proyecto consiste en el desarrollo de un **Data Logger de precisión para sistemas de Protección Catódica**, basado en el microcontrolador **STM32U575VG** y ejecutándose sobre el sistema operativo en tiempo real **Azure RTOS (ThreadX)**.

### Contexto Funcional (Referencia: iBTVM)
El dispositivo está diseñado para inspeccionar tuberías y estructuras metálicas enterradas, funcionalmente similar al *iBTVM (Item #12400)*. Su propósito principal es:
1.  **Medición de Potenciales:** Captura de voltaje DC y AC (RMS) para verificar la efectividad de la protección catódica.
2.  **Geolocalización:** Sincronización de cada medición con coordenadas GPS precisas y Timestamp UTC.
3.  **Persistencia:** Almacenamiento seguro de logs y mediciones para análisis posterior.

---

## 2. Arquitectura de Software

El proyecto sigue estrictamente los principios de **Clean Architecture** adaptados a C embebido (Object-Oriented Programming in C). El objetivo es desacoplar la lógica de negocio (Domain) del hardware (Infrastructure/HAL).

### Diagrama de Dependencias (Dependency Rule)

El flujo de dependencia apunta **hacia adentro**. El Dominio no conoce la Infraestructura.

```mermaid
graph TD
    subgraph "Hardware & OS Layer"
        HAL[STM32 HAL / ThreadX]
    end

    subgraph "Infrastructure Layer"
        GPS_Impl[GPS Driver (NMEA)]
        ADC_Impl[ADC Mock / Real]
        EEPROM_Impl[EEPROM Driver]
    end

    subgraph "Interface Layer (Adapters)"
        IGPS[IGps Interface]
        IMeasure[IMeasurement Interface]
        IStore[IStorage Interface]
    end

    subgraph "Application Layer"
        LoggerTask[Logger Task]
        GPSTask[GPS Task]
    end

    subgraph "Domain Layer"
        Entity1[Medición Entity]
        Entity2[Ubicación Entity]
    end

    %% Dependencies
    Infrastructure Layer --> Interface Layer
    Infrastructure Layer --> HAL
    Application Layer --> Interface Layer
    Application Layer --> Domain Layer
    Interface Layer -.-> Domain Layer
```

### Estrategia de Interfaces (V-Tables en C)
Para lograr polimorfismo y desacoplamiento sin C++, utilizamos `structs` con punteros a función. Esto permite inyectar dependencias (Mocks o Drivers reales) en tiempo de compilación o inicialización.

**Ejemplo de Contrato (Interface):**
```c
// Interfaces/imeasurement.h
typedef struct {
    // Puntero a función para obtener voltaje
    float (*get_voltage_dc)(void);
    float (*get_voltage_ac)(void);
    // Configuración
    void (*set_sensitivity)(uint8_t gain);
} IMeasurement_VTable;

typedef struct {
    const IMeasurement_VTable* vptr;
} IMeasurement;
```

---

## 3. Estructura del Proyecto

El árbol de directorios está organizado por capas arquitectónicas:

*   **`Application/`**: Orquestación.
    *   *Tasks*: Tareas de ThreadX (e.g., `gps_thread`, `logger_thread`).
    *   *UseCases*: Lógica de aplicación que coordina dominio e infraestructura.
*   **`Domain/`**: El núcleo agnóstico. **Prohibido incluir HAL aquí.**
    *   *Entities*: Objetos de negocio (`Reading`, `GeoPoint`).
    *   *Services*: Algoritmos puros (e.g., validación de rangos).
*   **`Interfaces/`**: Contratos abstractos (`.h` puros).
*   **`Infrastructure/`**: Implementaciones concretas.
    *   *Drivers*: Código que llama a la HAL de STM32.
    *   *Adapters*: Adaptadores que cumplen con las *Interfaces*.
*   **`Common/`**: Tipos compartidos (`types.h`), utilidades, logging.
*   **`BSP/`**: Board Support Package (Mapeo de pines, inicialización de relojes).
*   **`Core/` & `Drivers/`**: Código generado por STM32CubeMX (Capa más baja).

---

## 4. Especificaciones de Módulos

### 4.1. Módulo GPS (Infrastructure)
*   **Hardware:** USART2 con DMA e interrupción por *IDLE Line Detection* para eficiencia de energía y CPU.
*   **Librería:** `lwgps` para el parsing NMEA.
*   **Comportamiento:**
    *   La tarea de GPS espera semáforo de DMA.
    *   Decodifica NMEA.
    *   Expone datos solo cuando hay `FIX` válido.
    *   Sincroniza el RTC del sistema con el tiempo atómico del GPS.

### 4.2. Módulo de Medición (Domain/Infrastructure)
*   **Diseño:** Patrón *Strategy* para filtros digitales.
*   **Estado Actual:** Implementación `Mock` (simulada) para desarrollo de lógica de logueo.
*   **Futuro:** Implementación `Real` sobre ADC del STM32U5 (14/12-bit).
*   **Capacidades:**
    *   Lectura DC (Potencial de tubería).
    *   Lectura AC True RMS (Interferencia).
    *   Configuración de ganancia dinámica.

### 4.3. Módulo de Almacenamiento (Infrastructure)
*   **Hardware:** EEPROM M24M01E conectada vía I2C.
*   **Organización:** Sistema de archivos circular o secuencial simple (Raw logs).
*   **Estructura de Log:**
    ```c
    struct LogEntry {
        uint32_t timestamp; // Unix Time
        float latitude;
        float longitude;
        float voltage_dc;
        float voltage_ac;
        uint16_t flags;     // Status del fix, batería, etc.
    };
    ```

---

## 5. Stack Tecnológico

| Componente | Tecnología |
| :--- | :--- |
| **Microcontrolador** | STM32U575VGTx (Cortex-M33) |
| **RTOS** | Azure RTOS (ThreadX) |
| **Lenguaje** | C11 (con extensiones GNU) |
| **Build System** | STM32CubeIDE (GCC / Make) |
| **Code Style** | Clean Architecture, SOLID, MISRA C Guidelines (parcial) |
| **Drivers** | STM32 HAL / LL (Low Layer) |

## 6. Instrucciones de Compilación

1.  Importar proyecto en **STM32CubeIDE**.
2.  Asegurar que el archivo `.ioc` no sobrescriba las carpetas de arquitectura (Application, Domain, etc.).
3.  Compilar (`Ctrl+B`).
4.  Debug (`F11`) usando ST-Link.

---
**Arquitecto:** Tecna Smart Lab AI
**Fecha:** Febrero 2026
