# BareMetalEmbedded
 
# Curso de Programación de Microcontroladores STM32

¡Bienvenido al repositorio del curso práctico de programación de microcontroladores STM32! Este repositorio contiene el código fuente y ejemplos desarrollados durante el curso, basados en el libro **"Embedded Systems with STM32: From Registers to Real-World Applications"**. Aquí encontrarás material complementario para seguir las lecciones y practicar con ejercicios hands-on.

## 📚 Descripción

Este repositorio está diseñado para acompañar el curso teórico-práctico sobre el desarrollo de firmware para microcontroladores STM32 (familia Cortex-M4). El contenido abarca desde la configuración de herramientas básicas hasta la implementación de drivers para periféricos avanzados, siguiendo un enfoque **bare-metal** (acceso directo a registros) y utilizando estándares como CMSIS.

### Características principales:
- **Código organizado por capítulos**: Cada carpeta corresponde a un tema del libro.
- **Enfoque práctico**: Ejemplos funcionales para manipulación de GPIO, temporizadores, UART, ADC, SPI, I2C, DMA y más.
- **Herramientas estándar**: Uso de STM32CubeIDE, GNU Arm Toolchain y OpenOCD.
- **Compatibilidad con placa NUCLEO-F411RE**: Todos los ejemplos están probados en esta placa de desarrollo.

## 🗂️ Estructura del Repositorio

El repositorio sigue la estructura del libro, con carpetas numeradas según los capítulos:

1. **Setting Up the Tools**: Configuración de STM32CubeIDE, GNU Toolchain y OpenOCD.
2. **Peripheral Registers**: Manipulación de registros GPIO y relojes.
3. **Build Process & GNU Toolchain**: Proceso de compilación y uso de herramientas CLI.
4. **Linker Script & Startup File**: Configuración de memoria y vector de interrupciones.
5. **Make Build System**: Automatización de builds con Makefiles.
6. **CMSIS**: Implementación de drivers usando el estándar CMSIS-Core.
7. **GPIO Drivers**: Control de LEDs y botones.
8. **SysTick Timer**: Generación de delays y manejo de tiempo.
9. **General-Purpose Timers**: Modos PWM y captura de eventos.
10. **UART Protocol**: Comunicación serial y drivers.
11. **ADC**: Lectura de señales analógicas.
12. **SPI**: Comunicación con periféricos (ej. ADXL345).
13. **I2C**: Protocolo para sensores y memorias.
14. **EXTI**: Manejo de interrupciones externas.
15. **RTC**: Reloj en tiempo real y alarmas.
16. **IWDG**: Watchdog para sistemas críticos.
17. **DMA**: Transferencias de datos sin CPU.
18. **Power Management**: Modos de bajo consumo.

## 🛠️ Prerrequisitos

### Hardware
- Placa de desarrollo **NUCLEO-F411RE**.
- Componentes adicionales: sensores, LEDs, resistores (según ejemplos).

### Software
- **STM32CubeIDE** ([Descarga](https://www.st.com/en/development-tools/stm32cubeide.html)).
- **GNU Arm Embedded Toolchain** ([Instalación](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)).
- **OpenOCD** (Incluido en STM32CubeIDE).
- Terminal serial (ej. PuTTY, Tera Term).

### Conocimientos
- Programación en C.
- Conceptos básicos de electrónica y microcontroladores.

## ⚙️ Instalación

1. Clona el repositorio:
   ```bash
   git clone https://github.com/tu-usuario/stm32-course.git
