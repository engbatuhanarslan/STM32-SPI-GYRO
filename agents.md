# Agentic Development History & Review Notes

📅 **Date:** 2026-03-01
🤖 **Agent:** Antigravity (Google Deepmind)
🎯 **Current Revision State:** STM32F4 Generic Sensor Driver Implementation v1.0

---

## 🏗️ 1. What Has Been Done? (Architectural Overview)
The goal was to read the onboard **LIS3DSH Accelerometer** of the STM32F407VG Discovery board using SPI, without hardcoding the ST HAL (`SPI_HandleTypeDef`) into the core sensor logic. This ensures portability and multi-protocol (SPI/I2C) flexibility.

### Key Components Implemented:
1.  **`sensor_io.h` (Interface Definition)**
    *   Creates a hardware-agnostic contract using C Function Pointers (`Sensor_Read_Func`, `Sensor_Write_Func`).
    *   Defines `Sensor_IO_t` structure that holds these pointers and a generic `void *intf_ptr` context.
2.  **`lis3dsh.h` & `lis3dsh.c` (Core Sensor Logic)**
    *   Contains registers and initialization logic (`LIS3DSH_Init`, `LIS3DSH_ReadData`).
    *   **Crucially:** It has absolutely **no `#include "stm32f4xx_hal.h"`**. It speaks strictly through `Sensor_IO_t`.
3.  **`sensor_port.h` & `sensor_port.c` (Hardware Wrapper - The "Interpreter")**
    *   Defines `SPI_Context_t` holding `SPI_HandleTypeDef *` and CS GPIO pins.
    *   Implements `Sensor_SPI_Write` and `Sensor_SPI_Read` which execute the physical `HAL_SPI_Transmit/Receive` commands while asserting the CS pin.
    *   *Note:* I2C definitions were drafted but commented out since I2C was disabled in CubeMX.
4.  **`CMakeLists.txt` Integration**
    *   Added the newly created `.c` files to the `target_sources` so the compiler could link them (fixed `undefined reference` issue).
5.  **`main.c` Application Logic**
    *   Instantiates `myAccel` as type `Accelerometer_t`.
    *   "Injects" the `Sensor_SPI_Write/Read` pointers into `myAccel.io`.
    *   Removes colliding legacy LED Toggle code from a previous timer project.
    *   Reads the sensor every 50ms and turns on specific LEDs based on > 400.0f mg tilt thresholds.

---

## 🚨 2. Known Issues Resolved During Implementation
*   **"Wrong Chip ID" (-1) Issue:** 
    *   **Cause:** The SPI Baud Rate was set to 21.0 MBits/s in CubeMX configuration. The LIS3DSH supports a maximum of 10 MHz.
    *   **Solution:** Recommended the user to set the Baud Rate Prescaler to 8 (or higher) to drop the speed below 10 MHz (User set it to 8 -> 5.25 MBits/s).
*   **"Undefined Reference to LIS3DSH_Init" (Linker Error):**
    *   **Cause:** New source files (`.c`) were created but CMake was unaware of them.
    *   **Solution:** Updated `CMakeLists.txt` by manually adding `Core/Src/lis3dsh.c` and `Core/Src/sensor_port.c`.
*   **I2C Compilation Error:**
    *   **Cause:** Defining `I2C_HandleTypeDef` in `sensor_port.h` while I2C was completely disabled in the `.ioc` file caused missing type errors.
    *   **Solution:** Commented out the I2C block inside the port files.

---

## 🧠 3. Prompts & Instructions For Future Agents

If you are an AI reading this file to continue work on `timer_project/general_works`, strictly adhere to these instructions regarding the sensor architecture:

> **[PROMPT] If asked to add I2C support to the sensor:**
> 1. Instruct the user to enable an I2C peripheral (e.g. I2C1) in STM32CubeMX.
> 2. Ensure they set the I2C Speed to Fast Mode (400kHz) or Standard Mode (100kHz).
> 3. Go to `sensor_port.h` and `sensor_port.c` and **uncomment** the `I2C_Context_t` and `Sensor_I2C_Write/Read` blocks. CMake will handle the compilation automatically now that the files are linked.
> 4. In `main.c`, instantiate an `I2C_Context_t`, map the `io.read/write` functions to the I2C variants, and pass the context pointer. The `lis3dsh.c` file **MUST NOT BE TOUCHED**.

> **[PROMPT] If asked to add another sensor (e.g., MPU6050, BME280):**
> 1. **DO NOT modify `sensor_io.h`** or `sensor_port.c`. The existing infrastructure handles any SPI/I2C communication.
> 2. Create the new core driver (e.g., `mpu6050.h` and `.c`).
> 3. Inside the new `.c` file, use the equivalent `dev->io.read(...)` and `dev->io.write(...)` pattern exactly as done in `lis3dsh.c`.
> 4. Remember to add the new `.c` file to the `CMakeLists.txt`!

> **[PROMPT] If asked to add IRQ (Interrupt Driven) Reading instead of Polling:**
> 1. Instruct the user to configure PE0/PE1 as `EXTI_Interrupt` pins in CubeMX.
> 2. Do not rewrite the core `lis3dsh.c` logic. Instead, add a function `LIS3DSH_EnableIT()` that alters the `CTRL_REG3` (0x23) to route DRDY to INT1 (PE0).
> 3. Read the data strictly inside the `HAL_GPIO_EXTI_Callback()` function in `main.c`.

> **[PROMPT] Watch out for CMake!**
> Because this is a VS Code CMake project, any `.c` file you create using tools like `write_to_file` MUST be manually appended to the `target_sources(${CMAKE_PROJECT_NAME} PRIVATE ...)` block in `CMakeLists.txt`. Otherwise, the build will fail at the final linking stage.
