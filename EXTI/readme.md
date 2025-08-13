
# Ejemplo y Documentación de GPIO en STM32F4

Este proyecto muestra cómo configurar y manipular los pines GPIO en microcontroladores STM32F4, incluyendo el uso avanzado de bit-banding para acceso eficiente a bits individuales.

## 1. Características de los pines GPIO en STM32

- Cada pin puede funcionar como entrada, salida, función alternativa o entrada analógica.
- Soportan diferentes configuraciones eléctricas: push-pull, open-drain, pull-up, pull-down o sin resistencias.
- Los puertos GPIO están agrupados (GPIOA, GPIOB, ..., GPIOI) y cada uno tiene hasta 16 pines.
- Permiten interrupciones externas y remapeo de funciones periféricas.

## 2. Descripción de los registros GPIO

- **MODER:** Selecciona el modo de cada pin (entrada, salida, alterno, analógico).
- **OTYPER:** Configura el tipo de salida (push-pull/open-drain).
- **OSPEEDR:** Selecciona la velocidad de salida.
- **PUPDR:** Configura resistencias pull-up/pull-down.
- **IDR:** Registro de entrada, lee el estado lógico de los pines.
- **ODR:** Registro de salida, permite escribir el estado lógico de los pines.
- **BSRR:** Permite setear o resetear pines de forma atómica (ideal para evitar glitches).
- **LCKR:** Permite bloquear la configuración de los pines.
- **AFR[2]:** Selecciona la función alternativa de cada pin.

## 3. Modos de configuración con el registro MODER

Cada pin tiene 2 bits en el registro MODER:

- 00: Entrada digital
- 01: Salida digital
- 10: Función alternativa (periféricos)
- 11: Analógico

Ejemplo para configurar PD12 como salida:
```c
GPIOD->MODER &=~ GPIO_MODER_MODE12; // Limpia los bits
GPIOD->MODER |= GPIO_MODER_MODE12_0; // Configura como salida
```

## 4. ¿Qué es bit-banding?

Bit-banding es una característica de los microcontroladores ARM Cortex-M3, M4 y M7 que permite acceder y modificar bits individuales de memoria y periféricos como si fueran variables independientes. Esto se logra mapeando cada bit a una dirección específica en una región especial de memoria.

**Ventajas:**
- Permite operaciones atómicas sobre bits (sin riesgo de condiciones de carrera).
- Facilita la manipulación eficiente de pines y banderas.

**No disponible en Cortex-M0/M0+/M23.**

## 5. Uso de los registros ODR, IDR, BSRR y beneficios del bit-banding

- **ODR (Output Data Register):** Permite escribir el estado de salida de los pines. Ejemplo: `GPIOD->ODR |= GPIO_ODR_OD12;`
- **IDR (Input Data Register):** Permite leer el estado de los pines. Ejemplo: `if(GPIOA->IDR & GPIO_IDR_ID0)`
- **BSRR (Bit Set/Reset Register):** Permite poner en alto o bajo un pin de forma atómica. Ejemplo: `GPIOD->BSRR = GPIO_BSRR_BS12;` (set) o `GPIOD->BSRR = GPIO_BSRR_BR12;` (reset)

**Bit-banding** permite manipular directamente un bit específico de un registro, por ejemplo:
```c
#define LED1 BITBAND_ACCESS(GPIOD->ODR, 12)
LED1 = 1; // Enciende el LED
LED1 = 0; // Apaga el LED
```
Esto es útil para evitar operaciones de lectura-modificación-escritura y garantiza acceso seguro y eficiente.

## 6. Consejos adicionales

- Utiliza BSRR para cambiar el estado de pines en aplicaciones críticas o multitarea.
- Configura correctamente las resistencias pull-up/pull-down según el circuito externo.
- Recuerda que la velocidad de los pines (OSPEEDR) puede afectar el consumo y la EMI.
- Consulta el Reference Manual y el datasheet para detalles eléctricos y de configuración.

---
**Referencia:** RM0090 Reference Manual STM32F407/417, sección GPIO y bit-banding.
