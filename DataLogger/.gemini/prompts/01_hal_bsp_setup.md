# HAL & BSP Implementation - Clean Architecture Strategy

**Role:** Embedded Software Architect & Senior C Developer.
**Context:** Data Logger Project (STM32U575).
**Objective:** Implement the Hardware Abstraction Layer (HAL) and Board Support Package (BSP) adhering strictly to Clean Architecture and SOLID principles.

## Guidelines
1.  **Dependency Rule:** Code in `Interfaces` must NOT depend on `Infrastructure` or STM32 HAL libraries. Use `stdint.h` types only.
2.  **Polymorphism:** Use C-style V-Tables (`struct` of function pointers) to define contracts.
3.  **Encapsulation:** Concrete implementations must hide the STM32 HAL handles and specific hardware details.

---

## Phase 1: Common Definitions
**File:** `Common/error_codes.h`

Define a standardized error handling enumeration to be used across the system.
*   **Type:** `typedef enum`
*   **Prefix:** `DL_` (DataLogger) or generic `SYS_`.
*   **Values:**
    *   `SYS_OK` (0)
    *   `SYS_ERROR_GENERIC`
    *   `SYS_ERROR_TIMEOUT`
    *   `SYS_ERROR_BUSY`
    *   `SYS_ERROR_NULL_PTR`
    *   `SYS_ERROR_INVALID_PARAM`

---

## Phase 2: Hardware Interfaces (The Contracts)
**Location:** `Interfaces/`

Define the following "V-Table" interfaces. Each interface should define a `typedef struct` containing function pointers.

### 1. GPIO Interface (`i_gpio.h`)
*   **Context:** Abstract Pin Control.
*   **V-Table:** `IGpio`
    *   `init(port, pin, mode)` (Abstract inputs, not HAL_GPIO_Init types)
    *   `write(port, pin, state)`
    *   `read(port, pin)`
    *   `toggle(port, pin)`

### 2. UART Interface (`i_uart.h`)
*   **Context:** Serial communication (e.g., for GPS, debug).
*   **V-Table:** `IUart`
    *   `send(data, length)`
    *   `receive(buffer, length)`
    *   `register_callback(event, function_ptr)` (For interrupts/DMA)
    *   `receive_asyn(buffer, length)` (For interrupts/DMA start)

### 3. I2C Interface (`i_i2c.h`)
*   **Context:** Chip communication (e.g., EEPROM).
*   **V-Table:** `II2c`
    *   `read_reg(dev_addr, reg_addr, data, length)`
    *   `write_reg(dev_addr, reg_addr, data, length)`
    *   `is_ready(dev_addr)`

### 4. ADC Interface (`i_adc.h`)
*   **Context:** Analog measurement.
*   **V-Table:** `IAdc`
    *   `start_conversion(channel)`
    *   `get_value(channel, float* output)` (Return normalized value or voltage)
    *   `calibrate()`
    *   `start_conversion_asyn(buffer, size)` (convert via DMA /IT)
    *   `register_callback` (register conversion complete callback)

---

## Phase 3: Concrete Implementation (STM32U5)
**Location:** `Infrastructure/Drivers/STM32/`

Implement the interfaces defined above using the **STM32 HAL Drivers**.

### Pattern Implementation
For each driver (e.g., `stm32_uart.c`):
1.  **Private Context:** encapsulate `UART_HandleTypeDef` locally.
2.  **Implementation:** Implement the function pointers defined in the interface.
3.  **Constructor/Getter:** Provide a global access function that returns the interface pointer.
    *   Example: `const IUart* STM32_UART_GetApi(void);`

### Requirements
*   Map `IGpio` calls to `HAL_GPIO_WritePin`, etc.
*   Map `IUart` calls to `HAL_UART_Transmit_DMA` / `HAL_UART_Receive_DMA` (or IT/Blocking based on requirement).
*   Ensure all functions map `HAL_StatusTypeDef` to your `Common/error_codes.h` values.

---

**Output Requirement:**
Please generate the code for Phase 1, Phase 2, and Phase 3 sequentially. Ensure header guards (`#ifndef`) and proper commenting (Doxygen style).
