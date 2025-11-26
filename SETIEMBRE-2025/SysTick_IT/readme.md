

# Demostración del uso de SysTick para Base de Tiempo y Detección de Eventos de Botones

Este proyecto demuestra el uso del temporizador SysTick como base de tiempo en microcontroladores STM32F4 para la lectura periódica de botones, detección de eventos (presionar, mantener presionado, liberar) y la implementación de un mecanismo de "debounce" para evitar lecturas erróneas debido al ruido eléctrico.

## 1. Introducción a SysTick como Base de Tiempo

El temporizador SysTick es un periférico presente en los microcontroladores ARM Cortex-M que proporciona una fuente de tiempo confiable y configurable. En este proyecto, SysTick se configura para generar interrupciones periódicas, creando una base de tiempo que permite ejecutar tareas de manera regular, como la lectura del estado de un botón.

## 2. Lectura Periódica de Botones y Detección de Eventos

La base de tiempo generada por SysTick se utiliza para leer el estado de un botón físico de forma periódica. El código implementa una máquina de estados que detecta los siguientes eventos:

-   **BUTTON\_ON\_PRESS:** El botón ha sido presionado.
-   **BUTTON\_KEEP\_ALIVE:** El botón se mantiene presionado.
-   **BUTTON\_ON\_RELEASE:** El botón ha sido liberado.

Estos eventos se utilizan para controlar el estado de un LED, encendiéndolo al presionar el botón, manteniéndolo encendido mientras se mantiene presionado y apagándolo al liberar el botón.

## 3. Debounce de Botones Físicos

### ¿Qué es el Debounce?

El "debounce" es una técnica utilizada para eliminar lecturas falsas de botones físicos. Cuando se presiona o libera un botón, los contactos metálicos dentro del interruptor rebotan entre sí varias veces antes de establecer una conexión estable. Este rebote genera una serie rápida de pulsos de encendido y apagado que pueden ser interpretados erróneamente por el microcontrolador como múltiples pulsaciones.

### ¿Cómo Funciona el Debounce en este Proyecto?

Para solucionar este problema, el código implementa un mecanismo de debounce que ignora los cambios de estado del botón durante un breve período de tiempo después de la primera detección de un cambio. Esto se logra mediante los siguientes pasos:

1.  **Detección Inicial:** Cuando se detecta un cambio en el estado del botón (por ejemplo, de no presionado a presionado), se inicia un temporizador.
2.  **Período de Debounce:** Durante un período de tiempo específico (en este caso, 20ms), se ignoran todos los cambios en el estado del botón.
3.  **Validación:** Después de transcurrido el período de debounce, se lee el estado del botón nuevamente. Si el estado es el mismo que el detectado inicialmente, se considera una pulsación válida. De lo contrario, se ignora.

Este mecanismo asegura que solo se detecten pulsaciones reales del botón, eliminando las lecturas falsas causadas por el rebote de los contactos.

## 4. Implementación en el Código

El código utiliza la función `button_process()` para implementar la máquina de estados del botón y el mecanismo de debounce. Esta función se llama periódicamente desde la interrupción del SysTick, asegurando una lectura regular del estado del botón.

## 5. Estructura del Proyecto

-   **Src/main.c:** Contiene la función principal (`main()`) que inicializa el hardware, configura el SysTick y ejecuta el bucle principal del programa. También incluye la lógica para procesar los eventos del botón y controlar el LED.
-   **BSP/delay/delay.c y BSP/delay/delay.h:** Implementan las funciones de retardo basadas en el SysTick, incluyendo `delay_ms()`, `ticksGet()` y `tickInc()`.
-   **stm32f4xx.h:** Archivo de encabezado proporcionado por STMicroelectronics que define los registros y las estructuras de datos para el microcontrolador STM32F4.

## 6. Uso

1.  Cargue el código en un microcontrolador STM32F4.
2.  Conecte un botón a la entrada GPIOA pin 0.
3.  Conecte un LED a la salida GPIOD pin 12.
4.  Observe cómo el LED se enciende al presionar el botón, permanece encendido mientras se mantiene presionado y se apaga al liberar el botón.

## 7. Conclusión

Este proyecto proporciona un ejemplo práctico de cómo utilizar el temporizador SysTick como base de tiempo para la lectura periódica de botones y la implementación de un mecanismo de debounce. Estas técnicas son fundamentales para el desarrollo de aplicaciones embebidas que interactúan con el mundo exterior a través de botones y otros dispositivos de entrada.
