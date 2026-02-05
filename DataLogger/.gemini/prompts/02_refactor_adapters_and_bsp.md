# Refactor Adapters & Implement BSP

You are a Senior Embedded Software Engineer. Your task is to refactor the current `Infrastructure` layer to strictly adhere to **Clean Architecture** and **SOLID** principles, specifically fixing violations identified in a recent code review.

## Context
- **Project**: DataLogger (STM32U5 + ThreadX)
- **Architecture**: Hexagonal (Ports & Adapters)
- **Problem**: 
    1. "Drivers" are actually "Adapters" (implementing Interfaces) but are misplaced.
    2. UART Adapter uses global static callbacks (Singleton anti-pattern), preventing multiple instances.
    3. Application layer directly accesses `GPIOA` / `huart1` (DIP violation / Leaky Abstraction).

## Instructions

### Phase 1: File Organization (Cleanup)
1.  **Move & Rename**:
    -   Move `Infrastructure/Drivers/STM32/stm32_gpio.c` -> `Infrastructure/Adapters/stm32_gpio_adapter.c`.
    -   Move `Infrastructure/Drivers/STM32/stm32_uart.c` -> `Infrastructure/Adapters/stm32_uart_adapter.c`.
    -   Create corresponding headers if missing or rename existing ones.
2.  **Update Includes**: Ensure `main.c` and other consumers reference the new locations/headers.

### Phase 2: Refactor UART Adapter (Fix SRP/Singleton)
1.  **Goal**: Support multiple UART instances (e.g., GPS, Logger, Console) without collision.
2.  **Implementation**:
    -   Refactor `stm32_uart_adapter.c`.
    -   Instead of single `static` callbacks, implement a **Registry** (array of structs) that maps a `UART_HandleTypeDef*` to its specific `DL_UartCallback_t`.
    -   In `HAL_UART_TxCpltCallback`, look up the handle in the registry and call the specific callback.
    -   Protect the registry with a simple check (max instances).

### Phase 3: Implement BSP Layer (Fix DIP)
1.  **Goal**: Encapsulate hardware specifics (`GPIOA`, `GPIO_PIN_5`) so `main.c` / Application layer never sees `stm32u5xx_hal.h` types.
2.  **Create Files**: `BSP/bsp.h` and `BSP/bsp.c`.
3.  **Define Constants**:
    -   Create abstract IDs: `BSP_LED_STATUS`, `BSP_GPS_UART`, `BSP_CONSOLE_UART`.
4.  **Implement Accessors**:
    -   `void* BSP_GetGpioHandle(BspGpioId_t id);` -> returns `GPIO_TypeDef*`
    -   `uint16_t BSP_GetGpioPin(BspGpioId_t id);` -> returns `GPIO_PIN_x`
    -   `void* BSP_GetUartHandle(BspUartId_t id);` -> returns `&huartX`
5.  **Refactor Main**:
    -   Update `Core/Src/main.c` to use `BSP_Get...` instead of macros like `LED0_GPIO_Port`.

## Validation Constraints
-   **Strict Pointers**: `Interfaces/*.h` must ONLY use `void*` handles.
-   **No HAL in App**: `Application/` must NOT include `main.h` or `stm32_hal.h`.
-   **Error Handling**: All BSP lookups must handle invalid IDs (return NULL/0).
