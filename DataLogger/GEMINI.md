# DataLogger - STM32U575 Firmware

## Project Overview

This project is an embedded firmware application for the **STM32U575VGTx** microcontroller, developed using **STM32CubeIDE**. It utilizes **Azure RTOS (ThreadX)** as the real-time operating system and the **STM32 HAL (Hardware Abstraction Layer)** for peripheral control.

## Project Structure

*   **`DataLogger.ioc`**: STM32CubeMX configuration file. Used to generate initialization code for peripherals and middleware. **Modify hardware settings here.**
*   **`Core/`**: Contains the main application entry point and hardware initialization.
    *   `Src/main.c`: Initializes the HAL, System Clock, and Peripherals. Starts the ThreadX kernel.
    *   `Src/stm32u5xx_it.c`: Interrupt Service Routines.
*   **`AZURE_RTOS/`**: Azure RTOS (ThreadX) configuration.
    *   `App/app_azure_rtos.c`: Defines the ThreadX application entry point (`tx_application_define`) and initializes the memory pool.
*   **`Drivers/`**:
    *   `STM32U5xx_HAL_Driver/`: ST's Hardware Abstraction Layer drivers.
    *   `CMSIS/`: Cortex Microcontroller Software Interface Standard.
*   **`Middlewares/`**: Contains the ThreadX kernel source code.

## Building and Running

### Using STM32CubeIDE (Recommended)
1.  Open **STM32CubeIDE**.
2.  Import this directory as an existing project (`File > Import > General > Existing Projects into Workspace`).
3.  Select the project and click **Project > Build Project**.
4.  Run/Debug using the IDE's debug configuration.

### Command Line (If Makefiles are generated)
If the project is configured to generate Makefiles (usually found in `Debug/` or `Release/` folders after an IDE build):
```bash
cd Debug
make all
```

## Development Conventions

### Code Regeneration Safety
**CRITICAL:** This project uses code generation via STM32CubeMX. To prevent your changes from being overwritten:
*   **ALWAYS** write your code inside the provided user code sections:
    ```c
    /* USER CODE BEGIN ... */
    // Your code here
    /* USER CODE END ... */
    ```
*   **NEVER** modify code outside these blocks unless you are certain it won't be regenerated or you accept the risk of losing it.

### RTOS Integration
*   **Initialization:** The `main()` function in `main.c` initializes hardware and then calls `MX_ThreadX_Init()`.
*   **Application Setup:** ThreadX resources (threads, semaphores, queues) are typically initialized in `AZURE_RTOS/App/app_azure_rtos.c` or functions called by it (e.g., `App_ThreadX_Init`).

### Coding Style
*   Follow the existing STM32 HAL naming conventions (e.g., `HAL_GPIO_WritePin`).
*   Ensure thread stacks are sized appropriately in the ThreadX configuration.
