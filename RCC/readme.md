
# Ejemplo: Configuración del Reloj en STM32F407VG (168MHz)

Este proyecto muestra cómo configurar el microcontrolador STM32F407VG para trabajar a 168MHz usando el oscilador externo HSE de 8MHz y el PLL. El código está ampliamente comentado para facilitar el aprendizaje.

## Descripción del Código (`main.c`)

1. **Habilitación del HSE:** Se activa el oscilador externo de 8MHz y se espera a que esté listo.
2. **Configuración del PLL:** Se selecciona el HSE como fuente, se ajustan los factores M, N y P para obtener 168MHz.
3. **Encendido del PLL:** Se habilita el PLL y se espera a que esté listo.
4. **Prescalers de buses:** Se configuran los divisores para APB1 (42MHz) y APB2 (84MHz).
5. **Latencia de Flash:** Se ajusta la latencia de la memoria Flash para soportar la alta frecuencia.
6. **Selección de SYSCLK:** Se selecciona el PLL como fuente principal del sistema.
7. **Actualización de SystemCoreClock:** Se actualiza la variable global con la nueva frecuencia.

## Características relevantes del RCC en STM32F407

- El RCC (Reset and Clock Control) gestiona todas las fuentes de reloj y reseteo del microcontrolador.
- Permite seleccionar y configurar diferentes fuentes de reloj y ajustar los prescalers para los buses y periféricos.

## Fuentes de reloj disponibles

- **HSI (High Speed Internal):** Oscilador interno de 16MHz.
- **HSE (High Speed External):** Oscilador externo, típicamente 8MHz (puede ser cristal u oscilador en modo bypass).
- **LSI (Low Speed Internal):** Oscilador interno de baja velocidad (~32kHz).
- **LSE (Low Speed External):** Cristal externo de baja velocidad (32.768kHz, para RTC).
- **PLL (Phase Locked Loop):** Multiplicador de frecuencia configurable, puede tomar como entrada HSI o HSE.

## Proceso para habilitar cada fuente de reloj

1. **HSI:**
   - Se habilita con `RCC->CR |= RCC_CR_HSION;`
   - Esperar con `while(!(RCC->CR & RCC_CR_HSIRDY));`
2. **HSE:**
   - Se habilita con `RCC->CR |= RCC_CR_HSEON;`
   - Esperar con `while(!(RCC->CR & RCC_CR_HSERDY));`
   - Para modo bypass: `RCC->CR |= RCC_CR_HSEBYP;`
3. **PLL:**
   - Configurar fuente, factores M, N, P, Q en `RCC->PLLCFGR`.
   - Habilitar con `RCC->CR |= RCC_CR_PLLON;`
   - Esperar con `while(!(RCC->CR & RCC_CR_PLLRDY));`

## Configuración del PLL

- **M:** Divide la frecuencia de entrada (2-63). Ejemplo: 8MHz/8 = 1MHz.
- **N:** Multiplica la frecuencia (192-432). Ejemplo: 1MHz x 336 = 336MHz.
- **P:** Divide la salida principal (2, 4, 6, 8). Ejemplo: 336MHz/2 = 168MHz.
- **Q:** Para USB, SDIO, RNG (no usado en este ejemplo).

## Selección de la fuente de SYSCLK

Se selecciona en `RCC->CFGR` usando el campo SW:
- 00: HSI
- 01: HSE
- 10: PLL
Después de seleccionar, verificar el campo SWS para confirmar el cambio.

## Consideraciones sobre los buses APB1 y APB2

- **APB1:** No debe superar 42MHz. Si SYSCLK es mayor, usar divisor adecuado (DIV4 en este ejemplo).
- **APB2:** No debe superar 84MHz. Usar divisor adecuado si SYSCLK es mayor (DIV2 en este ejemplo).
- Configurar correctamente los prescalers para evitar daños o mal funcionamiento de periféricos.

## Latencia de Flash

La memoria Flash necesita tiempo para acceder a datos a altas frecuencias. Se debe configurar la latencia (número de wait states) en el registro `FLASH->ACR` según la frecuencia de SYSCLK y el voltaje de operación:

- Para 168MHz y voltaje de 2.7-3.6V, se requieren 5 wait states (`FLASH_ACR_LATENCY_5WS`).
- Si la latencia es insuficiente, el microcontrolador puede fallar o corromper datos.

**Referencia:** Ver tabla de latencia en el Reference Manual del STM32F407.
